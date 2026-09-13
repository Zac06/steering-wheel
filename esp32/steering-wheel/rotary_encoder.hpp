#pragma once

#include <cstdint>

#include "Arduino.h"

#define ROT_DEBOUNCE_TIME 5

class rotary_encoder {
    public:
        using callback_t = std::function<void()>;

    private:
        uint8_t gpio_clk;
        uint8_t gpio_dt;

        uint8_t last_state_clk;
        int64_t counter;
        uint8_t dir;
        bool has_changed;
        uint64_t last_valid_pulse;

        callback_t cw_cb;
        callback_t ccw_cb;

    public:
        enum direction {
            clockwise=0,
            counterclockwise
        };

        rotary_encoder(uint8_t gpio_clk, uint8_t gpio_dt);

        void begin();

        void update();

        int64_t get_counter() const;

        void set_counter(int64_t counter);

        void reset();

        bool changed();

        uint8_t get_dir() const;

        virtual void on_clockwise();

        virtual void on_counterclockwise();

        void set_on_clockwise(callback_t cb);

        void set_on_counterclockwise(callback_t cb);
};