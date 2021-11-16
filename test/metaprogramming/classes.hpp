#pragma once

#include <actors-framework/detail/type_list.hpp>
#include <cstdint>
#include <string>
#include <type_traits>

using std::is_same;
using namespace actors_framework::type_traits;
using l1 = type_list<int, float, std::string>;
using r1 = type_list<std::string, float, int>;
