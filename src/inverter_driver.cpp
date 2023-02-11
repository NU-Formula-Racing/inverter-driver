#include "inverter_driver.h"

#include "temp_converters.h"

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
            inverter_temp_adc = value_2b;
            break;
        case static_cast<uint8_t>(regId::SPEED_ACTUAL):
            rpm = *reinterpret_cast<int16_t*>(&value_2b) / 32767.0f * 5500;
            break;
    }
};

float Inverter::GetMotorTemperature() { return Motor_Thermistor().ADCToTemperature(motor_temp_adc); };

float Inverter::GetInverterTemperature() { return Inverter_Thermistor().ADCToTemperature(inverter_temp_adc); };

float Inverter::GetRPM() { return rpm; };

void Inverter::GetStatus()
{
    // TODO: implement
    RequestRead(0);
    t_byte_1 = 0x40;
};

void Inverter::RequestTorque(uint16_t percent)
{
    uint16_t value = static_cast<uint16_t>(percent * 200.0f / 100.0f);
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
    t_byte_1 = static_cast<uint8_t>(regId::SPEED_ACTUAL);
    Transmission_Msg.EncodeAndSend();
}
void Inverter::RequestPowerStageTemp(uint8_t freq)
{
    RequestRead(freq);
    t_byte_1 = static_cast<uint8_t>(regId::T_IGBT);
    Transmission_Msg.EncodeAndSend();
}
