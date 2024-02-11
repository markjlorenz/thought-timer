#pragma once

#include <Arduino.h>

class BleImpl;

class Ble {
    private:
        BleImpl *pImpl;

    public:
        Ble();
        ~Ble();

        void setup();
        void loop();
        void sleep();
        void writeButton(unsigned long time);

        // UBaseType_t getHighWaterMark(TaskHandle_t taskHandle);
        // std::string getHeapStats();

        // std::string getAllStats();
};
