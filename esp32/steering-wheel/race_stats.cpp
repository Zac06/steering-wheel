#include "race_stats.hpp"
#include "serial_manager.hpp"

#include<string>
#include<cstdint>
#include<cstring>
#include<cstdlib>

char race_stats::buf[SERIAL_START_BUFSIZE];

rpm_info::rpm_info()
    :rpm(0), max_rpm(0), changed(true)
{}

speed_info::speed_info()
    :speed(0), max_speed(0), changed(true)
{
    strcpy(unit, "KMH");
}

fuel_info::fuel_info()
    :current_lap_cons(0), last_lap_cons(0), liters_per_lap(0), remaining_laps(0), fuel(0), fuel_percent(0), changed(true)
{
    strcpy(unit, "L");
}

lap_info::lap_info()
    :current_lap(0), total_laps(0), curr_sector(0), changed(true)
{
    strcpy(last_lap_time, "00:00:00");
    strcpy(best_lap_time, "00:00:00");
    strcpy(current_lap_time, "00:00:00");
    strcpy(sector1_time, "00:00:00");
    strcpy(sector2_time, "00:00:00");
}

gear_info::gear_info()
    :current('N'), max_gears(0), blink_gear_up('N'), blink_gear_up_prev('N'), changed(true)
{}


rpm_info race_stats::rpm=rpm_info();
speed_info race_stats::speed=speed_info();
fuel_info race_stats::fuel=fuel_info();
lap_info race_stats::lap=lap_info();
gear_info race_stats::gear=gear_info();

bool generic_prefix_check(char *str, const char *comp){
    if(str==NULL || comp==NULL){
        return false;
    }

    size_t size=strlen(str);
    if(size<=3){
        return false;
    }

    const char *prefix=strtok(str, "|");
    if(prefix==NULL || strcmp(prefix, comp)){
        return false;
    }

    return true;
}

bool rpm_info::parse(const char *str_in){
    char *str=strcpy(race_stats::buf, str_in);

    if(!generic_prefix_check(str, RSTAT_STRID_RPM)){
        return false;
    }

    const char *ss_rpm=strtok(NULL, "|");
    const char *ss_max_rpm=strtok(NULL, "|");

    if(ss_rpm==NULL || ss_max_rpm==NULL){
        return false;
    }

    this->rpm=atoi(ss_rpm);
    this->max_rpm=atoi(ss_max_rpm);

    this->changed=true;
    
    return true;
}

bool speed_info::parse(const char *str_in){
    char *str=strcpy(race_stats::buf, str_in);

    if(!generic_prefix_check(str, RSTAT_STRID_SPEED)){
        return false;
    }

    const char *ss_speed=strtok(NULL, "|");
    const char *ss_max_speed=strtok(NULL, "|");
    const char *ss_unit=strtok(NULL, "|");

    if(ss_speed==NULL || ss_max_speed==NULL || ss_unit==NULL){
        return false;
    }

    if(strlen(ss_unit)>=RSTAT_UNIT_SIZE){
        return false;
    }

    this->speed=atof(ss_speed);
    this->max_speed=atof(ss_max_speed);
    
    if(!strcmp(ss_unit, "KMH") || !strcmp(ss_unit, "MPH")){       //if not a valid measuring size
        strcpy(this->unit, ss_unit);
    }else{
        strcpy(this->unit, "WTH");
    }

    this->changed=true;
    
    return true;
}

bool fuel_info::parse(const char *str_in){
    char *str=strcpy(race_stats::buf, str_in);

    if(!generic_prefix_check(str, RSTAT_STRID_FUEL)){
        return false;
    }

    const char *ss_current_lap_cons=strtok(NULL, "|");
    const char *ss_last_lap_cons=strtok(NULL, "|");
    const char *ss_liters_per_lap=strtok(NULL, "|");
    const char *ss_remaining_laps=strtok(NULL, "|");
    const char *ss_fuel=strtok(NULL, "|");
    const char *ss_fuel_percent=strtok(NULL, "|");
    const char *ss_unit=strtok(NULL, "|");

    if(ss_current_lap_cons==NULL || ss_last_lap_cons==NULL || ss_liters_per_lap==NULL || ss_remaining_laps==NULL
        || ss_fuel==NULL || ss_fuel_percent==NULL || ss_unit==NULL){

        return false;
    }

    if(strlen(ss_unit)>=RSTAT_UNIT_SIZE){
        return false;
    }

    this->current_lap_cons=atof(ss_current_lap_cons);
    this->last_lap_cons=atof(ss_last_lap_cons);
    this->liters_per_lap=atof(ss_liters_per_lap);
    this->remaining_laps=atof(ss_remaining_laps);
    this->fuel=atof(ss_fuel);
    this->fuel_percent=atof(ss_fuel_percent);
    
    if(!strcmp(ss_unit, "Liters")){
        strcpy(this->unit, "L");
    }else{
        strcpy(this->unit, "?");
    }

    this->changed=true;
    
    return true;
}

