#define CATCH_CONFIG_MAIN // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include "classes.hpp"

TEST_CASE("metaprogramming") {
    SECTION("simple") {
        REQUIRE(std::is_same_v<int, type_list_at_t<l1, 0>>);
        REQUIRE(std::is_same_v<float, type_list_at_t<l1, 1>>);
        REQUIRE(std::is_same_v<std::string, type_list_at_t<l1, 2>>);
        REQUIRE(3u == type_list_size_v<l1>);
        REQUIRE(type_list_size_v<r1> == type_list_size_v<l1>);
        REQUIRE((std::is_same_v<type_list_at_t<l1, 0>, type_list_at_t<r1, 2>>) );
        REQUIRE((std::is_same_v<type_list_at_t<l1, 1>, type_list_at_t<r1, 1>>) );
        REQUIRE((std::is_same_v<type_list_at_t<l1, 2>, type_list_at_t<r1, 0>>) );
    }
}
