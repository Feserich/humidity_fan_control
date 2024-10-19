#include <Arduino.h>
#include <WiFi.h>
#include <cstdint>

#include "SHTSensor.h"
#include "fan_control.h"
#include "observer.h"

/*
 *  Const var
 */

constexpr float UPPER_HUMIDITY_THRESHOLD{ 70.0 };
constexpr float LOWER_HUMIDITY_THRESHOLD{ 67.0 };

/*
 *  Function Declaration
 */

float read_sensor_values_cbk();
bool cyclic_task_read_sensor_cbk(void*);
bool cyclic_task_threshold_check_cbk(void*);
bool cyclic_task_check_button_status(void*);

/*
 *  Const var
 */
constexpr std::uint32_t ENABLE_FAN_BUTTON_INTERVALL{ 24 * 60 * 60 * 1000 };
constexpr std::uint32_t READ_SENSOR_INTERVALL{ 5 * 60 * 1000 };
constexpr std::uint32_t CHECK_THRESHOLD_INTERVALL{ 30 * 60 * 1000 };
constexpr int BUTTON_GPIO{ D7 };

/*
 *  Static var
 */
using timer_type = Timer<4>;
timer_type timer;
timer_type::Task threshold_check_task{ nullptr };
SHTSensor sht_{ SHTSensor::SHT2X };
Observer<float, 5> observer{ []() { return read_sensor_values_cbk(); }, UPPER_HUMIDITY_THRESHOLD, LOWER_HUMIDITY_THRESHOLD };
FanControl fan_control{};
bool enable_fan_24h_flag{ false };

/*
 *  Function Definition
 */

float read_sensor_values_cbk()
{
    float humidity_value{ 0.0 };

    if (sht_.readSample())
    {
        Serial.print("sht:\n");
        humidity_value = sht_.getHumidity();

        Serial.print("  Humidity: ");
        Serial.print(humidity_value, 2);
        Serial.print("\n");

        Serial.print("  Temperature:  ");
        Serial.print(sht_.getTemperature(), 2);
        Serial.print("\n");
    }
    else
    {
        Serial.print("Error in readSample()\n");
    }

    return humidity_value;
}

bool cyclic_task_read_sensor_cbk(void*)
{
    // read from SHT sensor
    observer.read_sensor();

    // keep timer active
    return true;
}

bool cyclic_task_threshold_check_cbk(void*)
{
    if (observer.is_over_threshold())
    {
        Serial.print("Over threshold, enable fan\n");
        fan_control.enable_fan();
    }
    else
    {
        Serial.print("Under threshold, disable fan\n");
        fan_control.disable_fan();
    }

    // keep timer active
    return true;
}

bool disable_fan_cbk(void*)
{
    Serial.print("Disable Fan callback\n");
    fan_control.disable_fan();
    // start again the humidity observer task
    threshold_check_task = timer.every(CHECK_THRESHOLD_INTERVALL, cyclic_task_threshold_check_cbk);

    return false;
}

bool cyclic_task_check_button_status(void*)
{
    if (enable_fan_24h_flag)
    {
        Serial.print("Enable Fan for 24h\n");
        // Cancel the humidity observer task
        if (threshold_check_task != nullptr)
        {
            timer.cancel(threshold_check_task);
        }

        fan_control.enable_fan();

        // Disable the Fan in 24h

        timer.in(ENABLE_FAN_BUTTON_INTERVALL, disable_fan_cbk);
        enable_fan_24h_flag = false;
    }

    return true;
}

void button_pressed_ISR() { enable_fan_24h_flag = true; }

void setup()
{
    // init GPIO
    fan_control.init();
    pinMode(BUTTON_GPIO, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(BUTTON_GPIO), button_pressed_ISR, RISING);

    Serial.begin(9600);

    // init SHT sensor
    Wire.begin();

    if (sht_.init())
    {
        Serial.print("init(): success\n");
    }
    else
    {
        Serial.print("init(): failed\n");
    }

    // Disable WiFi and BT for low energy
    btStop();
    WiFi.mode(WIFI_OFF);

    // Set cyclic timer
    threshold_check_task = timer.every(CHECK_THRESHOLD_INTERVALL, cyclic_task_threshold_check_cbk);
    timer.every(READ_SENSOR_INTERVALL, cyclic_task_read_sensor_cbk);
    timer.every(1000, cyclic_task_check_button_status);
}

void loop()
{
    // only run the timer lib in the endless loop
    timer.tick();
}
