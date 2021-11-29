#include <actors-framework/make_message.hpp>
#include <cassert>

#include <chrono>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

#include <actors-framework/core.hpp>
#include <actors-framework/link.hpp>

std::atomic_int count;
const int max_queue = 100;

template<typename Task, typename... Args>
auto make_task(actors_framework::supervisor& executor_, const std::string& command, Args... args) -> void {
    actors_framework::send(executor_, executor_->address(), command, std::move(Task(std::forward<Args>(args)...)));
}

template<typename Task, typename... Args>
auto make_task_broadcast(actors_framework::supervisor& executor_, const std::string& command, Args... args) -> void {
    executor_->broadcast(make_message(executor_->address(), command, std::move(Task(std::forward<Args>(args)...))));
}

auto thread_pool_deleter = [](actors_framework::abstract_executor* ptr) -> void {
    ptr->stop();
    delete ptr;
};

class worker_t2 final : public actors_framework::basic_async_actor {
public:
    explicit worker_t2(actors_framework::supervisor_abstract* ptr)
        : actors_framework::basic_async_actor(ptr, "bot2") {
        count++;
        std::cout << "bot2 created:" << address().get() << std::endl;
        add_handler("spawn_broadcast", &worker_t2::spawn_broadcast);
        add_handler("print_links", &worker_t2::print_links);
    }

    auto spawn_broadcast(actors_framework::address_t addr) -> void {
        std::cout << address().type() << "(" << address().get() << ")"
                  << " got actor: " << addr.type() << "(" << addr.get() << ")" << std::endl;
        if (addr.type() == "bot3") {
            std::cout << address().type() << "(" << address().get() << ")"
                      << " linking with it" << std::endl;
            auto self_addr = address();
            actors_framework::link(self_addr, addr);
        }
    }

    auto print_links() -> void {
        std::cout << address().type() << "(" << address().get() << ")"
                  << " linked with:\n";
        auto addrs = all_view_address();
        for (const auto& a : addrs) {
            std::cout << '\t' << a << std::endl;
        }
    }
};

class worker_t3 final : public actors_framework::basic_async_actor {
public:
    explicit worker_t3(actors_framework::supervisor_abstract* ptr)
        : actors_framework::basic_async_actor(ptr, "bot3") {
        count++;
        std::cout << address().type() << "(" << address().get() << ")"
                  << " created" << std::endl;
        add_handler("spawn_broadcast", &worker_t3::spawn_broadcast);
        add_handler("print_links", &worker_t3::print_links);
    }

    auto spawn_broadcast(actors_framework::address_t addr) -> void {
        std::cout << address().type() << "(" << address().get() << ")"
                  << " got actor: " << addr.type() << "(" << addr.get() << ")" << std::endl;
    }

    auto print_links() -> void {
        std::cout << address().type() << "(" << address().get() << ")"
                  << " linked with:\n";
        auto addrs = all_view_address();
        for (const auto& a : addrs) {
            std::cout << '\t' << a << std::endl;
        }
    }
};

class worker_t final : public actors_framework::basic_async_actor {
public:
    explicit worker_t(actors_framework::supervisor_abstract* ptr /*, int a, double b*/)
        : actors_framework::basic_async_actor(ptr, "bot1") {
        count++;
        std::cout << address().type() << "(" << address().get() << ")"
                  << " created" << std::endl;
        add_handler("spawn_actor_in_bot", &worker_t::spawn_worker);
        add_handler("spawn_broadcast", &worker_t::spawn_broadcast);
        add_handler("print_links", &worker_t::print_links);
    }

    auto spawn_worker() -> void {
        auto addr = address_book("manager-2");
        actors_framework::send(addr, address(), "create_worker2");
        actors_framework::send(addr, address(), "create_worker3");
    }

    auto spawn_broadcast(actors_framework::address_t addr) -> void {
        std::cout << address().type() << "(" << address().get() << ")"
                  << " got actor: " << addr.type() << "(" << addr.get() << ")" << std::endl;
        if (addr.type() == "bot2") {
            std::cout << address().type() << "(" << address().get() << ")"
                      << " linking with it" << std::endl;
            auto self_addr = address();
            actors_framework::link(self_addr, addr);
        }
    }

    auto print_links() -> void {
        std::cout << address().type() << "(" << address().get() << ")"
                  << " linked with:\n";
        auto addrs = all_view_address();
        for (const auto& a : addrs) {
            std::cout << '\t' << a << std::endl;
        }
    }
};

