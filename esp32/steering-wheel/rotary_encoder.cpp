#include "rotary_encoder.hpp"

#include <cstdint>

#include "Arduino.h"

rotary_encoder::rotary_encoder(uint8_t gpio_clk, uint8_t gpio_dt)
    : gpio_clk(gpio_clk), gpio_dt(gpio_dt),

      counter(0), dir(clockwise),

      last_state_clk(HIGH), last_valid_pulse(0) {
}

void rotary_encoder::begin() {
    pinMode(gpio_clk, INPUT_PULLUP);
    pinMode(gpio_dt, INPUT_PULLUP);

    last_state_clk = digitalRead(gpio_clk);
}

void rotary_encoder::update() {
    uint8_t curr_state_clk = digitalRead(gpio_clk);

    if (last_state_clk == HIGH && curr_state_clk == LOW) {
        if (millis() - last_valid_pulse > ROT_DEBOUNCE_TIME) {
            uint8_t curr_state_dt = digitalRead(gpio_dt);

            if (curr_state_dt == HIGH) {
                counter--;
                dir = counterclockwise;

                on_counterclockwise();

            } else {
                counter++;
                dir = clockwise;

                on_clockwise();
            }
        }

        has_changed = true;
        last_valid_pulse=millis();
    }

    last_state_clk = curr_state_clk;
}

int64_t rotary_encoder::get_counter() const {
    return counter;
}

void rotary_encoder::set_counter(int64_t counter) {
    this->counter = counter;
}

uint8_t rotary_encoder::get_dir() const {
    return dir;
}

void rotary_encoder::reset() {
    counter = 0;
    dir = clockwise;
}

bool rotary_encoder::changed() {
    if (has_changed) {
        has_changed = false;

        return true;
    }

    return false;
}

void rotary_encoder::on_clockwise(){
    if (cw_cb) {
        cw_cb();
    }
}

void rotary_encoder::on_counterclockwise(){
    if (ccw_cb) {
        ccw_cb();
    }
}

void rotary_encoder::set_on_clockwise(callback_t cb){
    cw_cb = cb;
}

void rotary_encoder::set_on_counterclockwise(callback_t cb){
    ccw_cb = cb;
}