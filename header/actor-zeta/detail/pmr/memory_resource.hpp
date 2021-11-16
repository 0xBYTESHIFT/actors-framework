#pragma once

#include <actor-zeta/config.hpp>
#include <cstddef>
#include <cstdio>

#if __has_include(<memory_resource>)
#include <memory_resource>
#elif __has_include(<experimental/memory_resource>)
#include <experimental/memory_resource>
#endif

namespace actor_zeta::detail::pmr {

#if __has_include(<memory_resource>)
    using memory_resource = std::pmr::memory_resource;

#else
    using memory_resource = std::experimental::pmr::memory_resource;

#endif

} // namespace actor_zeta::detail::pmr
