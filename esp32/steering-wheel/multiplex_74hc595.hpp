#pragma once

#include<Arduino.h>

#include<cstdint>

class multiplex_74hc595 {
    private:

        uint8_t latch_pin;
        uint8_t clock_pin;
        uint8_t data_pin;

    public:

        multiplex_74hc595(uint8_t latch_pin, uint8_t clock_pin, uint8_t data_pin);

        void begin();

        void shift_byte_out(uint8_t byte);
};