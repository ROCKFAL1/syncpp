#include <iostream>
#include <thread>
#include <string>
#include <stack>

#include <syncpp/channel.hpp>

using namespace syncpp;

int main(int, char**) {
    channel<std::string> chann;

    std::thread thr1([&] {
        for(int i = 1; i <= 100; i++)
            chann << std::to_string(i);
    });
    std::thread thr2([&] {
        for(int i = 1; i <= 100; i++)
            std::cout << chann.waiting_pop() << std::endl;
        
    });

    thr1.join();
    thr2.join();
}
