#include "button_matrix.hpp"

button_matrix::button_matrix(std::vector<uint8_t> row_pins, std::vector<uint8_t> col_pins)
    : row_pins(row_pins), col_pins(col_pins) {

    buttons.resize(row_pins.size());
    for (uint8_t r = 0; r < row_pins.size(); r++) {
        buttons[r].resize(col_pins.size());

        for (uint8_t c = 0; c < col_pins.size(); c++) {
            buttons[r][c].set_position(r, c);
        }
    }
}

void button_matrix::begin() {
    for (uint8_t r = 0; r < row_pins.size(); r++) {
        pinMode(row_pins[r], OUTPUT);
        digitalWrite(row_pins[r], LOW);
    }

    for (uint8_t c = 0; c < col_pins.size(); c++) {
        pinMode(col_pins[c], INPUT);
    }
}

void button_matrix::update() {
    for (uint8_t r = 0; r < row_pins.size(); r++) {
        digitalWrite(row_pins[r], HIGH);

        // let the line settle before reading columns
        delayMicroseconds(5);

        for (uint8_t c = 0; c < col_pins.size(); c++) {
            buttons[r][c].update(digitalRead(col_pins[c]));
        }

        digitalWrite(row_pins[r], LOW);

        delayMicroseconds(5);
    }
}

uint8_t button_matrix::get_rows() const {
    return row_pins.size();
}

uint8_t button_matrix::get_cols() const {
    return col_pins.size();
}

matrix_button& button_matrix::at(uint8_t row, uint8_t col) {
    return buttons[row][col];
}

void button_matrix::set_on_press_all(matrix_button::callback_t cb) {
    for (std::vector<matrix_button> &row : buttons) {
        for (matrix_button &btn : row) {
            btn.set_on_press(cb);
        }
    }
}

void button_matrix::set_on_release_all(matrix_button::callback_t cb) {
    for (std::vector<matrix_button> &row : buttons) {
        for (matrix_button &btn : row) {
            btn.set_on_release(cb);
        }
    }
}