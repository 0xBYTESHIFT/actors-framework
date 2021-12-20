#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include <algorithm>
#include <chrono>
#include <limits>
#include <numeric>

#include "classes.hpp"

using namespace std::chrono_literals;

void helper_0(size_t actors0_count, size_t actors1_count, size_t count, size_t workers, size_t max_queue = std::numeric_limits<size_t>::max()) {
    ZoneScoped;
    af::supervisor supervisor0(new supervisor_lite("supervisor0", workers, max_queue));
    for (size_t i = 0; i < actors0_count; i++) {
        af::send(supervisor0, af::address_t::empty_address(), "create_actor0", i);
    }
    for (size_t i = 0; i < actors1_count; i++) {
        af::send(supervisor0, af::address_t::empty_address(), "create_actor1", i);
    }
    std::this_thread::sleep_for(100ms); //wait for linking to happen

    for (size_t i = 0; i < count; ++i) {
        const auto id = i % actors0_count;
        af::delegate_send(supervisor0, actor0::get_name(id), names::actor0::send_task_name);
    }

    size_t iterations = 0;

    while (true) {
        const size_t tasks = actor1::count_task.load(std::memory_order_acquire);

        if (tasks == count) {
            break;
        }
        std::this_thread::sleep_for(10ms);
        if (iterations > count * 100) {
            break; //means failure
        }
        ++iterations;
    }
}

void helper_1(size_t actors0_count, size_t actors1_count, size_t count, size_t workers, size_t max_queue = std::numeric_limits<size_t>::max()) {
    ZoneScoped;
    af::supervisor supervisor0(new supervisor_lite("supervisor0", workers, max_queue));
    af::supervisor supervisor1(new supervisor_lite("supervisor1", workers, max_queue));
    af::link(supervisor0, supervisor1);
    std::this_thread::sleep_for(100ms); //wait for linking to happen

    for (size_t i = 0; i < actors0_count; i++) {
        af::send(supervisor0, af::address_t::empty_address(), "create_actor0", (size_t) i);
    }
    for (size_t i = 0; i < actors1_count; i++) {
        af::send(supervisor1, af::address_t::empty_address(), "create_actor1", (size_t) i);
    }
    std::this_thread::sleep_for(100ms); //wait for linking to happen

    for (size_t i = 0; i < count; ++i) {
        const auto id = i % actors0_count;
        af::delegate_send(supervisor0, actor0::get_name(id), names::actor0::send_task_name);
    }

    size_t iterations = 0;

    while (true) {
        const size_t tasks = actor1::count_task.load(std::memory_order_acquire);

        if (tasks == count) {
            break;
        }
        std::this_thread::sleep_for(10ms);
        if (iterations > count * 100) {
            break; //means failure
        }
        ++iterations;
    }
}

TEST_CASE("send_1_to_1_common_supervisor") {
    ZoneScoped;
    size_t count = 1000;
    helper_0(1, 1, count, 3);

    auto tasks = actor1::count_task.load(std::memory_order_acquire);
    REQUIRE(tasks == count);
}

TEST_CASE("send_1_to_N_common_supervisor") {
    ZoneScoped;
    size_t count = 1000;
    helper_0(1, 10, count, 12);

    auto tasks = actor1::count_task.load(std::memory_order_acquire);
    REQUIRE(tasks == count);
}

TEST_CASE("send_N_to_1_common_supervisor") {
    ZoneScoped;
    size_t count = 1000;
    helper_0(10, 1, count, 12);

    auto tasks = actor1::count_task.load(std::memory_order_acquire);
    REQUIRE(tasks == count);
}

TEST_CASE("send_N_to_N_common_supervisor") {
    ZoneScoped;
    size_t count = 1000;
    helper_0(10, 10, count, 21);

    auto tasks = actor1::count_task.load(std::memory_order_acquire);
    REQUIRE(tasks == count);
}

TEST_CASE("send_1_to_1_diff_supervisor") {
    ZoneScoped;
    size_t count = 1000;
    helper_1(1, 1, count, 2);

    auto tasks = actor1::count_task.load(std::memory_order_acquire);
    REQUIRE(tasks == count);
}

TEST_CASE("send_1_to_N_diff_supervisor") {
    ZoneScoped;
    size_t count = 1000;
    helper_1(1, 10, count, 12);

    auto tasks = actor1::count_task.load(std::memory_order_acquire);
    REQUIRE(tasks == count);
}

TEST_CASE("send_N_to_1_diff_supervisor") {
    ZoneScoped;
    size_t count = 1000;
    helper_1(10, 1, count, 12);

    auto tasks = actor1::count_task.load(std::memory_order_acquire);
    REQUIRE(tasks == count);
}

TEST_CASE("send_N_to_N_diff_supervisor") {
    ZoneScoped;
    size_t count = 1000;
    helper_1(10, 10, count, 21);

    auto tasks = actor1::count_task.load(std::memory_order_acquire);
    REQUIRE(tasks == count);
}
