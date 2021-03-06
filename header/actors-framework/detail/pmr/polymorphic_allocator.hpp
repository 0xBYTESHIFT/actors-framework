#pragma once

#include <actors-framework/config.hpp>

#include <cassert>
#include <cstddef>

#include <new>
#include <tuple>
#include <utility>

#if __has_include(<memory_resource>)
#include <memory_resource>
#elif __has_include(<experimental/memory_resource>)
#include <experimental/memory_resource>
#endif

namespace actors_framework::detail::pmr {

#if __has_include(<memory_resource>)

    template<class T>
    using polymorphic_allocator = std::pmr::polymorphic_allocator<T>;

#else

    template<class T>
    using polymorphic_allocator = std::experimental::pmr::polymorphic_allocator<T>;

#endif

} // namespace actors_framework::detail::pmr
