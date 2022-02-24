#include <iostream>
#include <thread>
#include <vector>

#include <sync/storing_mutex.hpp>

using namespace sync;

constexpr int THR_COUNT = 100;

storing_mutex integer(1);

void cout_and_inc() {
    integer.locked([](std::optional<int>& val) { std::cout << val.value()++ << std::endl; });
}

int main() {
    std::vector<std::thread> thr_pool;
    std::cout << "Start!" << std::endl;
    for(size_t i = 0; i < THR_COUNT; ++i) {
        thr_pool.emplace_back([&]() { cout_and_inc(); });
    }

    for(size_t i = 0; i < THR_COUNT; ++i) {
        thr_pool[i].join();
    }
    std::cout << "Finish!" << std::endl;
}