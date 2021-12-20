#pragma once

#include <actors-framework/executor/executable.hpp>

namespace actors_framework::executor {

    class unprofiled {
    public:
        virtual ~unprofiled() = default;

        template<class Worker>
        void before_shutdown(Worker*) {
            ZoneScoped;
        }

        template<class Worker>
        void before_resume(Worker*, executable*) {
            ZoneScoped;
        }

        template<class Worker>
        void after_resume(Worker*, executable*) {
            ZoneScoped;
        }

        template<class Worker>
        void after_completion(Worker*, executable*) {
            ZoneScoped;
        }

    protected:
        template<class WorkerOrCoordinator>
        static auto cast(WorkerOrCoordinator* self) -> decltype(self->data()) {
            ZoneScoped;
            return self->data();
        }
    };

} // namespace actors_framework::executor
