#pragma once

#include "map"
#include "string"
#include "type_traits"
#include "vector"

#include "boost/lexical_cast.hpp"
#include "boost/property_tree/ptree.hpp"
#include "boost/regex.hpp"

namespace TUtils {
template <class NUMBER_TYPE>
NUMBER_TYPE toNumber(const std::wstring& numberStr);

std::string toBase64(std::string& inputString);
std::wstring toBase64(std::wstring& inputString);

std::string fromBase64(std::string& inputString);
std::wstring fromBase64(std::wstring& inputString);

std::wstring toWstring(const char* inputString);
std::wstring toWstring(const wchar_t* inputString);
std::wstring toWstring(const std::string& inputString);
std::wstring toWstring(const boost::property_tree::wptree& propertyTree);
std::wstring toWstring(bool value);
std::wstring toWstring(wchar_t value);

template <class NUMBER_TYPE>
typename std::enable_if<std::is_arithmetic<NUMBER_TYPE>::value, std::wstring>::type toWstring(NUMBER_TYPE number);

std::string toString(const std::wstring& inputString);
std::string toString(const char* inputString);

std::map<std::wstring, std::wstring> toStdMap(const boost::property_tree::wptree& tree);

boost::property_tree::wptree toWPtree(const std::wstring& jsonData);
boost::property_tree::wptree toWPtree(const std::vector<std::wstring>& data);

std::vector<std::wstring> toStringVector(const boost::property_tree::wptree& vectorWPtree);
std::vector<std::wstring> toStringVector(const std::wstring& jsonData);

bool toBool(const std::wstring& value);

template <class NUMBER_TYPE>
std::vector<NUMBER_TYPE> parseNumbers(const std::wstring& numbersText);
std::vector<std::wstring> split(const std::wstring& enumString, const std::wstring& delimiter);

template <class T>
std::vector<T> unique(const std::vector<T>& data);

template <class NUMBER_TYPE>
inline NUMBER_TYPE toNumber(const std::wstring& numberStr)
{
    NUMBER_TYPE answer = boost::lexical_cast<NUMBER_TYPE>(numberStr);
    return answer;
}

template <class NUMBER_TYPE>
inline typename std::enable_if<std::is_arithmetic<NUMBER_TYPE>::value, std::wstring>::type toWstring(NUMBER_TYPE number)
{
    return std::to_wstring(number);
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
        numbers.push_back(TUtils::toNumber<NUMBER_TYPE>(*rectIterator++));
    }

    return numbers;
}
template <class T>
std::vector<T> unique(const std::vector<T>& data)
{
    std::vector<T> result;

    for (auto currentValueIterator = data.begin();
         currentValueIterator != data.end();
         currentValueIterator++) {
        if (currentValueIterator == std::find(data.begin(), data.end(), *currentValueIterator)) {
            result.push_back(*currentValueIterator);
        }
    }

    return result;
}
}
