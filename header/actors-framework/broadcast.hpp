#pragma once
#include <actors-framework/base/supervisor.hpp>
#include <actors-framework/make_message.hpp>

namespace actors_framework {

    template<class Sender,
             class... Args,
             class = type_traits::enable_if_t<std::is_base_of_v<base::supervisor, Sender>>>
    void broadcast(Sender& a1, Args... args) {
        a1->broadcast(
            make_message(
                std::forward<Args>(args)...));
    }

} // namespace actors_framework
