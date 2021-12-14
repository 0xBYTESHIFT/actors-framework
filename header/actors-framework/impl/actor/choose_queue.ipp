#pragma once

#ifdef USE_CAF_LOCKFREE_QUEUE
#include <actors-framework/impl/actor/cooperative_actor_CAF_lockfree.ipp>
#else

#ifdef USE_SIMPLE_QUEUE
#include <actors-framework/impl/actor/cooperative_actor_simple.ipp>
#else

#ifdef USE_MOODYCAMEL_LOCKFREE_QUEUE
#include <actors-framework/impl/actor/cooperative_actor_moodycamel_lockfree.ipp>
#else

#include <actors-framework/impl/actor/cooperative_actor_simple.ipp>
#pragma message("queue not selected, using simple queue")

#endif
#endif
#endif
