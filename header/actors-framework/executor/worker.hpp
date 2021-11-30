#pragma once

#include <cassert>
#include <cstddef>
#include <thread>

#include <actors-framework/executor/executable.hpp>
#include <actors-framework/executor/execution_device.hpp>

namespace actors_framework::executor {

    template<class Policy>
    class executor_t;
    ///
    /// @brief
    /// @tparam Policy
    ///
    template<class Policy>
    class worker final : public execution_device {
    public:
        using job_ptr = executable*;
        using executor_ptr = executor_t<Policy>*;
        using policy_data = typename Policy::worker_data;

        worker(size_t worker_id, executor_ptr worker_parent,
               const policy_data& init, size_t throughput);
        worker(const worker&) = delete;
        worker& operator=(const worker&) = delete;

        void start();

        void external_enqueue(job_ptr job);
        void execute(job_ptr job) override;
        auto parent() -> executor_ptr;
        auto id() const -> size_t;
        auto get_thread() -> std::thread&;
        auto data() -> policy_data&;
        auto max_throughput() -> size_t;

    private:
        void run_();

        const size_t max_throughput_;
        const size_t id_;
        std::thread this_thread_;
        executor_ptr parent_;
        policy_data data_;
        Policy policy_;
    };

    template<class Policy>
    worker<Policy>::worker(size_t worker_id, executor_ptr worker_parent,
                           const policy_data& init, size_t throughput)
        : execution_device()
        , max_throughput_(throughput)
        , id_(worker_id)
        , parent_(worker_parent)
        , data_(init) {
    }

    template<class Policy>
    void worker<Policy>::start() {
        ///assert(this_thread_.get_id() == std::thread::id{}); TODO: do not check; see implement asio
        auto current_worker = this;
        this_thread_ = std::thread{
            [current_worker] {
                /// TODO: hook
                current_worker->run_();
                /// TODO: hook
            }};
    }

    template<class Policy>
    void worker<Policy>::external_enqueue(job_ptr job) {
        assert(job != nullptr); /// TODO: do not check
        policy_.external_enqueue(this, job);
    }

    template<class Policy>
    void worker<Policy>::execute(job_ptr job) {
        assert(job != nullptr); /// TODO: do not check
        policy_.internal_enqueue(this, job);
    }

    template<class Policy>
    auto worker<Policy>::parent() -> executor_ptr {
        return parent_;
    }

    template<class Policy>
    auto worker<Policy>::id() const -> size_t {
        return id_;
    }

    template<class Policy>
    auto worker<Policy>::get_thread() -> std::thread& {
        return this_thread_;
    }

    template<class Policy>
    auto worker<Policy>::data() -> policy_data& {
        return data_;
    }

    template<class Policy>
    auto worker<Policy>::max_throughput() -> size_t {
        return max_throughput_;
    }

    template<class Policy>
    void worker<Policy>::run_() {
        for (;;) {
            auto job = policy_.dequeue(this);
            assert(job != nullptr); /// TODO: do not check
            policy_.before_resume(this, job);
            auto res = job->run(this, max_throughput_);
            policy_.after_resume(this, job);
            switch (res) {
                case executable_result::resume: {
                    policy_.resume_job_later(this, job);
                    break;
                }
                case executable_result::done: {
                    policy_.after_completion(this, job);
                    intrusive_ptr_release(job);
                    break;
                }
                case executable_result::awaiting: {
                    intrusive_ptr_release(job);
                    break;
                }
                case executable_result::shutdown: {
                    policy_.after_completion(this, job);
                    policy_.before_shutdown(this);
                    return;
                }
            }
        }
    }

} // namespace actors_framework::executor