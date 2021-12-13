#pragma once

#include <actors-framework/base/actor_abstract.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/detail/simple_queue.hpp>
#include <actors-framework/detail/single_reader_queue.hpp>
#include <actors-framework/executor/executable.hpp>
#include <actors-framework/forwards.hpp>

namespace actors_framework::base {
    ///
    /// @brief Specialization of actor with scheduling functionality
    ///

    using max_throughput_t = std::size_t;

    template<class Queue>
    class cooperative_actor : public actor_abstract
        , public executor::executable {
    public:
        using mailbox_t = Queue;

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
        void cleanup_();
        void consume_(message&);

        auto mailbox_() -> mailbox_t&;
        auto activate_(executor::execution_device* ctx) -> bool;
        auto reactivate_(message& x) -> void;
        auto next_message_() -> message_ptr;
        auto has_next_message_() -> bool;

        auto context_(executor::execution_device*) -> void;
        auto context_() const -> executor::execution_device*;
        auto supervisor_() -> supervisor_abstract*;

        supervisor_abstract* supervisor_m_;
        executor::execution_device* executor_m_;
        message* current_message_m_;
        mailbox_t mailbox_m_;
    };

    template<>
    class cooperative_actor<detail::single_reader_queue<message>> : public actor_abstract
        , public executor::executable {
    public:
        using mailbox_t = detail::single_reader_queue<message>;

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
        enum class state : int {
            empty = 0x01,
            busy
        };

        auto flags_() const -> int;
        void flags_(int new_value);

        void cleanup_();
        bool consume_from_cache_();
        void consume_(message&);

        auto mailbox_() -> mailbox_t&;
        auto activate_(executor::execution_device* ctx) -> bool;
        auto reactivate_(message& x) -> void;
        auto next_message_() -> message_ptr;
        auto has_next_message_() -> bool;
        void push_to_cache_(message_ptr ptr);

        auto context_(executor::execution_device*) -> void;
        auto context_() const -> executor::execution_device*;
        auto supervisor_() -> supervisor_abstract*;

        supervisor_abstract* supervisor_m_;
        executor::execution_device* executor_m_;
        message* current_message_m_;
        mailbox_t mailbox_m_;
        std::atomic<int> flags_m_;
    };

    template<>
    class cooperative_actor<detail::simple_queue<std::unique_ptr<message>>> : public actor_abstract
        , public executor::executable {
    public:
        using mailbox_t = detail::simple_queue<std::unique_ptr<message>>;

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
        enum class state : int {
            empty = 0x01,
            busy
        };

        auto flags_() const -> int;
        void flags_(int new_value);

        void cleanup_();
        void consume_(message_ptr&);

        auto mailbox_() -> mailbox_t&;
        auto activate_(executor::execution_device* ctx) -> bool;
        auto reactivate_(message_ptr& x) -> void;
        auto next_message_() -> message_ptr;
        auto has_next_message_() -> bool;

        auto context_(executor::execution_device*) -> void;
        auto context_() const -> executor::execution_device*;
        auto supervisor_() -> supervisor_abstract*;

        supervisor_abstract* supervisor_m_;
        executor::execution_device* executor_m_;
        std::unique_ptr<message> current_message_m_;
        mailbox_t mailbox_m_;
        std::atomic<int> flags_m_;
    };

    using cooperative_actor_lockfree = cooperative_actor<detail::single_reader_queue<message>>;
    using cooperative_actor_simple = cooperative_actor<detail::simple_queue<std::unique_ptr<message>>>;

    template<class T>
    auto intrusive_ptr_add_ref(T* ptr) -> typename std::enable_if_t<std::is_same_v<T*, cooperative_actor_lockfree*>> {
        ptr->intrusive_ptr_add_ref_impl();
    }

    template<class T>
    auto intrusive_ptr_release(T* ptr) -> typename std::enable_if_t<std::is_same_v<T*, cooperative_actor_lockfree*>> {
        ptr->intrusive_ptr_release_impl();
    }

    template<class T>
    auto intrusive_ptr_add_ref(T* ptr) -> typename std::enable_if_t<std::is_same_v<T*, cooperative_actor_simple*>> {
        ptr->intrusive_ptr_add_ref_impl();
    }

    template<class T>
    auto intrusive_ptr_release(T* ptr) -> typename std::enable_if_t<std::is_same_v<T*, cooperative_actor_simple*>> {
        ptr->intrusive_ptr_release_impl();
    }

} // namespace actors_framework::base
