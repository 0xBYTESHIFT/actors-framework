#include <chrono>
#include <iostream>

#include "case0.hpp"
#include "case1.hpp"
#include "case2.hpp"

#include "classes.hpp"

using namespace std::chrono_literals;
static constexpr size_t count = 1024 * 100;

void reset() {
    actor1::count_stack_lite = 0;
    actor1::count_stack_heavy = 0;
    actor1::count_heap_lite = 0;
    actor1::count_heap_heavy = 0;
    actor1::time_ns_stack_lite = 0;
    actor1::time_ns_stack_heavy = 0;
    actor1::time_ns_heap_lite = 0;
    actor1::time_ns_heap_heavy = 0;
}

int main() {
    std::cout << "count:" << count << " task_size:" << task_size << "\n";
    std::cout << "sizeof(task_stack_lite):" << sizeof(task_stack_lite) << "\n";
    std::cout << "sizeof(task_stack_heavy):" << sizeof(task_stack_heavy) << "\n";
    std::cout << "sizeof(task_heap_lite):" << sizeof(task_heap_lite) << "\n";
    std::cout << "sizeof(task_heap_heavy):" << sizeof(task_heap_heavy) << "\n";
    std::cout << std::endl;

    case0(count);
    reset();
    std::cout << std::endl;

    case1(count);
    reset();
    std::cout << std::endl;

    /*
    case2(count);
    reset();
    std::cout << std::endl;
    */
}
