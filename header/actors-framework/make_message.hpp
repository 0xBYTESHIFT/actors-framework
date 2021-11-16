#pragma once

#include <actors-framework/base/address.hpp>
#include <actors-framework/base/basic_actor.hpp>
#include <actors-framework/base/handler.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/base/supervisor.hpp>
#include <actors-framework/impl/handler.ipp>

namespace actors_framework {

    template<class T>
    auto make_message_ptr(base::address_t sender_, T name) -> base::message* {
        return new base::message(std::move(sender_), std::forward<T>(name));
    }

    template<class T, typename Arg>
    auto make_message_ptr(base::address_t sender_, T name, Arg&& arg) -> base::message* {
        return new base::message(std::move(sender_), std::forward<T>(name), std::move(detail::any(std::forward<type_traits::decay_t<Arg>>(arg))));
    }

    template<class T, typename... Args>
    auto make_message_ptr(base::address_t sender_, T name, Args&&... args) -> base::message* {
        return new base::message(sender_, std::forward<T>(name), std::move(detail::any(std::tuple<type_traits::decay_t<Args>...>{std::forward<Args>(args)...})));
    }

    template<class T>
    auto make_message(base::address_t sender_, T name) -> base::message_ptr {
        return base::message_ptr(new base::message(std::move(sender_), std::forward<T>(name)));
    }

    template<class T, typename Arg>
    auto make_message(base::address_t sender_, T name, Arg&& arg) -> base::message_ptr {
        return base::message_ptr(new base::message(std::move(sender_), std::forward<T>(name), std::move(detail::any(std::forward<type_traits::decay_t<Arg>>(arg)))));
    }

    template<class T, typename... Args>
    auto make_message(base::address_t sender_, T name, Args&&... args) -> base::message_ptr {
        return base::message_ptr(new base::message(sender_, std::forward<T>(name), std::move(detail::any(std::tuple<type_traits::decay_t<Args>...>{std::forward<Args>(args)...}))));
    }

} // namespace actors_framework