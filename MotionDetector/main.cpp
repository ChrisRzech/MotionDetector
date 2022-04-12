#include "S01602DTR.hpp"
#include "HCSR04.hpp"
#include <stdio.h>

void printError(S01602DTR& lcd, bool error)
{
    lcd.setCursorPosition(1, 0);
    if(error)
        lcd << "Error";
    else
        lcd << "     "; //Overwrite with spaces
}

void printDistance(S01602DTR& lcd, uint16_t distance)
{
    if(distance != UINT16_MAX)
    {
        char buf[17] = {'\0'};
        int8_t len = snprintf(buf, 17, "Dist: %ucm", distance);
        for(int8_t i = len; i < 16; i++)
            buf[i] = ' ';
    
        lcd << buf;
    }
    
    printError(lcd, distance == UINT16_MAX);
}

int main()
{
    S01602DTR lcd(portD, portB, Port::Pin::_0, Port::Pin::_1, Port::Pin::_2);
    HCSR04 sonar(portC, Port::Pin::_0, Port::Pin::_1);
    
    while(true)
    {
        lcd.returnHome();
        
        printDistance(lcd, sonar.getDistance());
        
        waitMs(50);
    }
}