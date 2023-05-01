// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

// This file was originally written by William Finlayson.  I (Nikolaus
// Gebhardt) did some minor modifications and changes to it and integrated it
// into Irrlicht. Thanks a lot to William for his work on this and that he gave
// me his permission to add it into Irrlicht using the zlib license.

// After Irrlicht 0.12, Michael Zoech did some improvements to this renderer, I
// merged this into Irrlicht 0.14, thanks to him for his work.

#include "IrrCompileConfig.h"
#ifdef _IRR_COMPILE_WITH_OPENGL_

#include "COpenGLSLMaterialRenderer.h"
#include "IGPUProgrammingServices.h"
#include "IShaderConstantSetCallBack.h"
#include "IMaterialRendererServices.h"
#include "IVideoDriver.h"
#include "os.h"
#include "COpenGLDriver.h"

namespace irr
{
namespace video
{


//! Constructor
COpenGLSLMaterialRenderer::COpenGLSLMaterialRenderer(boost::shared_ptr<video::COpenGLDriver> driver,
		const c8* vertexShaderEntryPointName,
		E_VERTEX_SHADER_TYPE vsCompileTarget,
		const c8* pixelShaderEntryPointName,
		E_PIXEL_SHADER_TYPE psCompileTarget,
		const c8* geometryShaderEntryPointName,
		E_GEOMETRY_SHADER_TYPE gsCompileTarget,
		scene::E_PRIMITIVE_TYPE inType, scene::E_PRIMITIVE_TYPE outType,
		u32 verticesOut,
		boost::shared_ptr<IShaderConstantSetCallBack> callback,
		boost::shared_ptr<video::IMaterialRenderer> baseMaterial,
		s32 userData)
	: IMaterialRendererServices(driver), CallBack(callback), BaseMaterial(baseMaterial),
		Program(0), Program2(0), UserData(userData)
{
	#ifdef _DEBUG
	setDebugName("COpenGLSLMaterialRenderer");
	#endif

	//entry points must always be main, and the compile target isn't selectable
	//it is fine to ignore what has been asked for, as the compiler should spot anything wrong
	//just check that GLSL is available

	if (!getVideoDriver()->queryFeature(EVDF_ARB_GLSL))
		return;
}

//! Destructor
COpenGLSLMaterialRenderer::~COpenGLSLMaterialRenderer()
{
	boost::shared_ptr<COpenGLDriver> lockedDriver = getVideoDriver<COpenGLDriver>();

	if (Program)
	{
		GLhandleARB shaders[8];
		GLint count;
		lockedDriver->extGlGetAttachedObjects(Program, 8, &count, shaders);
		// avoid bugs in some drivers, which return larger numbers
		count=core::min_(count,8);
		for (GLint i=0; i<count; ++i)
			lockedDriver->extGlDeleteObject(shaders[i]);
		lockedDriver->extGlDeleteObject(Program);
		Program = 0;
	}

	if (Program2)
	{
		GLuint shaders[8];
		GLint count;
		lockedDriver->extGlGetAttachedShaders(Program2, 8, &count, shaders);
		// avoid bugs in some drivers, which return larger numbers
		count=core::min_(count,8);
		for (GLint i=0; i<count; ++i)
			lockedDriver->extGlDeleteShader(shaders[i]);
		lockedDriver->extGlDeleteProgram(Program2);
		Program2 = 0;
	}

	UniformInfo.clear();
}

void COpenGLSLMaterialRenderer::init(s32& outMaterialTypeNr,
		const c8* vertexShaderProgram,
		const c8* pixelShaderProgram,
		const c8* geometryShaderProgram,
		scene::E_PRIMITIVE_TYPE inType, scene::E_PRIMITIVE_TYPE outType,
		u32 verticesOut)
{
	outMaterialTypeNr = -1;

	if (!createProgram())
		return;

#if defined(GL_ARB_vertex_shader) && defined (GL_ARB_fragment_shader)
	if (vertexShaderProgram)
		if (!createShader(GL_VERTEX_SHADER_ARB, vertexShaderProgram))
			return;

	if (pixelShaderProgram)
		if (!createShader(GL_FRAGMENT_SHADER_ARB, pixelShaderProgram))
			return;
#endif

	boost::shared_ptr<COpenGLDriver> lockedDriver = getVideoDriver<COpenGLDriver>();

#if defined(GL_ARB_geometry_shader4) || defined(GL_EXT_geometry_shader4) || defined(GL_NV_geometry_program4) || defined(GL_NV_geometry_shader4)
	if (geometryShaderProgram && lockedDriver->queryFeature(EVDF_GEOMETRY_SHADER))
	{
		if (!createShader(GL_GEOMETRY_SHADER_EXT, geometryShaderProgram))
			return;
#if defined(GL_ARB_geometry_shader4) || defined(GL_EXT_geometry_shader4) || defined(GL_NV_geometry_shader4)
		if (Program2) // Geometry shaders are supported only in OGL2.x+ drivers.
		{
			lockedDriver->extGlProgramParameteri(Program2, GL_GEOMETRY_INPUT_TYPE_EXT, lockedDriver->primitiveTypeToGL(inType));
			lockedDriver->extGlProgramParameteri(Program2, GL_GEOMETRY_OUTPUT_TYPE_EXT, lockedDriver->primitiveTypeToGL(outType));
			if (verticesOut==0)
				lockedDriver->extGlProgramParameteri(Program2, GL_GEOMETRY_VERTICES_OUT_EXT, lockedDriver->MaxGeometryVerticesOut);
			else
				lockedDriver->extGlProgramParameteri(Program2, GL_GEOMETRY_VERTICES_OUT_EXT, core::min_(verticesOut, lockedDriver->MaxGeometryVerticesOut));
		}
#elif defined(GL_NV_geometry_program4)
		if (verticesOut==0)
			Driver->extGlProgramVertexLimit(GL_GEOMETRY_PROGRAM_NV, Driver->MaxGeometryVerticesOut);
		else
			Driver->extGlProgramVertexLimit(GL_GEOMETRY_PROGRAM_NV, core::min_(verticesOut, Driver->MaxGeometryVerticesOut));
#endif
	}
#endif

	if (!linkProgram())
		return;

	// register myself as new material
	outMaterialTypeNr = lockedDriver->addMaterialRenderer(getSharedThis());
}


bool COpenGLSLMaterialRenderer::OnRender(IMaterialRendererServices* service,
					E_VERTEX_TYPE vtxtype)
{
	// call callback to set shader constants
	if (CallBack && (Program||Program2))
		CallBack->OnSetConstants(this, UserData);

	return true;
}


void COpenGLSLMaterialRenderer::OnSetMaterial(const video::SMaterial& material,
				const video::SMaterial& lastMaterial,
				bool resetAllRenderstates,
				video::IMaterialRendererServices* services)
{
	boost::shared_ptr<COpenGLDriver> lockedDriver = getVideoDriver<COpenGLDriver>();

	if (material.MaterialType != lastMaterial.MaterialType || resetAllRenderstates)
	{
		if (Program2)
			lockedDriver->extGlUseProgram(Program2);
		else if (Program)
			lockedDriver->extGlUseProgramObject(Program);

		if (BaseMaterial)
			BaseMaterial->OnSetMaterial(material, material, true, this);
	}

	//let callback know used material
	if (CallBack)
		CallBack->OnSetMaterial(material);

	for (u32 i=0; i<MATERIAL_MAX_TEXTURES; ++i)
		lockedDriver->setActiveTexture(i, material.getTexture(i));
	lockedDriver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}


void COpenGLSLMaterialRenderer::OnUnsetMaterial()
{
	boost::shared_ptr<COpenGLDriver> lockedDriver = getVideoDriver<COpenGLDriver>();

	if (Program)
		lockedDriver->extGlUseProgramObject(0);
	if (Program2)
		lockedDriver->extGlUseProgram(0);

	if (BaseMaterial)
		BaseMaterial->OnUnsetMaterial();
}


//! Returns if the material is transparent.
bool COpenGLSLMaterialRenderer::isTransparent() const
{
	return BaseMaterial ? BaseMaterial->isTransparent() : false;
}


bool COpenGLSLMaterialRenderer::createProgram()
{
	boost::shared_ptr<COpenGLDriver> lockedDriver = getVideoDriver<COpenGLDriver>();

	if (lockedDriver->Version>=200)
		Program2 = lockedDriver->extGlCreateProgram();
	else
		Program = lockedDriver->extGlCreateProgramObject();
	return true;
}


bool COpenGLSLMaterialRenderer::createShader(GLenum shaderType, const char* shader)
{
	boost::shared_ptr<COpenGLDriver> lockedDriver = getVideoDriver<COpenGLDriver>();

	if (Program2)
	{
		GLuint shaderHandle = lockedDriver->extGlCreateShader(shaderType);
		lockedDriver->extGlShaderSource(shaderHandle, 1, &shader, NULL);
		lockedDriver->extGlCompileShader(shaderHandle);

		GLint status = 0;

#ifdef GL_VERSION_2_0
		lockedDriver->extGlGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
#endif

		if (status != GL_TRUE)
		{
			os::Printer::log("GLSL shader failed to compile", ELL_ERROR);
			// check error message and log it
			GLint maxLength=0;
			GLint length;
#ifdef GL_VERSION_2_0
			lockedDriver->extGlGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH,
					&maxLength);
#endif
			if (maxLength)
			{
				GLchar *infoLog = new GLchar[maxLength];
				lockedDriver->extGlGetShaderInfoLog(shaderHandle, maxLength, &length, infoLog);
				os::Printer::log(reinterpret_cast<const c8*>(infoLog), ELL_ERROR);
				delete [] infoLog;
			}

			return false;
		}

		lockedDriver->extGlAttachShader(Program2, shaderHandle);
	}
	else
	{
		GLhandleARB shaderHandle = lockedDriver->extGlCreateShaderObject(shaderType);

		lockedDriver->extGlShaderSourceARB(shaderHandle, 1, &shader, NULL);
		lockedDriver->extGlCompileShaderARB(shaderHandle);

		GLint status = 0;

#ifdef GL_ARB_shader_objects
		lockedDriver->extGlGetObjectParameteriv(shaderHandle, GL_OBJECT_COMPILE_STATUS_ARB, &status);
#endif

		if (!status)
		{
			os::Printer::log("GLSL shader failed to compile", ELL_ERROR);
			// check error message and log it
			GLint maxLength=0;
			GLsizei length;
#ifdef GL_ARB_shader_objects
			lockedDriver->extGlGetObjectParameteriv(shaderHandle,
					GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
#endif
			if (maxLength)
			{
				GLcharARB *infoLog = new GLcharARB[maxLength];
				lockedDriver->extGlGetInfoLog(shaderHandle, maxLength, &length, infoLog);
				os::Printer::log(reinterpret_cast<const c8*>(infoLog), ELL_ERROR);
				delete [] infoLog;
			}

			return false;
		}

		lockedDriver->extGlAttachObject(Program, shaderHandle);
	}
	return true;
}


bool COpenGLSLMaterialRenderer::linkProgram()
{
	boost::shared_ptr<COpenGLDriver> lockedDriver = getVideoDriver<COpenGLDriver>();

	if (Program2)
	{
		lockedDriver->extGlLinkProgram(Program2);

		GLint status = 0;

#ifdef GL_VERSION_2_0
		lockedDriver->extGlGetProgramiv(Program2, GL_LINK_STATUS, &status);
#endif

		if (!status)
		{
			os::Printer::log("GLSL shader program failed to link", ELL_ERROR);
			// check error message and log it
			GLint maxLength=0;
			GLsizei length;
#ifdef GL_VERSION_2_0
			lockedDriver->extGlGetProgramiv(Program2, GL_INFO_LOG_LENGTH, &maxLength);
#endif
			if (maxLength)
			{
				GLchar *infoLog = new GLchar[maxLength];
				lockedDriver->extGlGetProgramInfoLog(Program2, maxLength, &length, infoLog);
				os::Printer::log(reinterpret_cast<const c8*>(infoLog), ELL_ERROR);
				delete [] infoLog;
			}

			return false;
		}

		// get uniforms information

		GLint num = 0;
#ifdef GL_VERSION_2_0
		lockedDriver->extGlGetProgramiv(Program2, GL_ACTIVE_UNIFORMS, &num);
#endif

		if (num == 0)
		{
			// no uniforms
			return true;
		}

		GLint maxlen = 0;
#ifdef GL_VERSION_2_0
		lockedDriver->extGlGetProgramiv(Program2, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxlen);
#endif

		if (maxlen == 0)
		{
			os::Printer::log("GLSL: failed to retrieve uniform information", ELL_ERROR);
			return false;
		}

		// seems that some implementations use an extra null terminator
		++maxlen;
		c8 *buf = new c8[maxlen];

		UniformInfo.clear();
		UniformInfo.reallocate(num);

		for (GLint i=0; i < num; ++i)
		{
			SUniformInfo ui;
			memset(buf, 0, maxlen);

			GLint size;
			lockedDriver->extGlGetActiveUniform(Program2, i, maxlen, 0, &size, &ui.type, reinterpret_cast<GLchar*>(buf));
			ui.name = buf;

			UniformInfo.push_back(ui);
		}

		delete [] buf;
	}
	else
	{
		lockedDriver->extGlLinkProgramARB(Program);

		GLint status = 0;

#ifdef GL_ARB_shader_objects
		lockedDriver->extGlGetObjectParameteriv(Program, GL_OBJECT_LINK_STATUS_ARB, &status);
#endif

		if (!status)
		{
			os::Printer::log("GLSL shader program failed to link", ELL_ERROR);
			// check error message and log it
			GLint maxLength=0;
			GLsizei length;
#ifdef GL_ARB_shader_objects
			lockedDriver->extGlGetObjectParameteriv(Program,
					GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
#endif
			if (maxLength)
			{
				GLcharARB *infoLog = new GLcharARB[maxLength];
				lockedDriver->extGlGetInfoLog(Program, maxLength, &length, infoLog);
				os::Printer::log(reinterpret_cast<const c8*>(infoLog), ELL_ERROR);
				delete [] infoLog;
			}

			return false;
		}

		// get uniforms information

		GLint num = 0;
	#ifdef GL_ARB_shader_objects
		lockedDriver->extGlGetObjectParameteriv(Program, GL_OBJECT_ACTIVE_UNIFORMS_ARB, &num);
	#endif

		if (num == 0)
		{
			// no uniforms
			return true;
		}

		GLint maxlen = 0;
	#ifdef GL_ARB_shader_objects
		lockedDriver->extGlGetObjectParameteriv(Program, GL_OBJECT_ACTIVE_UNIFORM_MAX_LENGTH_ARB, &maxlen);
	#endif

		if (maxlen == 0)
		{
			os::Printer::log("GLSL: failed to retrieve uniform information", ELL_ERROR);
			return false;
		}

		// seems that some implementations use an extra null terminator
		++maxlen;
		c8 *buf = new c8[maxlen];

		UniformInfo.clear();
		UniformInfo.reallocate(num);

		for (int i=0; i < num; ++i)
		{
			SUniformInfo ui;
			memset(buf, 0, maxlen);

			GLint size;
			lockedDriver->extGlGetActiveUniformARB(Program, i, maxlen, 0, &size, &ui.type, reinterpret_cast<GLcharARB*>(buf));
			ui.name = buf;

			UniformInfo.push_back(ui);
		}

		delete [] buf;
	}

	return true;
}


void COpenGLSLMaterialRenderer::setBasicRenderStates(const SMaterial& material,
						const SMaterial& lastMaterial,
						bool resetAllRenderstates)
{
	boost::shared_ptr<COpenGLDriver> lockedDriver = getVideoDriver<COpenGLDriver>();

	// forward
	lockedDriver->setBasicRenderStates(material, lastMaterial, resetAllRenderstates);
}


bool COpenGLSLMaterialRenderer::setVertexShaderConstant(const c8* name, const f32* floats, int count)
{
	return setPixelShaderConstant(name, floats, count);
}

bool COpenGLSLMaterialRenderer::setVertexShaderConstant(const c8* name, const bool* bools, int count)
{
	return setPixelShaderConstant(name, bools, count);
}

bool COpenGLSLMaterialRenderer::setVertexShaderConstant(const c8* name, const s32* ints, int count)
{
	return setPixelShaderConstant(name, ints, count);
}

void COpenGLSLMaterialRenderer::setVertexShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	os::Printer::log("Cannot set constant, please use high level shader call instead.", ELL_WARNING);
}

bool COpenGLSLMaterialRenderer::setPixelShaderConstant(const c8* name, const f32* floats, int count)
{
	u32 i;
	const u32 num = UniformInfo.size();

	for (i=0; i < num; ++i)
	{
		if (UniformInfo[i].name == name)
			break;
	}

	if (i == num)
		return false;

	boost::shared_ptr<COpenGLDriver> lockedDriver = getVideoDriver<COpenGLDriver>();

#if defined(GL_VERSION_2_0)||defined(GL_ARB_shader_objects)
	GLint Location=0;
	if (Program2)
		Location= lockedDriver->extGlGetUniformLocation(Program2,name);
	else
		Location= lockedDriver->extGlGetUniformLocationARB(Program,name);

	bool status = true;

	switch (UniformInfo[i].type)
	{
		case GL_FLOAT:
			lockedDriver->extGlUniform1fv(Location, count, floats);
			break;
		case GL_FLOAT_VEC2:
			lockedDriver->extGlUniform2fv(Location, count/2, floats);
			break;
		case GL_FLOAT_VEC3:
			lockedDriver->extGlUniform3fv(Location, count/3, floats);
			break;
		case GL_FLOAT_VEC4:
			lockedDriver->extGlUniform4fv(Location, count/4, floats);
			break;
		case GL_FLOAT_MAT2:
			lockedDriver->extGlUniformMatrix2fv(Location, count/4, false, floats);
			break;
		case GL_FLOAT_MAT3:
			lockedDriver->extGlUniformMatrix3fv(Location, count/9, false, floats);
			break;
		case GL_FLOAT_MAT4:
			lockedDriver->extGlUniformMatrix4fv(Location, count/16, false, floats);
			break;
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
			{
				const GLint id = static_cast<GLint>(*floats);
				lockedDriver->extGlUniform1iv(Location, 1, &id);
			}
			break;
		default:
			status = false;
			break;
	}
	return status;
#else
	return false;
#endif
}

bool COpenGLSLMaterialRenderer::setPixelShaderConstant(const c8* name, const bool* bools, int count)
{
	u32 i;
	const u32 num = UniformInfo.size();

	for (i=0; i < num; ++i)
	{
		if (UniformInfo[i].name == name)
			break;
	}

	if (i == num)
		return false;

	boost::shared_ptr<COpenGLDriver> lockedDriver = getVideoDriver<COpenGLDriver>();

#if defined(GL_VERSION_2_0)||defined(GL_ARB_shader_objects)
	GLint Location=0;
	if (Program2)
		Location= lockedDriver->extGlGetUniformLocation(Program2,name);
	else
		Location= lockedDriver->extGlGetUniformLocationARB(Program,name);

	bool status = true;

	switch (UniformInfo[i].type)
	{
		case GL_BOOL:
			lockedDriver->extGlUniform1iv(Location, count, (GLint*)bools);
			break;
		case GL_BOOL_VEC2:
			lockedDriver->extGlUniform2iv(Location, count/2, (GLint*)bools);
			break;
		case GL_BOOL_VEC3:
			lockedDriver->extGlUniform3iv(Location, count/3, (GLint*)bools);
			break;
		case GL_BOOL_VEC4:
			lockedDriver->extGlUniform4iv(Location, count/4, (GLint*)bools);
			break;
		default:
			status = false;
			break;
	}
	return status;
#else
	return false;
#endif
}

bool COpenGLSLMaterialRenderer::setPixelShaderConstant(const c8* name, const s32* ints, int count)
{
	u32 i;
	const u32 num = UniformInfo.size();

	for (i=0; i < num; ++i)
	{
		if (UniformInfo[i].name == name)
			break;
	}

	if (i == num)
		return false;

	boost::shared_ptr<COpenGLDriver> lockedDriver = getVideoDriver<COpenGLDriver>();

#if defined(GL_VERSION_2_0)||defined(GL_ARB_shader_objects)
	GLint Location=0;
	if (Program2)
		Location= lockedDriver->extGlGetUniformLocation(Program2,name);
	else
		Location= lockedDriver->extGlGetUniformLocationARB(Program,name);

	bool status = true;

	switch (UniformInfo[i].type)
	{
		case GL_INT:
			lockedDriver->extGlUniform1iv(Location, count, ints);
			break;
		case GL_INT_VEC2:
			lockedDriver->extGlUniform2iv(Location, count/2, ints);
			break;
		case GL_INT_VEC3:
			lockedDriver->extGlUniform3iv(Location, count/3, ints);
			break;
		case GL_INT_VEC4:
			lockedDriver->extGlUniform4iv(Location, count/4, ints);
			break;
		case GL_SAMPLER_1D:
		case GL_SAMPLER_2D:
		case GL_SAMPLER_3D:
		case GL_SAMPLER_CUBE:
		case GL_SAMPLER_1D_SHADOW:
		case GL_SAMPLER_2D_SHADOW:
			lockedDriver->extGlUniform1iv(Location, 1, ints);
			break;
		default:
			status = false;
			break;
	}
	return status;
#else
	return false;
#endif
}

void COpenGLSLMaterialRenderer::setPixelShaderConstant(const f32* data, s32 startRegister, s32 constantAmount)
{
	os::Printer::log("Cannot set constant, use high level shader call.", ELL_WARNING);
}

} // end namespace video
} // end namespace irr


#endif

