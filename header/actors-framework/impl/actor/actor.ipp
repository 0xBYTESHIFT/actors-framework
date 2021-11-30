#pragma once

#include <actors-framework/base/actor.hpp>
#include <actors-framework/base/actor_abstract.hpp>
#include <actors-framework/base/address.hpp>
#include <actors-framework/base/message.hpp>

namespace actors_framework::base {
    actor::actor(std::nullptr_t)
        : ptr_(nullptr) {}

    actor& actor::operator=(std::nullptr_t) {
        ptr_.reset();
        return *this;
    }

    actor::~actor() {}

    auto actor::address() const noexcept -> address_t {
        return ptr_->address();
    }

    auto actor::operator->() const noexcept -> actor_abstract* {
        return ptr_.get();
    }

    actor::operator bool() const noexcept {
        return static_cast<bool>(ptr_);
    }

    bool actor::operator!() const noexcept {
        return !ptr_;
    }

    auto actor::type() const -> const std::string& {
        return ptr_->type();
    }

    void actor::swap_(actor& other) noexcept {
        using std::swap;
        ptr_.swap(other.ptr_);
    }

} // namespace actors_framework::base
