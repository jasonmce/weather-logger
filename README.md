# Arduino Weather Logger

![Weather Logger Mounted](./readme-media/weather-logger-mounted.webp)

An IoT weather monitoring system that collects temperature, humidity, and light level data and sends it to ThingSpeak for visualization and analysis.  Design based on parts I had lying around collecting dust from previously completed or abandoned projects.  Long term goal is to include additional sensors to monitor my garden and control my rain barrel based irrigation system based on weather conditions.

I think total cost of the parts would be about $35.
- Solar panel = $13
- Solar charge controller w/battery = $16
- NodeMCU = $5
- DHT22 = $2
- Photocell = $1
Next time I'll use a cheaper solar panel and charge controller, but I intentionally paid more for ease of development (buying time).

## Features

- 🌡️ Temperature monitoring using DHT22 sensor
- 💧 Humidity measurement
- ☀️ Light level detection
- 📶 WiFi signal strength monitoring
- 📊 Data visualization provided by ThingSpeak
- 🔔 Monitoring alarms provided by ThingSpeak
- 🔋 Power-efficient design with deep sleep mode
- 🖥️ Local OLED display for current readings

## Hardware Requirements

- NodeMCU V2.0 (ESP8266)
    Could easily use any ESP8266 or ESP32 board (just change wifi library)
    WiFi access point within range of the NodeMCU
- DHT22 Temperature & Humidity Sensor
- Photocell (Light Dependent Resistor)
- 10kΩ Resistor
- Breadboard and jumper wires
- USB cable for programming

## Hardware Setup

### Pin Connections

| NodeMCU Pin | Connection               |
|-------------|--------------------------|
| D0          | Connected to RST for deep sleep |
| D1          | DHT22 Data Pin           |
| A0          | Light Sensor Circuit     |
| 3.3V        | Power for sensors        |
| GND         | Ground                   |

### Light Sensor Circuit
1. Connect one leg of the photocell to 3.3V
2. Connect the other leg to A0 and one end of 10kΩ resistor
3. Connect the other end of the resistor to GND

## Software Requirements

- Arduino IDE with ESP8266 board support
- Required Arduino Libraries:
  - `ESP8266WiFi`
  - `ThingSpeak`
  - `DHT sensor library`

## Setup Instructions

1. **Clone this repository** to your Arduino projects folder
2. **Rename `credentials.example.h` to `credentials.h`**
3. **Edit `credentials.h`** with your WiFi and ThingSpeak credentials:
   ```h
   #define SECRET_SSID "your_wifi_ssid"
   #define SECRET_PASS "your_wifi_password"
   #define SECRET_CH_ID 1109913         // Replace with your ThingSpeak Channel ID
   #define SECRET_WRITE_APIKEY "YOUR_WRITE_API_KEY"  // Replace with your ThingSpeak Write API Key
   ```

4. **Upload the sketch** to your NodeMCU
5. **Monitor the Serial Console** (9600 baud) to verify operation
6. **Monitor the OLED display** to verify valid data is being displayed

## ThingSpeak Integration

This project sends data to a ThingSpeak channel with the following field mapping:

| Field | Measurement |
|-------|-------------|
| 1     | Temperature (°F) |
| 2     | Light Level (%) |
| 3     | Humidity (%) |
| 4     | WiFi Signal Strength (dBm) |

### Channel Configuration

![ThingSpeak Channel Settings](./readme-media/weather-logger-thingspeak-channel-settings.webp)

### Data Visualization

![ThingSpeak Channel Display](./readme-media/weather-logger-thingspeak-channel-display.webp)

View your data at: [ThingSpeak Channel](https://thingspeak.mathworks.com/channels/1109913/)

## Power Management

The device is designed for low power consumption:
- Deep sleep mode between readings (default: 30 minutes)
- Wakes up to take measurements and transmit data
- LED indicator shows when the device is active

## Field Testing

One of the best ways to test this device is to mount it to your solar power system and let it run for a while.  Make sure the forecast is free of rain if you leave it exposed like this, and has similar sun exposore to where you plan to eventually leave it.  The device will collect data for a while and then go to sleep.  When it wakes up, it will collect more data and go to sleep again.  This process will repeat as long as the solar power system stays charged.

Removing the solar input from the charging system until the battery is drained will allow you to test the device's power management and recovery capabilities.

![Field Test Setup](./readme-media/weather-logger-field-test-layout.webp)

## Final Assembly

I used TinkerCAD to design a box to house the NodeMCU breadboard and power components I had in a weatherproof enclosure that could be mounted behind the solar panel for weather protection and consolidation.  It's translucent so the power LED's faint glow can be seen through the enclosure at night.

For reliability, I replaced the solar power jack input with wires clamped into the circuit and USB power cable with direct wiring.

![Circuit Layout](./readme-media/weather-logger-layout-before-boxing.webp)

![Components Layout](./readme-media/weather-logger-components-in-box.webp)

## Customization

You can adjust the following in the code:
- Sleep duration: Modify `sleepSeconds` in `weather-logger.ino`
- Sensor pins: Update pin definitions in the respective header files
- Measurement units: Adjust temperature conversion in `dht22.cpp` if needed

## Troubleshooting

- **Device not connecting to WiFi**: Check your credentials and network settings
- **Incorrect sensor readings**: Verify wiring and sensor connections
- **Deep sleep not working**: Ensure D0 is connected to RST
- **Upload issues**: Disconnect D0 from RST before uploading new code

## Potential improvements

- Redesign the enclosre to be more weatherproof and provide easier access to the internals
- Externally mounted battery level indicator on the enclosure
- Weatherproof reset button, because not having one is honestly hubris
- Switch to a more energy efficient ESP32
- Switch to an ESP32 with more analog inputs for things like soil moisture monitoring
- Add a rain gauge to monitor rainfall and a wind speed sensor to monitor wind speed, since they both use same base code for hall effect sensors
## License

This project is open source and available under the MIT License.

## Acknowledgments

- Based on various open-source weather monitoring projects
- Uses ThingSpeak for data visualization
- Inspired by the ESP8266 community
