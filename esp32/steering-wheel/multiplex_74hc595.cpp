#include "multiplex_74hc595.hpp"

#include<Arduino.h>

#include<cstdint>

multiplex_74hc595::multiplex_74hc595(uint8_t latch_pin, uint8_t clock_pin, uint8_t data_pin)
    :   latch_pin(latch_pin),
        clock_pin(clock_pin),
        data_pin(data_pin)
{
}

void multiplex_74hc595::begin(){
    pinMode(latch_pin, OUTPUT);
    pinMode(clock_pin, OUTPUT);
    pinMode(data_pin, OUTPUT);
}

void multiplex_74hc595::shift_byte_out(uint8_t byte){
    digitalWrite(latch_pin, LOW);

    shiftOut(data_pin, clock_pin, LSBFIRST, byte);

    digitalWrite(latch_pin, HIGH);
}