#pragma once
#include <actors-framework/base/supervisor_abstract.hpp>

#include <actors-framework/base/address.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/base/supervisor.hpp>

namespace actors_framework::base {

    supervisor::~supervisor() {}

    address_t supervisor::address() const noexcept {
        return address_t(ptr_.get());
    }

    detail::string_view supervisor::type() const {
        return ptr_->type();
    }

    void supervisor::swap(supervisor& other) noexcept {
        using std::swap;
        ptr_.swap(other.ptr_);
    }

    supervisor::supervisor(std::nullptr_t)
        : ptr_(nullptr) {}

    supervisor& supervisor::operator=(std::nullptr_t) {
        ptr_.reset();
        return *this;
    }

} // namespace actors_framework::base
