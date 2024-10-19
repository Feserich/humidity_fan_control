#ifndef OBSERVER_H
#define OBSERVER_H

/**
 * @file
 *
 * @brief
 *
 */

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>

using humidity_t = float;

// constexpr std::size_t NUM_VALUES_TO_OBSERVE{ 5 };

template <typename T, std::size_t NUM_VALUES_TO_OBSERVE> class Observer
{

public:
    Observer() = delete;

    explicit Observer(std::function<T(void)> get_sensor_value, T const upper_threshold)
        : get_sensor_value_{ get_sensor_value }
        , upper_threshold_{ upper_threshold }
        , lower_threshold_{ upper_threshold }
    {
    }

    explicit Observer(std::function<T(void)> get_sensor_value, T upper_threshold, T lower_threshold)
        : get_sensor_value_{ get_sensor_value }
        , upper_threshold_{ upper_threshold }
        , lower_threshold_{ lower_threshold }
    {
    }

    /// @brief Read temperature and humidity
    void read_sensor()
    {
        T current_value{ get_sensor_value_() };

        last_values_idx_++;
        // if idx is greater than array size, reset it.
        if (last_values_idx_ >= last_sensor_values_.size())
        {
            last_values_idx_ = 0;
        }

        last_sensor_values_[last_values_idx_] = current_value;
    }

    T get_latest_value() { return last_sensor_values_[last_values_idx_]; }

    /// @brief Check if sensor value is over threshold in the last values
    /// @return True if all observed values are above the threshold
    bool is_over_threshold()
    {
        if (over_threshold_)
        {
            // if all values are below lower threshold, then set threshold flag to false
            if (std::all_of(last_sensor_values_.cbegin(), last_sensor_values_.cend(),
                            [this](T val) { return (val < this->lower_threshold_); }))
            {
                over_threshold_ = false;
            }
        }
        else
        {
            // if all values are above upper threshold, then set threshold flag to true
            if (std::all_of(last_sensor_values_.cbegin(), last_sensor_values_.cend(),
                            [this](T val) { return (val > this->upper_threshold_); }))
            {
                over_threshold_ = true;
            }
        }

        return over_threshold_;
    }

private:
    std::function<T(void)> get_sensor_value_;

    const T upper_threshold_;

    const T lower_threshold_;

    bool over_threshold_{ false };

    std::array<T, NUM_VALUES_TO_OBSERVE> last_sensor_values_{ 0 };

    std::size_t last_values_idx_{ 0 };
};

#endif