#pragma once
#include <actors-framework/executor/executable.hpp>

namespace actors_framework::executor {

    executable::~executable() {
        ZoneScoped;
    }

} // namespace actors_framework::executor
