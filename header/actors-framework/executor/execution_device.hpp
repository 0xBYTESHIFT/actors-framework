#pragma once

#include <actors-framework/forwards.hpp>

namespace actors_framework::executor {
    ///
    /// @brief
    ///
    struct execution_device {
        execution_device() = default;

        execution_device(execution_device&&) = delete;

        execution_device(const execution_device&) = delete;

        virtual ~execution_device();

        virtual void execute(executable*) = 0;
    };
} // namespace actors_framework::executor
