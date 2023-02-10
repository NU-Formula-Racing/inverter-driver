#pragma once
#include <map>

/**
 * @brief Returns interpolated value given a mapping
 *
 * @param map
 * @param x
 * @return float
 */
float Interpolate(const std::map<float, float> &map, float x)
{
    typedef std::map<float, float>::const_iterator iterator;
    iterator ub = map.upper_bound(x);
    if (ub == map.end())
    {
        return (--ub)->second;
    }
    if (ub == map.begin())
    {
        return ub->second;
    }
    iterator lb = ub;
    lb--;
    const float delta = (x - lb->first) / (ub->first - lb->first);
    return delta * ub->second + (1 - delta) * lb->second;
}

/**
 * @brief Class for converting ADC count to temperature for the KTY81-210 thermistor
 *
 */
class Motor_Thermistor
{
public:
    float ADCToTemperature(uint16_t ADC)
    {
        // Conversion eq: R1 * (1 - ADC/ADCMax) = R2*ADC/ADCMax
        float rhs = kSeriesResistance * ADC / kADCMax;
        float lhs = 1 - (ADC / kADCMax);
        float resistance = rhs / lhs;
        return Interpolate(resistance_to_temperature_lut_, resistance);
    }

private:
    // consts from datasheet
    std::map<float, float> resistance_to_temperature_lut_{
        {980, -55}, {1030, -50}, {1135, -40}, {1247, -30}, {1367, -20}, {1495, -10}, {1630, 0},   {1772, 10},
        {1922, 20}, {2000, 25},  {2080, 30},  {2245, 40},  {2417, 50},  {2597, 60},  {2785, 70},  {2980, 80},
        {3182, 90}, {3392, 100}, {3607, 110}, {3817, 120}, {3915, 125}, {4008, 130}, {4166, 140}, {4280, 150}};
    std::map<float, float> temperature_to_resistance_lut_{
        {-55, 980}, {-50, 1030}, {-40, 1135}, {-30, 1247}, {-20, 1367}, {-10, 1495}, {0, 1630},   {10, 1772},
        {20, 1922}, {25, 2000},  {30, 2080},  {40, 2245},  {50, 2417},  {60, 2597},  {70, 2785},  {80, 2980},
        {90, 3182}, {100, 3392}, {110, 3607}, {120, 3817}, {125, 3915}, {130, 4008}, {140, 4166}, {150, 4280}};
    const float kSeriesResistance{4000.0f};
    const float kADCMax{1 << 15};
};

/**
 * @brief Class for converting ADC count to IGBT temperature
 *
 */
class Inverter_Thermistor
{
public:
    float ADCToTemperature(float ADC) { return Interpolate(adc_to_temperature_lut_, ADC); }

private:
    // consts from datasheet
    std::map<float, float> adc_to_temperature_lut_{
        {28480, 125}, {28179, 120}, {27851, 115}, {27497, 110}, {27114, 105}, {26702, 100}, {26261, 95},  {25792, 90},
        {25296, 85},  {24775, 80},  {24232, 75},  {23671, 70},  {23097, 65},  {22515, 60},  {21933, 55},  {21357, 50},
        {20793, 45},  {20250, 40},  {19733, 35},  {19247, 30},  {18797, 25},  {18387, 20},  {18017, 15},  {17688, 10},
        {17400, 5},   {17151, 0},   {16938, -5},  {16757, -10}, {16609, -15}, {16487, -20}, {16387, -25}, {16308, -30}};
};