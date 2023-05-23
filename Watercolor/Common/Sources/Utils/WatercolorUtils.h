#pragma once

#include "string"
#include "vector"

#include "wx/wx.h"

namespace Watercolor::Utils {
wxArrayString toWxArrayString(std::vector<std::wstring>& inputArray);
};
