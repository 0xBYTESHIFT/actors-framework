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

        inline size_t max_throughput() const {
            return max_throughput_;
        }

        inline size_t num_workers() const {
            return num_workers_;
        }

        virtual void start() = 0;

        virtual void stop() = 0;

    protected:
        std::atomic<size_t> next_worker_;

        std::size_t max_throughput_;

        std::size_t num_workers_;
    };

    void cleanup_and_release(executable* ptr);

} // namespace actors_framework::executor
