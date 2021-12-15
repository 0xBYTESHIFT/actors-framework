#pragma once
#include <chrono>
#include <iostream>

#include "classes.hpp"
using namespace std::chrono_literals;

void case0(size_t count) {
    af::supervisor supervisor0(new supervisor_lite("supervisor0", 3, count * 10));
    af::send(supervisor0, af::address_t::empty_address(), "create_actor0", (size_t) 0);
    af::send(supervisor0, af::address_t::empty_address(), "create_actor1", (size_t) 0);
    std::this_thread::sleep_for(100ms); //wait for linking to happen

    for (size_t i = 0; i < count; ++i) {
        af::delegate_send(supervisor0, actor0::get_name(0), names::actor0::send_stack_lite_name);
    }
    for (size_t i = 0; i < count; ++i) {
        af::delegate_send(supervisor0, actor0::get_name(0), names::actor0::send_stack_heavy_name);
    }
    for (size_t i = 0; i < count; ++i) {
        af::delegate_send(supervisor0, actor0::get_name(0), names::actor0::send_heap_lite_name);
    }
    for (size_t i = 0; i < count; ++i) {
        af::delegate_send(supervisor0, actor0::get_name(0), names::actor0::send_heap_heavy_name);
    }

    while (true) {
        int slite = actor1::count_stack_lite.load(std::memory_order_acquire);
        int sheavy = actor1::count_stack_heavy.load(std::memory_order_acquire);
        int hlite = actor1::count_heap_lite.load(std::memory_order_acquire);
        int hheavy = actor1::count_heap_heavy.load(std::memory_order_acquire);

        if (slite == count and
            sheavy == count and
            hlite == count and
            hheavy == count) {
            break;
        }
        std::this_thread::sleep_for(200ms);
    }
    std::cout << "stack_lite ns:" << actor1::time_ns_stack_lite << "\n";
    std::cout << "stack_heavy ns:" << actor1::time_ns_stack_heavy << "\n";
    std::cout << "heap_lite ns:" << actor1::time_ns_heap_lite << "\n";
    std::cout << "heap_heavy ns:" << actor1::time_ns_heap_heavy << "\n";
}
