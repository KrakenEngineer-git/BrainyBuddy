#include "ThreadPool.hpp"

ThreadPool::ThreadPool(size_t num_threads)
    : stop_(false) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers_.emplace_back([this]() { worker(); });
    }
}

ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(tasks_mutex_);
        stop_ = true;
    }
    tasks_cv_.notify_all();
    for (std::thread &worker : workers_) {
        worker.join();
    }
}

void ThreadPool::enqueue_task(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(tasks_mutex_);
        tasks_.push(task);
    }
    tasks_cv_.notify_one();
}

void ThreadPool::worker() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(tasks_mutex_);
            tasks_cv_.wait(lock, [this]() { return !tasks_.empty() || stop_; });
            if (stop_ && tasks_.empty()) {
                break;
            }
            task = tasks_.front();
            tasks_.pop();
        }
        task();
    }
}