#pragma once
#include "ATMega32Utils.hpp"
    
class HCSR04
{
public:
    static constexpr uint16_t minDistance = 2; //Centimeter
    static constexpr uint16_t maxDistance = 400; //Centimeter

    HCSR04(const Port&, Port::Pin trigger, Port::Pin echo);
    
    uint16_t getDistance() const; //Centimeter, NOTE allow 60ms between consecutive calls
    
private:
    uint16_t ping() const; //Microsecond
    uint16_t convert(uint16_t time) const; //Centimeter
    
    static constexpr uint8_t conversionScalar = 58;

    const Port& m_port;
    Port::Pin m_triggerPin;
    Port::Pin m_echoPin;
};