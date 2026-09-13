#include "button.hpp"


#include<Arduino.h>
#include<cstdint>

button::button(int p_gpio, int p_mode)
    : gpio(p_gpio), mode(p_mode), last_state(false), last_bounce(0), has_pressed(false), has_released(false) {
}

void button::begin() {
    pinMode(gpio, mode);
    last_state = state();
}

void button::set_gpio(int gpio) {
    this->gpio = gpio;
    pinMode(gpio, mode);
}

int button::get_gpio() const {
    return this->gpio;
}

void button::set_mode(int mode) {
    this->mode = mode;
    pinMode(gpio, mode);
}

int button::get_mode() const {
    return this->mode;
}

bool button::state() const {
    if (mode == INPUT) {
        return digitalRead(gpio);
    }

    if (mode == INPUT_PULLUP) {
        return !digitalRead(gpio);
    }

    return false;
}

bool button::is_down() const {
    return state();
}

bool button::is_up() const {
    return !state();
}

void button::update() {
    bool curr_state = state();

    if (curr_state == last_state) {
        last_bounce = millis();
    }else if(millis()-last_bounce>DEBOUNCE_TIME){
        if(curr_state){
            has_pressed=true;

            on_press();
        }else{
            has_released=true;

            on_release();
        }

        last_state = curr_state;
        last_bounce = millis();
    }

    
}

bool button::pressed(){
    if(has_pressed){
        has_pressed=false;

        return true;
    }

    return false;
}

bool button::released(){
    if(has_released){
        has_released=false;

        return true;
    }

    return false;
}

void button::on_press() {
    if (press_cb) {
        press_cb();
    }
}

void button::on_release() {
    if (release_cb) {
        release_cb();
    }
}

void button::set_on_press(callback_t cb) {
    press_cb = cb;
}

void button::set_on_release(callback_t cb) {
    release_cb = cb;
}
