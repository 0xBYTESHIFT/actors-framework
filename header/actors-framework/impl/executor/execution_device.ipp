#pragma once
#include <actors-framework/executor/execution_device.hpp>

namespace actors_framework::executor {

    execution_device::~execution_device() {
        ZoneScoped;
    }

} // namespace actors_framework::executor
