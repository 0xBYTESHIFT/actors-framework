#pragma once

#include <actors-framework/base/actor_abstract.hpp>
#include <actors-framework/base/address.hpp>
#include <actors-framework/base/handler.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/impl/handler.ipp>

#include <iostream>

namespace actors_framework::base {

    static void error_sync_contacts(const std::string& name, const std::string& error) {
        std::string mes = std::string("ERROR") + "\n" +
                          "Actor name:" + name + "\n" +
                          "Not initialization address type:" + error;
        std::cerr << mes << std::endl;
    }

    actor_abstract::actor_abstract(std::string type)
        : communication_module(std::move(type)) {
        add_handler("add_link", &actor_abstract::add_link_);
        add_handler("remove_link", &actor_abstract::remove_link_);
    }

    actor_abstract::~actor_abstract() {
    }

    void* actor_abstract::operator new(std::size_t, void* ptr) {
        return ptr;
    }

    auto actor_abstract::address_book(const std::string& name) -> address_t {
        auto it = contacts_.find(name);
        if (it == contacts_.end()) {
            auto mes = std::string("actor ") + address().type() + " can't find contact '" + name + "'";
            throw std::out_of_range(std::move(mes));
        }
        return it->second;
    }

    auto actor_abstract::all_view_address() const -> std::set<std::string> {
        std::set<std::string> tmp;
        for (const auto& [name, address] : contacts_) {
            tmp.emplace(name);
        }
        return tmp;
    }

    void actor_abstract::remove_link_() {
        const auto& address = current_message()->sender();
        remove_link_impl_(address);
    }

    void actor_abstract::add_link_(address_t address) {
        if (address && this != address.get()) {
            auto mes = std::string("actor ") + this->type() +
                       " linking with " + address.type();
            std::cout << mes << std::endl;
            auto name = address.type();
            contacts_.emplace(std::move(name), std::move(address));
        } else {
            error_sync_contacts(type(), address.type());
        }
    }

    void actor_abstract::remove_link_impl_(const address_t& address) {
        auto it = contacts_.find(address.type());
        if (it != contacts_.end()) {
            contacts_.erase(it);
        }
    }

    auto actor_abstract::address() noexcept -> address_t {
        return address_t(this);
    }

} // namespace actors_framework::base
