#pragma once

#include <array>
#include <atomic>
#include <optional>
#include <cstddef>

template <typename T, std::size_t Capacity>
class LockFreeRingBuffer {
public:
    // Called strictly by Control Thread (Producer)
    bool push(const T& item) noexcept {
        const auto currentTail = tail_.load(std::memory_order_relaxed);
        const auto nextTail = (currentTail + 1) % Capacity;

        if (nextTail == head_.load(std::memory_order_acquire)) {
            return false; // Queue full - soft drop
        }

        buffer_[currentTail] = item;
        tail_.store(nextTail, std::memory_order_release);
        return true;
    }

    // Called strictly by Telemetry Thread (Consumer)
    std::optional<T> pop() noexcept {
        const auto currentHead = head_.load(std::memory_order_relaxed);

        if (currentHead == tail_.load(std::memory_order_acquire)) {
            return std::nullopt; // Queue empty
        }

        T item = buffer_[currentHead];
        head_.store((currentHead + 1) % Capacity, std::memory_order_release);
        return item;
    }

private:
    std::array<T, Capacity> buffer_{};
    alignas(64) std::atomic<std::size_t> head_{0};
    alignas(64) std::atomic<std::size_t> tail_{0};
};