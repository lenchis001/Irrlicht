#include "Filesystem.h"

#include "boost/filesystem.hpp"
#include "boost/iostreams/device/mapped_file.hpp"
#include "codecvt"
#include "locale"

#include "Utils.h"

namespace Utils::Filesystem {
std::wstring readFile(const std::wstring& pathToFile)
{
    if (boost::filesystem::exists(pathToFile)) {
        boost::filesystem::wifstream reader(pathToFile);
        reader.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::consume_header>));

        auto fileSize = boost::filesystem::file_size(pathToFile);

        wchar_t* buffer = new wchar_t[fileSize + 1]; // +1 for zero end of string
        std::memset(buffer, NULL, fileSize * sizeof(wchar_t));
        reader.read(buffer, fileSize);

        auto fileData = toWstring(static_cast<const wchar_t*>(buffer));
        delete[] buffer;

        reader.close();
        return fileData;
    } else {
        throw std::exception("File isn't exist.");
    }
}

std::string readBytes(const std::wstring& pathToFile)
{
    if (boost::filesystem::exists(pathToFile)) {
        boost::filesystem::ifstream reader(pathToFile, std::ios::binary);

        auto fileSize = boost::filesystem::file_size(pathToFile);

        char* buffer = new char[fileSize];
        std::memset(buffer, NULL, fileSize);
        reader.read(buffer, fileSize);

        std::string result(buffer, fileSize);
        delete[] buffer;

        reader.close();
        return result;
    } else {
        throw std::exception("File isn't exist.");
    }
}

bool writeFile(const std::wstring& pathToFile, const std::wstring& fileData)
{
    bool result = false;

    boost::filesystem::wofstream writer(pathToFile);
    if (writer) {
        writer.write(fileData.c_str(), fileData.length());
        writer.close();
        result = true;
    }

    return result;
}

std::vector<std::wstring> collectFilesByTemplate(const std::wstring& folderPath, const std::wstring& fileNameRegExp)
{
    using namespace boost::filesystem;

    std::vector<std::wstring> pathes;
    boost::wregex expression(fileNameRegExp);

    recursive_directory_iterator end_itr;
    // cycle through the directory
    for (recursive_directory_iterator itr(folderPath); itr != end_itr; ++itr) {
        // If it's not a directory, list it. If you want to list directories too, just remove this check.
        if (is_regular_file(itr->path())) {

            if (boost::regex_match(itr->path().filename().wstring(), expression)) {
                pathes.push_back(itr->path().wstring());
            }
        }
    }

    return pathes;
}

}