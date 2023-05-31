#ifndef WORKER_HPP
#define WORKER_HPP

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
    void join();
    
    std::thread thread_;
private:
    void stop();
    void run();

    std::queue<std::function<void()>> tasks_;
    std::mutex tasks_mutex_;
    std::condition_variable tasks_cv_;
    bool stop_;
    std::mutex stop_mutex_;
};

#endif // WORKER_HPP
