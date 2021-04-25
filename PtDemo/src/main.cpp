#include "pt/Core/Logger.hpp"
#include <thread>
#include <chrono>

int main() {
    pt::Logger::get().initialize();

    auto t1 = std::thread([]() {
        for (int i = 0; i < 3; i++) {
            PT_LOG_ERROR("i = {}", i);
        }
    });
    auto t2 = std::thread([]() {
        for (int i = 0; i < 3; i++) {
            PT_LOG_WARNING("i = {}", i);
        }
    });
    auto t3 = std::thread([]() {
        for (int i = 0; i < 3; i++) {
            PT_LOG_INFO("i = {}", i);
        }
    });
    auto t4 = std::thread([]() {
        for (int i = 0; i < 3; i++) {
            PT_LOG_TRACE("i = {}", i);
        }
    });
    auto t5 = std::thread([]() {
        for (int i = 0; i < 3; i++) {
            PT_ASSERT(i == 0);
        }
    });

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();

    return 0;
}