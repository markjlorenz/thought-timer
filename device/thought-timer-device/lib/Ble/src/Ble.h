#pragma once
#include <Arduino.h>
#include <string>

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
        void writeBattery(std::string battery);
};
