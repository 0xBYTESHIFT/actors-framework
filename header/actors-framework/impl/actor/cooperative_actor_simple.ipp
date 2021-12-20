#pragma once
#include <cassert>
#include <iostream>

#include <actors-framework/base/address.hpp>
#include <actors-framework/base/cooperative_actor_simple.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/base/supervisor_abstract.hpp>
#include <actors-framework/executor/abstract_executor.hpp>
#include <actors-framework/executor/execution_device.hpp>

namespace actors_framework::base {

    cooperative_actor::cooperative_actor(supervisor_abstract* supervisor, std::string type)
        : cooperative_actor_base(supervisor, std::move(type)) {
        ZoneScoped;
        flags_(state::empty);
    }

    cooperative_actor::~cooperative_actor() {
        ZoneScoped;
    }

    auto cooperative_actor::run(executor::execution_device* e, size_t max_throughput) -> executor::executable_result {
        ZoneScoped;
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

    void cooperative_actor::enqueue_base(message_ptr msg, executor::execution_device* e) {
        ZoneScoped;
        assert(msg);
        mailbox_().enqueue(std::move(msg));
        if (flags_() == state::empty) {
            intrusive_ptr_add_ref(this);
            if (e != nullptr) {
                context_(e);
                context_()->execute(this);
            } else {
                supervisor_()->executor()->execute(this);
            }
        }
    }

    void cooperative_actor::intrusive_ptr_add_ref_impl() {
        ZoneScoped;
        flags_(state::busy);
        ref();
    }

    void cooperative_actor::intrusive_ptr_release_impl() {
        ZoneScoped;
        flags_(state::empty);
        deref();
    }

    auto cooperative_actor::reactivate_(message_ptr& x) -> void {
        ZoneScoped;
        consume_(x);
    }

    auto cooperative_actor::next_message_() -> message_ptr {
        ZoneScoped;
        message_ptr ptr;
        bool status = mailbox_().try_pop(ptr);
        return ptr;
    }

    auto cooperative_actor::has_next_message_() -> bool {
        ZoneScoped;
        return mailbox_().count() > 0;
    }

    void cooperative_actor::consume_(message_ptr& x) {
        ZoneScoped;
        current_message_m_ = std::move(x);
        execute();
    }

    void cooperative_actor::cleanup_() {}

    auto cooperative_actor::current_message_impl() -> message* {
        return current_message_m_.get();
    }

    auto cooperative_actor::mailbox_() -> mailbox_t& {
        return mailbox_m_;
    }

} // namespace actors_framework::base
