#pragma once

#include <stdint.h>

#include <algorithm>
#include <array>
#include <chrono>
#include <functional>
#include <vector>

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

    virtual void RequestTorque();
};

class Inverter : public IInverter
{
    Inverter(ICAN &can_interface_): can_interface_(can_interface_){};

    void Initialize(){
        //bruh
    }

    private:
        ICAN &can_interface_;
        const int kTransmissionId = 0x210;
        const int kReceiveId = 0x180;
    
        CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_regid{};
        CANSignal<uint8_t, 8, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_1{};
        CANSignal<uint8_t, 16, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_2{};
        CANSignal<uint8_t, 24, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_3{};
        CANSignal<uint8_t, 32, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_4{};
        CANSignal<uint8_t, 40, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_5{};
        CANSignal<uint8_t, 48, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_6{};
        CANSignal<uint8_t, 64, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> t_byte_7{};

        CANTXMessage<8> Transmission_Msg{can_interface_, kTransmissionId, 8, 100, t_regid, t_byte_1, t_byte_2, t_byte_3, t_byte_4, t_byte_5, t_byte_6, t_byte_7};
        
        CANSignal<uint8_t, 0, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_regid{};
        CANSignal<uint8_t, 8, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_1{};
        CANSignal<uint8_t, 16, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)>r_byte_2{};
        CANSignal<uint8_t, 24, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_3{};
        CANSignal<uint8_t, 32, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_4{};
        CANSignal<uint8_t, 40, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_5{};
        CANSignal<uint8_t, 48, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_6{};
        CANSignal<uint8_t, 64, 8, CANTemplateConvertFloat(1), CANTemplateConvertFloat(0)> r_byte_7{};

        CANRXMessage<8> Receive_Msg{can_interface_, kReceiveId, r_regid, r_byte_1, r_byte_2, r_byte_3, r_byte_4, r_byte_5, r_byte_6, r_byte_7};


};
