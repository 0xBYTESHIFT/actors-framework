#pragma once
#include <actors-framework/detail/ref_counted.hpp>

namespace actors_framework {

    ref_counted::ref_counted()
        : rc_(1) {}

    ref_counted::ref_counted(const ref_counted&)
        : rc_(1) {}

    ref_counted::~ref_counted() {}

    ref_counted& ref_counted::operator=(const ref_counted&) {
        return *this;
    }
} // namespace actors_framework
