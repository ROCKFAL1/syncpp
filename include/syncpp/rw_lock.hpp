#pragma once
#include <shared_mutex>
#include <functional>
#include <type_traits>

#include "private/scoped_action.hpp"

namespace syncpp {

template<typename T> 
class rw_lock final {
public:

    rw_lock() noexcept { static_assert(std::is_default_constructible_v<T>, "T should have default c-tor" ); }
    rw_lock(const T& data) noexcept : _data(data) {} 
    rw_lock(const T&& data) noexcept : _data(data) {} 

    template<typename ...Args>
    rw_lock(const Args&&... args) noexcept : _data(T(std::forward<Args>(args)...)) {}

    rw_lock(const rw_lock&) = delete;
    rw_lock& operator=(const rw_lock&) = delete;
    rw_lock(const rw_lock&&) = delete;
    rw_lock& operator=(const rw_lock&&) = delete;

    template<typename F>
    std::invoke_result_t<F&&, const T&> read(F&& fn) noexcept {
        std::shared_lock sl(_shared_mutex);
        return std::invoke(std::forward<F>(fn), _data);         
    } 

    template<typename F>
    std::invoke_result_t<F&&, T&> write(F&& fn) noexcept {
        std::scoped_lock sl(_shared_mutex); 
        return std::invoke(std::forward<F>(fn), _data);   
    } 

    T data() noexcept {
        std::scoped_lock sl(_shared_mutex);
        return _data;
    }

private:
    T _data;
    std::shared_mutex _shared_mutex;
};

} //namespace syncpp