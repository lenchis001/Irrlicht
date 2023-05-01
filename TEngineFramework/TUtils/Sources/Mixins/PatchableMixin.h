#pragma once

#include "string"

#include "boost/shared_ptr.hpp"
#include "boost/weak_ptr.hpp"

class PatchableMixin {
public:
    void patchWorkingDirectory(const std::wstring& pathToFile);
    void unpatchWorkingDirectory();

    static std::wstring getFilename(const std::wstring& pathToFile);

private:
    std::wstring _workingDirectoryBuffer;
};
