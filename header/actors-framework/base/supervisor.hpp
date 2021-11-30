#pragma once

#include <actors-framework/detail/intrusive_ptr.hpp>
#include <actors-framework/forwards.hpp>

#include <type_traits>

namespace actors_framework::base {

    class supervisor final {
    public:
        supervisor() = default;
        supervisor(const supervisor& a) = delete;
        supervisor(supervisor&& a) = default;
        supervisor(std::nullptr_t);
        template<
            class T,
            class = type_traits::enable_if_t<std::is_base_of<supervisor_abstract, T>::value>>
        supervisor(intrusive_ptr<T> ptr)
            : ptr_(std::move(ptr)) {}

        template<
            class T,
            class = type_traits::enable_if_t<std::is_base_of<supervisor_abstract, T>::value>>
        supervisor(T* ptr)
            : ptr_(ptr) {}

        template<
            class T,
            class = type_traits::enable_if_t<std::is_base_of<supervisor_abstract, T>::value>>
        supervisor& operator=(intrusive_ptr<T> ptr) {
            supervisor tmp{std::move(ptr)};
            swap_(tmp);
            return *this;
        }

        template<
            class T,
            class = type_traits::enable_if_t<std::is_base_of<supervisor_abstract, T>::value>>
        supervisor& operator=(T* ptr) {
            supervisor tmp{ptr};
            swap_(tmp);
            return *this;
        }
        ~supervisor();

        supervisor& operator=(std::nullptr_t);
        supervisor& operator=(const supervisor& a) = delete;
        supervisor& operator=(supervisor&& a) = default;

        inline auto operator->() const noexcept -> supervisor_abstract* {
            return ptr_.get();
        }
        inline explicit operator bool() const noexcept {
            return static_cast<bool>(ptr_);
        }
        inline bool operator!() const noexcept {
            return !ptr_;
        }

        auto address() const noexcept -> address_t;

        inline auto get() const noexcept -> supervisor_abstract* {
            return ptr_.get();
        }

        auto type() const -> const std::string&;

    private:
        void swap_(supervisor&) noexcept;

        intrusive_ptr<supervisor_abstract> ptr_;
    };

    static_assert(std::is_move_constructible<supervisor>::value, "");
    static_assert(not std::is_copy_constructible<supervisor>::value, "");

} // namespace actors_framework::base
