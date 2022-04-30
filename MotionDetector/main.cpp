#include "Speaker.hpp"

int main()
{
    Speaker speaker(portB, Port::Pin::_3);

    for(uint16_t freq = 55; freq < 1760; freq += 2)
        speaker.emit(freq, 10);
}