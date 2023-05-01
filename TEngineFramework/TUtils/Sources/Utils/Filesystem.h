#pragma once

#include "numeric"
#include "string"
#include "type_traits"
#include "vector"

#include "boost/property_tree/ptree.hpp"

namespace TUtils::Filesystem {
std::wstring readText(const std::wstring& pathToFile);
std::string readBytes(const std::wstring& pathToFile);

bool write(const std::wstring& pathToFile, const std::wstring& fileData);
bool write(const std::wstring& pathToFile, const boost::property_tree::wptree& fileData);

bool writeVector(const std::wstring& pathToFile, const std::vector<std::wstring>& data);
std::vector<std::wstring> readVector(const std::wstring& pathToFile);

std::vector<std::wstring> collectFilesByTemplate(const std::wstring& folderPath, const std::wstring& fileNameRegExp);
};
