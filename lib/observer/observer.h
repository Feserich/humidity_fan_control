#ifndef OBSERVER_H
#define OBSERVER_H

/**
 * @file
 *
 * @brief
 *
 */

#include "SHTSensor.h"
#include <array>
#include <cstdint>

using humidity_t = float;

constexpr std::size_t NUM_VALUES_TO_OBSERVE{ 5 };

class Observer
{

public:
    Observer() = default;

    /// @brief Init sensor
    void init();

    /// @brief Read temperature and humidity
    void run();

    /// @brief Check if humidity is over threshold in the last values
    /// @return True if all observed values are above the threshold
    bool is_humidity_over_threshold();

private:
    /// @brief Read humidity from sensor
    void get_humidity();

    /// @brief Read temperature from sensor
    void get_temperature();

    SHTSensor sht_{ SHTSensor::SHT2X };

    std::array<humidity_t, NUM_VALUES_TO_OBSERVE> last_humidity_values_{ 0 };

    std::size_t last_values_idx_{ 0 };
};

#endif