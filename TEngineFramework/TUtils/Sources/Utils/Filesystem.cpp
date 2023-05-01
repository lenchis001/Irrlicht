#include "Filesystem.h"

#include "locale"
#include <codecvt>

#include "boost/filesystem.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/shared_ptr.hpp"
#include <boost/iostreams/device/mapped_file.hpp>

#include "Utils.h"

namespace TUtils::Filesystem {
std::wstring readText(const std::wstring& pathToFile)
{
    if (boost::filesystem::exists(pathToFile)) {
        boost::filesystem::wifstream reader(pathToFile);

        reader.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::consume_header>()));

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

bool write(const std::wstring& pathToFile, const std::wstring& fileData)
{
    bool result = false;

    boost::filesystem::wofstream writer(pathToFile);

    writer.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t, 0x10ffff, std::consume_header>()));

    if (writer) {
        writer.write(fileData.c_str(), fileData.length());
        writer.close();
        result = true;
    }

    return result;
}

bool write(const std::wstring& pathToFile, const boost::property_tree::wptree& fileData)
{
    std::wstringstream stream;
    boost::property_tree::json_parser::write_json_helper(stream, fileData, 1, true);

    return write(pathToFile, stream.str());
}

bool writeVector(const std::wstring& pathToFile, const std::vector<std::wstring>& data)
{
    auto tree = TUtils::toWPtree(data);

    return write(pathToFile, tree);
}

std::vector<std::wstring> readVector(const std::wstring& pathToFile)
{
    auto data = readText(pathToFile);

    return TUtils::toStringVector(data);
}

std::vector<std::wstring> collectFilesByTemplate(const std::wstring& folderPath, const std::wstring& fileNameRegExp)
{
    using namespace boost::filesystem;

    std::vector<std::wstring> pathes;

    if (is_directory(folderPath)) {
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
    }

    return pathes;
}

}