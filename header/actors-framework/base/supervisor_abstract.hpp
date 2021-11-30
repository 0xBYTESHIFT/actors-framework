#pragma once

#include <actors-framework/base/communication_module.hpp>
#include <actors-framework/detail/pmr/memory_resource.hpp>

namespace actors_framework::base {
    class supervisor_abstract : public communication_module {
    public:
        supervisor_abstract(detail::pmr::memory_resource*, std::string);
        supervisor_abstract(std::string);
        supervisor_abstract(supervisor_abstract*, std::string);
        ~supervisor_abstract() override;

        auto executor() noexcept -> executor::abstract_executor*;
        auto resource() const -> detail::pmr::memory_resource*;
        auto address() noexcept -> address_t;
        auto broadcast(message_ptr) -> void;
        auto broadcast(const std::string&, message_ptr) -> void;

    protected:
        using storage_contact_t = std::list<address_t>;
        using contacts_t = std::unordered_map<key_type, storage_contact_t>;
        using address_range_t = std::pair<contacts_t::const_iterator, contacts_t::const_iterator>;
        using communication_module::add_handler;

        template<
            class Actor,
            class... Args,
            class = type_traits::enable_if_t<std::is_base_of<actor_abstract, Actor>::value>>
        auto spawn_actor(Args&&... args) -> address_t {
            auto allocate_byte = sizeof(Actor);
            auto allocate_byte_alignof = alignof(Actor);
            void* buffer = resource()->allocate(allocate_byte, allocate_byte_alignof);
            auto* actor = new (buffer) Actor(this, std::forward<Args>(args)...);
            auto address = actor->address();
            add_actor_impl(actor);
            sync_(address);
            return address;
        }

        template<
            class Supervisor,
            class... Args,
            class = type_traits::enable_if_t<std::is_base_of<supervisor_abstract, Supervisor>::value>>
        auto spawn_supervisor(Args&&... args) -> address_t {
            auto allocate_byte = sizeof(Supervisor);
            auto allocate_byte_alignof = alignof(Supervisor);
            void* buffer = resource()->allocate(allocate_byte, allocate_byte_alignof);
            auto* supervisor = new (buffer) Supervisor(this, std::forward<Args>(args)...);
            auto address = supervisor->address();
            add_supervisor_impl(supervisor);
            sync_(address);
            return address;
        }

        virtual auto executor_impl() noexcept -> executor::abstract_executor* = 0;
        virtual auto add_actor_impl(actor) -> void = 0;
        virtual auto add_supervisor_impl(supervisor) -> void = 0;
        auto set_current_message(message_ptr) -> void;
        auto current_message_impl() -> message* final;
        auto address_book(const std::string& type) -> address_t;
        auto address_book() -> address_range_t;
        /**
        * debug method
        */
        auto all_view_address() const -> std::set<std::string>;

    private:
        void sync_(const base::address_t&);
        auto redirect_(const std::string& type, message* msg) -> void;
        void add_link_();
        void remove_link_();

        void add_link_impl_(address_t);
        void remove_link_impl_(const address_t&);

        contacts_t contacts_;
        message* current_message_;
        detail::pmr::memory_resource* memory_resource_;
    };

} // namespace actors_framework::base
