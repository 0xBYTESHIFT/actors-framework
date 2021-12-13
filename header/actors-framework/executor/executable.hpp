#pragma once

#include <cstddef>
#include <cstdint>
#include <type_traits>

#include <actors-framework/forwards.hpp>

namespace actors_framework::executor {
    ///
    /// @brief
    ///

    ///
    /// @enum
    /// @brief
    ///
    enum class executable_result : uint8_t {
        resume,
        awaiting,
        done,
        shutdown
    };

    struct executable {
        executable() = default;
        virtual ~executable();

        virtual void intrusive_ptr_add_ref_impl() = 0;
        virtual void intrusive_ptr_release_impl() = 0;
        virtual auto run(execution_device*, size_t max_throughput) -> executable_result = 0;
    };

    template<class T>
    auto intrusive_ptr_add_ref(T* ptr) -> typename std::enable_if_t<std::is_same_v<T*, executable*>> {
        ptr->intrusive_ptr_add_ref_impl();
    }

    template<class T>
    auto intrusive_ptr_release(T* ptr) -> typename std::enable_if_t<std::is_same_v<T*, executable*>> {
        ptr->intrusive_ptr_release_impl();
    }

} // namespace actors_framework::executor
