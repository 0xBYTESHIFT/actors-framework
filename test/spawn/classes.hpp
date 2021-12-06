#pragma once
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include <actors-framework.hpp>
#include <actors-framework/make_message.hpp>

const int max_queue = 100;

namespace af = actors_framework;

template<class Task, class... Args>
auto make_task(af::supervisor& executor_, const std::string& command, Args... args) -> void {
    auto task = Task(std::forward<Args>(args)...);
    af::send(executor_, executor_->address(), command, std::move(task));
}

template<class Task, class... Args>
auto make_task_broadcast(af::supervisor& executor_, const std::string& command, Args... args) -> void {
    auto task = Task(std::forward<Args>(args)...);
    auto msg = af::make_message(executor_->address(), command, std::move(task));
    executor_->broadcast(std::move(msg));
}

auto thread_pool_deleter = [](af::abstract_executor* ptr) -> void {
    ptr->stop();
    delete ptr;
};

class worker_t2 final : public af::basic_async_actor {
public:
    static inline std::atomic_int count = 0;
    static inline std::atomic_int callback_called = 0;
    static constexpr auto name = "bot2";
    static constexpr auto callback_name = "callback";

    explicit worker_t2(af::supervisor_abstract* ptr)
        : af::basic_async_actor(ptr, name) {
        std::cout << "worker2 spawned" << std::endl;
        ++count;
        add_handler("spawn_broadcast", &worker_t2::spawn_broadcast);
        add_handler(callback_name, &worker_t2::callback);
    }

    auto spawn_broadcast(af::address_t sender, af::address_t addr) -> void {
        std::cout << "spawn_broadcast called in " << name << " addr: " << addr.type() << std::endl;
        //nop
    }

    auto callback() -> void {
        std::cout << "callback called in " << name << std::endl;
        ++callback_called;
    }
};

class worker_t final : public af::basic_async_actor {
public:
    static inline std::atomic_int count = 0;
    static constexpr auto name = "bot1";
    static constexpr auto spawn_bot_name = "spawn_bot2";
    static constexpr auto ping_bot2_name = "ping_bot2";

    explicit worker_t(af::supervisor_abstract* ptr /*, int a, double b*/)
        : af::basic_async_actor(ptr, name) {
        ++count;
        std::cout << "worker1 spawned" << std::endl;
        add_handler(spawn_bot_name, &worker_t::spawn_worker);
        add_handler(ping_bot2_name, &worker_t::ping_bot2);
        add_handler("spawn_broadcast", &worker_t::spawn_broadcast);
    }

    auto spawn_worker() -> void {
        auto addr = address_book("manager-2");
        std::cout << "create_worker2 called by worker1" << std::endl;
        af::send(addr, address(), "create_worker2");
    }

    auto spawn_broadcast(af::address_t sender, af::address_t addr) -> void {
        std::cout << "spawn_broadcast called in " << name << " addr: " << addr.type() << std::endl;
        if (addr.type() == worker_t2::name) {
            auto self_addr = address();
            af::link(self_addr, addr);
        }
    }

    auto ping_bot2() -> void {
        auto addr = address_book(worker_t2::name);
        af::send(addr, address(), worker_t2::callback_name);
    }
};

class supervisor_lite final : public af::supervisor_abstract {
public:
    explicit supervisor_lite(std::string name)
        : supervisor_abstract(std::move(name))
        , e_(new af::executor_t<af::work_sharing>(1, max_queue),
             thread_pool_deleter)
        , system_{"add_link",
                  "sync_contacts",
                  "remove_link",
                  "spawn_actor",
                  "delegate",
                  "spawn_broadcast",
                  "create_worker",
                  "create_worker2"} {
        add_handler("spawn_broadcast", &supervisor_lite::spawn_broadcast);
        add_handler("create_worker", &supervisor_lite::create_worker);
        add_handler("create_worker2", &supervisor_lite::create_worker2);
        e_->start();
    }

    void create_worker2() {
        std::cout << "create_worker2 called in " << address().type() << std::endl;
        spawn_actor<worker_t2>();
    }

    void create_worker() {
        std::cout << "create_worker1 called in " << address().type() << std::endl;
        spawn_actor<worker_t>();
    }

    auto executor_impl() noexcept -> af::abstract_executor* final {
        return e_.get();
    }

    auto add_actor_impl(af::actor t) -> void final {
        spawn_broadcast(address(), t->address());
        actors_.emplace_back(std::move(t));
    }

    auto add_supervisor_impl(af::supervisor t) -> void final {
        supervisor_.emplace_back(std::move(t));
    }

    auto spawn_broadcast(af::address_t sender, af::address_t addr) -> void {
        auto contacts = address_book();
        std::cout << "spawn_broadcast called in " << address().type()
                  << " sender: " << sender.type()
                  << " addr: " << addr.type() << std::endl;
        for (auto it = contacts.first; it != contacts.second; ++it) {
            if (it->first != sender.type()) {
                auto mes = af::make_message(address(), "spawn_broadcast", address(), addr);
                broadcast(it->first, std::move(mes));
            }
        }
    }

    auto enqueue_base(af::message_ptr msg, af::execution_device*) -> void final {
        auto msg_ = std::move(msg);
        auto it = system_.find(msg_->command());
        if (it != system_.end()) {
            local(std::move(msg_));
        }
    }

private:
    auto local(af::message_ptr msg) -> void {
        std::cout << "local execution in supervisor '"
                  << address().type()
                  << "', cmd:" << msg->command() << std::endl;
        set_current_message(std::move(msg));
        execute();
    }

    std::unique_ptr<af::abstract_executor, decltype(thread_pool_deleter)> e_;
    std::vector<af::actor> actors_;
    std::vector<af::address_t> actors_for_broadcast_;
    std::vector<af::supervisor> supervisor_;
    std::unordered_set<std::string> system_;
};
