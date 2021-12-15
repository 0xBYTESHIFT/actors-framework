#pragma once
#include <actors-framework.hpp>

#include <atomic>
#include <iostream>
#include <unordered_set>
#include <vector>

namespace af = actors_framework;

static constexpr size_t task_size = 1024 * 10;

auto thread_pool_deleter = [](af::abstract_executor* ptr) -> void {
    ptr->stop();
    delete ptr;
};

struct task {
    decltype(std::chrono::steady_clock::now()) created = std::chrono::steady_clock::now();
};

struct task_stack_lite : task {
    double data;
};

struct task_stack_heavy : task {
    double data[task_size];
};

struct task_heap_lite : task {
    std::shared_ptr<int> ptr = std::make_shared<int>(0);
};

struct task_heap_heavy : task {
    std::vector<double> vec;
};

namespace names {
    namespace actor0 {
        static constexpr auto name = "actor0";
        static constexpr auto send_stack_lite_name = "stack_lite";
        static constexpr auto send_stack_heavy_name = "stack_heavy";
        static constexpr auto send_heap_lite_name = "heap_lite";
        static constexpr auto send_heap_heavy_name = "heap_heavy";
    }; // namespace actor0
    namespace actor1 {
        static constexpr auto name = "actor1";
        static constexpr auto callback_stack_lite_name = "stack_lite";
        static constexpr auto callback_stack_heavy_name = "stack_heavy";
        static constexpr auto callback_heap_lite_name = "heap_lite";
        static constexpr auto callback_heap_heavy_name = "heap_heavy";

    }; // namespace actor1
};     // namespace names

class actor1 final : public af::basic_async_actor {
public:
    static inline std::atomic_int count_stack_lite = 0;
    static inline std::atomic_int count_stack_heavy = 0;
    static inline std::atomic_int count_heap_lite = 0;
    static inline std::atomic_int count_heap_heavy = 0;
    static inline double time_ns_stack_lite = 0;
    static inline double time_ns_stack_heavy = 0;
    static inline double time_ns_heap_lite = 0;
    static inline double time_ns_heap_heavy = 0;

    static auto get_name(size_t id) -> std::string {
        return std::string(names::actor1::name) + std::to_string(id);
    }

    explicit actor1(af::supervisor_abstract* ptr, size_t id)
        : af::basic_async_actor(ptr, get_name(id)) {
        add_handler("spawn_broadcast", &actor1::spawn_broadcast);
        add_handler(names::actor1::callback_stack_lite_name, &actor1::callback_stack_lite);
        add_handler(names::actor1::callback_stack_heavy_name, &actor1::callback_stack_heavy);
        add_handler(names::actor1::callback_heap_lite_name, &actor1::callback_heap_lite);
        add_handler(names::actor1::callback_heap_heavy_name, &actor1::callback_heap_heavy);
    }

    auto spawn_broadcast(af::address_t sender, af::address_t addr) -> void {
        //nop
    }

    auto callback_stack_lite(task_stack_lite& task_) -> void {
        [[maybe_unused]] auto task = std::move(task_);
        count_stack_lite.fetch_add(1, std::memory_order_release);
        auto now = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(now - task.created);
        time_ns_stack_lite += diff.count();
        time_ns_stack_lite /= 2.0;
    }
    auto callback_stack_heavy(task_stack_heavy& task_) -> void {
        [[maybe_unused]] auto task = std::move(task_);
        count_stack_heavy.fetch_add(1, std::memory_order_release);
        auto now = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(now - task.created);
        time_ns_stack_heavy += diff.count();
        time_ns_stack_heavy /= 2;
    }
    auto callback_heap_lite(task_heap_lite& task_) -> void {
        [[maybe_unused]] auto task = std::move(task_);
        count_heap_lite.fetch_add(1, std::memory_order_release);
        auto now = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(now - task.created);
        time_ns_heap_lite += diff.count();
        time_ns_heap_lite /= 2;
    }
    auto callback_heap_heavy(task_heap_heavy& task_) -> void {
        [[maybe_unused]] auto task = std::move(task_);
        count_heap_heavy.fetch_add(1, std::memory_order_release);
        auto now = std::chrono::steady_clock::now();
        auto diff = std::chrono::duration_cast<std::chrono::nanoseconds>(now - task.created);
        time_ns_heap_heavy += diff.count();
        time_ns_heap_heavy /= 2;
    }
};

