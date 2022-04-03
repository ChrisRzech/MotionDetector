#include "ATMega32Utils.hpp"

Port portA(Port::Letter::A);
Port portB(Port::Letter::B);
Port portC(Port::Letter::C);
Port portD(Port::Letter::D);

void waitMs(uint16_t milliseconds)
{
    Register timer(TCNT0);
    Register timerControl(TCCR0);
    Register timerInterrupt(TIFR);
    
    timerControl.set(CS00, CS01);
    
    while(milliseconds-- > 0)
    {
        timer.setValue(256 - (F_CPU / 64) * 0.001);
        timerInterrupt.set(TOV0);
        while(!timerInterrupt.getBits(TOV0));
    }
    
    timerControl.clear();
}