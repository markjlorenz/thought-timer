#pragma once
#include <Arduino.h>
#include <string>

class BatteryImpl;

class Battery {
    private:
        BatteryImpl *pImpl;

    public:
        Battery();
        ~Battery();

        void setup();
        std::string check();
};
