#pragma once

#include "actors-framework/link.hpp"

namespace actors_framework {

    void link(base::address_t a1, base::address_t a2) {
        send(a1, a2, "add_link", a2);
        send(a2, a1, "add_link", a1);
    }
} // namespace actors_framework
