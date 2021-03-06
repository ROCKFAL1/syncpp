#pragma once 

#include <mutex>
#include <type_traits>
#include <utility>
#include <tuple>

namespace syncpp {

template<typename T>
class storing_mutex final {
public:
    using value_type      = T;
    using reference       = T&;  
    using pointer         = T*;
    using const_reference = typename const reference;
    using const_pointer   = typename const pointer;

    /**
     * @brief Construct a new with storing mutex object using default T c-tor.
     * If it is not there, then there will be a compilation error. 
     */
    storing_mutex() noexcept = default;

    /**
     * @brief Construct a new storing mutex object
     * 
     * @param data will be copied into 'storing_mutex' object
     */
    storing_mutex(const T& data) noexcept : _data(data) {} 

    /**
     * @brief Construct a new storing mutex object
     * 
     * @param data will be moved into 'storing_mutex' object
     */
    storing_mutex(const T&& data) noexcept : _data(data) {} 

    /**
     * @brief Construct a new storing mutex object
     * 
     * @tparam Args is arguments for constructing an object of type 'T'
     */
    template<typename ...Args>
    storing_mutex(const Args&&... args) noexcept : _data(T(std::forward<Args>(args)...)) {}

    storing_mutex(const storing_mutex&) = delete;
    storing_mutex& operator=(const storing_mutex&) = delete;
    storing_mutex(const storing_mutex&&) = delete;
    storing_mutex& operator=(const storing_mutex&&) = delete;

    template<typename F>
    std::invoke_result_t<F&&, T&> locked(F&& fn) noexcept {
        std::scoped_lock lock(_mutex);
        return std::invoke(std::forward<F>(fn), _data);
    }

    template<typename F>
    std::invoke_result_t<F&&, T*> locked(F&& fn) noexcept {
        std::scoped_lock lock(_mutex);
        return std::invoke(std::forward<F>(fn), &_data);
    }

    template<typename F>
    std::invoke_result_t<F&&, T> copied(F&& fn) noexcept {
        T local_data;
        {
            std::scoped_lock lock(_mutex);
            local_data = _data;
        }
         
        return std::invoke(std::forward<F>(fn), std::move(local_data));
    }

    T data() noexcept {          
        std::scoped_lock lock(_mutex);
        return _data;
    }

    template<size_t N>
    std::tuple_element_t<N, storing_mutex<T>>& get() {
        if      constexpr (N == 0) return _data;
        else if constexpr (N == 1) return _mutex;
    }

private:
    T          _data;
    std::mutex _mutex;
};

} //namespace syncpp

namespace std {

template<typename T>
struct tuple_size<::syncpp::storing_mutex<T>>
    : integral_constant<std::size_t, 2> {};

template<typename T>
struct tuple_element<0, ::syncpp::storing_mutex<T>> { using type = T&; };

template<typename T>
struct tuple_element<1, ::syncpp::storing_mutex<T>> { using type = std::mutex&; };
    
} //namespace std


