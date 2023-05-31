#ifndef THREAD_POOL_HPP
#define THREAD_POOL_HPP

#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include "Worker.hpp"

class ThreadPool {
public:
    ThreadPool(size_t num_workers);
    ~ThreadPool();

    void enqueue_task(std::function<void()> task);

private:
    std::vector<Worker> workers_;
    size_t next_worker_; 
};

#endif // THREAD_POOL_HPP
