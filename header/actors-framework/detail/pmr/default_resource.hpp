#pragma once

#include <actors-framework/config.hpp>
#include <actors-framework/detail/pmr/memory_resource.hpp>
#include <atomic>
#include <cassert>
#include <memory>
#include <new>

namespace actors_framework::detail::pmr {

#if __has_include(<memory_resource>)

    memory_resource* get_default_resource() noexcept;

    memory_resource* set_default_resource(memory_resource* new_res) noexcept;

    memory_resource* new_delete_resource() noexcept;

    memory_resource* null_memory_resource() noexcept;

#else

    memory_resource* get_default_resource() noexcept;

    memory_resource* set_default_resource(memory_resource* new_res) noexcept;

    memory_resource* new_delete_resource() noexcept;

    memory_resource* null_memory_resource() noexcept;

#endif

} // namespace actors_framework::detail::pmr
