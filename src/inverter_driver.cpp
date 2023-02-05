#include "inverter_driver.h"

void Inverter::RXCallback()
{
    uint32_t value_4b = static_cast<uint32_t>(r_byte_1 | r_byte_2 << 8 | r_byte_3 << 16 | r_byte_4 << 24);
    uint16_t value_2b = static_cast<uint16_t>(r_byte_1 | r_byte_2 << 8);
    switch (r_regid)
    {
        case static_cast<uint8_t>(regId::T_MOTOR):
            motor_temp_adc = value_2b;
            break;
        case static_cast<uint8_t>(regId::T_IGBT):
            inverter_temp = value_2b;
            break;
        case static_cast<uint8_t>(regId::SPEED_RPMMAX_INT):
            rpm = value_2b;
            break;
    }
};

uint32_t Inverter::GetMotorTemperature() { return motor_temp_adc; };

uint32_t Inverter::GetInverterTemperature() { return inverter_temp; };

uint32_t Inverter::GetRPM() { return rpm; };

void Inverter::GetStatus()
{
    RequestRead(0);
    t_byte_1 = 0x40;
};

void Inverter::RequestTorque(float percent)
{
    int16_t value = percent * 32767;
    t_byte_1 = (value >> 0) & 0xFF;
    t_byte_2 = (value >> 8) & 0xFF;
    t_byte_3 = 0;
    t_byte_4 = 0;
    t_regid = static_cast<uint8_t>(regId::TORQUE_SETPOI);
    Transmission_Msg.EncodeAndSend();
};
void Inverter::RequestRead(uint8_t freq)
{
    t_regid = static_cast<uint8_t>(regId::READ);
    t_byte_2 = freq;
    // clearing the other bytes (clearing 1 may not be necessary)
    t_byte_1 = 0;
    t_byte_3 = 0;
    t_byte_4 = 0;
}
void Inverter::RequestMotorTemperature(uint8_t freq)
{
    // request to READ values
    RequestRead(freq);
    t_byte_1 = static_cast<uint8_t>(regId::T_MOTOR);
    Transmission_Msg.EncodeAndSend();
}
void Inverter::RequestRPM(uint8_t freq)
{
    RequestRead(freq);
    t_byte_1 = static_cast<uint8_t>(regId::SPEED_RPMMAX_INT);
}
void Inverter::RequestPowerStageTemp(uint8_t freq)
{
    RequestRead(freq);
    t_byte_1 = static_cast<uint8_t>(regId::T_MOTOR);
    Transmission_Msg.EncodeAndSend();
}
