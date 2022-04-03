#include "S01602DTR.hpp"

int main()
{
    S01602DTR lcd(portD, portB, Port::Pin::_0, Port::Pin::_1, Port::Pin::_2);
    
    while(true)
    {
        int index = 0;
        for(uint8_t i = 0; i < UINT8_MAX; i++)
        {
            lcd << static_cast<char>(i);
            
            if(index == 16)
            {
                lcd.setCursorPosition(1, 0);
            }
            else if(index == 32)
            {
                waitMs(5000);
                lcd.clear();
                index = 0;
            }
            
            index++;
        }
        
        waitMs(5000);
        lcd.clear();
    }    
    
    while(true);
}