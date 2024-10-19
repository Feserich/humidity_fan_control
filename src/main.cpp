#include <Arduino.h>
#include <WiFi.h>
#include <cstdint>

#include <arduino-timer.h>

#include "observer.h"

Observer observer{};
Timer<2> timer;
constexpr int FAN_GPIO = D2;

bool read_sensor_values(void*)
{
    // read from SHT sensor
    observer.run();

    // keep timer active
    return true;
}

bool control_fan(void*)
{
    if (observer.is_humidity_over_threshold())
    {
        Serial.print("Over threshold, enable fan\n");

        digitalWrite(FAN_GPIO, LOW);
    }
    else
    {
        Serial.print("Under threshold, disable fan\n");
        digitalWrite(FAN_GPIO, HIGH);
    }

    // keep timer active
    return true;
}

void setup()
{
    // init GPIO
    pinMode(FAN_GPIO, OUTPUT);
    digitalWrite(FAN_GPIO, HIGH);

    Serial.begin(9600);

    // init observer
    observer.init();

    // Disable WiFi and BT for low energy
    btStop();
    WiFi.mode(WIFI_OFF);

    // Set cyclic timer
    constexpr std::uint32_t READ_SENSOR_INTERVALL{ 5 * 1000 };
    timer.every(READ_SENSOR_INTERVALL, read_sensor_values);

    constexpr std::uint32_t CONTROL_FAN_INTERVALL{ 30 * 1000 };
    timer.every(CONTROL_FAN_INTERVALL, control_fan);
}

void loop()
{
    // only run the timer lib in the endless loop
    timer.tick();
}
