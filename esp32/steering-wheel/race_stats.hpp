#pragma once

#include<string>

#include "serial_manager.hpp"

#define RSTAT_STRID_LEN 2

#define RSTAT_STRID_RPM "RP"
#define RSTAT_STRID_SPEED "VE"
#define RSTAT_STRID_FUEL "FU"
#define RSTAT_STRID_LAP "LA"
#define RSTAT_STRID_GEAR "GE"

#define RSTAT_UNIT_SIZE 16
#define RSTAT_LT_SIZE 20

class rpm_info {
    public:
        int rpm;
        int max_rpm;

        bool changed;

        rpm_info();

        bool parse(const char *str);
        rpm_info& get();
        
};

class speed_info {
    public:
        float speed;
        float max_speed;
        char unit[RSTAT_UNIT_SIZE];

        bool changed;

        speed_info();

        bool parse(const char *str);
        speed_info& get();
};

class fuel_info {
    public:
        float current_lap_cons;
        float last_lap_cons;
        float liters_per_lap;
        float remaining_laps;
        float fuel;
        float fuel_percent;
        char unit[RSTAT_UNIT_SIZE];

        bool changed;

        fuel_info();

        bool parse(const char *str);
        fuel_info& get();
};

typedef char laptime[RSTAT_LT_SIZE];

class lap_info {
    public:
        int current_lap;
        int total_laps;
        laptime current_lap_time;
        laptime last_lap_time;
        laptime best_lap_time;
        int curr_sector;
        laptime sector1_time;
        laptime sector2_time;

        bool changed;

        lap_info();

        bool parse(const char *str);
        lap_info& get();
};

class gear_info {
    public:
        char current;
        int max_gears;
        char blink_gear_up;
        char blink_gear_up_prev;

        bool changed;

        gear_info();

        bool parse(const char *str);
        gear_info& get();
};

class race_stats {
    public:
        static rpm_info rpm;
        static speed_info speed;
        static fuel_info fuel;
        static lap_info lap;
        static gear_info gear;

        static void update();

        static char buf[SERIAL_START_BUFSIZE];
};
