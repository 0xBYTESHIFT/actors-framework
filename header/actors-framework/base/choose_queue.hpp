#pragma once

#ifdef USE_CAF_LOCKFREE_QUEUE
#include <actors-framework/base/cooperative_actor_CAF_lockfree.hpp>
#else

#ifdef USE_SIMPLE_QUEUE
#include <actors-framework/base/cooperative_actor_simple.hpp>
#else

#ifdef USE_MOODYCAMEL_LOCKFREE_QUEUE
#include <actors-framework/base/cooperative_actor_moodycamel_lockfree.hpp>
#else

#include <actors-framework/base/cooperative_actor_simple.hpp>
#pragma message("queue not selected, using simple queue")

#endif
#endif
#endif
