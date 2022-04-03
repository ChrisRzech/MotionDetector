#pragma once
#include "ATMega32Utils.hpp"

class S01602DTR
{
public:
    static const uint8_t width = 16;
    static const uint8_t height = 2;

    S01602DTR(const Port& data, const Port& control, Port::Pin registerSelect, Port::Pin readWrite, Port::Pin enable);
    
    void put(char) const;
    void puts(const char*) const;
    const S01602DTR& operator<<(char) const;
    const S01602DTR& operator<<(const char*) const;
    
    void clear() const;
    void returnHome() const;
    void setCursorPosition(uint8_t row, uint8_t column) const;
    void displayControl(bool displayEnable, bool cursorEnable, bool cursorBlinking) const;
    void setCGRAM(const char* chars, int count) const;
    
private:
    enum class RegisterSelect {INSTRUCTION, DATA};
    
    void entryModeSet(bool incrementOrDecrement, bool shift) const;
    void functionSet(bool dataLength, bool lineCount, bool fontSize) const;
    void cursorDisplayShift(bool shiftCursor, bool shiftDisplay) const;
    void setCGRAMAddress(uint8_t address) const;
    void setDDRAMAddress(uint8_t address) const;
    void waitForNotBusy() const;
    uint8_t readAddress() const;
    void writeDataToAddress(uint8_t data) const;
    uint8_t readDataFromAddress() const;
    
    void write(RegisterSelect, uint8_t data) const;
    uint8_t read(RegisterSelect) const;
    
    void writeNoWait(RegisterSelect, uint8_t data) const;
    uint8_t readNoWait(RegisterSelect) const;
    
    void setRegisterSelectPin(RegisterSelect) const;

    const Port& m_dataPort;
    const Port& m_controlPort;
    const Port::Pin m_regSelectPin; //High = data input, low = instruction input
    const Port::Pin m_readWritePin; //High = read, low = write
    const Port::Pin m_enablePin;
};