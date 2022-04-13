#include "ATMega32Utils.hpp"

Port portA(DDRA, PORTA, PINA);
Port portB(DDRB, PORTB, PINB);
Port portC(DDRC, PORTC, PINC);
Port portD(DDRD, PORTD, PIND);

void waitMs(uint16_t milliseconds)
{
    Register16 counter(TCNT1);
    Register8 control(TCCR1B);
    Register8 flags(TIFR);

    counter.setValue(UINT16_MAX + 1 - (F_CPU / (1024 * 1'000.f) * milliseconds)); //Set how many counter ticks until desired elapsed time
    control.setBits(CS10, CS12); //1024 prescaler (counter increments every 8 microseconds)
    while(flags.getBits(TOV1) == 0); //Wait for overflow
    control.clearBits(CS10, CS11, CS12); //Stop (no clock source)
    flags.setBits(TOV1); //Clear overflow flag
}

void waitUs(uint16_t microseconds)
{
    Register16 counter(TCNT1);
    Register8 control(TCCR1B);
    Register8 flags(TIFR);
    
    counter.setValue(UINT16_MAX + 1 - (F_CPU / (8 * 1'000'000.f) * microseconds)); //Set how many counter ticks until desired elapsed time
    control.setBits(CS11); //8 prescaler (counter increments every microsecond)
    while(flags.getBits(TOV1) == 0); //Wait for overflow
    control.clearBits(CS10, CS11, CS12); //Stop (no clock source)
    flags.setBits(TOV1); //Clear overflow flag
}