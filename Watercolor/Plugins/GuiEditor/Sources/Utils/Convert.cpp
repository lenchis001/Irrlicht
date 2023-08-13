#include "Convert.h"

#include "boost/locale.hpp"
#include "boost/filesystem.hpp"

namespace GUIEditor::Utils {
    std::wstring toWstring(const char* inputString)
    {
        return toWstring(std::string(inputString));
    }

    std::wstring toWstring(const std::string& inputString)
    {
        std::wstring outputString = boost::locale::conv::utf_to_utf<wchar_t>(inputString);

        return outputString;
    }

    wxArrayString toWxArrayString(std::vector<std::wstring>& inputArray)
    {
        wxArrayString result;
        std::for_each(inputArray.begin(), inputArray.end(),
            [&](std::wstring item) { result.push_back(item); });

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