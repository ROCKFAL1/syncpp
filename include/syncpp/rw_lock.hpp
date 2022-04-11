#pragma once
#include <shared_mutex>
#include <functional>
#include <type_traits>

namespace syncpp {

template<typename T> 
class rw_lock final {
public:

    rw_lock() noexcept = default;
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

    template<size_t N>
    std::tuple_element_t<N, rw_lock<T>>& get() {
        if      constexpr (N == 0) return _data;
        else if constexpr (N == 1) return _shared_mutex;
    }

private:
    T                 _data;
    std::shared_mutex _shared_mutex;
};

} //namespace syncpp

namespace std {

template<typename T>
struct tuple_size<::syncpp::rw_lock<T>>
    : integral_constant<std::size_t, 2> {};

template<typename T>
struct tuple_element<0, ::syncpp::rw_lock<T>> { using type = T&; };

template<typename T>
struct tuple_element<1, ::syncpp::rw_lock<T>> { using type = std::shared_mutex&; };
    
} //namespace std