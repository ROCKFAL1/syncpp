#pragma once
#include <shared_mutex>
#include <functional>

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
        shared_mutex.lock_shared();
        priv::scoped_action<std::shared_mutex> on_exit(shared_mutex, [](std::shared_mutex& sm) { sm.unlock_shared(); });         
        return std::invoke(std::forward<F>(fn), _data);         
    } 

    template<typename F>
    std::invoke_result_t<F&&, T&> write(F&& fn) noexcept {
        shared_mutex.lock();
        priv::scoped_action<std::shared_mutex> on_exit(shared_mutex, [](std::shared_mutex& sm) { sm.unlock(); });         
        return std::invoke(std::forward<F>(fn), _data);   
    } 

private:
    T _data;
    std::shared_mutex shared_mutex;
};

} //namespace syncpp