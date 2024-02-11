#include "Ble.h"
#include <ArduinoBLE.h>

#include <esp_log.h>
static const char* TAG = "Ble";

#define DISPLAY_NAME "Thought Timer"

class BleImpl {
  public:
    BleImpl() :
        buttonService("00001234-0000-1000-8000-00805f9b34fb")
        ,buttonCharacteristic("00001235-0000-1000-8000-00805f9b34fb", BLERead | BLENotify)
        ,batteryCharacteristic("00001236-0000-1000-8000-00805f9b34fb", BLERead | BLENotify, 10)
    { }

    BLEService buttonService;
    BLEUnsignedIntCharacteristic buttonCharacteristic;
    BLEStringCharacteristic batteryCharacteristic;

    std::string prevBattery;

    void setup();
    void loop();
    void sleep();
};

Ble::Ble() : pImpl(new BleImpl()){ }
Ble::~Ble() { delete pImpl; }

void Ble::setup() { pImpl->setup(); }
void BleImpl::setup() {
    if (!BLE.begin()) {
        ESP_LOGE(TAG, "Failed to initialize BLE!");
        while (1);
    }

    buttonService.addCharacteristic(buttonCharacteristic);
    buttonService.addCharacteristic(batteryCharacteristic);
    BLE.addService(buttonService);

    batteryCharacteristic.writeValue("UNSET");

    // Build scan response data packet
    BLEAdvertisingData scanData;
    scanData.setLocalName(DISPLAY_NAME);
    BLE.setScanResponseData(scanData);

    // Build advertising data packet
    BLEAdvertisingData advData;
    advData.setAdvertisedService(buttonService);
    BLE.setAdvertisingData(advData);

    BLE.advertise();
}

void Ble::loop() { pImpl->loop(); }
void BleImpl::loop() {
    BLE.poll();
}

void Ble::writeButton(unsigned long  time) {
    pImpl->buttonCharacteristic.writeValue(time);
}

void Ble::writeBattery(std::string battery) {
    if (battery == pImpl->prevBattery) { return; }
    pImpl->batteryCharacteristic.writeValue(battery.c_str());
    pImpl->prevBattery = battery;
}

void Ble::sleep() { pImpl->sleep(); }
void BleImpl::sleep() {
    if (BLE.connected()) {
        BLE.disconnect();
    }

    BLE.end();
}