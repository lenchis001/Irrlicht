#include "WatercolorUtils.h"
#include "algorithm"

namespace Watercolor::Utils {
    wxArrayString toWxArrayString(
        std::vector<std::wstring>& inputArray)
    {
        wxArrayString result;
        std::for_each(inputArray.begin(), inputArray.end(),
            [&](std::wstring item) { result.push_back(item); });

        return result;
    }
}