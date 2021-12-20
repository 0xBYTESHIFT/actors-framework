#pragma once
#include <chrono>
#include <iostream>

#include "classes.hpp"
#include "utils.h"

using namespace std::chrono_literals;

void case3_to_5(size_t actors0_count, size_t actors1_count, size_t count, size_t workers, size_t max_queue) {
    ZoneScoped;
    const auto time0 = std::chrono::steady_clock::now();

    af::supervisor supervisor0(new supervisor_lite("supervisor0", workers, max_queue));
    af::supervisor supervisor1(new supervisor_lite("supervisor1", workers, max_queue));
    af::link(supervisor0, supervisor1);
    std::this_thread::sleep_for(100ms); //wait for linking to happen

    for (size_t i = 0; i < actors0_count; i++) {
        af::send(supervisor0, af::address_t::empty_address(), "create_actor0", i);
    }
    for (size_t i = 0; i < actors1_count; i++) {
        af::send(supervisor1, af::address_t::empty_address(), "create_actor1", i);
    }
    std::this_thread::sleep_for(100ms); //wait for linking to happen

    for (size_t i = 0; i < count; ++i) {
        const auto id = i % actors0_count;
        af::delegate_send(supervisor0, actor0::get_name(id), names::actor0::send_stack_lite_name);
    }
    for (size_t i = 0; i < count; ++i) {
        const auto id = i % actors0_count;
        af::delegate_send(supervisor0, actor0::get_name(id), names::actor0::send_stack_heavy_name);
    }
    for (size_t i = 0; i < count; ++i) {
        const auto id = i % actors0_count;
        af::delegate_send(supervisor0, actor0::get_name(id), names::actor0::send_heap_lite_name);
    }
    for (size_t i = 0; i < count; ++i) {
        const auto id = i % actors0_count;
        af::delegate_send(supervisor0, actor0::get_name(id), names::actor0::send_heap_heavy_name);
    }

    while (true) {
        const auto slite = actor1::count_stack_lite.load(std::memory_order_acquire);
        const auto sheavy = actor1::count_stack_heavy.load(std::memory_order_acquire);
        const auto hlite = actor1::count_heap_lite.load(std::memory_order_acquire);
        const auto hheavy = actor1::count_heap_heavy.load(std::memory_order_acquire);
        utils::print_counts();

        if (slite == count and
            sheavy == count and
            hlite == count and
            hheavy == count) {
            break;
        }
        std::this_thread::sleep_for(10ms);
    }
    const auto time1 = std::chrono::steady_clock::now();
    const auto diff = std::chrono::duration_cast<std::chrono::milliseconds>(time1 - time0).count();

    std::cout << "overall ms:" << diff << "\n";

    std::cout << "stack_lite ns:" << actor1::time_ns_stack_lite << "\n";
    std::cout << "stack_heavy ns:" << actor1::time_ns_stack_heavy << "\n";
    std::cout << "heap_lite ns:" << actor1::time_ns_heap_lite << "\n";
    std::cout << "heap_heavy ns:" << actor1::time_ns_heap_heavy << "\n";
}
