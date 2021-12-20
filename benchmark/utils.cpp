#include <iostream>

#include "utils.h"

namespace utils {
    void reset() {
        ZoneScoped;
        actor1::count_stack_lite = 0;
        actor1::count_stack_heavy = 0;
        actor1::count_heap_lite = 0;
        actor1::count_heap_heavy = 0;
        actor1::time_ns_stack_lite = 0;
        actor1::time_ns_stack_heavy = 0;
        actor1::time_ns_heap_lite = 0;
        actor1::time_ns_heap_heavy = 0;
    }

    void print_counts() {
        ZoneScoped;
        std::cout << "slite: " << actor1::count_stack_lite << "\n";
        std::cout << "sheavy: " << actor1::count_stack_heavy << "\n";
        std::cout << "hlite: " << actor1::count_heap_lite << "\n";
        std::cout << "hheavy: " << actor1::count_heap_heavy << "\n";
    }

}; // namespace utils
