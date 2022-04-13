#include "ATMega32Utils.hpp"

Port portA(Port::Letter::A);
Port portB(Port::Letter::B);
Port portC(Port::Letter::C);
Port portD(Port::Letter::D);

void waitMs(uint16_t milliseconds)
{
    Register8 counterH(TCNT1H);
    Register8 counterL(TCNT1L);
    Register8 control(TCCR1B);
    Register8 flags(TIFR);
    
    //Set how many counter ticks until desired elapsed time
    uint16_t value = UINT16_MAX + 1 - (F_CPU / (1024 * 1'000.f) * milliseconds);
    counterH.setValue((value & 0xFF00) >> 8);
    counterL.setValue(value & 0xFF);
    
    control.setBits(CS10, CS12); //1024 prescaler (counter increments every 8 microseconds)
    while(flags.getBits(TOV1) == 0); //Wait for overflow
    control.clearBits(CS10, CS11, CS12); //Stop (no clock source)
    flags.setBits(TOV1); //Clear overflow flag
}

void waitUs(uint16_t microseconds)
{
    Register8 counterH(TCNT1H);
    Register8 counterL(TCNT1L);
    Register8 control(TCCR1B);
    Register8 flags(TIFR);
    
    //Set how many counter ticks until desired elapsed time
    uint16_t value = UINT16_MAX + 1 - (F_CPU / (8 * 1'000'000.f) * microseconds);
    counterH.setValue((value & 0xFF00) >> 8);
    counterL.setValue(value & 0xFF);
    
    control.setBits(CS11); //8 prescaler (counter increments every microsecond)
    while(flags.getBits(TOV1) == 0); //Wait for overflow
    control.clearBits(CS10, CS11, CS12); //Stop (no clock source)
    flags.setBits(TOV1); //Clear overflow flag
}