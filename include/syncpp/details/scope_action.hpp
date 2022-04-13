#pragma once 
#include <functional>

namespace syncpp { namespace details {

class scope_action {
public:
    scope_action(std::function<void()>&& on_exit) noexcept : _on_exit(on_exit) {}
    ~scope_action() noexcept(noexcept(_on_exit())) { _on_exit(); }

private:
    std::function<void()> _on_exit;
}; //class scope_action

} } //namespace syncpp::details