#pragma once
#include "actors-framework/executor/policy/work_sharing.hpp"

namespace actors_framework::executor {

    work_sharing::~work_sharing() {
        ZoneScoped;
        // nop
    }

} // namespace actors_framework::executor
