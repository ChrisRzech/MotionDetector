#include "Speaker.hpp"

Speaker::Speaker(const Port& port, Port::Pin pin)
    : m_port{port}, m_pin{pin}
{
    m_port.setPinsOutput(m_pin);
}


void Speaker::emit(uint16_t freq, uint16_t duration) const
{
    /* NOTE using the hardware timers for wave generation is not going to work well since the resolution
       of the frequencies isn't good as you increase frequency (see formula for frequency under
       CTC mode on page 74 in ATmega32 documentation) */
    
    //NOTE hard-code a fine tuning of the frequencies by subtracting some constant (tuned by ear)
	uint16_t period = 1'000'000.f / freq - 80; //microseconds/cycle
    uint16_t cycles = duration / (1000.f / freq); //cycles
	
	for(uint16_t i = 0; i < cycles; i++)
	{
        m_port.setPinsHigh(m_pin);
        waitUs(period / 2);
        m_port.setPinsLow(m_pin);
        waitUs(period / 2);
	}
}