#include "inverter_driver.h"

void Inverter::RXCallback()
{
    float value = static_cast<uint8_t>(r_byte_1 | 
                                        r_byte_2 << 8 | 
                                        r_byte_3 << 16 | 
                                        r_byte_4 << 24);
    switch (r_regid){
        case static_cast<uint8_t>(regId::T_MOTOR):
            motor_temp = value;
            break;
        case static_cast<uint8_t>(regId::T_IGBT):
            inverter_temp = value;
            break;    
    }
};

float Inverter::GetMotorTemperature(){
    //bruh
};

float Inverter::GetInverterTemperature(){
    //bruh
};

float Inverter::GetRPM(){
    //bruh
};

void Inverter::GetStatus(){
    //bruh
};

void Inverter::RequestTorque(float percent)
{
    int16_t value = percent * 32766;
    t_byte_1 = (value>>0) & 0xFF;
    t_byte_2 = (value>>8) & 0xFF;
    t_byte_3 = 0;
    t_byte_4 = 0;
    t_regid = static_cast<uint8_t>(regId::TORQUE_SETPOI);
    Transmission_Msg.EncodeAndSend();
};