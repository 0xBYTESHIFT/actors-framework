
| compiler  | Master | Develop |
|:---:|:---:|:---:|
| gcc 9 - 11 | |[![ubuntu](https://github.com/0xBYTESHIFT/actors-framework/actions/workflows/ubuntu_gcc.yaml/badge.svg?branch=main)](https://github.com/0xBYTESHIFT/actors-framework/actions/workflows/ubuntu_gcc.yaml) |
|clang 8 - 11 | |[![ubuntu](https://github.com/0xBYTESHIFT/actors-framework/actions/workflows/ubuntu_clang.yaml/badge.svg?branch=main)](https://github.com/0xBYTESHIFT/actors-framework/actions/workflows/ubuntu_clang.yaml)|

actors-framework
========================

actors-framework is an open source C++17 virtual actor model implementation featuring lightweight & fast concurrent message passing.

## Example

### class method mode 

```C++

#include <actors-framework/core.hpp>

using actors_framework::basic_async_actor;

class key_value_storage_t final : public basic_async_actor {
public:
    explicit key_value_storage_t(dummy_supervisor &ref) : basic_async_actor(ref, "storage") {

        add_handler(
                "init",
                &key_value_storage_t::init
        );

        add_handler(
                "search",
                &key_value_storage_t::search
        );

        add_handler(
                "add",
                &key_value_storage_t::add
        );

    }

    ~key_value_storage_t() override = default;

    void init() {
       /// ...
    }

private:

    void search(std::string &key) {
        /// ...
    }

    void add(const std::string &key, const std::string &value) {
        /// ...
    }
    
};

```

## lambda mode

```C++

#include <actors-framework/core.hpp>

using actors_framework::basic_async_actor;

class storage_t final : public basic_async_actor {
public:
    storage_t() : basic_async_actor(nullptr, "storage") {
        add_handler(
                "update",
                [](storage_t & /*ctx*/, query_t& query) -> void {}
        );

        add_handler(
                "find",
                [](storage_t & /*ctx*/, query_t& query) -> void {}
        );

        add_handler(
                "remove",
                [](storage_t & /*ctx*/, query_t& query) -> void {}
        );
    }

    ~storage_t() override = default;
};

```

## Dependencies

* CMake >= 3.0
