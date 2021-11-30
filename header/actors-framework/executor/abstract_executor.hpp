#pragma once

#include <array>
#include <atomic>
#include <chrono>
#include <cstddef>

#include <actors-framework/forwards.hpp>

namespace actors_framework::executor {
    ///
    /// @brief
    ///
    class abstract_executor {
    public:
        abstract_executor(std::size_t num_worker_threads, std::size_t max_throughput);
        virtual ~abstract_executor() = default;

        virtual void execute(executable*) = 0;
        virtual void start() = 0;
        virtual void stop() = 0;
        auto max_throughput() const -> size_t;
        auto num_workers() const -> size_t;

    protected:
        std::atomic<size_t> next_worker_;
        const std::size_t max_throughput_;
        const std::size_t num_workers_;
    };

    void cleanup_and_release(executable* ptr);

} // namespace actors_framework::executor
