#pragma once

#include "boost/function.hpp"

#include "StandartThreadTypes.h"

typedef char ThreadType; // i hope, 128 threads will be enough for you :)
typedef boost::function<void()> QueueFunction;

class IFunctionsProcessingManager {
public:
    virtual void addFuctionToQueue(ThreadType threadType, QueueFunction function) = 0;
    virtual void processFunctions(ThreadType threadType)= 0;

    virtual void processAll(ThreadType threadType) = 0;

    virtual bool isQueueEmpty(ThreadType threadType) = 0;
};