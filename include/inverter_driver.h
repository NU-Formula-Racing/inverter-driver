#pragma once

#include "can_interface.h"
#include "virtualTimer.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

class Status
{
public:
    bool OK = 0;
    bool Rdy = 0;
    bool Ird_TI = 0;
    bool Ird_TM = 0;
    bool En = 0;
    enum class bitPos : uint8_t
    {
        EN = 0,
        OK = 4,
        Rdy = 14,
        Ird_TI = 23,
        Ird_TM = 26,
    };
};

class IInverter
{
public:
    virtual void Initialize(uint8_t torqueLimit) = 0;

    virtual float GetMotorTemperature();

    virtual float GetInverterTemperature();

    virtual float GetRPM();

    virtual Status GetStatus();

    virtual uint16_t GetLogicStatus();

    virtual uint16_t GetRequestedTorque();

    virtual void RequestTorque(uint8_t percent);
};

/**
 * @brief Class for interacting with the Bamocar D3 Inverter
 *
 */
class Inverter : public IInverter
{
public:
    Inverter(ICAN &can_interface_) : can_interface(can_interface_){};

    void Initialize(uint8_t torqueLimit = 100) override;
    float GetMotorTemperature() override;
    float GetInverterTemperature() override;
    float GetRPM() override;
    Status GetStatus() override;
    uint16_t GetLogicStatus() override;
    uint16_t GetRequestedTorque() override;
    // set vals
    void RequestTorque(uint8_t percent) override;
    void RXCallback();
    void RequestMotorTemperature(uint8_t freq);
    void RequestPowerStageTemp(uint8_t freq);
    void RequestRPM(uint8_t freq);
    void RequestStatus(uint8_t freq);

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
    uint16_t logic_status;
    float rpm;
    Status status;
    uint8_t torque_percent;
    ICAN &can_interface;
    const uint16_t kTorqueLimit =
        32767;  // set to 200 for testing purposes only, should be 32767 to achieve maximum torque at 100%
    // CAN addresses
    const uint16_t kTransmissionId = 0x201;
    const uint16_t kReceiveId = 0x181;
    uint8_t sw_torque_limit;

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
        TORQUE_OUT = 0xA0,
        KERN_STATUS = 0x40,
        LOGIC_STATUS = 0xD8,
    };

    CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_regid{};
    CANSignal<uint8_t, 8, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_1{};
    CANSignal<uint8_t, 16, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_2{};
    CANSignal<uint8_t, 24, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_3{};
    CANSignal<uint8_t, 32, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_4{};

    CANTXMessage<5> Transmission_Msg{
        can_interface, kTransmissionId, 5, 100, t_regid, t_byte_1, t_byte_2, t_byte_3, t_byte_4};

    CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_regid{};
    CANSignal<uint8_t, 8, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_1{};
    CANSignal<uint8_t, 16, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_2{};
    CANSignal<uint8_t, 24, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_3{};
    CANSignal<uint8_t, 32, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_4{};
    CANSignal<uint8_t, 40, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_5{};

    CANRXMessage<6> Receive_Msg{can_interface,
                                kReceiveId,
                                [this]() { RXCallback(); },
                                r_regid,
                                r_byte_1,
                                r_byte_2,
                                r_byte_3,
                                r_byte_4,
                                r_byte_5};
};
