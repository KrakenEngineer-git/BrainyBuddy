#include "Worker.hpp"
#include <iostream>

Worker::Worker(): stop_(false) {
    thread_ = std::thread([this]() { this->run(); });
}

Worker::~Worker() {
    std::cout << "Worker destructor called" << std::endl;
    stop();
    if (thread_.joinable())
        thread_.join();
}

void Worker::join() {
    if (thread_.joinable()) {
        thread_.join();
    }
}

void Worker::stop() {
    {
        std::lock_guard<std::mutex> lock(stop_mutex_);
        stop_ = true;
    }
    tasks_cv_.notify_all();
}

void Worker::enqueue_task(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(tasks_mutex_);
        tasks_.push(task);
    }
    tasks_cv_.notify_one();
}

void Worker::run() {
    while (true) {
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> tasks_lock(tasks_mutex_);
            tasks_cv_.wait(tasks_lock, [this]() { 
                std::lock_guard<std::mutex> stop_lock(stop_mutex_);
                return !tasks_.empty() || stop_; 
            });

            if(stop_ && tasks_.empty()) return;

            task = std::move(tasks_.front());
            tasks_.pop();
        }
        task();
    }
}
