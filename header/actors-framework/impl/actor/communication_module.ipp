#pragma once
#include <iostream>
#include <vector>

#include <actors-framework/base/address.hpp>
#include <actors-framework/base/communication_module.hpp>
#include <actors-framework/base/handler.hpp>
#include <actors-framework/base/message.hpp>
#include <actors-framework/impl/handler.ipp>

namespace actors_framework::base {

    void error_duplicate_handler(const std::string& error) {
        std::cerr << "Duplicate" << '\n';
        std::cerr << "Handler: " << error << '\n';
        std::cerr << "Duplicate" << std::endl;
    }

    void error_add_handler(const std::string& error) {
        std::cerr << "error add handler" << '\n';
        std::cerr << "Handler: " << error << '\n';
        std::cerr << "error add handler" << std::endl;
    }

    void error_skip(const std::string& sender, const std::string& reciever, const std::string& handler) {
        std::cerr << "WARNING" << '\n';
        std::cerr << "Skip, can't find handler: " << reciever << "::" << handler;
        std::cerr << " sender: " << sender << "\n";
        std::cerr << "WARNING" << std::endl;
    }

    void communication_module::execute() {
        auto it = handlers_.find(current_message()->command());
        if (it != handlers_.end()) {
            return it->second->invoke(*this);
        } else {
            auto sender = current_message()->sender().type();
            auto reciever = this->type();
            error_skip(sender, reciever, current_message()->command());
        }
    }

    bool communication_module::on(const std::string& name, handler* aa) {
        auto it = handlers_.find(name);
        bool status = false;
        if (it == handlers_.end()) {
            auto it1 = handlers_.emplace(name, std::unique_ptr<handler>(aa));
            status = it1.second;
            if (status == false) {
                error_add_handler(name);
            }
        } else {
            error_duplicate_handler(name);
        }

        return status;
    }

    void communication_module::enqueue(message_ptr msg) {
        enqueue(std::move(msg), nullptr);
    }

    auto communication_module::message_types() const -> std::set<std::string> {
        std::set<std::string> types;

        for (const auto& i : handlers_) {
            types.emplace(std::string(i.first.begin(), i.first.end()));
        }

        return types;
    }

    auto communication_module::type() const -> const std::string& {
        return type_;
    }

    communication_module::~communication_module() {}

    communication_module::communication_module(std::string type)
        : type_(std::move(type)) {}

    void communication_module::enqueue(message_ptr msg, executor::execution_device* e) {
        enqueue_base(std::move(msg), e);
    }
    auto communication_module::current_message() -> message* {
        return current_message_impl();
    }

} // namespace actors_framework::base
