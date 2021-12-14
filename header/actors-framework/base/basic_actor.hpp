#pragma once

#include <actors-framework/base/actor.hpp>
#include <actors-framework/base/choose_queue.hpp>

namespace actors_framework::base {

    ///
    /// @brief A generic actor
    ///

    using basic_async_actor =
#ifdef USE_CAF_LOCKFREE_QUEUE
        cooperative_actor_CAF_lockfree;
#pragma message("queue CAF selected")
#else

#ifdef USE_MOODYCAMEL_LOCKFREE_QUEUE
        cooperative_actor_moodycamel_lockfree;
#pragma message("queue MOODYCAMEL selected")
#else

#ifdef USE_SIMPLE_QUEUE
        cooperative_actor_simple;
#pragma message("queue SIMPLE selected")
#else

        cooperative_actor_simple;
#pragma message("queue not selected, using simple queue")
#endif
#endif
#endif

} // namespace actors_framework::base
