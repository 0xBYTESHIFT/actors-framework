#pragma once

#include <actors-framework/base/supervisor.hpp>
#include <actors-framework/base/supervisor_abstract.hpp>
#include <actors-framework/forwards.hpp>
#include <actors-framework/make_message.hpp>

namespace actors_framework {

    template<typename... Args>
    void send(base::supervisor& supervisor, Args... args) {
        supervisor->enqueue(
            make_message(
                std::forward<Args>(args)...));
    }

    template<typename... Args>
    void delegate_send(const base::supervisor& supervisor, std::string type, Args... args) {
        supervisor->enqueue(
            make_message(
                supervisor->address(),
                "delegate",
                std::move(type),
                std::move(make_message_ptr(
                    base::address_t::empty_address(),
                    std::forward<Args>(args)...))));
    }

    template<typename... Args>
    void delegate_send(const base::supervisor& supervisor, base::address_t type, Args... args) {
        supervisor->enqueue(
            make_message(
                supervisor->address(),
                "delegate",
                std::move(type),
                std::move(make_message_ptr(
                    base::address_t::empty_address(),
                    std::forward<Args>(args)...))));
    }

    template<typename... Args>
    void delegate_send(base::address_t& supervisor, std::string type, Args... args) {
        supervisor.enqueue(
            make_message(
                supervisor,
                "delegate",
                std::move(type),
                std::move(make_message_ptr(
                    base::address_t::empty_address(),
                    std::forward<Args>(args)...))));
    }

    template<typename... Args>
    void send(const base::supervisor& supervisor, Args... args) {
        supervisor->enqueue(
            make_message(
                std::forward<Args>(args)...));
    }

    template<typename... Args>
    void send(base::actor& actor, Args... args) {
        actor->enqueue(
            make_message(
                std::forward<Args>(args)...));
    }

    template<typename... Args>
    void send(base::actor&& actor, Args... args) {
        actor->enqueue(
            make_message(
                std::forward<Args>(args)...));
    }

    template<typename... Args>
    void send(base::address_t&& address, Args... args) {
        address.enqueue(
            make_message(
                std::forward<Args>(args)...));
    }

    template<typename... Args>
    void send(base::address_t& address, Args... args) {
        address.enqueue(
            make_message(
                std::forward<Args>(args)...));
    }

    void send(base::address_t address, base::message_ptr msg);

} // namespace actors_framework
