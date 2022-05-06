#pragma once
#include "ATmega32Utils.hpp"

//NOTE Exact part is unknown, no or inaccurate documentation
class Speaker
{
public:
    Speaker(const Port&, Port::Pin);
    
    void emit(uint16_t freq, uint16_t duration) const; //freq in Hz, duration in milliseconds

private:
    const Port& m_port;
    Port::Pin m_pin;
};