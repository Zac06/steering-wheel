#include "serial_manager.hpp"

#include<Arduino.h>
#include<string>
#include<queue>
#include<cstring>

circ_buf::circ_buf(bool overwrite)
    :buf_read(0), buf_write(0), buf_count(0), overwrite(overwrite)
{
}

void circ_buf::push(uint8_t buf[SERIAL_START_BUFSIZE]) {
    if (buf_count==CIRC_BUF_SIZE) {
        if (!overwrite) {
            //discard new insertion
            return;
        }
        //drop the oldest entry
        buf_read = (buf_read+1) % CIRC_BUF_SIZE;
        buf_count--;
    }
    
    //syscall to make it fast
    memcpy(bufs[buf_write], buf, SERIAL_START_BUFSIZE);
    buf_write = (buf_write + 1) % CIRC_BUF_SIZE;

    buf_count++;
}

void circ_buf::get(uint8_t buf_dst[SERIAL_START_BUFSIZE]) {
    if (buf_count==0) {
        //empty shi
        return;
    }
 
    memcpy(buf_dst, bufs[buf_read], SERIAL_START_BUFSIZE);
}

void circ_buf::pop(){
    if (buf_count==0) {
        //empty shi
        return;
    }

    buf_read = (buf_read+1) % CIRC_BUF_SIZE;
    buf_count--;
}

bool circ_buf::empty() const{
    return buf_count==0;
}

std::string circ_buf::get() {
    if (buf_count==0) {
        return std::string();
    }

    const char* src = reinterpret_cast<const char*>(bufs[buf_read]);
    return std::string(src, strnlen(src, SERIAL_START_BUFSIZE));
}


int serial_manager::baudrate=115200;
//std::string serial_manager::buffer;
uint8_t serial_manager::buffer[SERIAL_START_BUFSIZE];
size_t serial_manager::bufsize = 0;
circ_buf serial_manager::messages=circ_buf();;

int serial_manager::get_baudrate(){
    return baudrate;
}

void serial_manager::set_baudrate(int p_baudrate){
    baudrate=p_baudrate;
}

void serial_manager::begin(int p_baudrate){
    baudrate=p_baudrate;

    //buffer.reserve(SERIAL_START_BUFSIZE);
    buffer[0]='\0';
    bufsize=0;

    Serial.begin(baudrate);
}

void serial_manager::update(){
    while(Serial.available()>0){
        uint8_t tmp=Serial.read();

        if(tmp=='\r'||tmp=='\n'){
            if(bufsize>0){
                messages.push(buffer);

                //buffer.clear();
                buffer[0]='\0';
                bufsize=0;
            }

        }else if(bufsize>=SERIAL_START_BUFSIZE-1){
            //discard the message since it's too long (so we prevent further problems)
            buffer[0]='\0';
            bufsize=0;
        }else{
            buffer[bufsize]=tmp;
            bufsize++;
            buffer[bufsize]='\0';
        }
    }
}

/*std::string& serial_manager::buf(){
    return buffer;
}*/

uint8_t* serial_manager::buf(){
    return buffer;
}

bool serial_manager::available(){
    return !messages.empty();
}

void serial_manager::get_msg(uint8_t dst[SERIAL_START_BUFSIZE]){
    messages.get(dst);    
}

std::string serial_manager::get_msg(){
    return messages.get();
}

void serial_manager::remove_msg(){
    messages.pop();
}