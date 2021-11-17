#pragma once

#include <actors-framework/core.hpp>
#include <iostream>
#include <list>

#define TRACE(msg) \
    { std::cout << __FILE__ << ":" << __LINE__ << "::" << __func__ << " : " << msg << std::endl; }

class dummy_executor final : public actors_framework::abstract_executor {
public:
    dummy_executor(uint64_t threads, uint64_t throughput)
        : abstract_executor(threads, throughput) {}

    void execute(actors_framework::executable* ptr) override {
        TRACE("execute(actors_framework::executable* ptr) +++");
        ptr->run(nullptr, max_throughput());
        intrusive_ptr_release(ptr);
        TRACE("execute(actors_framework::executable* ptr) ---");
    }

    void start() override {}

    void stop() override {}
};

class storage_t;
class test_handlers;

class dummy_supervisor final
    : public actors_framework::supervisor_abstract {
public:
    static uint64_t constructor_counter;
    static uint64_t destructor_counter;
    static uint64_t executor_impl_counter;
    static uint64_t add_actor_impl_counter;
    static uint64_t add_supervisor_impl_counter;
    static uint64_t enqueue_base_counter;

public:
    explicit dummy_supervisor(uint64_t threads, uint64_t throughput)
        : supervisor_abstract("dummy_supervisor")
        , executor_(new dummy_executor(threads, throughput)) {
        executor_->start();
        constructor_counter++;

        add_handler("create_storage", &dummy_supervisor::create_storage);
        add_handler("create_test_handlers", &dummy_supervisor::create_test_handlers);
    }

    void create_storage();

    void create_test_handlers();

    ~dummy_supervisor() override = default;

    void start() {}
    void stop() {}

    auto executor_impl() noexcept -> actors_framework::abstract_executor* override {
        TRACE("+++");
        executor_impl_counter++;
        return executor_.get();
    }

    auto add_actor_impl(actors_framework::actor a) -> void final {
        TRACE("+++");
        actors_.emplace_back(std::move(a));
        add_actor_impl_counter++;
    }
    auto add_supervisor_impl(actors_framework::supervisor s) -> void final {
        TRACE("+++");
        supervisor_.emplace_back(std::move(s));
        add_supervisor_impl_counter++;
    }

    auto actors_count() const -> size_t {
        return actors_.size();
    }

    auto supervisors_count() const -> size_t {
        return supervisor_.size();
    }

    auto last_actor() -> actors_framework::actor& {
        assert(actors_count() > 0);
        TRACE("+++");
        return actors_.back();
    }

    auto last_supervisor() -> actors_framework::supervisor& {
        assert(supervisors_count() > 0);
        return supervisor_.back();
    }

    void enqueue_base(actors_framework::message_ptr msg, actors_framework::execution_device*) override {
        TRACE(msg->command());
        enqueue_base_counter++;
        set_current_message(std::move(msg));
        supervisor_abstract::execute();
    }

private:
    std::unique_ptr<dummy_executor> executor_;
    std::list<actors_framework::actor> actors_;
    std::list<actors_framework::supervisor> supervisor_;
};

uint64_t dummy_supervisor::constructor_counter = 0;
uint64_t dummy_supervisor::destructor_counter = 0;

uint64_t dummy_supervisor::executor_impl_counter = 0;

uint64_t dummy_supervisor::add_actor_impl_counter = 0;
uint64_t dummy_supervisor::add_supervisor_impl_counter = 0;

uint64_t dummy_supervisor::enqueue_base_counter = 0;

namespace storage_names {
    static constexpr auto name = "storage";
    static constexpr auto init = "init";
    static constexpr auto search = "search";
    static constexpr auto add = "add";
    static constexpr auto delete_table = "delete_table";
    static constexpr auto create_table = "create_table";
}; // namespace storage_names

class storage_t final : public actors_framework::basic_async_actor {
public:
    static uint64_t constructor_counter;
    static uint64_t destructor_counter;

    static uint64_t init_counter;
    static uint64_t search_counter;
    static uint64_t add_counter;
    static uint64_t delete_table_counter;
    static uint64_t create_table_counter;

public:
    explicit storage_t(actors_framework::supervisor_abstract* ptr)
        : actors_framework::basic_async_actor(ptr, storage_names::name) {
        add_handler(
            storage_names::init,
            &storage_t::init);

        add_handler(
            storage_names::search,
            &storage_t::search);

        add_handler(
            storage_names::add,
            &storage_t::add);

        add_handler(
            storage_names::delete_table,
            &storage_t::delete_table);

        add_handler(
            storage_names::create_table,
            &storage_t::create_table);

        constructor_counter++;
    }

