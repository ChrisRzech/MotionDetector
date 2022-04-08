#pragma once
#include <avr/io.h>
#include <stdint.h>

//Speed of the clock. If hardware clock is changed, change this accordingly.
#define F_CPU 8'000'000

enum Bit : uint8_t {_0, _1, _2, _3, _4, _5, _6, _7, ALL};

constexpr uint8_t convertBitsToBitfield()
{
    return 0;
}

template<typename Head, typename... Tail>
constexpr uint8_t convertBitsToBitfield(Head head, Tail... tail)
{
    if(head == Bit::ALL)
    {
        return UINT8_MAX;
    }
    else
    {
        if(sizeof...(tail) == 0)
            return (1 << head);
        else
            return (1 << head) | convertBitsToBitfield(tail...);
    }
}

/* In ATMega32, registers are 8-bit. */
class Register
{
public:
    using RegType = volatile uint8_t;

    Register() = default;
    Register(RegType& reg)
        : m_reg{&reg}
    {}
    
    //Set the value
    void setValue(uint8_t value) const
    {
        *m_reg = value;
    }
    
    //Get the value
    uint8_t getValue() const
    {
        return *m_reg;
    }
    
    //Clear all bits
    void clear() const
    {
        *m_reg = 0;
    }
    
    //Set the value given bits, unspecified bits are cleared
    template<typename... Bits>
    void set(Bits... bits) const
    {
        *m_reg = convertBitsToBitfield(bits...);
    }
    
    //Set individual bits
    template<typename... Bits>
    void setBits(Bits... bits) const
    {
        *m_reg |= convertBitsToBitfield(bits...);
    }
    
    //Clear individual bits
    template<typename... Bits>
    void clearBits(Bits... bits) const
    {
        *m_reg &= ~convertBitsToBitfield(bits...);
    }
    
    //Get individual bit values (to get the value of a specific bit, mask the result)
    template<typename... Bits>
    uint8_t getBits(Bits... bits) const
    {
        return *m_reg & convertBitsToBitfield(bits...);
    }

private:
    RegType* m_reg = nullptr;
};

/* A port is a I/O represented as physical pins on the device. For the ATMega32, there are 4 ports. */
class Port
{
public:
    using Pin = Bit;
    enum class Letter {A, B, C, D}; //Ports supported by ATMega32

    Port(Letter letter)
    {
        switch(letter)
        {
            case Port::Letter::A:
            {
                m_ddrReg = DDRA;
                m_portReg = PORTA;
                m_pinReg = PINA;
                break;
            }
            
            case Port::Letter::B:
            {
                m_ddrReg = DDRB;
                m_portReg = PORTB;
                m_pinReg = PINB;
                break;
            }
            
            case Port::Letter::C:
            {
                m_ddrReg = DDRC;
                m_portReg = PORTC;
                m_pinReg = PINC;
                break;
            }
            
            case Port::Letter::D:
            {
                m_ddrReg = DDRD;
                m_portReg = PORTD;
                m_pinReg = PIND;
                break;
            }
        }
    }
    
    template<typename... Pins>
    void setPinsInput(Pins... pins) const
    {
        m_ddrReg.clearBits(pins...);
    }

    template<typename... Pins>
    void setPinsOutput(Pins... pins) const
    {
        m_ddrReg.setBits(pins...);
    }
    
    void setPins(uint8_t value) const
    {
        m_portReg.setValue(value);
    }

    template<typename... Pins>
    void setPinsHigh(Pins... pins) const
    {
        m_portReg.setBits(pins...);
    }

    template<typename... Pins>
    void setPinsLow(Pins... pins) const
    {
        m_portReg.clearBits(pins...);
    }

    bool getPin(Pin pin) const
    {
        return m_pinReg.getBits(pin);
    }

    template<typename... Pins>
    uint8_t getPins(Pins... pins) const
    {
        return m_pinReg.getBits(pins...);
    }
    
    uint8_t getPins() const
    {
        return m_pinReg.getValue();
    }

private:
    Register m_ddrReg;
    Register m_portReg;
    Register m_pinReg;
};

extern Port portA;
extern Port portB;
extern Port portC;
extern Port portD;

//Uses TCNT0, which is a 8-bit resolution timer/counter
void waitMs(uint16_t milliseconds);