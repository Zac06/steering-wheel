#include <Wire.h>

class arduino_pot {
public:
  static void begin(int sda = 21, int scl = 22, uint8_t addr = 0x08) {
    _addr = addr;
    last_millis=0;
    last_value=0;
    has_changed=false;
  }

  static void update() {
    if(millis()-last_millis<1){
        return;
    }

    last_millis=millis();

    uint8_t bytesReceived = Wire.requestFrom(_addr, (uint8_t)2);

    if (bytesReceived == 2) {
      uint8_t highByte = Wire.read();
      uint8_t lowByte = Wire.read();
      uint16_t raw = (highByte << 8) | lowByte;  // 0-1023 from Uno

      _value = map(raw, 0, 1023, 1, 1024);  // mapped to 1-1024

      if(_value!=last_value){
        has_changed=true;
        last_value=_value;
      }
    } else {
      // flush any partial/stale bytes so the bus doesn't desync
      while (Wire.available()) Wire.read();
    }
  }

  static uint16_t value() {
    return _value;
  }
  
  static bool changed(){
    return has_changed;
  }

  static uint16_t get(){
    has_changed=false;

    return _value;
  }

private:
  static uint8_t _addr;
  static uint16_t _value;
  static uint64_t last_millis;
  static uint16_t last_value;
  static bool has_changed;
};


uint8_t arduino_pot::_addr;
uint16_t arduino_pot::_value;
uint64_t arduino_pot::last_millis;
uint16_t arduino_pot::last_value;
bool  arduino_pot::has_changed;