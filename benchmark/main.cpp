#include <chrono>
#include <iostream>
#include <limits>

#include "case0_to_2.hpp"
#include "case3_to_5.hpp"

#include "classes.hpp"

using namespace std::chrono_literals;

int main(int argc, char* argv[]) {
    std::this_thread::sleep_for(1s); //tracy init
    size_t count = 1024 * 1024;
    size_t max_throughput = std::numeric_limits<size_t>::max();

    if (argc >= 2) {
        count = std::stoull(argv[1]);
    }

    if (argc >= 3) {
        max_throughput = std::stoull(argv[2]);
    }

    std::cout << "count:" << count << " task_size:" << task_size << "\n";
    std::cout << "sizeof(task_stack_lite):" << sizeof(task_stack_lite) << "\n";
    std::cout << "sizeof(task_stack_heavy):" << sizeof(task_stack_heavy) << "\n";
    std::cout << "sizeof(task_heap_lite):" << sizeof(task_heap_lite) << "\n";
    std::cout << "sizeof(task_heap_heavy):" << sizeof(task_heap_heavy) << "\n";
    std::cout << std::endl;

    std::cout << "case0" << std::endl;
    case0_to_2(1, 1, count, 3, max_throughput);
    utils::reset();
    std::cout << std::endl;

    std::cout << "case1" << std::endl;
    case0_to_2(1, 3, count, 5, max_throughput);
    utils::reset();
    std::cout << std::endl;

    std::cout << "case2" << std::endl;
    //std::cout << "skipped\n";
    case0_to_2(3, 1, count, 5, max_throughput);
    utils::reset();
    std::cout << std::endl;

    std::cout << "case3" << std::endl;
    case3_to_5(1, 1, count, 3, max_throughput);
    utils::reset();
    std::cout << std::endl;

    std::cout << "case4" << std::endl;
    case3_to_5(1, 3, count, 5, max_throughput);
    utils::reset();
    std::cout << std::endl;

    std::cout << "case5" << std::endl;
    //std::cout << "skipped\n";
    case3_to_5(3, 1, count, 5, max_throughput);
    utils::reset();
    std::cout << std::endl;
    std::this_thread::sleep_for(1s); //tracy deinit
}
