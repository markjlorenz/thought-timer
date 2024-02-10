// Include necessary libraries
#include <Arduino.h>
#include <esp_sleep.h>

#include <esp_log.h>
static const char* TAG = "main";

// Define the pin number for the LED
const gpio_num_t ledPin = GPIO_NUM_5;
const gpio_num_t buttonPin = GPIO_NUM_14;

BaseType_t pinCore = tskNO_AFFINITY; // 1
const int taskUnit = 1024;

TaskHandle_t blinkTaskHandle;
void blinkTask(void *);

TaskHandle_t sleepTaskHandle;
void sleepTask(void *);

#include <ArduinoBLE.h>
BLEService buttonService("00001234-0000-1000-8000-00805f9b34fb");
BLEUnsignedIntCharacteristic buttonCharacteristic("00001235-0000-1000-8000-00805f9b34fb", BLERead | BLENotify);


// Function to put the board into deep sleep mode
void deepSleep() {
  Serial.println("Going into deep sleep...");
  esp_sleep_enable_ext0_wakeup(buttonPin, LOW); // Enable wakeup on button press
  esp_deep_sleep_start();
}

void setup() {
  Serial.begin(115200);

  if (!BLE.begin()) {
    Serial.println("failed to initialize BLE!");
    while (1);
  }

  buttonService.addCharacteristic(buttonCharacteristic);
  BLE.addService(buttonService);

  // Build scan response data packet
  BLEAdvertisingData scanData;
  scanData.setLocalName("Thought Timer");
  BLE.setScanResponseData(scanData);

  // Build advertising data packet
  BLEAdvertisingData advData;
  advData.setAdvertisedService(buttonService);
  BLE.setAdvertisingData(advData);

  BLE.advertise();

  // Set the LED pin as an output
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

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
  BLE.poll();
  vTaskDelay(100 / portTICK_PERIOD_MS); //miliseconds
}

void blinkTask(void* parameter) {
  bool buttonDown = false;

  while(true) {
    if (digitalRead(buttonPin) == LOW && !buttonDown) {
      buttonDown = true;

      // LED ON
      digitalWrite(ledPin, HIGH);
      digitalWrite(LED_BUILTIN, HIGH);
      ESP_LOGI(TAG, "BLINK");

      buttonCharacteristic.writeValue(millis());

      vTaskDelay(500 / portTICK_PERIOD_MS); //miliseconds
      continue;
    }

    if (digitalRead(buttonPin) == HIGH) {
      buttonDown = false;
    }

    // Turn the LED off
    digitalWrite(ledPin, LOW);
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(100 / portTICK_PERIOD_MS); //miliseconds
  }
}

void sleepTask(void* parameter) {
  const unsigned long inactivityThreshold = 240000; // 60 seconds
  unsigned long lastActivityTime = millis();
  int lastState = digitalRead(buttonPin);

  while(true) {
    if (digitalRead(buttonPin) != lastState) {
      lastActivityTime = millis();
    }
    lastState = digitalRead(buttonPin);

    if (millis() - lastActivityTime >= inactivityThreshold) {
      ESP_LOGI(TAG, "No activity for 60 seconds, going to sleep...");
      vTaskDelay(100 / portTICK_PERIOD_MS); // Small delay for stability
      deepSleep(); // Enter deep sleep mode
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
