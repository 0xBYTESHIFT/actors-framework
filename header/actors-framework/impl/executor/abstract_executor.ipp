#pragma once
#include <vector>

#include <actors-framework/executor/abstract_executor.hpp>
#include <actors-framework/executor/execution_device.hpp>
#include <actors-framework/executor/executor.hpp>

namespace actors_framework::executor {

    abstract_executor::abstract_executor(
        std::size_t num_worker_threads,
        std::size_t max_throughput)
        : next_worker_(0)
        , max_throughput_(max_throughput)
        , num_workers_(num_worker_threads) {
    }

    void cleanup_and_release(executable* ptr) {
        class dummy_unit final : public execution_device {
        public:
            dummy_unit() = default;

            void execute(executable* job) override {
                executables_.push_back(job);
            }

            std::vector<executable*> executables_;
        };
        dummy_unit dummy{};
        while (!dummy.executables_.empty()) {
            dummy.executables_.pop_back();
        }

        intrusive_ptr_release(ptr);
    }

} // namespace actors_framework::executor
