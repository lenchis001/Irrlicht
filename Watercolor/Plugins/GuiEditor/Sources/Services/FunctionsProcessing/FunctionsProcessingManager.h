#pragma once

#include "functional"
#include "map"
#include "vector"

#include "boost/any.hpp"
#include "boost/function.hpp"
#include "boost/interprocess/sync/scoped_lock.hpp"
#include "boost/thread.hpp"

#include"IFunctionsProcessingManager.h"

class FunctionsProcessingManager : public IFunctionsProcessingManager {
public:
    FunctionsProcessingManager();
    ~FunctionsProcessingManager();

    virtual void addFuctionToQueue(ThreadType threadType, QueueFunction function) override;
    virtual void processFunctions(ThreadType threadType) override;

    virtual void processAll(ThreadType threadType) override;

    virtual bool isQueueEmpty(ThreadType threadType) override;

private:
    boost::recursive_mutex _queueMutex;
    boost::any _threadPool;

    std::map<ThreadType, std::vector<QueueFunction>> _functionsQueue;
};