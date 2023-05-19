// DelayTask.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include <chrono>
#include <cstdint>  // uint64_t
#include <future>  // async
#include <thread>  // this_thread

template <typename F, typename... Args>
auto timed_run(const std::uint64_t delay_ms, F&& f, Args&&... args) {
    return std::async(std::launch::async, [=]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(delay_ms));
        f(args...);
    });
}

int main() {
    using namespace std::chrono_literals;
    auto print_dots = []() {
        for (int i{ 4 }; i > 0; --i) {
            std::this_thread::sleep_for(10ms);
            printf(".\n");
        }
    };
    bool is_connect = true;
    auto print_square = [=](int n) { printf("{%d}\n", n * n); printf("%d\n", is_connect); };
    auto f1{ timed_run(0, print_dots) };
    auto f2{ timed_run(2000, print_square, 2) };
    //f1.get();
    //f2.get();
    getchar();
    return 0;
}
