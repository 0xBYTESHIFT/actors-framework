#pragma once
#include <cassert>
#include <iostream>

#include <actors-framework/base/address.hpp>
#include <actors-framework/base/cooperative_actor.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/base/supervisor_abstract.hpp>
#include <actors-framework/executor/abstract_executor.hpp>
#include <actors-framework/executor/execution_device.hpp>

namespace actors_framework::base {

    cooperative_actor_base::cooperative_actor_base(supervisor_abstract* supervisor, std::string type)
        : actor_abstract(std::move(type))
        , supervisor_m_(supervisor) {
        flags_(state::empty);
    }

    cooperative_actor_base::~cooperative_actor_base() {}

    auto cooperative_actor_base::activate_(executor::execution_device* ctx) -> bool {
        //assert(ctx != nullptr);
        if (ctx) {
            context_(ctx);
        }
        return true;
    }

    auto cooperative_actor_base::context_() const -> executor::execution_device* {
        return executor_m_;
    }

    void cooperative_actor_base::context_(executor::execution_device* e) {
        if (e != nullptr) {
            executor_m_ = e;
        }
    }

    auto cooperative_actor_base::supervisor_() -> supervisor_abstract* {
        return supervisor_m_;
    }

    auto cooperative_actor_base::flags_() const -> state {
        return flags_m_.load(std::memory_order_relaxed);
    }

    void cooperative_actor_base::flags_(state new_value) {
        flags_m_.store(new_value, std::memory_order_relaxed);
    }
} // namespace actors_framework::base
