#pragma once

#include <actors-framework/base/address.hpp>
#include <actors-framework/base/basic_actor.hpp>
#include <actors-framework/base/handler.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/base/supervisor.hpp>
#include <actors-framework/utils/tracy_include.hpp>

#include <actors-framework/impl/handler.ipp>

namespace actors_framework {

    template<class T, class... Args>
    auto make_message_ptr(base::address_t sender_, T name, Args&&... args) -> base::message* {
        ZoneScoped;
        auto tuple = std::tuple<type_traits::decay_t<Args>...>{std::forward<Args>(args)...};
        auto any = detail::any(std::move(tuple));
        return new base::message(std::move(sender_), std::move(name), std::move(any));
    }

    template<class T, class... Args>
    auto make_message(base::address_t sender_, T name, Args&&... args) -> base::message_ptr {
        ZoneScoped;
        auto tuple = std::tuple<type_traits::decay_t<Args>...>{std::forward<Args>(args)...};
        auto any = detail::any(std::move(tuple));
        auto ptr = new base::message(std::move(sender_), std::move(name), std::move(any));
        return base::message_ptr(ptr);
    }

} // namespace actors_framework
