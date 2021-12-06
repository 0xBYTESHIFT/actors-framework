#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>
#include <chrono>

#include "classes.hpp"

TEST_CASE("spawn") {
    using namespace std::chrono_literals;

    af::supervisor supervisor1(new supervisor_lite("manager-1"));
    af::supervisor supervisor2(new supervisor_lite("manager-2"));
    std::cout << "linking manager-1 and manager-2 called in main" << std::endl;
    af::link(supervisor1, supervisor2);
    std::cout << "linking manager-1 and manager-2 done in main\n"
              << std::endl;

    {
        REQUIRE(worker_t::count == 0);
        std::cout << "create_worker called in main" << std::endl;
        af::send(supervisor1, af::address_t::empty_address(), "create_worker"); //creates bot1
        std::this_thread::sleep_for(100ms);
        REQUIRE(worker_t::count == 1);
        std::cout << "create_worker done in main\n"
                  << std::endl;
    }

    std::cout << "sup1 name: " << supervisor1->address().type()
              << " sup2 name: " << supervisor2->address().type() << std::endl;
    std::cout << "delegate_send called in main" << std::endl;
    af::delegate_send(supervisor1, worker_t::name, "add_link", supervisor2->address()); //link bot1 with manager-2
    std::this_thread::sleep_for(100ms);
    std::cout << "delegate_send done in main\n"
              << std::endl;

    {
        REQUIRE(worker_t2::count == 0);
        std::cout << "create_worker2 called in main" << std::endl;
        af::delegate_send(supervisor1, worker_t::name, worker_t::spawn_bot_name); //creates bot2
        std::this_thread::sleep_for(100ms);
        REQUIRE(worker_t2::count == 1);
        std::cout << "create_worker2 done in main\n"
                  << std::endl;
    }

    //check that bot1 is explicitly linked to bot2 after spawning of bot2
    {
        const int counts = 10;
        REQUIRE(worker_t2::callback_called == 0);

        for (int i = 0; i < counts; ++i) {
            af::delegate_send(supervisor1, worker_t::name, worker_t::ping_bot2_name); //calles callback in bot2 from bot1
        }
        std::this_thread::sleep_for(100ms);
        REQUIRE(worker_t2::callback_called == counts);
    }
}
