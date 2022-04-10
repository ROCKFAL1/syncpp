#pragma once 

#include <mutex>
#include <type_traits>

namespace syncpp {

template<typename T>
class storing_mutex final {
public:

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

private:
    T _data;
    std::mutex _mutex;
};

} //namespace syncpp