#pragma once

#include<string>
#include<queue>

#include<cstdint>

#define SERIAL_START_BUFSIZE 1024
#define CIRC_BUF_SIZE 16

class circ_buf {
    private:
        size_t buf_read;
        size_t buf_write;
        size_t buf_count;
        bool overwrite;

        uint8_t bufs[CIRC_BUF_SIZE][SERIAL_START_BUFSIZE];

    public:
        circ_buf(bool overwrite=true);

        void push(uint8_t buf[SERIAL_START_BUFSIZE]);
        void get(uint8_t buf_dst[SERIAL_START_BUFSIZE]);
        std::string get();
        void pop();
        bool empty() const;

};

class serial_manager {
    private:
        static int baudrate;
        //static std::string buffer;
        static uint8_t buffer[SERIAL_START_BUFSIZE];
        static size_t bufsize;
        //static std::queue<std::string> messages;
        static circ_buf messages;

    public:
        static void set_baudrate(int p_baudrate);
        static int get_baudrate();

        static void begin(int p_baudrate=115200);

        static void update();

        //static std::string& buf();
        static uint8_t* buf();
        static bool available();
        static void get_msg(uint8_t dst[SERIAL_START_BUFSIZE]);
        static std::string get_msg();
        static void remove_msg();

};