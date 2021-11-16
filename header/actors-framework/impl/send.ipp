#pragma once

#include <actors-framework/send.hpp>

namespace actors_framework {

    void send(base::address_t address, base::message_ptr msg) {
        address.enqueue(std::move(msg));
    }

} // namespace actors_framework