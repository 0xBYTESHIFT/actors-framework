#pragma once

#include <actors-framework/base/cooperative_actor.hpp>
#include <actors-framework/base/metadata.hpp>
#include <actors-framework/forwards.hpp>

namespace actors_framework::executor {

    template<class Policy>
    struct profiled final : Policy {
        using executor_type = profiled_executor<profiled<Policy>>;

        static actor_id id_of(executable* job) {
            auto ptr = static_cast<base::cooperative_actor*>(job);
            return ptr != nullptr ? /*ptr->*/ 1 : 0;
        }

        template<class Worker>
        void before_resume(Worker* worker, executable* job) {
            Policy::before_resume(worker, job);
            auto parent = static_cast<executor_type*>(worker->parent());
            parent->start_measuring(worker->id(), id_of(job));
        }

        template<class Worker>
        void after_resume(Worker* worker, executable* job) {
            Policy::after_resume(worker, job);
            auto parent = static_cast<executor_type*>(worker->parent());
            parent->stop_measuring(worker->id(), id_of(job));
        }

        template<class Worker>
        void after_completion(Worker* worker, executable* job) {
            Policy::after_completion(worker, job);
            auto parent = static_cast<executor_type*>(worker->parent());
            parent->remove_job(id_of(job));
        }
    };

} // namespace actors_framework::executor