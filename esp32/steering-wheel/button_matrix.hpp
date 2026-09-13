#pragma once

#include <Arduino.h>
#include <cstdint>
#include <vector>
#include <functional>

#include "matrix_button.hpp"

class button_matrix {
    private:
        std::vector<uint8_t> row_pins;
        std::vector<uint8_t> col_pins;

        std::vector<std::vector<matrix_button>> buttons;

    public:
        button_matrix(std::vector<uint8_t> row_pins,
                      std::vector<uint8_t> col_pins);

        void begin();

        void update();

        uint8_t get_rows() const;
        uint8_t get_cols() const;

        matrix_button& at(uint8_t row, uint8_t col);

        // assign the same callback to every button in the matrix, so you
        // don't have to register one per cell
        void set_on_press_all(matrix_button::callback_t cb);
        void set_on_release_all(matrix_button::callback_t cb);
};