#pragma once

#include <condition_variable>
#include <limits>
#include <memory>
#include <set>
#include <thread>
#include <vector>

#include <actors-framework/detail/ref_counted.hpp>
#include <actors-framework/executor/abstract_executor.hpp>
#include <actors-framework/executor/executable.hpp>
#include <actors-framework/executor/worker.hpp>
#include <actors-framework/utils/tracy_include.hpp>

namespace actors_framework::executor {
    /// @brief
    /// @tparam Policy
    ///
    template<class Policy>
    class executor_t : public abstract_executor {
    public:
        using job_ptr = executable*;
        using policy_data = typename Policy::coordinator_data;
        using worker_type = worker<Policy>;

        executor_t(size_t num_worker_threads, size_t max_throughput_param);

        auto worker_by_id(size_t x) -> worker_type*;
        void execute(job_ptr job) override;
        auto data() -> policy_data&;

    protected:
        void start() override;
        void stop() override;

    private:
        std::vector<std::unique_ptr<worker_type>> workers_;
        policy_data data_;
        Policy policy_;
    };

    template<class Policy>
    executor_t<Policy>::executor_t(size_t num_worker_threads, size_t max_throughput_param)
        : abstract_executor(num_worker_threads, max_throughput_param)
        , data_(this) {
        ZoneScoped;
    }

    template<class Policy>
    auto executor_t<Policy>::worker_by_id(size_t x) -> worker_type* {
        ZoneScoped;
        return workers_[x].get();
    }

    template<class Policy>
    void executor_t<Policy>::execute(job_ptr job) {
        ZoneScoped;
        policy_.central_enqueue(this, job);
    }

    template<class Policy>
    auto executor_t<Policy>::data() -> policy_data& {
        ZoneScoped;
        return data_;
    }

    template<class Policy>
    void executor_t<Policy>::start() {
        ZoneScoped;
        typename worker_type::policy_data init{this};
        auto num = num_workers();
        workers_.reserve(num);

        for (size_t i = 0; i < num; ++i) {
            workers_.emplace_back(new worker_type(i, this, init, max_throughput_));
        }

        for (auto& w : workers_) {
            w->start();
        }
    }

    template<class Policy>
    void executor_t<Policy>::stop() {
        ZoneScoped;
        /// shutdown workers
        struct shutdown_helper final
            : public executable
            , public ref_counted {
            auto run(execution_device* ptr, size_t) -> executable_result override {
                ZoneScoped;
                assert(ptr != nullptr);
                {
                    std::unique_lock<std::mutex> guard(mtx);
                    last_worker = ptr;
                    cv.notify_all();
                    return executable_result::shutdown;
                }
            }

            void intrusive_ptr_add_ref_impl() override {
                intrusive_ptr_add_ref(this);
            }

            void intrusive_ptr_release_impl() override {
                intrusive_ptr_release(this);
            }

            shutdown_helper()
                : last_worker(nullptr) {}

            std::mutex mtx;
            std::condition_variable cv;
            execution_device* last_worker;
        };

        shutdown_helper sh;

        std::set<worker_type*> alive_workers;

        auto num = num_workers();

        for (size_t i = 0; i < num; ++i) {
            alive_workers.insert(worker_by_id(i));
            sh.ref();
        }

        while (!alive_workers.empty()) {
            (*alive_workers.begin())->external_enqueue(&sh);

            { /// lifetime scope of guard
                std::unique_lock<std::mutex> guard(sh.mtx);
                sh.cv.wait(guard, [&] { return sh.last_worker != nullptr; });
            }

            alive_workers.erase(static_cast<worker_type*>(sh.last_worker));
            sh.last_worker = nullptr;
        }

        for (auto& w : workers_) {
            w->get_thread().join(); /// wait until all workers finish working
        }

        /// run cleanup code for each resumable
        auto f = cleanup_and_release;
        for (auto& w : workers_)
            policy_.foreach_resumable(w.get(), f);
        policy_.foreach_central_resumable(this, f);
    }

} // namespace actors_framework::executor
