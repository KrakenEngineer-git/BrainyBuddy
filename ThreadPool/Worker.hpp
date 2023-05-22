#ifndef WORKER_HPP
#define WORKER_HPP

#include <vector>
#include <queue>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>

class Worker {
public:
    Worker();

    ~Worker();

    void enqueue_task(std::function<void()> task);
    void wait_for_tasks_to_complete();

private:
    void run();

    std::thread thread_;
    std::queue<std::function<void()>> tasks_;
    std::mutex tasks_mutex_;
    std::condition_variable tasks_cv_;
    bool stop_;
    std::mutex stop_mutex_;
};

#endif // WORKER_HPP