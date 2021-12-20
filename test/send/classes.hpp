#pragma once
#include <actors-framework.hpp>

#include <atomic>
#include <iostream>
#include <unordered_set>
#include <vector>

namespace af = actors_framework;

auto thread_pool_deleter = [](af::abstract_executor* ptr) -> void {
    ZoneScoped;
    ptr->stop();
    delete ptr;
};

struct task {
    int data;
};

namespace names {
    namespace actor0 {
        static constexpr auto name = "actor0";
        static constexpr auto send_task_name = "send_task";
    }; // namespace actor0
    namespace actor1 {
        static constexpr auto name = "actor1";
        static constexpr auto callback_task_name = "callback_task";

    }; // namespace actor1
};     // namespace names

class actor1 final : public af::basic_async_actor {
public:
    static inline std::atomic<size_t> count_task = 0;

    static auto get_name(size_t id) -> std::string {
        return std::string(names::actor1::name) + std::to_string(id);
    }

    explicit actor1(af::supervisor_abstract* ptr, size_t id)
        : af::basic_async_actor(ptr, get_name(id)) {
        ZoneScoped;
        add_handler("spawn_broadcast", &actor1::spawn_broadcast);
        add_handler(names::actor1::callback_task_name, &actor1::callback_task);
    }

    auto spawn_broadcast(af::address_t sender, af::address_t addr) -> void {
        ZoneScoped;
        //nop
    }

    auto callback_task(task& task_) -> void {
        ZoneScoped;
        [[maybe_unused]] auto task = std::move(task_);
        count_task.fetch_add(1, std::memory_order_release);
    }
};

class actor0 final : public af::basic_async_actor {
    size_t actor1_count_ = 0;
    size_t actor1_id_ = 0;

public:
    static auto get_name(size_t id) -> std::string {
        return std::string(names::actor0::name) + std::to_string(id);
    }

    explicit actor0(af::supervisor_abstract* ptr, size_t id)
        : af::basic_async_actor(ptr, get_name(id)) {
        ZoneScoped;
        add_handler("spawn_broadcast", &actor0::spawn_broadcast);
        add_handler(names::actor0::send_task_name, &actor0::send_task);
    }

    auto spawn_broadcast(af::address_t sender, af::address_t addr) -> void {
        ZoneScoped;
        std::string type_str = addr.type();
        type_str.erase(type_str.size() - 1); //delete last letter (id)
        if (type_str == names::actor1::name) {
            auto self_addr = address();
            af::link(self_addr, addr);
            actor1_count_++;
        }
    }

    auto send_task() -> void {
        ZoneScoped;
        task t;
        auto& id = actor1_id_;

        auto addr = address_book(actor1::get_name(id));
        af::send(addr, address(), names::actor1::callback_task_name, std::move(t));

        ++id;
        if (id == actor1_count_) {
            id = 0;
        }
    }
};

class supervisor_lite final : public af::supervisor_abstract {
public:
    explicit supervisor_lite(std::string name, size_t workers, size_t throughput)
        : supervisor_abstract(std::move(name))
        , e_(new af::executor_t<af::work_sharing>(workers, throughput),
             thread_pool_deleter)
        , system_{"add_link",
                  "sync_contacts",
                  "remove_link",
                  "spawn_actor",
                  "delegate",
                  "spawn_broadcast",
                  "create_actor0",
                  "create_actor1"} {
        ZoneScoped;
        add_handler("spawn_broadcast", &supervisor_lite::spawn_broadcast);
        add_handler("create_actor0", &supervisor_lite::create_actor0);
        add_handler("create_actor1", &supervisor_lite::create_actor1);
        e_->start();
    }

    void create_actor0(size_t id) {
        ZoneScoped;
        spawn_actor<actor0>(id);
    }

    void create_actor1(size_t id) {
        ZoneScoped;
        spawn_actor<actor1>(id);
    }

    auto executor_impl() noexcept -> af::abstract_executor* final {
        ZoneScoped;
        return e_.get();
    }

    auto add_actor_impl(af::actor t) -> void final {
        ZoneScoped;
        spawn_broadcast(address(), t->address());
        actors_.emplace_back(std::move(t));
    }

    auto add_supervisor_impl(af::supervisor t) -> void final {
        ZoneScoped;
    }

    auto spawn_broadcast(af::address_t sender, af::address_t addr) -> void {
        ZoneScoped;
        auto contacts = address_book();
        for (auto it = contacts.first; it != contacts.second; ++it) {
            if (it->first != sender.type()) {
                auto mes = af::make_message(address(), "spawn_broadcast", address(), addr);
                broadcast(it->first, std::move(mes));
            }
        }
    }

    auto enqueue_base(af::message_ptr msg, af::execution_device*) -> void final {
        ZoneScoped;
        auto msg_ = std::move(msg);
        auto it = system_.find(msg_->command());
        if (it != system_.end()) {
            local(std::move(msg_));
        }
    }

private:
    auto local(af::message_ptr msg) -> void {
        ZoneScoped;
        set_current_message(std::move(msg));
        execute();
    }

    std::unique_ptr<af::abstract_executor, decltype(thread_pool_deleter)> e_;
    std::vector<af::actor> actors_;
    std::unordered_set<std::string> system_;
};
