#pragma once

#include <actors-framework/base/actor.hpp>
#include <actors-framework/base/cooperative_actor.hpp>

namespace actors_framework::base {

    ///
    /// @brief A generic actor
    ///

    using basic_async_actor =
#ifdef USE_LOCKFREE_QUEUE
        cooperative_actor_lockfree;
#else
#ifdef USE_SIMPLE_QUEUE
        cooperative_actor_simple;
#else
        cooperative_actor_simple;
#pragma message("queue not selected, using simple queue")
#endif
#endif

} // namespace actors_framework::base
