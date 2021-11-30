#pragma once
#include <utility>

#include <actors-framework/base/address.hpp>
#include <actors-framework/base/message.hpp>

namespace actors_framework::base {

    message::message(address_t sender, std::string name)
        : sender_(std::move(sender))
        , command_(std::move(name))
        , body_() {}

    message::message(address_t sender, std::string name, detail::any body)
        : sender_(std::move(sender))
        , command_(std::move(name))
        , body_(std::move(body)) {}

    message::message()
        : sender_(address_t::empty_address()) {}

    auto message::command() const noexcept -> const std::string& {
        return command_;
    }

    auto message::clone() const -> message* {
        return new message(sender_, command_, body_);
    }

    message::operator bool() {
        return !command_.empty() || bool(sender_) || body_.has_value();
    }

    void message::swap(message& other) noexcept {
        using std::swap;
        swap(sender_, other.sender_);
        swap(command_, other.command_);
        swap(body_, other.body_);
    }

    bool message::is_high_priority() const {
        return false;
    }

    auto message::body() -> detail::any& {
        assert(body_.has_value());
        return body_;
    }

    auto message::sender() & noexcept -> address_t& {
        return sender_;
    }

    auto message::sender() && noexcept -> address_t&& {
        return std::move(sender_);
    }

    auto message::sender() const& noexcept -> address_t const& {
        return sender_;
    }

} // namespace actors_framework::base
