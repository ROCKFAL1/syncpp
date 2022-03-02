#pragma once
#include <functional>

namespace syncpp { namespace priv {

/**
 * @brief This is a simple utility class to perform an action on an object when exiting the scope
 * 
 * @tparam T is the type of object to perform an action on
 */
template<typename T>
class scoped_action final {
public:
    scoped_action(T& ref, std::function<void(T&)> on_exit) : _ref(ref), _on_exit(on_exit) {}

    ~scoped_action() {
        _on_exit(_ref);
    }

private:
    T& _ref;
    std::function<void(T&)> _on_exit;       
};

} //namespace priv
} //namespace syncpp