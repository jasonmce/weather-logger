/**
 * Weather Logger for ESP8266
 * 
 * Monitors environmental conditions (temperature, humidity, light levels) and transmits
 * data to ThingSpeak. Implements deep sleep for power efficiency.
 * 
 * Hardware:
 * - NodeMCU 0.9 (ESP-12E)
 * - DHT22 on D1
 * - Photocell voltage divider on A0 (10kΩ pulldown)
 * - OLED Display (SSD1306) on I2C
 * 
 * Deep Sleep:
 * - D0 connected to RST for wakeup
 * - Disconnect D0-RST before uploading
 */

// Standard Library Includes
#include <ESP8266WiFi.h>

// Third-party Libraries
#include "ThingSpeak.h"

// Project Headers
#include "dht22.h"
#include "light.h"
#include "communications.h"
#include "oled.h"

// Global Instances
WiFiClient client;      // WiFi client for network communication
Dht dht;                // Temperature and humidity sensor
LightSensor light;      // Ambient light sensor

// Configuration Constants
namespace Config {
    // ThingSpeak field indices (must match your ThingSpeak channel configuration)
    const uint8_t THERMOMETER_INDEX = 1;    // Temperature in Fahrenheit
    const uint8_t LIGHT_INDEX = 2;          // Light level percentage
    const uint8_t HUMIDITY_INDEX = 3;       // Relative humidity
    const uint8_t WIFI_STRENGTH_INDEX = 4;  // WiFi signal strength in dBm

    // Timing Constants
    const unsigned long SLEEP_DURATION_SEC = 1800;  // 30 minutes between readings
    const unsigned long DISPLAY_HOLD_MS = 5000;     // Time to show readings on display
}

// Global Variables
unsigned long sleepSeconds = Config::SLEEP_DURATION_SEC;  // Configurable sleep duration

// Forward Declarations
void enterDeepSleep();
void takeReadings();
void updateDisplay(float temp_f);
void sendToThingSpeak(float temp_f, int lux, float humidity);
 
/**
 * @brief Enters deep sleep mode for the configured duration
 * 
 * Puts the ESP8266 into deep sleep mode to conserve power between readings.
 * The device will wake up automatically after sleepSeconds.
 */
void enterDeepSleep() {
    Serial.println("Entering deep sleep for " + String(sleepSeconds) + " seconds");
    ESP.deepSleep(sleepSeconds * 1e6, WAKE_RF_DEFAULT);
}

/**
 * @brief Takes sensor readings and updates the display
 * 
 * @param[out] temp_f Reference to store temperature in Fahrenheit
 * @param[out] lux Reference to store light level in percentage
 * @param[out] humidity Reference to store relative humidity
 * 
 * @note DHT22 requires ~250ms for readings and may return data up to 2 seconds old
 */
void takeReadings(float& temp_f, int& lux, float& humidity) {
    dht.takeMeasurements();
    temp_f = dht.tempF();
    lux = light.percent();
    humidity = dht.humidity();
}

/**
 * @brief Updates the OLED display with the current temperature
 * 
 * @param temp_f Current temperature in Fahrenheit
 */
void updateDisplay(float temp_f) {
    char temp_text[20];
    snprintf(temp_text, sizeof(temp_text), "%0.1f°F", temp_f);
    drawText("Temperature", temp_text);
}

/**
 * @brief Sends sensor data to ThingSpeak
 * 
 * @param temp_f Temperature in Fahrenheit
 * @param lux Light level in percentage
 * @param humidity Relative humidity percentage
 */
void sendToThingSpeak(float temp_f, int lux, float humidity) {
    ThingSpeak.setField(Config::THERMOMETER_INDEX, temp_f);
    ThingSpeak.setField(Config::LIGHT_INDEX, static_cast<float>(lux));
    ThingSpeak.setField(Config::HUMIDITY_INDEX, humidity);
    ThingSpeak.setField(Config::WIFI_STRENGTH_INDEX, get_rssi());
    thingspeaksenddata();
}

/**
 * @brief Main setup function - runs once on device startup
 * 
 * Initializes hardware, takes readings, sends data to ThingSpeak,
 * updates the display, and puts the device back to sleep.
 */
void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    Serial.println("\nWeather Logger Starting...");
    
    // Configure LED pin
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);  // Turn LED on to indicate activity
    
    // Initialize sensors
    dht.sensorPin(D1);
    light.sensorPin(A0);
    
    // Initialize display and ThingSpeak
    oled_setup();
    ThingSpeak.begin(client);
    
    // Connect to WiFi or sleep if connection fails
    if (!connect_wifi()) {
        Serial.println("WiFi connection failed. Entering deep sleep...");
        enterDeepSleep();
        return;  // Should never reach here
    }
    
    // Take sensor readings
    float temp_f, humidity;
    int lux;
    takeReadings(temp_f, lux, humidity);
    
    // Log readings to serial
    Serial.printf("Temperature: %.1f°F, Humidity: %.1f%%, Light: %d%%\n", 
                 temp_f, humidity, lux);
    
    // Update display and send data
    updateDisplay(temp_f);
    sendToThingSpeak(temp_f, lux, humidity);
    
    // Keep display on for configured time to allow for visual checking
    delay(Config::DISPLAY_HOLD_MS);
    
    // Turn off LED and clear display to save power
    digitalWrite(LED_BUILTIN, HIGH);  // Turn LED off
    clearDisplay();
    
    enterDeepSleep();
}

/**
 * @brief Main loop function - intentionally left empty
 * 
 * The application uses deep sleep between readings, so all functionality is handled
 * in setup(). The device resets after each wakeup from deep sleep.
 */
void loop() {
    // Not used - device resets after each reading cycle
    // All functionality is handled in setup()
}
