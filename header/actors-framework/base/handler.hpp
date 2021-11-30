#pragma once

#include <functional>

#include <actors-framework/forwards.hpp>

namespace actors_framework::base {
    ///
    /// @brief Abstract concept of an action
    ///
    class handler {
    public:
        virtual ~handler() = default;

        virtual void invoke(communication_module&) = 0;
    };

    class helper final : public handler {
    public:
        ~helper() override = default;

        template<class F>
        helper(F&& f);

        template<class F, class ClassPtr>
        helper(F&& f, ClassPtr* self);

        void invoke(communication_module& ctx) final {
            helper_(ctx);
        }

    private:
        std::function<void(communication_module&)> helper_;
    };

    template<class F>
    inline auto make_handler(F&& f) -> handler* {
        return new helper(std::forward<F>(f));
    }

    template<class F, class ClassPtr>
    inline auto make_handler(F&& f, ClassPtr* self) -> handler* {
        return new helper(std::forward<F>(f), self);
    }

} // namespace actors_framework::base
