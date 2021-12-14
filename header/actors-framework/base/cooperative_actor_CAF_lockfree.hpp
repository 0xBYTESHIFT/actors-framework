#pragma once

#include <actors-framework/base/actor_abstract.hpp>
#include <actors-framework/base/cooperative_actor.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/detail/single_reader_queue.hpp>
#include <actors-framework/executor/executable.hpp>
#include <actors-framework/forwards.hpp>

namespace actors_framework::base {

    using queue_t = detail::single_reader_queue<message>;

    class cooperative_actor : public cooperative_actor_base {
    public:
        using mailbox_t = queue_t;

        ~cooperative_actor() override;

        auto run(executor::execution_device*, max_throughput_t) -> executor::executable_result final;
        void intrusive_ptr_add_ref_impl() override;
        void intrusive_ptr_release_impl() override;

    protected:
        cooperative_actor(supervisor_abstract*, std::string);

        void enqueue_base(message_ptr, executor::execution_device*) final;

        // Non thread-safe method
        auto current_message_impl() -> message* override;

    private:
        using cooperative_actor_base::flags_;

        void cleanup_() final;
        bool consume_from_cache_();
        void consume_(message&);

        auto mailbox_() -> mailbox_t&;
        auto reactivate_(message& x) -> void;
        auto next_message_() -> message_ptr;
        auto has_next_message_() -> bool final;
        void push_to_cache_(message_ptr ptr);

        using cooperative_actor_base::context_;
        using cooperative_actor_base::supervisor_;

        message* current_message_m_;
        mailbox_t mailbox_m_;
    };

    template<class T>
    auto intrusive_ptr_add_ref(T* ptr) -> typename std::enable_if_t<std::is_same_v<cooperative_actor, T>> {
        ptr->intrusive_ptr_add_ref_impl();
    }

    template<class T>
    auto intrusive_ptr_release(T* ptr) -> typename std::enable_if_t<std::is_same_v<cooperative_actor, T>> {
        ptr->intrusive_ptr_release_impl();
    }

}; // namespace actors_framework::base
