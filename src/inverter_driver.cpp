#include "inverter_driver.h"

#include "temp_converters.h"

/**
 * @brief Initialize the inverter driver, can take in a SW torque limit of 0-100
 *
 * @param torqueLimit
 */
void Inverter::Initialize(uint8_t torqueLimit = 100)
{
    can_interface.RegisterRXMessage(Receive_Msg);
    sw_torque_limit = torqueLimit;
}

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
        case static_cast<uint8_t>(regId::KERN_STATUS):
            status.En = (value_2b & (1 << static_cast<uint8_t>(Status::bitPos::EN))) != 0;
            status.OK = (value_2b & (1 << static_cast<uint8_t>(Status::bitPos::OK))) != 0;
            status.Rdy = (value_2b & (1 << static_cast<uint8_t>(Status::bitPos::Rdy))) != 0;
            status.Ird_TI = (value_2b & (1 << static_cast<uint8_t>(Status::bitPos::Ird_TI))) != 0;
            status.Ird_TM = (value_2b & (1 << static_cast<uint8_t>(Status::bitPos::Ird_TM))) != 0;
            break;
        case static_cast<uint8_t>(regId::LOGIC_STATUS):
            logic_status = value_2b;
    }
};

/**
 * @brief Get the motor temperature in degrees C
 *
 * @return float
 */
float Inverter::GetMotorTemperature() { return Motor_Thermistor().ADCToTemperature(motor_temp_adc); };

/**
 * @brief Get the inverter temperature in degrees C
 *
 * @return float
 */
float Inverter::GetInverterTemperature() { return Inverter_Thermistor().ADCToTemperature(inverter_temp_adc); };

/**
 * @brief Get the RPM of the motor
 *
 * @return float
 */
float Inverter::GetRPM() { return rpm; };

/**
 * @brief Get the status of the inverter. Options are EN, OK, Rdy, Ird_TI, Ird_TM.
 *
 * @return Status
 */
Status Inverter::GetStatus() { return status; };

/**
 * @brief Get the logic status of the inverter
 *
 * @return uint16_t
 */
uint16_t Inverter::GetLogicStatus() { return logic_status; };

/**
 * @brief Get the requested torque in percent
 *
 * @return uint16_t
 */
uint16_t Inverter::GetRequestedTorque() { return torque_percent; }

/**
 * @brief Request a torque percentage from the inverter from 0-100.
 *
 * @param percent
 */
void Inverter::RequestTorque(uint8_t percent)
{
    uint16_t value = static_cast<uint16_t>(percent * (kTorqueLimit / 100.0f) * (sw_torque_limit / 100.0f));
    torque_percent = value / kTorqueLimit * 100;
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
void Inverter::RequestStatus(uint8_t freq)
{
    RequestRead(freq);
    t_byte_1 = static_cast<uint8_t>(regId::KERN_STATUS);
    Transmission_Msg.EncodeAndSend();

    t_byte_1 = static_cast<uint8_t>(regId::LOGIC_STATUS);
    Transmission_Msg.EncodeAndSend();
}
