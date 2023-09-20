#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <atomic>
#include <thread>
#include <vector>

class ThreadPool
{
public:
    ThreadPool() = default;

private:
    std::vector<std::thread> pool;
};

#endif