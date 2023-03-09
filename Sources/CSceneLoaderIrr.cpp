// Copyright (C) 2010-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CSceneLoaderIrr.h"
#include "ISceneNodeAnimatorFactory.h"
#include "ISceneUserDataSerializer.h"
#include "ISceneManager.h"
#include "IVideoDriver.h"
#include "IFileSystem.h"
#include "os.h"

namespace irr
{
namespace scene
{

//! Constructor
CSceneLoaderIrr::CSceneLoaderIrr(boost::shared_ptr<ISceneManager> smgr, boost::shared_ptr<io::IFileSystem> fs)
 : SceneManagerAwareMixin(smgr), FileSystem(fs),
   IRR_XML_FORMAT_SCENE(L"irr_scene"), IRR_XML_FORMAT_NODE(L"node"), IRR_XML_FORMAT_NODE_ATTR_TYPE(L"type"),
   IRR_XML_FORMAT_ATTRIBUTES(L"attributes"), IRR_XML_FORMAT_MATERIALS(L"materials"),
   IRR_XML_FORMAT_ANIMATORS(L"animators"), IRR_XML_FORMAT_USERDATA(L"userData")
{

}

//! Destructor
CSceneLoaderIrr::~CSceneLoaderIrr()
{

}

//! Returns true if the class might be able to load this file.
bool CSceneLoaderIrr::isALoadableFileExtension(const io::path& filename) const
{
	return core::hasFileExtension(filename, "irr");
}

//! Returns true if the class might be able to load this file.
bool CSceneLoaderIrr::isALoadableFileFormat(io::IReadFile *file) const
{
	// todo: check inside the file
	return true;
}

//! Loads the scene into the scene manager.
bool CSceneLoaderIrr::loadScene(io::IReadFile* file, ISceneUserDataSerializer* userDataSerializer,
	boost::shared_ptr<ISceneNode> rootNode)
{
	if (!file)
	{
		os::Printer::log("Unable to open scene file", ELL_ERROR);
		return false;
	}

	io::IXMLReader* reader = FileSystem->createXMLReader(file);
	if (!reader)
	{
		os::Printer::log("Scene is not a valid XML file", file->getFileName().c_str(), ELL_ERROR);
		return false;
	}

	boost::shared_ptr<scene::ISceneManager> lockedSceneManager = getSceneManager();

	// TODO: COLLADA_CREATE_SCENE_INSTANCES can be removed when the COLLADA loader is a scene loader
	bool oldColladaSingleMesh = lockedSceneManager->getParameters()->getAttributeAsBool(COLLADA_CREATE_SCENE_INSTANCES);
	lockedSceneManager->getParameters()->setAttribute(COLLADA_CREATE_SCENE_INSTANCES, false);

	// read file
	while (reader->read())
	{
		readSceneNode(reader, rootNode, userDataSerializer);
	}

	// restore old collada parameters
	lockedSceneManager->getParameters()->setAttribute(COLLADA_CREATE_SCENE_INSTANCES, oldColladaSingleMesh);

	// clean up
	reader->drop();
	return true;
}


//! Reads the next node
void CSceneLoaderIrr::readSceneNode(io::IXMLReader* reader, boost::shared_ptr<ISceneNode> parent,
	ISceneUserDataSerializer* userDataSerializer)
{
	if (!reader)
		return;

	boost::shared_ptr<scene::ISceneNode> node = 0;

	boost::shared_ptr<scene::ISceneManager> lockedSceneManager = getSceneManager();
	if (!parent && IRR_XML_FORMAT_SCENE==reader->getNodeName())
		node = lockedSceneManager->getRootSceneNode();
	else if (parent && IRR_XML_FORMAT_NODE==reader->getNodeName())
	{
		// find node type and create it
		core::stringc attrName = reader->getAttributeValue(IRR_XML_FORMAT_NODE_ATTR_TYPE.c_str());

		node = lockedSceneManager->addSceneNode(attrName.c_str(), parent);

		if (!node)
			os::Printer::log("Could not create scene node of unknown type", attrName.c_str());
	}
	else
		node=parent;

	// read attributes
	while(reader->read())
	{
		bool endreached = false;

		const wchar_t* name = reader->getNodeName();

		switch (reader->getNodeType())
		{
		case io::EXN_ELEMENT_END:
			if ((IRR_XML_FORMAT_NODE  == name) ||
				(IRR_XML_FORMAT_SCENE == name))
			{
				endreached = true;
			}
			break;
		case io::EXN_ELEMENT:
			if (IRR_XML_FORMAT_ATTRIBUTES == name)
			{
				// read attributes
				io::IAttributes* attr = FileSystem->createEmptyAttributes(getSceneManager()->getVideoDriver());
				attr->read(reader, true);

				if (node)
					node->deserializeAttributes(attr);

				attr->drop();
			}
			else
			if (IRR_XML_FORMAT_MATERIALS == name)
				readMaterials(reader, node);
			else
			if (IRR_XML_FORMAT_ANIMATORS == name)
				readAnimators(reader, node);
			else
			if (IRR_XML_FORMAT_USERDATA  == name)
				readUserData(reader, node, userDataSerializer);
			else
			if ((IRR_XML_FORMAT_NODE  == name) ||
				(IRR_XML_FORMAT_SCENE == name))
			{
				readSceneNode(reader, node, userDataSerializer);
			}
			else
			{
				os::Printer::log("Found unknown element in irrlicht scene file",
						core::stringc(name).c_str());
			}
			break;
		default:
			break;
		}

		if (endreached)
			break;
	}
	if (node && userDataSerializer)
		userDataSerializer->OnCreateNode(node);
}

//! reads materials of a node
void CSceneLoaderIrr::readMaterials(io::IXMLReader* reader, boost::shared_ptr<ISceneNode> node)
{
	u32 nr = 0;

	while(reader->read())
	{
		const wchar_t* name = reader->getNodeName();

		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT_END:
			if (IRR_XML_FORMAT_MATERIALS == name)
				return;
			break;
		case io::EXN_ELEMENT:
			if (IRR_XML_FORMAT_ATTRIBUTES == name)
			{
				boost::shared_ptr<scene::ISceneManager> lockedSceneManager = getSceneManager();

				// read materials from attribute list
				io::IAttributes* attr = FileSystem->createEmptyAttributes(lockedSceneManager->getVideoDriver());
				attr->read(reader);

				if (node && node->getMaterialCount() > nr)
				{
					lockedSceneManager->getVideoDriver()->fillMaterialStructureFromAttributes(
						node->getMaterial(nr), attr);
				}

				attr->drop();
				++nr;
			}
			break;
		default:
			break;
		}
	}
}


//! reads animators of a node
void CSceneLoaderIrr::readAnimators(io::IXMLReader* reader, boost::shared_ptr<ISceneNode> node)
{
	while(reader->read())
	{
		const wchar_t* name = reader->getNodeName();

		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT_END:
			if (IRR_XML_FORMAT_ANIMATORS == name)
				return;
			break;
		case io::EXN_ELEMENT:
			if (IRR_XML_FORMAT_ATTRIBUTES == name)
			{
				boost::shared_ptr<scene::ISceneManager> lockedSceneManager = getSceneManager();

				// read animator data from attribute list
				io::IAttributes* attr = FileSystem->createEmptyAttributes(lockedSceneManager->getVideoDriver());
				attr->read(reader);

				if (node)
				{
					core::stringc typeName = attr->getAttributeAsString("Type");
					boost::shared_ptr<ISceneNodeAnimator> anim = lockedSceneManager->createSceneNodeAnimator(typeName.c_str(), node);

					if (anim)
					{
						anim->deserializeAttributes(attr);
					}
				}

				attr->drop();
			}
			break;
		default:
			break;
		}
	}
}


//! reads user data of a node
void CSceneLoaderIrr::readUserData(io::IXMLReader* reader, boost::shared_ptr<ISceneNode> node, ISceneUserDataSerializer* userDataSerializer)
{
	while(reader->read())
	{
		const wchar_t* name = reader->getNodeName();

		switch(reader->getNodeType())
		{
		case io::EXN_ELEMENT_END:
			if (IRR_XML_FORMAT_USERDATA == name)
				return;
			break;
		case io::EXN_ELEMENT:
			if (IRR_XML_FORMAT_ATTRIBUTES == name)
			{
				// read user data from attribute list
				io::IAttributes* attr = FileSystem->createEmptyAttributes(getSceneManager()->getVideoDriver());
				attr->read(reader);

				if (node && userDataSerializer)
				{
					userDataSerializer->OnReadUserData(node, attr);
				}

				attr->drop();
			}
			break;
		default:
			break;
		}
	}
}

} // scene
} // irr

