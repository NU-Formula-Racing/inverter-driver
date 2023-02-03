#include "inverter_driver.h"

void Inverter::RXCallback()
{
    float value = static_cast<uint8_t>(r_byte_1 << 48 | 
                                        r_byte_2 << 40 | 
                                        r_byte_3 << 32 | 
                                        r_byte_4 << 24 | 
                                        r_byte_5 << 16 | 
                                        r_byte_6 << 8 | 
                                        r_byte_7);
    switch (r_regid){
        case static_cast<uint8_t>(regId::T_MOTOR):
            motor_temp = value;
            break;
        case static_cast<uint8_t>(regId::T_IGBT):
            inverter_temp = value;
            break;    
    }
}
void Inverter::GetStatus(){
    //bruh
}

void Inverter::RequestTorque(float percent)
{
    int16_t value = percent * 32766;
    t_byte_1 = (value>>8) & 0xFF;
    t_byte_2 = (value>>0) & 0xFF;
    t_byte_3 = 0;
    t_byte_4 = 0;
    t_byte_5 = 0;
    t_byte_6 = 0;
    t_byte_7 = 0;
    t_regid = static_cast<uint8_t>(regId::TORQUE_SETPOI);
    Transmission_Msg.EncodeAndSend();
}