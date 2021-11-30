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

    inline void error() {
        std::cerr << " WARNING " << std::endl;
        std::cerr << " WRONG ADDRESS " << std::endl;
        std::cerr << " WARNING " << std::endl;
    }

    cooperative_actor::cooperative_actor(supervisor_abstract* supervisor, std::string type)
        : actor_abstract(std::move(type))
        , supervisor_m_(supervisor) {
        flags_(static_cast<int>(state::empty));
        mailbox_().try_unblock();
    }

    cooperative_actor::~cooperative_actor() {}

    auto cooperative_actor::run(executor::execution_device* e, size_t max_throughput) -> executor::executable_result {
        if (!activate_(e)) {
            return executor::executable_result::done;
        }

        size_t handled_msgs = 0;

        message_ptr ptr;

        while (handled_msgs < max_throughput && !mailbox_().cache().empty()) {
            do {
                ptr = next_message_();
                if (!ptr) {
                    if (mailbox_().try_block()) {
                        return executor::executable_result::awaiting;
                    }
                }
            } while (!ptr);
            consume_from_cache_();
            ++handled_msgs;
        }

        while (handled_msgs < max_throughput) {
            do {
                ptr = next_message_();
                if (!ptr) {
                    if (mailbox_().try_block()) {
                        return executor::executable_result::awaiting;
                    }
                }
            } while (!ptr);
            reactivate_(*ptr);
            ++handled_msgs;
        }

        while (!ptr) {
            ptr = next_message_();
            push_to_cache_(std::move(ptr));
        }

        if (!has_next_message_() && mailbox_().try_block()) {
            return executor::executable_result::awaiting;
        }

        return executor::executable_result::awaiting;
    }

    void cooperative_actor::enqueue_base(message_ptr msg, executor::execution_device* e) {
        assert(msg);
        mailbox_().enqueue(msg.release());
        if (flags_() == static_cast<int>(state::empty)) {
            intrusive_ptr_add_ref(this);
            if (e != nullptr) {
                context_(e);
                context_()->execute(this);
            } else {
                supervisor_()->executor()->execute(this);
            }
        }

        /*
            switch ( mailbox_().enqueue(msg.release())) {
                case detail::enqueue_result::unblocked_reader: {
                    intrusive_ptr_add_ref(this);
                    if (e != nullptr) {
                        context_(e);
                        context_()->execute(this);
                    } else {
                        env().executor().execute(this);
                    }
                    break;
                }
                case detail::enqueue_result::queue_closed: {
                    assert(false);
                    break;
                }
                case detail::enqueue_result::success:
                    break;
            }
             */
    }

    void cooperative_actor::intrusive_ptr_add_ref_impl() {
        flags_(static_cast<int>(state::busy));
        mailbox_().try_block();
        ref();
    }

    void cooperative_actor::intrusive_ptr_release_impl() {
        flags_(static_cast<int>(state::empty));
        mailbox_().try_unblock();
        deref();
    }

    auto cooperative_actor::activate_(executor::execution_device* ctx) -> bool {
        //assert(ctx != nullptr);
        if (ctx) {
            context_(ctx);
        }
        return true;
    }

    auto cooperative_actor::reactivate_(message& x) -> void {
        consume_(x);
    }

    auto cooperative_actor::next_message_() -> message_ptr {
        auto& cache = mailbox_().cache();
        auto i = cache.begin();
        auto e = cache.separator();
        if (i == e || !i->is_high_priority()) {
            auto hp_pos = i;
            auto tmp = mailbox_().try_pop();
            while (tmp != nullptr) {
                cache.insert(tmp->is_high_priority() ? hp_pos : e, tmp);
                if (hp_pos == e && !tmp->is_high_priority())
                    --hp_pos;
                tmp = mailbox_().try_pop();
            }
        }
        message_ptr result;
        i = cache.begin();
        if (i != e)
            result.reset(cache.take(i));
        return result;
    }

    auto cooperative_actor::has_next_message_() -> bool {
        auto& mbox = mailbox_();
        auto& cache = mbox.cache();
        return cache.begin() != cache.separator() || mbox.can_fetch_more();
    }

    void cooperative_actor::push_to_cache_(message_ptr ptr) {
        assert(ptr != nullptr);
        if (!ptr->is_high_priority()) {
            mailbox_().cache().insert(mailbox_().cache().end(), ptr.release());
            return;
        }
        auto high_priority = [](const message& val) {
            return val.is_high_priority();
        };
        auto& cache = mailbox_().cache();
        auto e = cache.end();
        cache.insert(std::partition_point(cache.continuation(), e, high_priority), ptr.release());
    }

    void cooperative_actor::consume_(message& x) {
        current_message_m_ = &x;
        execute();
    }

    bool cooperative_actor::consume_from_cache_() {
        auto& cache = mailbox_().cache();
        auto i = cache.continuation();
        auto e = cache.end();
        while (i != e) {
            consume_(*i);
            cache.erase(i);
            return true;
        }
        return false;
    }

    void cooperative_actor::cleanup_() {}

    auto cooperative_actor::current_message_impl() -> message* {
        return current_message_m_;
    }

    auto cooperative_actor::context_() const -> executor::execution_device* {
        return executor_m_;
    }

    void cooperative_actor::context_(executor::execution_device* e) {
        if (e != nullptr) {
            executor_m_ = e;
        }
    }

    auto cooperative_actor::supervisor_() -> supervisor_abstract* {
        return supervisor_m_;
    }

    auto cooperative_actor::flags_() const -> int {
        return flags_m_.load(std::memory_order_relaxed);
    }

    void cooperative_actor::flags_(int new_value) {
        flags_m_.store(new_value, std::memory_order_relaxed);
    }
    auto cooperative_actor::mailbox_() -> mailbox_t& {
        return mailbox_m_;
    }

} // namespace actors_framework::base
