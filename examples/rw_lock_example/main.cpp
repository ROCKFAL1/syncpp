#include <iostream>
#include <thread>
#include <vector>

#include <syncpp/rw_lock.hpp>

using namespace syncpp;
using namespace std::chrono_literals;

rw_lock<int> rw(0);

void read() {
    
    rw.read([](const int& val) { std::this_thread::sleep_for(1s); std::cout << "read: " << val << std::endl;});
}

void inc() {
    rw.write([](int& val) { std::this_thread::sleep_for(5s); std::cout << "inc: " << ++val << std::endl; }); 
}

std::vector<std::thread> thr_pool;

int main() {
    thr_pool.emplace_back(read);
    thr_pool.emplace_back(read);
    thr_pool.emplace_back(inc);
    thr_pool.emplace_back(read);
    thr_pool.emplace_back(read);
    thr_pool.emplace_back(read);

    for(auto& thr : thr_pool) {
        thr.join();
    }
    
    return 0;
}