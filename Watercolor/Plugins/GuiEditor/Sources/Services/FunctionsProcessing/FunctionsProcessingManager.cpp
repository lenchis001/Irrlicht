#include "FunctionsProcessingManager.h"
#include "boost/asio/packaged_task.hpp"
#include "boost/asio/post.hpp"
#include "boost/asio/thread_pool.hpp"
#include "thread"

bool FunctionsProcessingManager::isQueueEmpty(ThreadType threadType)
{
    boost::interprocess::scoped_lock<boost::recursive_mutex> _locker(_queueMutex);

    bool result = false;
    if (_functionsQueue.find(threadType) != _functionsQueue.end()) {
        result = !_functionsQueue.at(threadType).size();
    }
    else {
        result = true;
    }

    return result;
}

FunctionsProcessingManager::FunctionsProcessingManager()
{
    this->_threadPool = boost::make_shared<boost::asio::thread_pool>(std::thread::hardware_concurrency());
}

FunctionsProcessingManager::~FunctionsProcessingManager()
{
    auto threadPool = boost::any_cast<boost::shared_ptr<boost::asio::thread_pool>>(this->_threadPool);

    threadPool->stop();
    threadPool->join();
}

void FunctionsProcessingManager::addFuctionToQueue(ThreadType threadType, QueueFunction function)
{
    if (threadType == ThreadTypes::THREAD_POOL) {
        boost::asio::post(*boost::any_cast<boost::shared_ptr<boost::asio::thread_pool>>(this->_threadPool), function);
    } else {
        boost::interprocess::scoped_lock<boost::recursive_mutex> _locker(_queueMutex);

        if (_functionsQueue.find(threadType) == _functionsQueue.end())
            _functionsQueue.insert({ threadType, std::vector<QueueFunction>() });

        _functionsQueue.at(threadType).push_back(function);
    }
}

void FunctionsProcessingManager::processFunctions(ThreadType threadType)
{
    _queueMutex.lock();
    std::vector<QueueFunction> queueFunctions;
    auto threadQueue = _functionsQueue.find(threadType);
    if (threadQueue != _functionsQueue.end()) {
        queueFunctions = std::move(threadQueue->second);
    }
    _queueMutex.unlock();

    for (QueueFunction currentFunction : queueFunctions)
        currentFunction();
}

void FunctionsProcessingManager::processAll(ThreadType threadType)
{
    while (!isQueueEmpty(threadType)) {
        processFunctions(threadType);
    }
}
