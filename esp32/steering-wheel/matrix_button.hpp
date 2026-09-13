#pragma once

#include <Arduino.h>
#include <cstdint>
#include <functional>

#include "button.hpp"

#define MATRIX_DEBOUNCE_TIME 15

// A single cell in a button matrix. Unlike `button`, it does not read its
// own GPIO - its raw state is fed in by button_matrix::update() during a
// scan, since a matrix column can't be read in isolation.
class matrix_button {
    public:
        using callback_t = std::function<void(uint8_t, uint8_t)>;

    private:
        uint8_t row, col;

        bool virtual_state;

        int last_state;
        uint32_t last_bounce;

        bool has_pressed, has_released;

        callback_t press_cb;
        callback_t release_cb;

    public:
        matrix_button();
        matrix_button(uint8_t row, uint8_t col);

        void set_position(uint8_t row, uint8_t col);
        void set_state(bool virtual_state);

        bool is_down() const;
        bool is_up() const;

        void update(bool curr_state);

        void set_on_press(callback_t cb);
        void set_on_release(callback_t cb);

        bool pressed();
        bool released();

        virtual void on_press();
        virtual void on_release();
};