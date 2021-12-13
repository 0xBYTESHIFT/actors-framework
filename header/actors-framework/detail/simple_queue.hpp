#pragma once
#include <atomic>
#include <mutex>
#include <queue>

namespace actors_framework::detail {
    template<class T>
    class simple_queue {
        std::mutex lock_;
        std::queue<T> queue_;
        std::atomic<size_t> size_;

    public:
        auto enqueue(T data) -> void {
            std::lock_guard lg{lock_};
            queue_.push(std::move(data));
            ++size_;
        };

        auto try_pop(T& data) -> bool {
            std::lock_guard lg{lock_};
            if (queue_.empty()) {
                return false;
            }
            data = std::move(queue_.front());
            queue_.pop();
            --size_;
            return true;
        }

        auto count() -> size_t {
            return size_;
        }
    };
}; // namespace actors_framework::detail
