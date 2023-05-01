#include "boost/function.hpp"

#include "string"

namespace Types::Event {
#define EMPTY_CALLBACK []() {}
#define EMPTY_CALLBACK_WITH_PARAM [](auto _) {}

typedef boost::function<void(bool)> BoolEventCallback;
}