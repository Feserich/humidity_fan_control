#ifndef FAN_CONTROL_H
#define FAN_CONTROL_H
/**
 * @file
 *
 * @brief
 *
 */
#include <arduino-timer.h>
#include <atomic>
#include <cstdint>

class FanControl
{
    static constexpr int FAN_GPIO{ D2 };
    static constexpr uint8_t ENABLE_FAN_LEVEL{ LOW };
    static constexpr uint8_t DISABLE_FAN_LEVEL{ HIGH };

public:
    FanControl() = default;

    void init()
    {
        // init GPIO
        pinMode(FAN_GPIO, OUTPUT);
        digitalWrite(FAN_GPIO, DISABLE_FAN_LEVEL);
    }

    void disable_fan()
    {
        if (fan_enabled_)
        {
            Serial.print("Disable Fan GPIO\n");
            digitalWrite(FAN_GPIO, DISABLE_FAN_LEVEL);
            fan_enabled_ = false;
        }
    }

    void enable_fan()
    {
        if (!fan_enabled_)
        {
            Serial.print("Enable Fan GPIO\n");
            digitalWrite(FAN_GPIO, ENABLE_FAN_LEVEL);
            fan_enabled_ = true;
        }
    }

private:
    bool fan_enabled_{ false };
};

#endif