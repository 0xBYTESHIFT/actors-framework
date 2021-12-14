#pragma once

#include <actors-framework/base/actor_abstract.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/executor/executable.hpp>
#include <actors-framework/forwards.hpp>

namespace actors_framework::base {
    ///
    /// @brief Specialization of actor with scheduling functionality
    ///

    using max_throughput_t = std::size_t;

    class cooperative_actor_base : public actor_abstract
        , public executor::executable {
    public:
        ~cooperative_actor_base() override;

    protected:
        cooperative_actor_base(supervisor_abstract*, std::string);

        enum class state : int {
            empty = 0x01,
            busy
        };

        auto flags_() const -> state;
        void flags_(state new_value);

        virtual void cleanup_() = 0;

        auto activate_(executor::execution_device* ctx) -> bool;
        virtual auto has_next_message_() -> bool = 0;

        auto context_(executor::execution_device*) -> void;
        auto context_() const -> executor::execution_device*;
        auto supervisor_() -> supervisor_abstract*;

    private:
        supervisor_abstract* supervisor_m_;
        executor::execution_device* executor_m_;
        std::atomic<state> flags_m_;
    };
} // namespace actors_framework::base
