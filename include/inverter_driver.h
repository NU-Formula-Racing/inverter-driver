#pragma once

#include "virtualTimer.h"
#include "can_interface.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

class IInverter
{
public:
    virtual void Initialize();

    virtual float GetMotorTemperature();

    virtual float GetInverterTemperature();

    virtual float GetRPM();

    virtual void GetStatus();

    virtual void RequestTorque(float percent);
};

class Inverter : public IInverter
{
    Inverter(ICAN &can_interface_): can_interface_(can_interface_){};

    virtual float GetMotorTemperature() override ;
    virtual float GetInverterTemperature() override ;
    virtual float GetRPM() override;
    virtual void GetStatus() override;
    virtual void RequestTorque(float percent) override;
    virtual void RXCallback();

    private:
        float motor_temp;
        float inverter_temp;
        float rpm;
        ICAN &can_interface_;
        // CAN addresses
        const uint16_t kTransmissionId = 0x210;
        const uint16_t kReceiveId = 0x180;
    
        CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_regid{};
        CANSignal<uint8_t, 8, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_1{};
        CANSignal<uint8_t, 16, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_2{};
        CANSignal<uint8_t, 24, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_3{};
        CANSignal<uint8_t, 32, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_4{};
        
        CANTXMessage<5> Transmission_Msg{can_interface_, kTransmissionId, 5, 100, t_regid, t_byte_1, t_byte_2, t_byte_3, t_byte_4};
        
        CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_regid{};
        CANSignal<uint8_t, 8, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_1{};
        CANSignal<uint8_t, 16, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)>r_byte_2{};
        CANSignal<uint8_t, 24, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_3{};
        CANSignal<uint8_t, 32, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_4{};
        CANSignal<uint8_t, 40, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_5{};

        CANRXMessage<6> Receive_Msg{can_interface_, kReceiveId, [this]() { RXCallback();}, r_regid, r_byte_1, r_byte_2, r_byte_3, r_byte_4, r_byte_5};

        enum class regId:uint8_t 
        {
            TORQUE_SETPOI = 0x90,
            T_MOTOR = 0x49,
            T_IGBT = 0x4A,
        };
};
