// utilities.hpp
#ifndef UTILITIES_HPP
#define UTILITIES_HPP

#include <memory>

/*only needed in c++ less than c++14 workaround for make_unique*/
template<typename T, typename... Args>
std::unique_ptr<T> make_unique(Args&&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

#endif // UTILITIES_HPP
