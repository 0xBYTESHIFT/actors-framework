#pragma once

#include <actors-framework/base/address.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/base/actor.hpp>
#include <actors-framework/base/actor_abstract.hpp>

namespace actors_framework::base {
    actor::~actor() {}

    address_t actor::address() const noexcept {
        return ptr_->address();
    }

    detail::string_view actor::type() const {
        return ptr_->type();
    }

    void actor::swap(actor& other) noexcept {
        using std::swap;
        ptr_.swap(other.ptr_);
    }

    actor::actor(std::nullptr_t)
        : ptr_(nullptr) {}

    actor& actor::operator=(std::nullptr_t) {
        ptr_.reset();
        return *this;
    }
} // namespace actors_framework::base
