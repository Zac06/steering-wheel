#include "matrix_button.hpp"

matrix_button::matrix_button(uint8_t p_row, uint8_t p_col)
    : row(p_row), col(p_col), virtual_state(false), last_state(false), last_bounce(0),
      has_pressed(false), has_released(false) {
}

matrix_button::matrix_button()
    : row(-1), col(-1), virtual_state(false), last_state(false), last_bounce(0),
      has_pressed(false), has_released(false) {
}

void matrix_button::set_position(uint8_t row, uint8_t col){
    this->row=row;
    this->col=col;
}

bool matrix_button::is_down() const {
    return virtual_state;
}

bool matrix_button::is_up() const {
    return !virtual_state;
}

void matrix_button::update(bool curr_state) {
    virtual_state=curr_state;

    if (curr_state == last_state) {
        last_bounce = millis();
    }else if(millis()-last_bounce>MATRIX_DEBOUNCE_TIME){
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

void matrix_button::set_state(bool state){
    virtual_state=state;
}

void matrix_button::set_on_press(callback_t cb) {
    press_cb = cb;
}

void matrix_button::set_on_release(callback_t cb) {
    release_cb = cb;
}

bool matrix_button::pressed() {
    if (has_pressed) {
        has_pressed = false;
        return true;
    }

    return false;
}

bool matrix_button::released() {
    if (has_released) {
        has_released = false;
        return true;
    }

    return false;
}

void matrix_button::on_press() {
    if (press_cb) {
        press_cb(row, col);
    }
}

void matrix_button::on_release() {
    if (release_cb) {
        release_cb(row, col);
    }
}
