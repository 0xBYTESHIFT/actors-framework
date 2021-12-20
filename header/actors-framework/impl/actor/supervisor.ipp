#pragma once
#include <actors-framework/base/supervisor_abstract.hpp>

#include <actors-framework/base/address.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/base/supervisor.hpp>

namespace actors_framework::base {

    supervisor::supervisor(std::nullptr_t)
        : ptr_(nullptr) {
        ZoneScoped;
    }

    supervisor::~supervisor() {
        ZoneScoped;
    }

    auto supervisor::operator=(std::nullptr_t) -> supervisor& {
        ptr_.reset();
        return *this;
    }
    auto supervisor::operator->() const noexcept -> supervisor_abstract* {
        return ptr_.get();
    }
    bool supervisor::operator!() const noexcept {
        return !ptr_;
    }
    supervisor::operator bool() const noexcept {
        return static_cast<bool>(ptr_);
    }

    auto supervisor::address() const noexcept -> address_t {
        return address_t(ptr_.get());
    }

    auto supervisor::type() const -> const std::string& {
        return ptr_->type();
    }

    void supervisor::swap_(supervisor& other) noexcept {
        ptr_.swap(other.ptr_);
    }

} // namespace actors_framework::base
