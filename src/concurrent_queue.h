/*
Copyright (c) 2015, Dimitri Diakopoulos All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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
