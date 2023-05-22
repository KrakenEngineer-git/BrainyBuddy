#ifndef RESPONSE_FUTURE_HPP
#define RESPONSE_FUTURE_HPP

#include <future>

class ResponseFuture {
public:
    ResponseFuture() = default;

    ResponseFuture(ResponseFuture&& other) noexcept : promise(std::move(other.promise)), future(std::move(other.future)) {}

    ResponseFuture& operator=(ResponseFuture&& other) noexcept {
        promise = std::move(other.promise);
        future = std::move(other.future);
        return *this;
    }

    void SetValue(std::string value);

    std::future<std::string>& GetFuture();

private:
    std::promise<std::string> promise;
    std::future<std::string> future = promise.get_future();
};

#endif // RESPONSE_FUTURE_HPP
