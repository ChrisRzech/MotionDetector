#include "S01602DTR.hpp"
#include "HCSR04.hpp"
#include "Speaker.hpp"
#include <stdio.h>

constexpr uint16_t g_sonarDelay = 70;

void printDistance(const S01602DTR& lcd, uint16_t distance)
{
    char buf[17] = {'\0'};
    int8_t len = snprintf(buf, 17, "%ucm", distance);
    for(int8_t i = len; i < 16; i++)
        buf[i] = ' ';
    lcd << buf;
}

uint16_t getDistance(const HCSR04& sonar)
{
    uint16_t distance;
    do
    {
        distance = sonar.getDistance();
    } while(distance == HCSR04::error);
    return distance;
}

uint16_t calibrate(const HCSR04& sonar, uint16_t duration)
{
    uint16_t sampleCount = duration / g_sonarDelay;
    uint16_t sum = 0;
    
    for(uint16_t i = 1; i <= sampleCount; i++)
    {
        uint16_t distance = getDistance(sonar);
        sum += distance;
        
        waitMs(g_sonarDelay);
    }
    
    return sum / sampleCount;
}

bool inRange(uint16_t value, uint16_t low, uint16_t high)
{
    return value >= low && value <= high;
}

int main()
{
    S01602DTR lcd(portD, portB, Port::Pin::_0, Port::Pin::_1, Port::Pin::_2);
    HCSR04 sonar(portC, Port::Pin::_0, Port::Pin::_1);
    Speaker speaker(portB, Port::Pin::_3);
    
    lcd << "Calibrating...";
    uint16_t calibratedDistance = calibrate(sonar, 1000);
    constexpr uint16_t calibrationDeviation = 10;
    uint16_t lowCalibrationDistance = calibratedDistance - calibrationDeviation;
    uint16_t highCalibrationDistance = calibratedDistance + calibrationDeviation;

    lcd.clear();
    lcd << "Cali: ";
    printDistance(lcd, calibratedDistance);
    
    while(true)
    {
        uint16_t distance = getDistance(sonar);
        
        lcd.setCursorPosition(1, 0);
        lcd << "Dist: ";
        printDistance(lcd, distance);
        
        if(inRange(distance, lowCalibrationDistance, highCalibrationDistance))
            waitMs(g_sonarDelay);
        else
            speaker.emit(1000, g_sonarDelay);
    }
}