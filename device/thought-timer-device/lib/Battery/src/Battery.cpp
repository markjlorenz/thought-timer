#include "Battery.h"
#include <UMS3.h>

#include <esp_log.h>
static const char* TAG = "Battery";

class BatteryImpl {
  public:
    BatteryImpl() { }

    UMS3 ums3;

    void setup();
    std::string check();
};

Battery::Battery() : pImpl(new BatteryImpl()){ }
Battery::~Battery() { delete pImpl; }

void Battery::setup() { pImpl->setup(); }
void BatteryImpl::setup() {
  ums3.begin();
}

std::string Battery::check() { return pImpl->check(); }
std::string BatteryImpl::check() {
    float battery = ums3.getBatteryVoltage();

    // USB power
    if (ums3.getVbusPresent()) {
        return "USB POWER";
    }

    // running from battery
    if (battery < 3.1) { // critical!
        return "CRITICAL";
    } else {
        return std::to_string(battery);
    }
}