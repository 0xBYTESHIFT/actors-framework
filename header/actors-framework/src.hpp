#pragma once

#include <actors-framework.hpp>

#include <actors-framework/detail/impl/pmr/default_resource.ipp>
#include <actors-framework/detail/pmr/polymorphic_allocator.hpp>
#include <cstdint>

#include <actors-framework/impl/actor/actor.ipp>
#include <actors-framework/impl/actor/actor_abstract.ipp>
#include <actors-framework/impl/actor/address.ipp>
#include <actors-framework/impl/actor/communication_module.ipp>
#include <actors-framework/impl/actor/cooperative_actor.ipp>
#include <actors-framework/impl/actor/cooperative_actor_simple.ipp>
#include <actors-framework/impl/actor/supervisor.ipp>
#include <actors-framework/impl/actor/supervisor_abstract.ipp>

#include <actors-framework/impl/detail/aligned_allocate.ipp>
#include <actors-framework/impl/detail/ref_counted.ipp>

#include <actors-framework/impl/executor/abstract_executor.ipp>
#include <actors-framework/impl/executor/executable.ipp>
#include <actors-framework/impl/executor/execution_device.ipp>
#include <actors-framework/impl/executor/work_sharing.ipp>

#include <actors-framework/impl/messaging/message.ipp>

#include <actors-framework/impl/link.ipp>
#include <actors-framework/impl/send.ipp>
