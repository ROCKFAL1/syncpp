#pragma once
#include <type_traits>
#include <condition_variable>
#include <queue>
#include <optional>
#include <numeric>

#include "details/scope_action.hpp"

namespace syncpp {

namespace priv {

//CRTP-style channel interface
template<typename T,
    typename Channel>
class basic_channel {

public:
    using value_type      = T;
    using reference       = T&;  
    using pointer         = T*;
    using const_reference = const T&;
    using const_pointer   = const T*;

    void waiting_push(const_reference value) noexcept {
        static_cast<Channel*>(this)->waiting_push_impl(value);
    } 

    [[nodiscard]] bool push(const_reference value) noexcept {
        return static_cast<Channel*>(this)->push_impl(value);
    } 
   
    value_type waiting_pop() noexcept {
        return static_cast<Channel*>(this)->waiting_pop_impl();
    } 

    std::optional<value_type> pop() noexcept { 
        return static_cast<Channel*>(this)->pop_impl();
    }

    Channel& operator>> (reference out_val) noexcept {
        std::swap(out_val, waiting_pop());
        return *static_cast<Channel*>(this);
    }

    Channel& operator<< (const_reference value) noexcept {
        waiting_push(value);
        return *static_cast<Channel*>(this);
    }

}; //class basic_channel

} //namespace priv

template<typename T,
    size_t BufferSize = std::numeric_limits<size_t>::max(),
    typename Container = std::queue<T>>
class channel
    : public priv::basic_channel<T, channel<T, BufferSize, Container>> {

public:
    using value_type      = T;
    using reference       = T&;  
    using pointer         = T*;
    using const_reference = const T&;
    using const_pointer   = const T*;
    using container_type  = Container;

    static_assert(std::is_same<value_type, typename container_type::value_type>::value,
                    "Container::value_type and T must be equal");

    static_assert(BufferSize != 0,
        "BufferSize must be greater than 0");
    
    channel() noexcept = default;

    constexpr static size_t buffer_max_size() { return BufferSize; }

    size_t size() const {
        std::scoped_lock _(_mutex);    
        return _container.size();
    }

    bool is_empty() const {
        return size() == 0;
    }

private:
    size_t size_not_safe() const { return _container.size(); }

    void waiting_push_impl(const_reference value) noexcept {
        details::scope_action _([&] { _cv.notify_one(); });  
        std::unique_lock ul(_mutex);
        if(size_not_safe() >= buffer_max_size()) 
            _cv.wait(ul, [&] { return size_not_safe() < buffer_max_size(); });
        
        _container.push(value);     
        // Because of details::scope_action and std::unique_lock, after return we get something like:
        // _mutex.unlock();
        // _cv.notify_one();     
    }

    [[nodiscard]] bool push_impl(const_reference value) noexcept {
        details::scope_action _([&] { _cv.notify_one(); });  
        std::scoped_lock sl(_mutex);
        if(size_not_safe() >= buffer_max_size()) return false;

        _container.push(value);
        return true;
        // Because of details::scope_action and std::unique_lock, after return we get something like:
        // _mutex.unlock();
        // _cv.notify_one();
    }

    value_type waiting_pop_impl() noexcept {       
        details::scope_action _([&] { _cv.notify_one(); });   
        std::unique_lock ul(_mutex);              
        if(size_not_safe() == 0) 
            _cv.wait(ul, [&] { return size_not_safe() > 0; });
        
        details::scope_action pop_on_exit([&] { _container.pop(); });
        return _container.front();
        // Because of details::scope_action and std::unique_lock, after return we get something like:
        // _container.pop();
        // _mutex.unlock();
        // _cv.notify_one();
    }

    std::optional<value_type> pop_impl() noexcept {  
        details::scope_action _([&] { _cv.notify_one(); });   
        std::scoped_lock sl(_mutex); 
        if(size_not_safe() == 0) return std::nullopt;

        details::scope_action pop_on_exit([&] { _container.pop(); });
        return _container.front();
        // Because of details::scope_action and std::unique_lock, after return we get something like:
        // _container.pop();
        // _mutex.unlock();
        // _cv.notify_one();
    }

    container_type          _container;
    std::condition_variable _cv;
    mutable std::mutex      _mutex;
    
    friend class priv::basic_channel<T, channel<T, BufferSize, Container>>;

}; //class channel

template<typename T>
class channel<T, 1>
    : public priv::basic_channel<T, channel<T, 1>> {

public:
    using value_type      = T;
    using reference       = T&;  
    using pointer         = T*;
    using const_reference = const T&;
    using const_pointer   = const T*;
    
    channel() noexcept = default;

    bool is_empty() const {
        std::scoped_lock _(_mutex);   
        return !_value.has_value();
    }

private:
    size_t is_empty_not_safe() const { return !_value.has_value(); }

    void waiting_push_impl(const_reference value) noexcept {
        details::scope_action _([&] { _cv.notify_one(); });  
        std::unique_lock ul(_mutex);
        if(!is_empty_not_safe()) 
            _cv.wait(ul, [&] { return is_empty_not_safe(); });
        
        _value = value;    
        // Because of details::scope_action and std::unique_lock, after return we get something like:
        // _mutex.unlock();
        // _cv.notify_one();  
    }

    [[nodiscard]] bool push_impl(const_reference value) noexcept {
        details::scope_action _([&] { _cv.notify_one(); });  
        std::scoped_lock sl(_mutex);
        if(!is_empty_not_safe()) return false;

        _value = value;    
        return true;
        // Because of details::scope_action and std::unique_lock, after return we get something like:
        // _mutex.unlock();
        // _cv.notify_one();
    }

    value_type waiting_pop_impl() noexcept {       
        details::scope_action _([&] { _cv.notify_one(); });   
        std::unique_lock ul(_mutex);      
        details::scope_action reset([&] { _value.reset(); });          
        if(is_empty_not_safe()) 
            _cv.wait(ul, [&] { return !is_empty_not_safe(); });
        
        return _value.value();
        // Because of details::scope_action and std::unique_lock, after return we get something like:
        // _value.reset();
        // _mutex.unlock();
        // _cv.notify_one();
    }

    std::optional<value_type> pop_impl() noexcept {  
        details::scope_action _([&] { _cv.notify_one(); });   
        std::scoped_lock sl(_mutex); 
        details::scope_action reset([&] { _value.reset(); });   
        return _value;
        // Because of details::scope_action and std::unique_lock, after return we get something like:
        // _value.reset();
        // _mutex.unlock();
        // _cv.notify_one();
    }

    std::optional<value_type>   _value;
    std::condition_variable     _cv;
    mutable std::mutex          _mutex;
    
    friend class priv::basic_channel<T, channel<T, 1>>;

}; //class channel

} //namespace syncpp