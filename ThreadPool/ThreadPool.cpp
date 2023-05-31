#include "ThreadPool.hpp"
#include <iostream>

ThreadPool::ThreadPool(size_t num_workers) : workers_(num_workers), next_worker_(0) {}

ThreadPool::~ThreadPool() {
    std::cout << "ThreadPool destructor called" << std::endl;
    for (auto& worker : workers_) {
        if(worker.thread_.joinable())
            worker.thread_.join();
    }
}

void ThreadPool::enqueue_task(std::function<void()> task) {
    workers_[next_worker_].enqueue_task(task);
    next_worker_ = (next_worker_ + 1) % workers_.size();
}
