#pragma once

#include "actors-framework/base/address.hpp"
#include "actors-framework/forwards.hpp"

namespace actors_framework {

    namespace detail {

    } // namespace detail

    template<
        class ChildrenSupervisor,
        class... Args,
        class = type_traits::enable_if_t<std::is_base_of<base::supervisor_abstract, ChildrenSupervisor>::value>>
    auto spawn_supervisor(Args&&... args) -> base::supervisor {
        return base::supervisor(new ChildrenSupervisor(std::forward<Args>(args)...));
    }

} // namespace actors_framework