    ~storage_t() override {
        destructor_counter++;
    }

private:
    void init() {
        init_counter++;
        TRACE("+++");
    }

    void search(std::string& key) {
        search_counter++;
        std::cerr << __func__ << " :: "
                  << "key: " << key
                  << std::endl;
    }

    void add(const std::string& key, const std::string& value) {
        add_counter++;
        std::cerr << __func__ << " :: "
                  << "key: " << key << " | "
                  << "value: " << value << " | "
                  << std::endl;
    }

    void delete_table(const std::string& name, const std::string& path, int type) {
        delete_table_counter++;
        std::cerr << __func__ << " :: "
                  << "table name: " << name << " | "
                  << "path: " << path << " | "
                  << "type: " << type << " | "
                  << std::endl;
    }

    void create_table(const std::string& name, const std::string& path, int type, int time_sync) {
        create_table_counter++;
        std::cerr << __func__ << " :: "
                  << "table name: " << name << " | "
                  << "path: " << path << " | "
                  << "type: " << type << " | "
                  << "time_sync: " << time_sync << " | "
                  << std::endl;
    }
};

uint64_t storage_t::constructor_counter = 0;
uint64_t storage_t::destructor_counter = 0;

uint64_t storage_t::init_counter = 0;
uint64_t storage_t::search_counter = 0;
uint64_t storage_t::add_counter = 0;
uint64_t storage_t::delete_table_counter = 0;
uint64_t storage_t::create_table_counter = 0;

namespace test_handlers_names {
    static constexpr auto name = "test_handlers";
    static constexpr auto ptr_0 = "ptr_0";
    static constexpr auto ptr_1 = "ptr_1";
    static constexpr auto ptr_2 = "ptr_2";
    static constexpr auto ptr_3 = "ptr_3";
    static constexpr auto ptr_4 = "ptr_4";
}; // namespace test_handlers_names

class test_handlers final : public actors_framework::basic_async_actor {
public:
    static uint64_t init_counter;

    static uint64_t ptr_0_counter;
    static uint64_t ptr_1_counter;
    static uint64_t ptr_2_counter;
    static uint64_t ptr_3_counter;
    static uint64_t ptr_4_counter;

public:
    test_handlers(actors_framework::supervisor_abstract* ptr)
        : actors_framework::basic_async_actor(ptr, test_handlers_names::name) {
        init();
        add_handler(
            test_handlers_names::ptr_0,
            []() {
                TRACE("+++");
                ptr_0_counter++;
            });

        add_handler(
            test_handlers_names::ptr_1,
            []() {
                TRACE("+++");
                ptr_1_counter++;
            });
        add_handler(
            test_handlers_names::ptr_2,
            [](int& data) {
                TRACE("+++");
                ptr_2_counter++;
            });
        add_handler(
            test_handlers_names::ptr_3,
            [](int data_1, int& data_2) {
                TRACE("+++");
                std::cerr << "ptr_3 : " << data_1 << " : " << data_2 << std::endl;
                ptr_3_counter++;
            });

        add_handler(
            test_handlers_names::ptr_4,
            [](int data_1, int& data_2, const std::string& data_3) {
                TRACE("+++");
                std::cerr << "ptr_4 : " << data_1 << " : " << data_2 << " : " << data_3 << std::endl;
                ptr_4_counter++;
            });
    }

    ~test_handlers() override = default;

private:
    void init() {
        TRACE("private init");
        init_counter++;
    }
};

uint64_t test_handlers::init_counter = 0;

uint64_t test_handlers::ptr_0_counter = 0;
uint64_t test_handlers::ptr_1_counter = 0;
uint64_t test_handlers::ptr_2_counter = 0;
uint64_t test_handlers::ptr_3_counter = 0;
uint64_t test_handlers::ptr_4_counter = 0;

void dummy_supervisor::create_storage() {
    spawn_actor<storage_t>();
}

void dummy_supervisor::create_test_handlers() {
    spawn_actor<test_handlers>();
}