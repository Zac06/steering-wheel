#include "multiplex_74hc595.hpp"
#include "seven_seg.hpp"
#include "button.hpp"
#include "rotary_encoder.hpp"
#include "serial_manager.hpp"
#include "button_matrix.hpp"
#include "arduino_pot.hpp"
#include "race_stats.hpp"

#include <cstdint>
#include <cstdio>
#include <cstring>

#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// 74HC595 Pins
const int data_pin = 27;
const int latch_pin = 25;
const int clock_pin = 26;

// Transistor Base Pins (Digit Selectors)
const std::vector<uint8_t> digit_pins = { 16, 4, 17, 5, 18 };

// Potentiometer sampling timer variables
unsigned long lastPotReadTime = 0;
const int potReadDelay = 100;  // Read every 100ms to keep display stable

multiplex_74hc595 ss_multi(latch_pin, clock_pin, data_pin);
seven_seg ss(ss_multi, digit_pins, 1000);

rotary_encoder rot=rotary_encoder(23, 19);
button btn_rot=button(33, INPUT_PULLUP);

// button btn_down=button(39, INPUT);
// button btn_up=button(36, INPUT);

int lastMillis = 0;

// =====================================================
// Sets up flicker-less seven segment display

seven_seg* display_ptr = nullptr;
hw_timer_t* displayTimer = nullptr;

const std::vector<uint8_t> row_pins = { 13, 12, 14 };
const std::vector<uint8_t> col_pins = { 36, 39, 34, 35 };

char lcdbuf[16];

button_matrix matrix(row_pins, col_pins);

void IRAM_ATTR onDisplayTimer() {
  if (display_ptr != nullptr) {
    display_ptr->update();
  }
}

void setup_display_timer() {
  display_ptr = &ss;

  displayTimer = timerBegin(1000000);  //1MHz timer
  timerAttachInterrupt(displayTimer, &onDisplayTimer);
  timerAlarm(displayTimer, 100, true, 0);  //checks every 100 microsecs
}

//==============================================

void util_print_speed(float value) {
    int v = (int)value;

    if (v < 0) {
        v = -v;
    }
    if (v > 9999) { v = 9999; }

    int digits[4];
    digits[0] = v % 10;
    digits[1] = (v / 10) % 10;
    digits[2] = (v / 100) % 10;
    digits[3] = (v / 1000) % 10;

    bool leading = true;  // still in the leading-zero region
    for (int i = 3; i >= 0; i--) {
        if (digits[i] != 0 || i == 0) {     //always print "0" (next things empty)
            leading = false;
        }

        if (leading) {
            ss.set_digit(i, 0);
        } else {
            ss.set_digit(i, digits[i] + '0');
        }
    }
}

void setup() {
    //Serial.begin(115200);
    serial_manager::begin();

    Wire.begin();
    Wire.setClock(100000);

    ss_multi.begin();
    ss.begin();
    setup_display_timer();

    rot.begin();
    rot.set_on_clockwise([](){
        Serial.println("CO|rot|cw");
    });
    rot.set_on_counterclockwise([](){
        Serial.println("CO|rot|ccw");
    });

    btn_rot.begin();
    btn_rot.set_on_press([](){
        Serial.println("CO|btn|12|down");
    });

    btn_rot.set_on_release([](){
        Serial.println("CO|btn|12|up");
    });

    ss.set_digit(0, '0');
    ss.set_digit(1, '1');
    ss.set_digit(2, '2');
    ss.set_digit(3, '3');
    ss.set_digit(4, 'N');

    lcd.init();
    lcd.backlight();
    lcd.clear();

    matrix.set_on_press_all([](uint8_t row, uint8_t col) {
        Serial.print("CO|btn|");
        Serial.print(String(row * col_pins.size() + col));
        Serial.println("|down");
    });

    matrix.set_on_release_all([](uint8_t row, uint8_t col) {
        Serial.print("CO|btn|");
        Serial.print(String(row * col_pins.size() + col));
        Serial.println("|up");
    });

    matrix.begin();
    arduino_pot::begin();
}

void loop() {
    matrix.update();
    rot.update();
    btn_rot.update();
    
    serial_manager::update();
    race_stats::update();
    arduino_pot::update();

    if (arduino_pot::changed()) {
        Serial.print("CO;steer;");
        Serial.println(arduino_pot::get());
    }

    if (race_stats::rpm.changed) {
        rpm_info& tmp = race_stats::rpm.get();

        snprintf(lcdbuf, sizeof(lcdbuf), "RPM %5d", tmp.rpm);
        lcd.setCursor(0, 0);
        lcd.print(lcdbuf);
    }

    if (race_stats::lap.changed) {
        lap_info& tmp = race_stats::lap.get();

        snprintf(lcdbuf, sizeof(lcdbuf), "LAP %s", &tmp.current_lap_time[3]);   //take away the first 3 characters (hour)
        lcd.setCursor(0,1);
        lcd.print(lcdbuf);      
    }

    if(race_stats::speed.changed){
        speed_info& tmp=race_stats::speed.get();

        util_print_speed(tmp.speed);
    }

    if(race_stats::gear.changed){
        gear_info& tmp=race_stats::gear.get();

        ss.set_digit(4, tmp.current);
    }

    ss.update();
}
