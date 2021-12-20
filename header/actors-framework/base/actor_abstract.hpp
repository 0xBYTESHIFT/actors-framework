#pragma once

#include <actors-framework/base/address.hpp>
#include <actors-framework/base/communication_module.hpp>
#include <actors-framework/forwards.hpp>

#include <new>
#include <unordered_map>
#include <utility>

namespace actors_framework::base {
    ///
    /// @brief Abstract concept of an actor
    ///

    class actor_abstract : public communication_module {
    public:
        actor_abstract() = delete;
        ~actor_abstract() override;

        // allow placement new (only)
        void* operator new(std::size_t, void* ptr);

        auto address() noexcept -> address_t;

    protected:
        actor_abstract(const actor_abstract&) = delete;
        actor_abstract(std::string);

        actor_abstract& operator=(const actor_abstract&) = delete;
        // prohibit copies, assignments, and heap allocations
        void* operator new(size_t);
        void* operator new[](size_t);

        auto address_book(std::string) -> address_t;
        /**
        * debug method
        */
        auto all_view_address() const -> std::set<std::string>;

    private:
        void remove_link_();
        void add_link_(address_t);
        void remove_link_impl_(const address_t&);

        std::unordered_map<std::string, address_t> contacts_;
    };

} // namespace actors_framework::base
