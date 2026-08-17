#pragma once

#include <array>
#include <cstddef>
#include <optional>

template <typename T, std::size_t Capacity>
class RingBuffer {
public:
    constexpr RingBuffer() noexcept = default;

    bool push(const T& item) noexcept {
        data_[head_] = item;
        head_ = (head_ + 1) % Capacity;
        if (size_ < Capacity) {
            ++size_;
        } else {
            tail_ = (tail_ + 1) % Capacity; // Overwrite oldest
        }
        return true;
    }

    [[nodiscard]] std::optional<T> pop() noexcept {
        if (size_ == 0) return std::nullopt;
        T item = data_[tail_];
        tail_ = (tail_ + 1) % Capacity;
        --size_;
        return item;
    }

    [[nodiscard]] constexpr std::size_t size() const noexcept { return size_; }
    [[nodiscard]] constexpr std::size_t capacity() const noexcept { return Capacity; }
    [[nodiscard]] bool empty() const noexcept { return size_ == 0; }

private:
    std::array<T, Capacity> data_{};
    std::size_t head_{0};
    std::size_t tail_{0};
    std::size_t size_{0};
};