#include "ThreadPool.hpp"
#include <iostream>

ThreadPool::ThreadPool(size_t num_workers) : workers_(num_workers), next_worker_(0) {}

ThreadPool::~ThreadPool() {
    std::cout << "ThreadPool destructor called" << std::endl;
}

void ThreadPool::enqueue_task(std::function<void()> task) {
    workers_[next_worker_].enqueue_task(task);

    next_worker_ = (next_worker_ + 1) % workers_.size();
}

void ThreadPool::wait_for_tasks_to_complete() {
    for (auto& worker : workers_) {
        worker.wait_for_tasks_to_complete();
    }
}


