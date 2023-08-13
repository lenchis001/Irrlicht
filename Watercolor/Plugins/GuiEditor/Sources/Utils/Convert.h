#pragma once

#include "string"
#include "vector"

#include "boost/regex.hpp"
#include "boost/lexical_cast.hpp"

#include "wx/wx.h"

namespace GUIEditor::Utils {
	std::wstring toWstring(const char* inputString);
	std::wstring toWstring(const std::string& inputString);
	wxArrayString toWxArrayString(std::vector<std::wstring>& inputArray);

    template <class NUMBER_TYPE>
    inline NUMBER_TYPE toNumber(const std::wstring& numberStr)
    {
        NUMBER_TYPE answer = boost::lexical_cast<NUMBER_TYPE>(numberStr);
        return answer;
    }

    template <class NUMBER_TYPE>
    std::vector<NUMBER_TYPE> parseNumbers(const std::wstring& numbersText)
    {
        std::vector<NUMBER_TYPE> numbers;

        boost::wregex numberRegExp(L"-?\\d+((\\.\\d)+)?");
        boost::wsregex_token_iterator rectIterator(numbersText.begin(),
            numbersText.end(), numberRegExp),
            end;

        while (rectIterator != end) {
            numbers.push_back(toNumber<NUMBER_TYPE>(*rectIterator++));
        }

        return numbers;
    }

    std::vector<std::wstring> collectFilesByTemplate(const std::wstring& folderPath, const std::wstring& fileNameRegExp);
}