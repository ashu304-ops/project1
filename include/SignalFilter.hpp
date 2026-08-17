#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <cstddef>

class SignalFilter {
public:
    explicit SignalFilter(size_t windowSize = 5) : windowSize_(windowSize) {
        history_.reserve(windowSize);
    }

    // Add new reading and compute moving average using STL algorithms
    float filter(float rawValue) {
        if (history_.size() >= windowSize_) {
            history_.erase(history_.begin()); // Maintain sliding window
        }
        history_.push_back(rawValue);

        // STL algorithm to compute sum
        float sum = std::accumulate(history_.begin(), history_.end(), 0.0f);
        float avg = sum / static_cast<float>(history_.size());

        // STL algorithm to clamp within safe operational limits (-5.0g to +5.0g)
        return std::clamp(avg, -5.0f, 5.0f);
    }

    // Get peak G-force in current window using std::max_element
    float getPeak() const {
        if (history_.empty()) return 0.0f;
        auto maxIt = std::max_element(history_.begin(), history_.end(),
            [](float a, float b) { return std::abs(a) < std::abs(b); });
        return *maxIt;
    }

private:
    size_t windowSize_;
    std::vector<float> history_;
};