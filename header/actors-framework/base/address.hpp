#pragma once

#include <actors-framework/forwards.hpp>

namespace actors_framework::base {
    ///
    /// @brief A compact location expressor
    ///
    class address_t final {
    public:
        address_t(address_t&& other) noexcept;
        address_t(const address_t& other);
        explicit address_t(actor_abstract*);
        explicit address_t(supervisor_abstract*);
        ~address_t() noexcept;

        address_t& operator=(address_t&& other) noexcept;
        address_t& operator=(const address_t& other);
        auto operator!() const noexcept -> bool;
        operator bool() const noexcept;

        static auto empty_address() -> address_t {
            static address_t tmp;
            return tmp;
        }
        auto enqueue(message_ptr) noexcept -> void;
        auto type() const -> const std::string&;
        void swap(address_t& other);
        void* get() const;

    private:
        address_t() noexcept;

        communication_module* ptr_;
    };

    static_assert(!std::is_default_constructible_v<address_t>, "");
    static_assert(std::is_move_constructible_v<address_t>, "");
    static_assert(std::is_move_assignable_v<address_t>, "");
    static_assert(std::is_copy_constructible_v<address_t>, "");
    static_assert(std::is_copy_assignable_v<address_t>, "");

} // namespace actors_framework::base
