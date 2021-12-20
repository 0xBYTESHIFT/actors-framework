#pragma once

#include <actors-framework/detail/intrusive_ptr.hpp>
#include <actors-framework/detail/type_list.hpp>
#include <actors-framework/forwards.hpp>
#include <actors-framework/utils/tracy_include.hpp>

//smart actor
namespace actors_framework::base {
    ///
    /// @brief An Actor's entity container
    ///
    class actor final {
    public:
        actor() = default;
        actor(const actor& a) = delete;
        actor(actor&& a) = default;
        actor(std::nullptr_t);
        ~actor();

        actor& operator=(const actor& a) = delete;
        actor& operator=(actor&& a) = default;
        actor& operator=(std::nullptr_t);
        auto operator->() const noexcept -> actor_abstract*;
        explicit operator bool() const noexcept;
        auto operator!() const noexcept -> bool;

        template<
            class T,
            class = type_traits::enable_if_t<std::is_base_of_v<actor_abstract, T>>>
        actor(intrusive_ptr<T> ptr)
            : ptr_(std::move(ptr)) {
            ZoneScoped;
        }

        template<
            class T,
            class = type_traits::enable_if_t<std::is_base_of_v<actor_abstract, T>>>
        actor(T* ptr)
            : ptr_(ptr) {
            ZoneScoped;
        }

        template<
            class T,
            class = type_traits::enable_if_t<std::is_base_of_v<actor_abstract, T>>>
        auto operator=(intrusive_ptr<T> ptr) -> actor& {
            ZoneScoped;
            actor tmp{std::move(ptr)};
            swap_(tmp);
            return *this;
        }

        template<
            class T,
            class = type_traits::enable_if_t<std::is_base_of_v<actor_abstract, T>>>
        auto operator=(T* ptr) -> actor& {
            ZoneScoped;
            actor tmp{ptr};
            swap_(tmp);
            return *this;
        }

        auto address() const noexcept -> address_t;
        auto type() const -> const std::string&;

    private:
        void swap_(actor&) noexcept;

        intrusive_ptr<actor_abstract> ptr_;
    };

    static_assert(std::is_move_constructible_v<actor>, "");
    static_assert(not std::is_copy_constructible_v<actor>, "");
} // namespace actors_framework::base
