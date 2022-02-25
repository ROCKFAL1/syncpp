#pragma once 

#include <mutex>
#include <type_traits>
#include <optional>

namespace sync {

    template<typename T>
    class storing_mutex final {
    public:

        /**
         * @brief Construct a new without data. If there is no content in this class,
         * then when you try to get a value from this mutex, 
         * you will get std::nullopt. In order to put an object, 
         * you can use 'storing_mutex::reset()' and its overloads
         */
        storing_mutex() = default;

        /**
         * @brief Construct a new storing mutex object
         * 
         * @param data will be copied into 'storing_mutex' object
         */
        storing_mutex(T& data) : _data(data) {} 

        /**
         * @brief Construct a new storing mutex object
         * 
         * @param data will be moved into 'storing_mutex' object
         */
        storing_mutex(T&& data) : _data(data) {} 

        /**
         * @brief Construct a new storing mutex object
         * 
         * @tparam Args is arguments for constructing an object of type 'T'
         */
        template<typename ...Args>
        storing_mutex(Args&&... args) : _data(T(std::forward<Args>(args)...)) {}

        storing_mutex(const storing_mutex&) = delete;
        storing_mutex& operator=(const storing_mutex&) = delete;
        storing_mutex(const storing_mutex&&) = delete;
        storing_mutex& operator=(const storing_mutex&&) = delete;

        template<typename F>
        std::invoke_result_t<F&&, std::optional<T>&> locked(F&& fn) {
            std::scoped_lock lock(_mutex);
            return std::invoke(std::forward<F>(fn), _data);
        }

        template<typename F>
        std::invoke_result_t<F&&, std::optional<T>*> locked(F&& fn) {
            std::scoped_lock lock(_mutex);
            return std::invoke(std::forward<F>(fn), &_data);
        }

        template<typename F>
        std::invoke_result_t<F&&, std::optional<T>> copied(F&& fn) const {
            return std::invoke(std::forward<F>(fn), _data);
        }
      
        void reset() {
            std::scoped_lock lock(_mutex);
            _data.reset();
        }

        void reset(T& data) {
            std::scoped_lock lock(_mutex);
            _data = data;
        }

        void reset(T&& data) {
            std::scoped_lock lock(_mutex);
            _data = std::forward<T>(data);
        }

        template<typename ...Args>
        void reset(Args&&... args) {
            std::scoped_lock lock(_mutex);
            _data = T(std::forward<Args>(args)...);
        }

        const std::optional<T>& data() {
            std::scoped_lock lock(_mutex);
            return _data;
        }

    private:
        std::optional<T> _data;
        std::mutex _mutex;
    };

} //namespace sync