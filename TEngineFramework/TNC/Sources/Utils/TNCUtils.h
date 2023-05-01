#pragma once

#include "string"

namespace TUtils {
struct Uri {
public:
    std::wstring QueryString, Path, Protocol, Host, Port;

    static Uri Parse(const std::wstring& uri);
}; // uri

}