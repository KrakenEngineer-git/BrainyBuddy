#include "ResponseFuture.hpp"

    void ResponseFuture::SetValue(std::string value) {
        promise.set_value(std::move(value));
    }

    std::future<std::string>& ResponseFuture::GetFuture() {
        return future;
    }