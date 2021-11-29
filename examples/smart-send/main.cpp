#include <cassert>

#include <chrono>
#include <iostream>
#include <memory>
#include <queue>
#include <thread>
#include <unordered_set>
#include <vector>

#include <actors-framework/broadcast.hpp>
#include <actors-framework/core.hpp>
#include <actors-framework/send.hpp>

template<typename Task, typename... Args>
auto make_task(actors_framework::supervisor& executor_, const std::string& command, Args... args) -> void {
    actors_framework::send(executor_, executor_->address(), command, std::move(Task(std::forward<Args>(args)...)));
}

template<typename Task, typename... Args>
auto make_task_broadcast(actors_framework::supervisor& executor_, const std::string& command, Args... args) -> void {
    auto address = executor_->address();
    actors_framework::broadcast(executor_, address, command, std::move(Task(std::forward<Args>(args)...)));
}

auto thread_pool_deleter = [](actors_framework::abstract_executor* ptr) {
    ptr->stop();
    delete ptr;
};

struct download_data final {
    download_data(const std::string& url, const std::string& user, const std::string& passwod)
        : url_(url)
        , user_(user)
        , passwod_(passwod) {}

    ~download_data() = default;
    std::string url_;
    std::string user_;
    std::string passwod_;
};

struct work_data final {
    ~work_data() = default;

    work_data(const std::string& data, const std::string& operatorName)
        : data_(data)
        , operator_name_(operatorName) {}

    std::string data_;
    std::string operator_name_;
};

static std::atomic<uint64_t> counter_download_data{0};
static std::atomic<uint64_t> counter_work_data{0};

class worker_t final : public actors_framework::basic_async_actor {
public:
    explicit worker_t(actors_framework::supervisor_abstract* ptr)
        : actors_framework::basic_async_actor(ptr, "bot") {
        add_handler(
            "download",
            &worker_t::download);

        add_handler(
            "work_data",
            &worker_t::work_data);
    }

    void download(download_data& data) {
        tmp_ = data.url_;
        counter_download_data++;
    }

    void work_data(work_data& data) {
        tmp_ = data.data_;
        counter_work_data++;
    }

    ~worker_t() override = default;

private:
    std::string tmp_;
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
              "spawn_actor", "create"} {
        e_->start();
        add_handler("create", &supervisor_lite::create);
    }

    void create() {
        spawn_actor<worker_t>();
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
    actors_framework::supervisor supervisor(new supervisor_lite());

    int const actors = 10;

    for (auto i = actors - 1; i > 0; --i) {
        actors_framework::send(supervisor, actors_framework::address_t::empty_address(), "create");
    }

    //actors_framework::spawn_actor<worker_t>(supervisor);

    int const task = 10000;

    for (auto i = task - 1; i > 0; --i) {
        make_task<download_data>(supervisor, "download", std::string("fb"), std::string("jack"), std::string("1"));
    }

    for (auto i = task - 1; i > 0; --i) {
        make_task_broadcast<work_data>(supervisor, "work_data", std::string("fb"), std::string("jack"));
    }

    std::this_thread::sleep_for(std::chrono::seconds(180));

    std::cerr << " Finish " << std::endl;
    std::cerr << "counter_download_data :" << counter_download_data.load() << std::endl;
    std::cerr << "counter_work_data :" << counter_work_data.load() << std::endl;
    assert(counter_download_data.load() == 9999);
    assert(counter_work_data.load() == 9999);
    return 0;
}
