#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include <algorithm>
#include <limits>
#include <numeric>

#include "classes.hpp"

TEST_CASE("resend_order") {
    ZoneScoped;

    using namespace std::chrono_literals;
    constexpr size_t count = 1024;
    constexpr size_t max_queue = std::numeric_limits<size_t>::max();

    af::supervisor supervisor0(new supervisor_lite("supervisor0", 2, max_queue));

    af::send(supervisor0, af::address_t::empty_address(), "create_actor0");
    af::send(supervisor0, af::address_t::empty_address(), "create_actor1");

    std::this_thread::sleep_for(100ms); //wait for linking to happen

    for (size_t i = 0; i < count; ++i) {
        af::delegate_send(supervisor0, actor0::get_name(), names::actor0::send_task_name);
    }

    while (true) {
        const int datas_size = actor1::datas_size.load();

        if (datas_size == count) {
            break;
        }
        std::this_thread::sleep_for(10ms);
    }

    std::vector<size_t> desired(count, 0);
    std::iota(desired.begin(), desired.end(), 0);

    auto& result = actor1::datas;
    REQUIRE(std::equal(result.begin(), result.end(), desired.begin()));
}
