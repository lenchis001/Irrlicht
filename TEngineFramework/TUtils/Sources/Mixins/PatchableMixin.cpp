#include "PatchableMixin.h"

#include "boost/filesystem.hpp"

void PatchableMixin::patchWorkingDirectory(const std::wstring& pathToFile)
{
    _workingDirectoryBuffer = boost::filesystem::current_path().wstring();

    auto levelFileDirectory = boost::filesystem::path(pathToFile).parent_path();

    boost::filesystem::current_path(levelFileDirectory);
}

void PatchableMixin::unpatchWorkingDirectory()
{
    boost::filesystem::current_path(_workingDirectoryBuffer.c_str());
}

std::wstring PatchableMixin::getFilename(const std::wstring& pathToFile)
{
    return boost::filesystem::path(pathToFile).filename().generic_wstring();
}
