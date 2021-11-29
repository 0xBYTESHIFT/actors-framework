#pragma once
#include <actors-framework/base/supervisor_abstract.hpp>
#include <actors-framework/link.hpp>
/// ------------------------------------------------
#include <actors-framework/base/actor.hpp>

#include <actors-framework/base/address.hpp>
#include <actors-framework/base/basic_actor.hpp>
#include <actors-framework/base/handler.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/base/supervisor.hpp>
#include <actors-framework/impl/handler.ipp>

#include <iostream>

namespace actors_framework::base {

    static void error_sync_contacts_in_supervisor(const std::string& name, const std::string& error) {
        std::cerr << "WARNING" << '\n';
        std::cerr << "Actor name : " << name << '\n';
        std::cerr << "Not initialization address type:" << error << '\n';
        std::cerr << "WARNING" << std::endl;
    }

    using detail::aligned_allocate;
    using detail::aligned_deallocate;
    using detail::DEFAULT_ALIGNMENT;
    using detail::is_supported_alignment;

    class new_delete_resource final : public detail::pmr::memory_resource {
    public:
    private:
        void* do_allocate(std::size_t bytes, std::size_t alignment = DEFAULT_ALIGNMENT) override {
            alignment = (is_supported_alignment(alignment)) ? alignment : DEFAULT_ALIGNMENT;
            return aligned_allocate(bytes, alignment, [](std::size_t size) { return ::operator new(size); });
        }

        void do_deallocate(void* p, std::size_t bytes, std::size_t alignment = DEFAULT_ALIGNMENT) override {
            aligned_deallocate(p, bytes, alignment, [](void* p) { ::operator delete(p); });
        }

        bool do_is_equal(const memory_resource& other) const noexcept override { return &other == this; }
    };

    supervisor_abstract::supervisor_abstract(detail::pmr::memory_resource* mr, std::string name)
        : communication_module(std::move(name))
        , memory_resource_(mr) {
        add_handler("delegate", &supervisor_abstract::redirect_);
        add_handler("add_link", &supervisor_abstract::add_link_);
        add_handler("remove_link", &supervisor_abstract::remove_link_);
    }

    supervisor_abstract::supervisor_abstract(std::string name)
        : communication_module(std::move(name))
        , memory_resource_(new new_delete_resource) {
        add_handler("delegate", &supervisor_abstract::redirect_);
        add_handler("add_link", &supervisor_abstract::add_link_);
        add_handler("remove_link", &supervisor_abstract::remove_link_);
    }

    supervisor_abstract::supervisor_abstract(supervisor_abstract* ptr, std::string name)
        : communication_module(std::move(name))
        , memory_resource_(ptr->resource()) {
        add_handler("delegate", &supervisor_abstract::redirect_);
        add_handler("add_link", &supervisor_abstract::add_link_);
        add_handler("remove_link", &supervisor_abstract::remove_link_);
    }
    supervisor_abstract::~supervisor_abstract() {}

    auto supervisor_abstract::current_message_impl() -> message* {
        return current_message_;
    }

    auto supervisor_abstract::set_current_message(message_ptr msg) -> void {
        current_message_ = msg.release();
    }

    auto supervisor_abstract::resource() const -> detail::pmr::memory_resource* {
        return memory_resource_;
    }

    auto supervisor_abstract::executor() noexcept -> executor::abstract_executor* {
        return executor_impl();
    }

    auto supervisor_abstract::redirect_(std::string& type, message* msg) -> void {
        message_ptr tmp(std::move(msg));
        auto type_t = std::move(type);
        tmp->sender() = std::move(address());
        send(address_book(std::move(type_t)), std::move(tmp));
    }

    auto supervisor_abstract::address() noexcept -> address_t {
        return address_t(this);
    }

    auto supervisor_abstract::all_view_address() const -> std::set<std::string> {
        std::set<std::string> tmp;
        for (const auto& [name, address] : contacts_) {
            tmp.emplace(name);
        }
        return tmp;
    }

    auto supervisor_abstract::address_book(const std::string& type) -> address_t {
        auto tmp = address_t::empty_address();
        auto result = contacts_.find(type);
        if (result != contacts_.end()) {
            tmp = *(result->second.begin());
        }
        return tmp;
    }

    auto supervisor_abstract::address_book() -> address_range_t {
        return std::make_pair(contacts_.cbegin(), contacts_.cend());
    }

    void supervisor_abstract::add_link_() {
        add_link_impl_(std::move(current_message()->sender()));
    }

    void supervisor_abstract::remove_link_() {
        remove_link_impl_(current_message()->sender());
    }

    void supervisor_abstract::add_link_impl_(address_t address) {
        if (address && this != address.get()) {
            auto name = address.type();
            auto it = contacts_.find(name);
            if (it == contacts_.end()) {
                auto result = contacts_.emplace(std::move(name), storage_contact_t());
                result.first->second.emplace_back(std::move(address));
                return;
            } else {
                it->second.emplace_back(std::move(address));
                return;
            }
        } else {
            error_sync_contacts_in_supervisor(type(), address.type());
        }
    }

    void supervisor_abstract::remove_link_impl_(const address_t& address) {
        auto name = address.type();
        auto it = contacts_.find(name);
        if (it == contacts_.end()) {
            // not find
        } else {
            auto end = it->second.end();
            for (auto i = it->second.begin(); i != end; ++i) {
                if (address.get() == i->get()) {
                    it->second.erase(i);
                }
            }
        }
    }

    auto supervisor_abstract::broadcast(message_ptr msg_) -> void {
        auto msg = std::move(msg_);

        for (auto& i : contacts_) {
            for (auto& j : i.second) {
                j.enqueue(message_ptr(msg->clone()));
            }
        }
    }

    auto supervisor_abstract::broadcast(const std::string& type_, message_ptr msg_) -> void {
        auto type = std::move(type_);
        auto msg = std::move(msg_);
        auto range = contacts_.find(type);
        for (auto& i : range->second) {
            i.enqueue(message_ptr(msg->clone()));
        }
    }

    void supervisor_abstract::sync_(const base::address_t& address) {
        auto address_tmp(address);
        add_link_impl_(address_t(address));
        send(address_tmp, supervisor_abstract::address(), "add_link");
        auto& sender = current_message()->sender();
        if (sender && this != sender.get()) {
            link(sender, address_tmp);
        }
    }

} // namespace actors_framework::base
