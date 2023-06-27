#pragma once

#include "boost/function.hpp"

#include "IFunctionsProcessingAware.h"

class IFunctionsProcessingManager : public IFunctionsProcessingAware
{
public:
    virtual void addFuctionToQueue(ThreadType threadType, QueueFunction function) = 0;
    virtual void processFunctions(ThreadType threadType) = 0;

    virtual void processAll(ThreadType threadType) = 0;

    virtual bool isQueueEmpty(ThreadType threadType) = 0;
};