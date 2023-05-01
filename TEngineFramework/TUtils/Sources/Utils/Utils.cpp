#include "Utils.h"

#include "boost/beast.hpp"
#include "boost/locale.hpp"
#include "boost/property_tree/json_parser.hpp"
#include "boost/regex.hpp"
#include "numeric"
#include <iterator>

namespace TUtils {
std::string toBase64(std::string& inputString)
{
    char* buffer = new char[boost::beast::detail::base64::encoded_size(inputString.size())];

    auto resultSize = boost::beast::detail::base64::encode(buffer, inputString.c_str(), inputString.size());

    return std::string(buffer);
}

std::wstring toBase64(std::wstring& inputString)
{
    std::string decodedByteString = toString(inputString);
    std::string encodedByteString = toBase64(decodedByteString);
    std::wstring result = toWstring(encodedByteString);

    return result;
}

std::string fromBase64(std::string& inputString)
{
    char* buffer = new char[boost::beast::detail::base64::decoded_size(inputString.size())];

    auto resultSize = boost::beast::detail::base64::decode(buffer, inputString.c_str(), inputString.size());

    return std::string(buffer);
}

std::wstring fromBase64(std::wstring& inputString)
{
    std::string encodedByteString = toString(inputString);
    std::string decodedByteString = fromBase64(encodedByteString);
    std::wstring result = toWstring(decodedByteString);

    return result;
}

std::wstring toWstring(const char* inputString)
{
    return toWstring(std::string(inputString));
}

std::wstring toWstring(const wchar_t* inputString)
{
    return std::wstring(inputString);
}

std::wstring toWstring(const std::string& inputString)
{
    std::wstring outputString = boost::locale::conv::utf_to_utf<wchar_t>(inputString);

    return outputString;
}

std::wstring toWstring(
    const boost::property_tree::wptree& propertyTree)
{
    std::wstringstream wss;

    boost::property_tree::json_parser::write_json(wss, propertyTree);

    return wss.str();
}

std::wstring toWstring(bool value)
{
    return value ? L"true" : L"false";
}

std::wstring toWstring(wchar_t value)
{
    std::wstring result(L"X");

    result[0] = value;

    return result;
}

std::string toString(const std::wstring& inputString)
{
    std::string outputString = boost::locale::conv::utf_to_utf<char>(inputString);

    return outputString;
}

std::string toString(const char* inputString)
{
    return std::string(inputString);
}

std::map<std::wstring, std::wstring> toStdMap(
    const boost::property_tree::wptree& tree)
{
    std::map<std::wstring, std::wstring> answer;

    for (auto& currentPair : tree) {
        answer.insert(std::pair<std::wstring, std::wstring>(currentPair.first,
            currentPair.second.data()));
    }

    return answer;
}

boost::property_tree::wptree toWPtree(const std::wstring& jsonData)
{
    std::wstringstream wss;
    wss.str(jsonData);
    boost::property_tree::wptree wptree;

    boost::property_tree::json_parser::read_json(wss, wptree);

    return wptree;
}

boost::property_tree::wptree toWPtree(const std::vector<std::wstring>& data)
{
    return std::accumulate(data.begin(), data.end(), boost::property_tree::wptree(), [](auto& result, auto& currentValue) {
        boost::property_tree::wptree child;
        child.put(L"", currentValue);

        result.push_back({ L"", child });

        return result;
    });
}

std::vector<std::wstring> toStringVector(const boost::property_tree::wptree& vectorWPtree)
{
    std::vector<std::wstring> stringsVector;

    for (auto& value : vectorWPtree) {
        stringsVector.push_back(value.second.data());
    }

    return stringsVector;
}

std::vector<std::wstring> toStringVector(const std::wstring& jsonData)
{
    boost::property_tree::wptree tree;
    std::wstringstream stream;

    stream.write(jsonData.c_str(), jsonData.size());
    boost::property_tree::json_parser::read_json(stream, tree);

    return toStringVector(tree);
}

bool toBool(const std::wstring& value)
{
    return value == L"true" ? true : false;
}

std::vector<std::wstring> split(const std::wstring& enumString, const std::wstring& delimiter)
{
    std::vector<std::wstring> parts;

    boost::wregex re(delimiter);
    boost::wsregex_token_iterator i(enumString.begin(), enumString.end(), re, -1);
    boost::wsregex_token_iterator j;

    unsigned count = 0;
    while (i != j) {
        parts.push_back(*i++);
    }
    return std::move(parts);
}
}