class actor0 final : public af::basic_async_actor {
    size_t actor1_count_ = 0;
    size_t actor1_id_slite_ = 0;
    size_t actor1_id_sheavy_ = 0;
    size_t actor1_id_hlite_ = 0;
    size_t actor1_id_hheavy_ = 0;

public:
    static auto get_name(size_t id) -> std::string {
        return std::string(names::actor0::name) + std::to_string(id);
    }

    explicit actor0(af::supervisor_abstract* ptr, size_t id)
        : af::basic_async_actor(ptr, get_name(id)) {
        add_handler("spawn_broadcast", &actor0::spawn_broadcast);
        add_handler(names::actor0::send_stack_lite_name, &actor0::send_stack_lite);
        add_handler(names::actor0::send_stack_heavy_name, &actor0::send_stack_heavy);
        add_handler(names::actor0::send_heap_lite_name, &actor0::send_heap_lite);
        add_handler(names::actor0::send_heap_heavy_name, &actor0::send_heap_heavy);
    }

    auto spawn_broadcast(af::address_t sender, af::address_t addr) -> void {
        std::string type_str = addr.type();
        type_str.erase(type_str.size() - 1); //delete last letter (id)
        if (type_str == names::actor1::name) {
            auto self_addr = address();
            af::link(self_addr, addr);
            actor1_count_++;
        }
    }

    auto send_stack_lite() -> void {
        task_stack_lite task;
        auto& id = actor1_id_slite_;

        auto addr = address_book(actor1::get_name(id));
        af::send(addr, address(), names::actor1::callback_stack_lite_name, std::move(task));

        ++id;
        if (id == actor1_count_) {
            id = 0;
        }
    }
    auto send_stack_heavy() -> void {
        task_stack_heavy task;
        auto& id = actor1_id_sheavy_;

        auto addr = address_book(actor1::get_name(id));
        af::send(addr, address(), names::actor1::callback_stack_heavy_name, std::move(task));

        ++id;
        if (id == actor1_count_) {
            id = 0;
        }
    }
    auto send_heap_lite() -> void {
        task_heap_lite task;
        auto& id = actor1_id_hlite_;

        auto addr = address_book(actor1::get_name(id));
        af::send(addr, address(), names::actor1::callback_heap_lite_name, std::move(task));

        ++id;
        if (id == actor1_count_) {
            id = 0;
        }
    }
    auto send_heap_heavy() -> void {
        task_heap_heavy task;
        task.vec.resize(task_size, 0);
        auto& id = actor1_id_hheavy_;

        auto addr = address_book(actor1::get_name(id));
        af::send(addr, address(), names::actor1::callback_heap_heavy_name, std::move(task));

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
        add_handler("spawn_broadcast", &supervisor_lite::spawn_broadcast);
        add_handler("create_actor0", &supervisor_lite::create_actor0);
        add_handler("create_actor1", &supervisor_lite::create_actor1);
        e_->start();
    }

    void create_actor0(size_t id) {
        spawn_actor<actor0>(id);
    }

    void create_actor1(size_t id) {
        spawn_actor<actor1>(id);
    }

    auto executor_impl() noexcept -> af::abstract_executor* final {
        return e_.get();
    }

    auto add_actor_impl(af::actor t) -> void final {
        spawn_broadcast(address(), t->address());
        actors_.emplace_back(std::move(t));
    }

    auto add_supervisor_impl(af::supervisor t) -> void final {
    }

    auto spawn_broadcast(af::address_t sender, af::address_t addr) -> void {
        auto contacts = address_book();
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
        set_current_message(std::move(msg));
        execute();
    }

    std::unique_ptr<af::abstract_executor, decltype(thread_pool_deleter)> e_;
    std::vector<af::actor> actors_;
    std::unordered_set<std::string> system_;
};
