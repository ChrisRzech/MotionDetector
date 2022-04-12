#include "HCSR04.hpp"

HCSR04::HCSR04(const Port& port, Port::Pin trigger, Port::Pin echo)
    : m_port{port}, m_triggerPin{trigger}, m_echoPin{echo}
{}

uint16_t HCSR04::getDistance() const
{
    return convert(ping());
}

uint16_t HCSR04::ping() const
{
    m_port.setPinsOutput(m_triggerPin);
    m_port.setPinsInput(m_echoPin);
    
    //10 microsecond pulse to send ultrasonic burst
    m_port.setPinsHigh(m_triggerPin);
    waitUs(10);
    m_port.setPinsLow(m_triggerPin);
    
    Register counterH(TCNT1H);
    Register counterL(TCNT1L);
    Register control(TCCR1B);
    Register flags(TIFR);
    
    //The device has a maximum distance, thus there is a maximum time
    static constexpr uint16_t maxTime = maxDistance * conversionScalar;
    static constexpr uint16_t timeoutValue = UINT16_MAX + 1 - maxTime;
    
    //Wait for ultrasonic bursts to come back. It is possible that they don't come back, so wait for a certain amount of time.
    counterH.setValue((timeoutValue & 0xFF00) >> 8);
    counterL.setValue(timeoutValue & 0xFF);
    control.setBits(CS11); //8 prescaler (counter increments every microsecond)
    while(!m_port.getPin(m_echoPin) && flags.getBits(TOV1) == 0); //Wait until echo pin is high or for overflow (timeout)
    control.clearBits(CS10, CS11, CS12); //Stop (no clock source)
    flags.setBits(TOV1); //Clear overflow flag
    
    //Echo pin never came back
    if(flags.getBits(TOV1) != 0)
        return UINT16_MAX;
    
    //Count duration of high echo pin
    counterH.setValue((timeoutValue & 0xFF00) >> 8);
    counterL.setValue(timeoutValue & 0xFF);
    control.setBits(CS11); //8 prescaler (counter increments every microsecond)
    while(m_port.getPin(m_echoPin) && flags.getBits(TOV1) == 0); //Wait until echo pin is low or for overflow (timeout)
    control.clearBits(CS10, CS11, CS12); //Stop (no clock source)
    uint16_t time = (flags.getBits(TOV1) == 0 ? TCNT1 - timeoutValue : UINT16_MAX);
    flags.setBits(TOV1); //Clear overflow flag after reading counter
    
    return time;
}

uint16_t HCSR04::convert(uint16_t time) const
{
    //NOTE Device documentation says dividing time in microseconds by this value will give distance in centimeters
    uint16_t dist = time / conversionScalar;
    return (dist >= minDistance && dist <= maxDistance) ? dist : UINT16_MAX;
}