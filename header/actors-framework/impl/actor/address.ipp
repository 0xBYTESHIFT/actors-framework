#pragma once

#include <actors-framework/base/address.hpp>
#include <actors-framework/base/basic_actor.hpp>
#include <actors-framework/base/handler.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/base/supervisor.hpp>
#include <actors-framework/impl/handler.ipp>

#include <actors-framework/base/address.hpp>
#include <actors-framework/base/communication_module.hpp>
#include <memory>

namespace {
    constexpr static auto non_type = "non-type";
}

namespace actors_framework::base {

    using message_ptr = std::unique_ptr<message>;

    address_t::address_t() noexcept
        : ptr_(nullptr) {
    }

    address_t::address_t(actor_abstract* ptr)
        : ptr_(ptr) {
        assert(ptr != nullptr);
    }

    address_t::address_t(supervisor_abstract* ptr)
        : ptr_(ptr) {
        assert(ptr != nullptr);
    }

    address_t::address_t(address_t&& other) noexcept {
        swap(other);
    }

    address_t::address_t(const address_t& other) {
        if (this != &other) {
            ptr_ = other.ptr_;
        }
    }

    address_t::~address_t() noexcept {
        ptr_ = nullptr;
    }

    address_t& address_t::operator=(address_t&& other) noexcept {
        if (this != &other) {
            swap(other);
        }
        return *this;
    }

    address_t& address_t::operator=(const address_t& other) {
        if (this != &other) {
            address_t tmp(other);
            swap(tmp);
        }
        return *this;
    }

    bool address_t::operator!() const noexcept {
        return !(static_cast<bool>(ptr_));
    }

    address_t::operator bool() const noexcept {
        return static_cast<bool>(ptr_);
    }

    void address_t::enqueue(message_ptr msg) noexcept {
        ptr_->enqueue(std::move(msg));
    }

    auto address_t::type() const -> const std::string& {
        return ptr_->type();
    }

    void address_t::swap(address_t& other) {
        using std::swap;
        std::swap(ptr_, other.ptr_);
    }

    void* address_t::get() const {
        return ptr_;
    }
} // namespace actors_framework::base
