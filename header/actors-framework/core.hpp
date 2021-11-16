#pragma once

#include <actors-framework/base/actor.hpp>
#include <actors-framework/base/actor_abstract.hpp>
#include <actors-framework/base/address.hpp>
#include <actors-framework/base/message.hpp>

#include <actors-framework/base/basic_actor.hpp>
#include <actors-framework/base/supervisor.hpp>
#include <actors-framework/base/supervisor_abstract.hpp>
#include <actors-framework/detail/any.hpp>
#include <actors-framework/detail/pmr/memory_resource.hpp>
#include <actors-framework/executor/abstract_executor.hpp>
#include <actors-framework/executor/executor.hpp>
#include <actors-framework/executor/policy/work_sharing.hpp>

namespace actors_framework {

    using base::actor;
    using base::actor_abstract;
    using base::address_t;
    using base::basic_async_actor;
    using base::make_handler;
    using base::message;
    using base::message_ptr;
    using base::supervisor;
    using base::supervisor_abstract;

    using executor::abstract_executor;
    using executor::executable;
    using executor::execution_device;
    using executor::executor_t;
    using executor::work_sharing;

} // namespace actors_framework
