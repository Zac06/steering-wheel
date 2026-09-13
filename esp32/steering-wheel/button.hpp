#pragma once

#include<Arduino.h>
#include<cstdint>
#include<functional>

#define DEBOUNCE_TIME 15

class button {
    public:
        using callback_t = std::function<void()>;

    private:
        int gpio;
        int mode;

        int last_state;
        uint32_t last_bounce;

        bool has_pressed, has_released;

        callback_t press_cb;
        callback_t release_cb;

    public:
        button(int p_gpio, int p_mode);

        void begin();
        
        void set_gpio(int gpio);

        int get_gpio() const;

        void set_mode(int mode);

        int get_mode() const;

        bool state() const;

        bool is_down() const;

        bool is_up() const;

        void update();

        virtual void on_press();

        virtual void on_release();

        void set_on_press(callback_t cb);

        void set_on_release(callback_t cb);

        bool pressed();

        bool released();
};