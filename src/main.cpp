#include <Arduino.h>

/**
 * This file is meant to show how to use the Inverter driver.
 */

#if defined(ARDUINO_TEENSY40) || defined(ARDUINO_TEENSY41)
#include "teensy_can.h"
// The bus number is a template argument for Teensy: TeensyCAN<bus_num>
TeensyCAN<1> can_bus{};
#endif

#ifdef ARDUINO_ARCH_ESP32
#include "esp_can.h"
// The tx and rx pins are constructor arguments to ESPCan, which default to TX = 5, RX = 4
ESPCAN can_bus{};
#endif

/**
 * @brief You also need to include the VirtualTimers library in order to use VirtualTimers and VirtualTimerGroups
 *
 */
#include "virtualTimer.h"
/**
 * @brief Include inverter driver
 */
#include "inverter_driver.h"

// Make a VirtualTimerGroup to add your timers to
VirtualTimerGroup timer_group{};

// Initialize inverter driver
Inverter inverter{can_bus};

void requestTorque()
{
    Serial.println("Requesting 200 torque");
    inverter.RequestTorque(0.006);
    delay(2000);
    Serial.println("Requesting 0 torque");
    inverter.RequestTorque(0);
};

void printTemperature() { Serial.println(inverter.GetMotorTemperature()); }

void setup()
{
    Serial.begin(115200);
    Serial.println("Started");
    can_bus.Initialize(ICAN::BaudRate::kBaud1M);
    inverter.RequestMotorTemperature(200);
    timer_group.AddTimer(5000, requestTorque);
    timer_group.AddTimer(1000, printTemperature);
    timer_group.AddTimer(10, [](){can_bus.Tick();});
};

void loop() { timer_group.Tick(millis()); };