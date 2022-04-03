#include "S01602DTR.hpp"

S01602DTR::S01602DTR(const Port& data, const Port& control, Port::Pin registerSelect, Port::Pin readWrite, Port::Pin enable)
    : m_dataPort{data}, m_controlPort{control}, m_regSelectPin{registerSelect}, m_readWritePin{readWrite}, m_enablePin{enable}
{
    m_controlPort.setPinsOutput(m_regSelectPin, m_readWritePin, m_enablePin);
    
    //NOTE Busy flag can't be read in this section. The writeNoWait() calls are basically functionSet()
    //calls but without waiting for busy flag (couldn't find documentation on this though).
    waitMs(50);
    writeNoWait(RegisterSelect::INSTRUCTION, 0b0011'0000);
    waitMs(5);
    writeNoWait(RegisterSelect::INSTRUCTION, 0b0011'0000);
    waitMs(1);
    //NOTE Busy flag can be read now
    
    functionSet(true, true, false); //8-bit interface, 2-line display, 5x8 format
    displayControl(true, false, false); //display on, cursor off, cursor blinking off
    entryModeSet(true, false); //increment address, shifting off
    cursorDisplayShift(true, true);
    clear();
}

void S01602DTR::put(char c) const
{
    writeDataToAddress(c);
}

void S01602DTR::puts(const char* string) const
{
    for(; *string != '\0'; string++)
        put(*string);
}

const S01602DTR& S01602DTR::operator<<(char c) const
{
    put(c);
    return *this;
}

const S01602DTR& S01602DTR::operator<<(const char* string) const
{
    puts(string);
    return *this;
}

void S01602DTR::clear() const
{
    write(RegisterSelect::INSTRUCTION, 0b0000'0001);
}

void S01602DTR::returnHome() const
{
    write(RegisterSelect::INSTRUCTION, 0b0000'0010);
}

void S01602DTR::setCursorPosition(uint8_t row, uint8_t col) const
{
    //NOTE Documentation says each row has 40 cells
    setDDRAMAddress(row * 40 + col);
}

void S01602DTR::displayControl(bool displayEnable, bool cursorEnable, bool cursorBlinking) const
{
    write(RegisterSelect::INSTRUCTION, 0b0000'1000 | (displayEnable << 2) | (cursorEnable << 1) | (cursorBlinking << 0));
}

void S01602DTR::setCGRAM(const char* chars, int count) const
{
    //Save address
    uint8_t address = readAddress();
    
    //Set CGRAM address to 0
    setCGRAMAddress(0x00);
    
    //Store custom characters
    for(int i = 0; i < 8; i++)
    {
        //Write each byte to CGRAM. If no more characters in array, zero out the rest.
        for(int j = 0; j < 8; j++)
            writeDataToAddress(i < count ? chars[i * sizeof(uint8_t) * 8 + j] : 0);
    }
    
    //Restore DDRAM address (set 8th bit to 1)
    setDDRAMAddress(address);
}

void S01602DTR::entryModeSet(bool incrOrDecr, bool shift) const
{
    write(RegisterSelect::INSTRUCTION, 0b0000'0100 | (incrOrDecr << 1) | (shift << 0));
}

void S01602DTR::functionSet(bool dataLength, bool lineCount, bool fontSize) const
{
    write(RegisterSelect::INSTRUCTION, 0b0010'0000 | (dataLength << 4) | (lineCount << 3) | (fontSize << 2));
}

void S01602DTR::cursorDisplayShift(bool shiftCursor, bool shiftDisplay) const
{
    write(RegisterSelect::INSTRUCTION, 0b0001'0000 | (shiftCursor << 3) | (shiftDisplay << 2));
}

void S01602DTR::setCGRAMAddress(uint8_t address) const
{
    write(RegisterSelect::INSTRUCTION, (address & ~0b1000'0000) | 0b0100'0000); //Clear bit-7 and set bit-6
}

void S01602DTR::setDDRAMAddress(uint8_t address) const
{
    write(RegisterSelect::INSTRUCTION, address | 0b1000'0000); //Set bit-7
}

void S01602DTR::waitForNotBusy() const
{
    while(readNoWait(RegisterSelect::INSTRUCTION) & 0b1000'0000); //Bit-7 is busy flag
}

uint8_t S01602DTR::readAddress() const
{
    return read(RegisterSelect::INSTRUCTION);
}

void S01602DTR::writeDataToAddress(uint8_t data) const
{
    write(RegisterSelect::DATA, data);
}

uint8_t S01602DTR::readDataFromAddress() const
{
    return read(RegisterSelect::DATA);
}

void S01602DTR::write(RegisterSelect select, uint8_t data) const
{
    waitForNotBusy();
    writeNoWait(select, data);
}

uint8_t S01602DTR::read(RegisterSelect select) const
{
    waitForNotBusy();
    return readNoWait(select);
}

void S01602DTR::writeNoWait(RegisterSelect select, uint8_t data) const
{
    setRegisterSelectPin(select);
    m_controlPort.setPinsLow(m_readWritePin);
    
    m_dataPort.setPinsOutput(Port::Pin::ALL);
    m_dataPort.setPins(data);
    
    m_controlPort.setPinsHigh(m_enablePin);
    waitMs(1);
    m_controlPort.setPinsLow(m_enablePin);
}

uint8_t S01602DTR::readNoWait(RegisterSelect select) const    
{
    setRegisterSelectPin(select);
    m_controlPort.setPinsHigh(m_readWritePin);
    
    m_controlPort.setPinsHigh(m_enablePin);
    waitMs(1);
    
    m_dataPort.setPinsInput(Port::Pin::ALL);
    uint8_t data = m_dataPort.getPins();
    
    m_controlPort.setPinsLow(m_enablePin);
    
    return data;
}

void S01602DTR::setRegisterSelectPin(RegisterSelect select) const
{
    switch(select)
    {
        case RegisterSelect::DATA:
        {
            m_controlPort.setPinsHigh(m_regSelectPin);
            break;
        }
        
        case RegisterSelect::INSTRUCTION:
        {
            m_controlPort.setPinsLow(m_regSelectPin);
            break;
        }
    }
}