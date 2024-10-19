
#include "observer.h"
#include <Arduino.h>
#include <Wire.h>

/*
 * Public Functions
 */

void Observer::init()
{
    Wire.begin();

    // let serial console settle
    delay(1000);

    if (sht_.init())
    {
        Serial.print("init(): success\n");
    }
    else
    {
        Serial.print("init(): failed\n");
    }
}

void Observer::run()
{
    if (sht_.readSample())
    {
        Serial.print("sht:\n");
        get_humidity();
        get_temperature();
    }
    else
    {
        Serial.print("Error in readSample()\n");
    }
}

bool Observer::is_humidity_over_threshold()
{
    constexpr humidity_t HUMIDITY_THRESHOLD{ 70.0 };
    bool over_threshold{ false };

    for (float val : last_humidity_values_)
    {
        if (val > HUMIDITY_THRESHOLD)
        {
            over_threshold = true;
        }
        else
        {
            over_threshold = false;
            break;
        }
    }

    return over_threshold;
}

/*
 * Private Functions
 */

void Observer::get_humidity()
{
    humidity_t humidity_value{ sht_.getHumidity() };

    Serial.print("  Humidity: ");
    Serial.print(humidity_value, 2);
    Serial.print("\n");

    last_humidity_values_[last_values_idx_] = humidity_value;
    last_values_idx_++;

    // if idx is greater than array size, reset it.
    if (last_values_idx_ >= last_humidity_values_.size())
    {
        last_values_idx_ = 0;
    }
}

void Observer::get_temperature()
{
    Serial.print("  Temperature:  ");
    Serial.print(sht_.getTemperature(), 2);
    Serial.print("\n");
}
