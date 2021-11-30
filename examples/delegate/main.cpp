#include <iostream>
#include <map>
#include <set>
#include <unordered_set>
#include <vector>

#include <actors-framework/core.hpp>
#include <actors-framework/send.hpp>

class storage_t final : public actors_framework::basic_async_actor {
public:
    storage_t(actors_framework::supervisor_abstract* ptr)
        : actors_framework::basic_async_actor(ptr, "storage") {
        add_handler(
            "update",
            &storage_t::update);

        add_handler(
            "find",
            &storage_t::find);

        add_handler(
            "remove",
            &storage_t::remote);

        add_handler(
            "status",
            [this]() {
                status();
            });
    }

    void status() {
        std::cerr << "status" << std::endl;
    }

    ~storage_t() override = default;

private:
    void update(std::string& data) {
        std::cerr << "update:" << data << std::endl;
    }

    void find() {
        std::cerr << "find" << std::endl;
    }

    void remote() {
        std::cerr << "remote" << std::endl;
    }
};

auto thread_pool_deleter = [](actors_framework::abstract_executor* ptr) {
    ptr->stop();
    delete ptr;
};

/// non thread safe
class supervisor_lite final : public actors_framework::supervisor_abstract {
public:
    explicit supervisor_lite()
        : supervisor_abstract("network")
        , e_(new actors_framework::executor_t<actors_framework::work_sharing>(
                 1,
                 100),
             thread_pool_deleter)
        , cursor(0)
        , system_{
              "sync_contacts",
              "add_link",
              "remove_link",
              "spawn_actor", "delegate", "create"} {
        e_->start();
        add_handler("create", &supervisor_lite::create);
    }

    void create() {
        spawn_actor<storage_t>();
    }

    ~supervisor_lite() override = default;

    auto executor_impl() noexcept -> actors_framework::abstract_executor* final { return e_.get(); }

    auto add_actor_impl(actors_framework::actor t) -> void final {
        actors_.push_back(std::move(t));
    }

    auto add_supervisor_impl(actors_framework::supervisor t) -> void final {
        supervisor_.emplace_back(std::move(t));
    }

    auto enqueue_base(actors_framework::message_ptr msg, actors_framework::execution_device*) -> void final {
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

    std::unique_ptr<actors_framework::abstract_executor, decltype(thread_pool_deleter)> e_;
    std::vector<actors_framework::actor> actors_;
    std::vector<actors_framework::supervisor> supervisor_;
    std::size_t cursor;
    std::unordered_set<std::string> system_;
};

int main() {
    actors_framework::supervisor dummy_supervisor(new supervisor_lite());
    actors_framework::send(dummy_supervisor, actors_framework::address_t::empty_address(), "create");
    actors_framework::delegate_send(dummy_supervisor, "storage", "update", std::string("payload"));
    actors_framework::delegate_send(dummy_supervisor, "storage", "find");
    actors_framework::delegate_send(dummy_supervisor, "storage", "remove");
    actors_framework::delegate_send(dummy_supervisor, "storage", "status");

    std::this_thread::sleep_for(std::chrono::seconds(180));

    return 0;
}
