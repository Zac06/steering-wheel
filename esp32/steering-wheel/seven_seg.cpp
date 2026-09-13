#include "esp32-hal.h"

#include<Arduino.h>

#include <algorithm>
#include <string>
#include <iterator>
#include <cstdint>
#include <utility>

#include "seven_seg.hpp"
#include "multiplex_74hc595.hpp"

seven_seg::seven_seg(multiplex_74hc595 segment_pins, std::vector<uint8_t> digit_pins, uint32_t permanence_microsec)

    :   segment_pins(segment_pins), 
        digit_pins(digit_pins),

        dim(0),
        current_digit(0),

        current_val(),
        current_dec(),

        last_update(0),
        permanence_microsec(permanence_microsec)

{
    
}

void seven_seg::begin(){
    dim=this->digit_pins.size();

    this->current_val=std::vector<uint8_t>(dim, 0);
    this->current_dec=std::vector<bool>(dim, 0);

    for(int i=0; i<dim; i++){
        pinMode(digit_pins[i], OUTPUT);
        digitalWrite(digit_pins[i], LOW);
    }

    last_update=micros();
}

void seven_seg::set_digit(uint8_t digit, uint8_t val, bool dec){
    current_val[digit]=val;
    current_dec[digit]=dec;
}

void seven_seg::update(){
    if(micros()-last_update>=permanence_microsec){
        last_update=micros();

        //stop the previous displaying of the digit
        digitalWrite(digit_pins[current_digit], LOW);

        current_digit++;
        if(current_digit>=dim){
            current_digit=0;
        }

        segment_pins.shift_byte_out(seven_seg_ascii[current_val[current_digit]] | current_dec[current_digit]);
        digitalWrite(digit_pins[current_digit], HIGH);
    }
}