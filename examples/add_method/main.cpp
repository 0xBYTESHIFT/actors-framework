#include <cassert>

#include <set>
#include <string>

#include <actors-framework/core.hpp>
#include <actors-framework/send.hpp>
#include <iostream>

class storage_t final : public actors_framework::basic_async_actor {
public:
    storage_t(actors_framework::supervisor_abstract* ptr)
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

        assert(actors_framework::detail::string_view("storage") == type());

        auto tmp = message_types();

        std::set<std::string> control = {"add_link", "remove_link", "update", "remove", "find"};

        std::set<std::string> diff;

        std::set_difference(tmp.begin(), tmp.end(), control.begin(), control.end(), std::inserter(diff, diff.begin()));

        assert(diff.empty());
    }

    ~storage_t() override = default;
};

class dummy_executor final : public actors_framework::abstract_executor {
public:
    dummy_executor(uint64_t threads, uint64_t throughput)
        : abstract_executor(threads, throughput) {}

    void execute(actors_framework::executable* ptr) override {
        ptr->run(nullptr, max_throughput());
        intrusive_ptr_release(ptr);
    }

    void start() override {}

    void stop() override {}
};

class dummy_supervisor final : public actors_framework::supervisor_abstract {
public:
    dummy_supervisor()
        : supervisor_abstract("dummy_supervisor")
        , e_(new dummy_executor(1, 1)) {
        e_->start();
        add_handler("create", &dummy_supervisor::create);
    }

    void create() {
        spawn_actor<storage_t>();
    }

    auto executor_impl() noexcept -> actors_framework::abstract_executor* final {
        return e_.get();
    }

    auto add_actor_impl(actors_framework::actor t) -> void final {
        actors_.emplace_back(std::move(t));
    }

    auto add_supervisor_impl(actors_framework::supervisor t) -> void final {
        supervisor_.emplace_back(std::move(t));
    }

    auto enqueue_base(actors_framework::message_ptr msg, actors_framework::execution_device*) -> void final {
        {
            set_current_message(std::move(msg));
            execute();
        }
    }

private:
    std::unique_ptr<actors_framework::abstract_executor> e_;
    std::vector<actors_framework::actor> actors_;
    std::vector<actors_framework::supervisor> supervisor_;
};

int main() {
    actors_framework::supervisor supervisor(new dummy_supervisor());
    actors_framework::send(supervisor, actors_framework::address_t::empty_address(), "create");

    return 0;
}
