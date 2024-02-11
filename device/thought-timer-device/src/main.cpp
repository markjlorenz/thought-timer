#include <Arduino.h>
#include <esp_sleep.h>

#include <esp_log.h>
static const char* TAG = "main";

// const gpio_num_t ledPin = GPIO_NUM_5;
// const gpio_num_t buttonPin = GPIO_NUM_14;
const gpio_num_t ledPin = GPIO_NUM_11;
const gpio_num_t buttonPin = GPIO_NUM_3;

BaseType_t pinCore = tskNO_AFFINITY; // 1
const int taskUnit = 1024;

TaskHandle_t blinkTaskHandle;
void blinkTask(void *);

TaskHandle_t sleepTaskHandle;
void sleepTask(void *);

#include <Ble.h>
Ble ble;
// #include <ArduinoBLE.h>
// BLEService buttonService("00001234-0000-1000-8000-00805f9b34fb");
// BLEUnsignedIntCharacteristic buttonCharacteristic("00001235-0000-1000-8000-00805f9b34fb", BLERead | BLENotify);

// #include <UMS3.h>

void deepSleep() {
  ESP_LOGI(TAG, "Going into deep sleep...");

  // if (BLE.connected()) {
  //   BLE.disconnect();
  // }

  // BLE.end();
  ble.sleep();

  esp_sleep_enable_ext0_wakeup(buttonPin, LOW); // Enable wakeup on button press
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);

  ble.setup();
  // if (!BLE.begin()) {
  //   ESP_LOGE(TAG, "failed to initialize BLE!");
  //   while (1);
  // }

  // buttonService.addCharacteristic(buttonCharacteristic);
  // BLE.addService(buttonService);

  // // Build scan response data packet
  // BLEAdvertisingData scanData;
  // scanData.setLocalName("Thought Timer");
  // BLE.setScanResponseData(scanData);

  // // Build advertising data packet
  // BLEAdvertisingData advData;
  // advData.setAdvertisedService(buttonService);
  // BLE.setAdvertisingData(advData);

  // BLE.advertise();

  // Set the LED pin as an output
  pinMode(ledPin,       OUTPUT);
  pinMode(LED_BUILTIN,  OUTPUT);
  pinMode(buttonPin,    INPUT_PULLUP);

  xTaskCreatePinnedToCore(
    blinkTask
    ,"Blink Task"
    ,taskUnit * 2
    ,NULL
    ,10
    ,&blinkTaskHandle
    ,pinCore
  );

  xTaskCreatePinnedToCore(
    sleepTask
    ,"Sleep Task"
    ,taskUnit * 2
    ,NULL
    ,10
    ,&sleepTaskHandle
    ,pinCore
  );
}

void loop() {
  ble.loop();
  // BLE.poll();
  vTaskDelay(100 / portTICK_PERIOD_MS); //miliseconds
}

void blinkTask(void* parameter) {
  bool buttonDown = false;

  while(true) {
    if (digitalRead(buttonPin) == LOW && !buttonDown) {
      buttonDown = true;

      digitalWrite(ledPin, HIGH);
      digitalWrite(LED_BUILTIN, HIGH);
      ESP_LOGI(TAG, "BLINK");

      ble.writeButton(millis());
      // buttonCharacteristic.writeValue(millis());

      vTaskDelay(500 / portTICK_PERIOD_MS); //miliseconds
      continue;
    }

    if (digitalRead(buttonPin) == HIGH) {
      buttonDown = false;
    }

    digitalWrite(ledPin, LOW);
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(100 / portTICK_PERIOD_MS); //miliseconds
  }
}

void sleepTask(void* parameter) {
  const unsigned long inactivityThreshold = 240000; // 4 minutes
  // const unsigned long inactivityThreshold = 10000; // 10 seconds
  unsigned long lastActivityTime = millis();
  int lastState = digitalRead(buttonPin);

  while(true) {
    if (digitalRead(buttonPin) != lastState) {
      lastActivityTime = millis();
    }
    lastState = digitalRead(buttonPin);

    if (millis() - lastActivityTime >= inactivityThreshold) {
      vTaskDelay(100 / portTICK_PERIOD_MS); // Small delay for stability
      deepSleep(); // Enter deep sleep mode
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
