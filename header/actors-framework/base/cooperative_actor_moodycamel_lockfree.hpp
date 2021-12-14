#pragma once

#include <actors-framework/base/actor_abstract.hpp>
#include <actors-framework/base/cooperative_actor.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/executor/executable.hpp>
#include <actors-framework/forwards.hpp>
#include <concurrentqueue.h> //moodycamel

namespace actors_framework::base {

    using queue_t = moodycamel::ConcurrentQueue<std::unique_ptr<message>>;

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
        void cleanup_() final;
        void consume_(message_ptr&);

        auto mailbox_() -> mailbox_t&;
        auto reactivate_(message_ptr& x) -> void;
        auto next_message_() -> message_ptr;
        auto has_next_message_() -> bool final;

        message_ptr current_message_m_;
        mailbox_t mailbox_m_;
    };

    template<class T>
    auto intrusive_ptr_add_ref(T* ptr) -> typename std::enable_if_t<std::is_same_v<T, cooperative_actor>> {
        ptr->intrusive_ptr_add_ref_impl();
    }

    template<class T>
    auto intrusive_ptr_release(T* ptr) -> typename std::enable_if_t<std::is_same_v<T, cooperative_actor>> {
        ptr->intrusive_ptr_release_impl();
    }

} // namespace actors_framework::base
