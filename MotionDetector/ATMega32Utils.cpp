#include "ATMega32Utils.hpp"

Port portA(Port::Letter::A);
Port portB(Port::Letter::B);
Port portC(Port::Letter::C);
Port portD(Port::Letter::D);

void waitMs(uint16_t milliseconds)
{
    Register counter(TCNT0);
    Register control(TCCR0);
    Register interrupt(TIFR);
    
    for(; milliseconds > 0; milliseconds--)
    {
        counter.setValue(UINT8_MAX + 1 - (F_CPU / 64) * 0.001); //Set counter initial value
        control.setBits(CS00, CS01); //64 prescaler (counter increments every 8 microseconds)
        while(!interrupt.getBits(TOV0)); //Wait for overflow
        control.clearBits(CS00, CS01); //Stop (no clock source)
        interrupt.setBits(TOV0); //Clear overflow flag
    }
}