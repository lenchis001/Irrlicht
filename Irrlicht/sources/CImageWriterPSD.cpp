// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#include "CImageWriterPSD.h"

#ifdef _IRR_COMPILE_WITH_PSD_WRITER_

#include "CImageLoaderPSD.h"
#include "IWriteFile.h"
#include "os.h" // for logging
#include "irrString.h"

namespace irr
{
namespace video
{

boost::shared_ptr<IImageWriter> createImageWriterPSD()
{
	return boost::make_shared<CImageWriterPSD>();
}

CImageWriterPSD::CImageWriterPSD()
{
#ifdef _DEBUG
	setDebugName("CImageWriterPSD");
#endif
}

bool CImageWriterPSD::isAWriteableFileExtension(const io::path& filename) const
{
	return core::hasFileExtension ( filename, "psd" );
}

bool CImageWriterPSD::writeImage(boost::shared_ptr<io::IWriteFile> file, boost::shared_ptr<IImage> image,u32 param) const
{
	os::Printer::log("PSD writer not yet implemented. Image not written.", ELL_WARNING);
	return false;
}

} // namespace video
} // namespace irr

#endif

