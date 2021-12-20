#pragma once

#include "send.hpp"

namespace actors_framework {

    void link(base::address_t actor1, base::address_t actor2);

    template<class Supervisor>
    void link(Supervisor* actor1, Supervisor* actor2) {
        ZoneScoped;
        auto a1 = actor1->address();
        auto a2 = actor2->address();
        link(std::move(a1), std::move(a2));
    }

    template<class Supervisor>
    void link(Supervisor& actor1, Supervisor& actor2) {
        ZoneScoped;
        auto a1 = actor1.address();
        auto a2 = actor2.address();
        link(std::move(a1), std::move(a2));
    }

    template<class Supervisor>
    void link(Supervisor& actor1, base::address_t actor2) {
        ZoneScoped;
        auto a1 = actor1.address();
        link(std::move(a1), std::move(actor2));
    }

} // namespace actors_framework
