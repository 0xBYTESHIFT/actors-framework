#pragma once

#include <actors-framework/send.hpp>
#include <actors-framework/utils/tracy_include.hpp>

namespace actors_framework {

    void send(base::address_t address, base::message_ptr msg) {
        ZoneScoped;
        address.enqueue(std::move(msg));
    }

} // namespace actors_framework
