#pragma once

#include "TNCTypes/TNCEvent.h"

#include "string"

namespace TNC {
class THttpClient {
public:
    static void get(const std::wstring& uri, Types::Event::WstringWithErrorCallback callback);
};
}