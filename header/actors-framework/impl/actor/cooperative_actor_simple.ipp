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

    cooperative_actor_simple::cooperative_actor(supervisor_abstract* supervisor, std::string type)
        : actor_abstract(std::move(type))
        , supervisor_m_(supervisor) {
        flags_(static_cast<int>(state::empty));
    }

    cooperative_actor_simple::~cooperative_actor() {}

    auto cooperative_actor_simple::run(executor::execution_device* e, size_t max_throughput) -> executor::executable_result {
        if (!activate_(e)) {
            return executor::executable_result::done;
        }

        size_t handled_msgs = 0;

        message_ptr ptr;

        while (handled_msgs < max_throughput) {
            ptr = next_message_();
            if (!ptr) {
                return executor::executable_result::awaiting;
            }
            reactivate_(ptr);
            ++handled_msgs;
        }

        return executor::executable_result::awaiting;
    }

    void cooperative_actor_simple::enqueue_base(message_ptr msg, executor::execution_device* e) {
        assert(msg);
        mailbox_().enqueue(std::move(msg));
        if (flags_() == static_cast<int>(state::empty)) {
            intrusive_ptr_add_ref(this);
            if (e != nullptr) {
                context_(e);
                context_()->execute(this);
            } else {
                supervisor_()->executor()->execute(this);
            }
        }
    }

    void cooperative_actor_simple::intrusive_ptr_add_ref_impl() {
        flags_(static_cast<int>(state::busy));
        ref();
    }

    void cooperative_actor_simple::intrusive_ptr_release_impl() {
        flags_(static_cast<int>(state::empty));
        deref();
    }

    auto cooperative_actor_simple::activate_(executor::execution_device* ctx) -> bool {
        //assert(ctx != nullptr);
        if (ctx) {
            context_(ctx);
        }
        return true;
    }

    auto cooperative_actor_simple::reactivate_(message_ptr& x) -> void {
        consume_(x);
    }

    auto cooperative_actor_simple::next_message_() -> message_ptr {
        message_ptr ptr;
        bool status = mailbox_().try_pop(ptr);
        return ptr;
    }

    auto cooperative_actor_simple::has_next_message_() -> bool {
        return mailbox_().count() > 0;
    }

    void cooperative_actor_simple::consume_(message_ptr& x) {
        current_message_m_ = std::move(x);
        execute();
    }

    void cooperative_actor_simple::cleanup_() {}

    auto cooperative_actor_simple::current_message_impl() -> message* {
        return current_message_m_.get();
    }

    auto cooperative_actor_simple::context_() const -> executor::execution_device* {
        return executor_m_;
    }

    void cooperative_actor_simple::context_(executor::execution_device* e) {
        if (e != nullptr) {
            executor_m_ = e;
        }
    }

    auto cooperative_actor_simple::supervisor_() -> supervisor_abstract* {
        return supervisor_m_;
    }

    auto cooperative_actor_simple::flags_() const -> int {
        return flags_m_.load(std::memory_order_relaxed);
    }

    void cooperative_actor_simple::flags_(int new_value) {
        flags_m_.store(new_value, std::memory_order_relaxed);
    }
    auto cooperative_actor_simple::mailbox_() -> mailbox_t& {
        return mailbox_m_;
    }

} // namespace actors_framework::base