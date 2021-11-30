#pragma once

#include <list>
#include <memory>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include <actors-framework/base/handler.hpp>
#include <actors-framework/detail/callable_trait.hpp>
#include <actors-framework/detail/ref_counted.hpp>
#include <actors-framework/forwards.hpp>

namespace actors_framework::base {

    using message_ptr = std::unique_ptr<message>;

    class communication_module : public ref_counted {
    public:
        using key_type = std::string;
        using handler_storage_t = std::unordered_map<key_type, std::unique_ptr<handler>>;

        communication_module() = delete;
        communication_module(const communication_module&) = delete;
        ~communication_module() override;

        communication_module& operator=(const communication_module&) = delete;

        auto type() const -> const std::string&;
        auto message_types() const -> std::set<std::string>;
        auto enqueue(message_ptr) -> void;
        void enqueue(message_ptr, executor::execution_device*);
        auto current_message() -> message*;

    protected:
        communication_module(std::string);

        virtual auto current_message_impl() -> message* = 0;
        virtual void enqueue_base(message_ptr, executor::execution_device*) = 0;

        template<class F>
        auto add_handler(const std::string& name, F&& f) -> typename std::enable_if<!std::is_member_function_pointer<F>::value>::type {
            on(name, make_handler(std::forward<F>(f)));
        }

        template<class F>
        auto add_handler(const std::string& name, F&& f) -> typename std::enable_if<std::is_member_function_pointer<F>::value>::type {
            on(name, make_handler(std::forward<F>(f), static_cast<typename type_traits::get_callable_trait_t<F>::class_type*>(this)));
        }

        void execute();
        bool on(const std::string&, handler*);

    private:
        handler_storage_t handlers_;
        const std::string type_;
    };

} // namespace actors_framework::base
