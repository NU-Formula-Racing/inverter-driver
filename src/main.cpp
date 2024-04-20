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
    if (millis() / 2000 % 2)
    {
        // Serial.println("Requesting 200 torque");
        inverter.RequestTorque(100);
    }
    else
    {
        // Serial.println("Requesting 0 torque");
        inverter.RequestTorque(0);
    }
};

void printEverything()
{
    Serial.print("Motor temp:");
    Serial.print(inverter.GetMotorTemperature());
    Serial.print("\n");

    Serial.print("Inverter temp:");
    Serial.print(inverter.GetInverterTemperature());
    Serial.print("\n");

    Serial.print("RPM:");
    Serial.print(inverter.GetRPM());
    Serial.print("\n");

    Serial.print("Requested torque:");
    Serial.print(inverter.GetRequestedTorque());
    Serial.print("\n");

    Serial.print("EN:");
    Serial.print(inverter.GetStatus().En);
    Serial.print(" OK:");
    Serial.print(inverter.GetStatus().OK);
    Serial.print(" Rdy:");
    Serial.print(inverter.GetStatus().Rdy);
    Serial.print(" Ird_TI:");
    Serial.print(inverter.GetStatus().Ird_TI);
    Serial.print(" Ird_TM:");
    Serial.print(inverter.GetStatus().Ird_TM);
    Serial.print("\n");

    Serial.print("Logic status:");
    Serial.print(inverter.GetLogicStatus(), BIN);
    Serial.print("\n");
}

void requestInverterReadings()
{
    inverter.RequestMotorTemperature(200);
    inverter.RequestPowerStageTemp(200);
    inverter.RequestRPM(200);
    inverter.RequestStatus(200);
}

void setup()
{
    Serial.begin(115200);
    Serial.println("Started");
    can_bus.Initialize(ICAN::BaudRate::kBaud1M);
    inverter.Initialize();

    timer_group.AddTimer(1000, requestTorque);
    timer_group.AddTimer(1000, printEverything);
    timer_group.AddTimer(1000, requestInverterReadings);
    timer_group.AddTimer(10, []() { can_bus.Tick(); });
};

void loop() { timer_group.Tick(millis()); };