class supervisor_lite final : public actors_framework::supervisor_abstract {
public:
    explicit supervisor_lite(std::string name)
        : supervisor_abstract(std::move(name))
        , e_(new actors_framework::executor_t<actors_framework::work_sharing>(1, max_queue),
             thread_pool_deleter)
        , system_{"add_link",
                  "sync_contacts",
                  "remove_link",
                  "spawn_actor", "delegate", "spawn_broadcast", "create_worker2", "create_worker3", "create_worker"} {
        e_->start();
        add_handler("spawn_broadcast", &supervisor_lite::spawn_broadcast);
        add_handler("create_worker", &supervisor_lite::create_worker);
        add_handler("create_worker2", &supervisor_lite::create_worker2);
        add_handler("create_worker3", &supervisor_lite::create_worker3);
    }

    void create_worker2() {
        spawn_actor<worker_t2>();
    }

    void create_worker3() {
        spawn_actor<worker_t3>();
    }

    void create_worker() {
        spawn_actor<worker_t>();
    }

    auto executor_impl() noexcept -> actors_framework::abstract_executor* final {
        return e_.get();
    }

    auto add_actor_impl(actors_framework::actor t) -> void final {
        std::cout << "sup: added actor:" << t.address() << " " << t.type() << " " << t->address().get() << std::endl;
        spawn_broadcast(address(), t->address());
        actors_.emplace_back(std::move(t));
    }

    auto add_supervisor_impl(actors_framework::supervisor t) -> void final {
        supervisor_.emplace_back(std::move(t));
    }

    auto spawn_broadcast(actors_framework::address_t sender, actors_framework::address_t addr) -> void {
        auto contacts = address_book();
        for (auto it = contacts.first; it != contacts.second; ++it) {
            if (it->first != sender.type()) {
                broadcast(it->first, actors_framework::make_message(address(), "spawn_broadcast", address(), addr));
            }
        }
    }

    auto enqueue_base(actors_framework::message_ptr msg, actors_framework::execution_device*) -> void final {
        auto msg_ = std::move(msg);
        auto it = system_.find(msg_->command());
        if (it != system_.end()) {
            local(std::move(msg_));
        }
    }

private:
    auto local(actors_framework::message_ptr msg) -> void {
        set_current_message(std::move(msg));
        execute();
    }

    std::unique_ptr<actors_framework::abstract_executor, decltype(thread_pool_deleter)> e_;
    std::vector<actors_framework::actor> actors_;
    std::vector<actors_framework::address_t> actors_for_broadcast_;
    std::vector<actors_framework::supervisor> supervisor_;
    std::unordered_set<std::string> system_;
};

auto main() -> int {
    //1. manager-1 spawns 5 actor named "bot1"
    //2. bot1 must send spawn request for types "bot2" and "bot3" to manager-2 upon spawning
    //3. manager-2 must spawn bot2 and bot3 actors on each request
    //4. manager-2 must link bot2 to bot3 upon each spawning
    //5. manager-2 must broadcast bots names to bot1
    //6. bot1 must link itself with type "bot2" and ignore "bot3"
    //7. spawned actors count must be equal 1 + sends * 2
    /*
        ┌──manager-1────┐◄────────────►┌────────────manager-2───────┐
        │               │              │                            │
        │ ┌─bot-1───┐   │              │  ┌─bot-2────┐   ┌─bot-3──┐ │
        │ │         ◄───┼──────────────┼──►          ◄───►        │ │
        │ │         │   │              │  │          │   │        │ │
        │ └─────────┘   │              │  └──────────┘   └────────┘ │
        │               │              │                            │
        └───────────────┘              └────────────────────────────┘
     */
    actors_framework::supervisor supervisor1(new supervisor_lite("manager-1"));
    actors_framework::supervisor supervisor2(new supervisor_lite("manager-2"));
    actors_framework::link(supervisor1, supervisor2);
    std::cout << "supervisors linked" << std::endl;

    int const sends = 1;
    int const actors = 1 + sends * 2;
    count = 0;

    actors_framework::send(supervisor1, actors_framework::address_t::empty_address(), "create_worker"); //actor creator, "bot1"
    actors_framework::delegate_send(supervisor1, "bot1", "add_link", supervisor2->address());           //link bot-1 with manager-2

    for (auto i = sends - 1; i >= 0; --i) {
        std::cout << "sending spawn request#" << i << std::endl;
        actors_framework::delegate_send(supervisor1, "bot1", "spawn_actor_in_bot");
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cerr << " Finish " << std::endl;

    const auto sleep_time = std::chrono::milliseconds(100);
    std::this_thread::sleep_for(sleep_time);
    actors_framework::delegate_send(supervisor1, "bot1", "print_links");
    std::this_thread::sleep_for(sleep_time);
    actors_framework::delegate_send(supervisor2, "bot2", "print_links");
    std::this_thread::sleep_for(sleep_time);
    actors_framework::delegate_send(supervisor2, "bot3", "print_links");
    std::this_thread::sleep_for(sleep_time);

    assert(count == actors);
    return 0;
}
