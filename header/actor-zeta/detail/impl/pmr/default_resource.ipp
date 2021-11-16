#pragma once

#include <actor-zeta/detail/aligned_allocate.hpp>
#include <actor-zeta/detail/pmr/default_resource.hpp>

namespace actor_zeta::detail::pmr {

#if __has_include(<memory_resource>)

    memory_resource* get_default_resource() noexcept {
        return std::pmr::get_default_resource();
    }

    memory_resource* set_default_resource(memory_resource* new_res) noexcept {
        return std::pmr::set_default_resource(new_res);
    }

    memory_resource* new_delete_resource() noexcept {
        return std::pmr::new_delete_resource();
    }

    memory_resource* null_memory_resource() noexcept {
        return std::pmr::null_memory_resource();
    }

#else

    memory_resource* get_default_resource() noexcept {
        return std::experimental::pmr::get_default_resource();
    }

    memory_resource* set_default_resource(memory_resource* new_res) noexcept {
        return std::experimental::pmr::set_default_resource(new_res);
    }

    memory_resource* new_delete_resource() noexcept {
        return std::experimental::pmr::new_delete_resource();
    }

    memory_resource* null_memory_resource() noexcept {
        return std::experimental::pmr::null_memory_resource();
    }

#endif

} // namespace actor_zeta::detail::pmr
