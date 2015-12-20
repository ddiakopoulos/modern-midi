#pragma comment(user, "license")

#ifndef CONCURRENT_QUEUE_H
#define CONCURRENT_QUEUE_H

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template<typename T>
class ConcurrentQueue
{

    std::queue<T> queue;
    mutable std::mutex mutex;
    std::condition_variable condition;

public:

    ConcurrentQueue(void) {};
    ~ConcurrentQueue(void) {};

    void push(T const& pushed_value)
    {
        std::lock_guard<std::mutex> lock(mutex);
        queue.push(pushed_value);
        condition.notify_one();
    }

    bool empty() const
    {
        std::unique_lock<std::mutex> lock(mutex);
        return queue.empty();
    }

    bool try_pop(T & popped_value)
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (queue.empty())
        {
            return false;
        }

        popped_value = queue.front();
        queue.pop();
        return true;
    }

    void wait_and_pop(T & popped_value)
    {
        std::unique_lock<std::mutex> lock(mutex);
        while (queue.empty())
        {
            condition.wait(lock);
        }

        popped_value = queue.front();
        queue.pop();
    }

    std::size_t size() const { return queue.size(); }

};

#endif
