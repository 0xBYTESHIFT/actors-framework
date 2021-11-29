#include <cassert>

#include <map>
#include <unordered_set>
#include <vector>

#include <actors-framework/core.hpp>
#include <actors-framework/link.hpp>

class dummy_executor final : public actors_framework::abstract_executor {
public:
    dummy_executor()
        : abstract_executor(1, 10000) {}

    void execute(actors_framework::executable* ptr) override {
        ptr->run(nullptr, max_throughput());
    }
    void start() override {}
    void stop() override {}
};

class storage_t final : public actors_framework::basic_async_actor {
public:
    explicit storage_t(actors_framework::supervisor_abstract* ptr)
        : actors_framework::basic_async_actor(ptr, "storage") {
        add_handler(
            "update",
            []() -> void {});

        add_handler(
            "find",
            []() -> void {});

        add_handler(

            "remove",
            []() -> void {});
    }

    ~storage_t() override = default;
};

class network_t final : public actors_framework::basic_async_actor {
public:
    ~network_t() override = default;

    explicit network_t(actors_framework::supervisor_abstract* ptr)
        : actors_framework::basic_async_actor(ptr, "network") {}
};

class supervisor_lite final : public actors_framework::supervisor_abstract {
public:
    explicit supervisor_lite(dummy_executor* ptr)
        : supervisor_abstract("network")
        , e_(ptr)
        , cursor(0)
        , system_{"sync_contacts", "add_link", "remove_link"} {
        add_handler("create_network", &supervisor_lite::create_network);
        add_handler("create_storage", &supervisor_lite::create_storage);
    }

    void create_storage() {
        spawn_actor<storage_t>();
    }

    void create_network() {
        spawn_actor<network_t>();
    }

    ~supervisor_lite() override = default;

    auto executor_impl() noexcept -> actors_framework::abstract_executor* final {
        return e_;
    }

    auto add_actor_impl(actors_framework::actor) -> void override {
    }

    auto add_supervisor_impl(actors_framework::supervisor) -> void override {
    }

    void enqueue_base(actors_framework::message_ptr msg, actors_framework::execution_device*) final {
        auto msg_ = std::move(msg);
        auto it = system_.find(msg_->command());
        if (it != system_.end()) {
            local(std::move(msg_));
        } else {
            redirect_robin(std::move(msg_));
        }
    }

private:
    auto local(actors_framework::message_ptr msg) -> void {
        set_current_message(std::move(msg));
        execute();
    }

    auto redirect_robin(actors_framework::message_ptr msg) -> void {
        if (!actors_.empty()) {
            actors_[cursor]->enqueue(std::move(msg));
            ++cursor;
            if (cursor >= actors_.size()) {
                cursor = 0;
            }
        }
    }

    actors_framework::abstract_executor* e_;
    std::vector<actors_framework::actor> actors_;
    std::size_t cursor;
    std::unordered_set<std::string> system_;
};

int main() {
    actors_framework::supervisor supervisor_tmp(new supervisor_lite(new dummy_executor));
    actors_framework::send(supervisor_tmp, actors_framework::address_t::empty_address(), "create_storage");
    actors_framework::send(supervisor_tmp, actors_framework::address_t::empty_address(), "create_network");
    actors_framework::supervisor supervisor1(new supervisor_lite(new dummy_executor));
    actors_framework::link(supervisor_tmp, supervisor1);
    actors_framework::send(supervisor1, actors_framework::address_t::empty_address(), "create_storage");
    actors_framework::send(supervisor1, actors_framework::address_t::empty_address(), "create_network");
    return 0;
}