bool lap_info::parse(const char *str_in){
    char *str=strcpy(race_stats::buf, str_in);

    if(!generic_prefix_check(str, RSTAT_STRID_LAP)){
        return false;
    }

    const char *ss_current_lap=strtok(NULL, "|");
    const char *ss_total_laps=strtok(NULL, "|");
    const char *ss_current_lap_time=strtok(NULL, "|");
    const char *ss_last_lap_time=strtok(NULL, "|");
    const char *ss_best_lap_time=strtok(NULL, "|");
    const char *ss_curr_sector=strtok(NULL, "|");
    const char *ss_sector1_time=strtok(NULL, "|");
    const char *ss_sector2_time=strtok(NULL, "|");

    if(ss_current_lap==NULL || ss_total_laps==NULL || ss_last_lap_time==NULL ||
        ss_best_lap_time==NULL || ss_curr_sector==NULL || ss_sector1_time==NULL || ss_sector2_time==NULL){
        return false;
    }

    this->current_lap=atoi(ss_current_lap);
    this->total_laps=atoi(ss_total_laps);

    if(strlen(ss_last_lap_time)>=RSTAT_LT_SIZE || strlen(ss_best_lap_time)>=RSTAT_LT_SIZE || strlen(ss_current_lap_time)>=RSTAT_LT_SIZE){
        strcpy(this->last_lap_time, "00:00:00");
        strcpy(this->best_lap_time, "00:00:00");
        strcpy(this->current_lap_time, "00:00:00");

        return false;
    }

    strcpy(this->current_lap_time, ss_current_lap_time);
    strcpy(this->last_lap_time, ss_last_lap_time);
    strcpy(this->best_lap_time, ss_best_lap_time);

    this->curr_sector=atoi(ss_curr_sector);

    if(strlen(ss_sector1_time)>=RSTAT_LT_SIZE || strlen(ss_sector2_time)>=RSTAT_LT_SIZE){
        strcpy(this->sector1_time, "00:00:00");
        strcpy(this->sector2_time, "00:00:00");

        return false;
    }

    strcpy(this->sector1_time, ss_sector1_time);
    strcpy(this->sector2_time, ss_sector2_time);
    
    this->changed=true;
    
    return true;
}

bool gear_info::parse(const char *str_in){
    char *str=strcpy(race_stats::buf, str_in);

    if(!generic_prefix_check(str, RSTAT_STRID_GEAR)){
        return false;
    }

    const char *ss_current=strtok(NULL, "|");
    const char *ss_max_gears=strtok(NULL, "|");
    const char *ss_blink_gear_up=strtok(NULL, "|");

    if(ss_current==NULL || ss_max_gears==NULL || ss_blink_gear_up==NULL){
        return false;
    }

    this->current=ss_current[0];
    this->max_gears=atoi(ss_max_gears);
    this->blink_gear_up_prev=this->blink_gear_up;
    this->blink_gear_up=ss_blink_gear_up[0];
    

    this->changed=true;
    
    return true;
}

rpm_info& rpm_info::get(){
    this->changed=false;

    return *this;
}

speed_info& speed_info::get(){
    this->changed=false;

    return *this;
}

fuel_info& fuel_info::get(){
    this->changed=false;

    return *this;
}

lap_info& lap_info::get(){
    this->changed=false;

    return *this;
}

gear_info& gear_info::get(){
    this->changed=false;

    return *this;
}

void race_stats::update(){
    while(serial_manager::available()){
        std::string tmp=serial_manager::get_msg();

        if(rpm.parse(tmp.data()) || speed.parse(tmp.data()) || fuel.parse(tmp.data()) || lap.parse(tmp.data()) || gear.parse(tmp.data())){    //will work since: 1. OR statements stop at the first OR, and 2. if the prefix is found, it will exit, else it will pass on the control of the queue since it's not a valid message. 
            serial_manager::remove_msg();
            continue;
        }else{
            break;
        }
    }
}

