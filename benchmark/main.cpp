#include <chrono>
#include <iostream>

#include "case0_to_2.hpp"
#include "case3_to_5.hpp"

#include "classes.hpp"

using namespace std::chrono_literals;
static constexpr size_t count = 1024 * 1024;

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

    std::cout << "case0" << std::endl;
    case0_to_2(1, 1, count, 3, count * 10);
    reset();
    std::cout << std::endl;

    std::cout << "case1" << std::endl;
    case0_to_2(1, 3, count, 5, count * 10);
    reset();
    std::cout << std::endl;

    std::cout << "case2" << std::endl;
    std::cout << "skipped\n";
    //case0_to_2(3, 1, count, 5, count * 10);
    reset();
    std::cout << std::endl;

    std::cout << "case3" << std::endl;
    case3_to_5(1, 1, count, 3, count * 10);
    reset();
    std::cout << std::endl;

    std::cout << "case4" << std::endl;
    case3_to_5(1, 3, count, 5, count * 10);
    reset();
    std::cout << std::endl;

    std::cout << "case5" << std::endl;
    std::cout << "skipped\n";
    //case3_to_5(3, 1, count, 5, count * 10);
    reset();
    std::cout << std::endl;
}
