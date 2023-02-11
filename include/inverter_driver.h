#pragma once

#include "can_interface.h"
#include "virtualTimer.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

class IInverter
{
public:
    virtual float GetMotorTemperature();

    virtual float GetInverterTemperature();

    virtual float GetRPM();

    virtual void GetStatus();

    virtual void RequestTorque(uint16_t percent);
};

/**
 * @brief Class for interacting with the Bamocar D3 Inverter
 *
 */
class Inverter : public IInverter
{
public:
    Inverter(ICAN &can_interface_) : can_interface_(can_interface_){};

    float GetMotorTemperature() override;
    float GetInverterTemperature() override;
    float GetRPM() override;
    void GetStatus() override;
    // set vals
    void RequestTorque(uint16_t percent) override;
    void RXCallback();
    void RequestMotorTemperature(uint8_t freq);
    void RequestPowerStageTemp(uint8_t freq);
    void RequestRPM(uint8_t freq);

    /**
     * @brief Sets the register ID to CAN Read, sets frequency byte, and clears other bytes。 Frequency must be values
     * 0-255, with 0 meaning a one-time request and 255 meaning stop requesting.
     *
     * @param freq
     */
    void RequestRead(uint8_t freq);

private:
    uint16_t motor_temp_adc;
    uint16_t inverter_temp_adc;
    float rpm;
    ICAN &can_interface_;
    // CAN addresses
    const uint16_t kTransmissionId = 0x201;
    const uint16_t kReceiveId = 0x181;

    // Register IDs from data sheet
    enum class regId : uint8_t
    {
        TORQUE_SETPOI = 0x90,
        T_MOTOR = 0x49,
        T_IGBT = 0x4A,
        READ = 0x3D,
        VOUT = 0x8A,
        FUN_ERRCANCEL = 0x8E,
        SPEED_ACTUAL = 0x30,
    };

    CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_regid{};
    CANSignal<uint8_t, 8, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_1{};
    CANSignal<uint8_t, 16, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_2{};
    CANSignal<uint8_t, 24, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_3{};
    CANSignal<uint8_t, 32, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_4{};

    CANTXMessage<5> Transmission_Msg{
        can_interface_, kTransmissionId, 5, 100, t_regid, t_byte_1, t_byte_2, t_byte_3, t_byte_4};

    CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_regid{};
    CANSignal<uint8_t, 8, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_1{};
    CANSignal<uint8_t, 16, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_2{};
    CANSignal<uint8_t, 24, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_3{};
    CANSignal<uint8_t, 32, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_4{};
    CANSignal<uint8_t, 40, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_5{};

    CANRXMessage<6> Receive_Msg{can_interface_,
                                kReceiveId,
                                [this]() { RXCallback(); },
                                r_regid,
                                r_byte_1,
                                r_byte_2,
                                r_byte_3,
                                r_byte_4,
                                r_byte_5};
};
