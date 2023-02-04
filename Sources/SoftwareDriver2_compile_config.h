// Copyright (C) 2002-2012 Nikolaus Gebhardt / Thomas Alten
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#ifndef __S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED__
#define __S_VIDEO_2_SOFTWARE_COMPILE_CONFIG_H_INCLUDED__

#include "IrrCompileConfig.h"

// mip mapping
#if defined ( SOFTWARE_DRIVER_2_MIPMAPPING )
	#define SOFTWARE_DRIVER_2_MIPMAPPING_MAX		8
	#define SOFTWARE_DRIVER_2_MIPMAPPING_LOD_BIAS	0
#else
	#define SOFTWARE_DRIVER_2_MIPMAPPING_MAX		1
	#define SOFTWARE_DRIVER_2_MIPMAPPING_LOD_BIAS	0
#endif

#define SOFTWARE_DRIVER_2_MIPMAPPING_SCALE (8/SOFTWARE_DRIVER_2_MIPMAPPING_MAX)

#ifndef REALINLINE
	#ifdef _MSC_VER
		#define REALINLINE __forceinline
	#else
		#define REALINLINE inline
	#endif
#endif

#endif
