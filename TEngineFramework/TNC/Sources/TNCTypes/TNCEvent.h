#pragma once

#include "map"
#include "string"

#include "boost/function.hpp"

namespace Types::Event {
typedef boost::function<void(bool, const std::wstring&)> WstringWithErrorCallback;
}