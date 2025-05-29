# Capstone

==========================
Smart Indoor Air Quality Monitoring System
Herbert Morrison Jr. – [62742023]
==========================

📌 OVERVIEW:
This project is a portable indoor air quality monitoring system designed to measure and report real-time air quality data. It tracks PM1.0, PM2.5, PM4.0, PM10, VOC, NOx, temperature, and humidity using the SEN55 sensor. The data is displayed locally on an LCD and remotely via MQTT through a mobile or desktop dashboard.

---

🧰 COMPONENTS:
- SEN55 Environmental Sensor (for PM, VOC, NOx, RH, Temp)
- ATmega328P Microcontroller (Arduino Uno/Nano)
- SIM800L GSM Module (for MQTT communication)
- LCD 16x2 I2C Display
- LEDs (Red, Green, Blue)
- Buzzer (for alert)
- Power Source (USB or Battery)
- Optional: Enclosure, SD card module (future work)

---

📦 FOLDER STRUCTURE:
- /report → Final capstone report (PDF)
- /presentation → Final presentation slides (PPTX)
- /demo → Demo video (if applicable)
- /documentation → This readme.txt and any manuals
- /code → Source code (Arduino .ino)
- /resources → Circuit diagram, sensor datasheets, etc.
- /data → Collected air quality readings & analysis
- /other → Extra files (photos, charts, calibration notes)

---

🔧 SETUP INSTRUCTIONS:

1. 🛠 HARDWARE CONNECTIONS:
   - Connect SEN55 to Arduino via I2C (SDA, SCL)
   - Connect SIM800L to SoftwareSerial (pins 8, 9)
   - Connect LEDs to pins A1–A3, buzzer to pin 4
   - Connect LCD via I2C to 0x27 address

2. 💻 SOFTWARE INSTALLATION:
   - Install [Arduino IDE](https://www.arduino.cc/en/software)
   - Required libraries (via Library Manager or GitHub):
     - SensirionI2CSen5x
     - TinyGSM
     - PubSubClient
     - LiquidCrystal_I2C
     - Timer
     - LowPower (by Rocket Scream)

3. 🔄 UPLOAD & RUN:
   - Open the .ino file in the Arduino IDE
   - Select the correct board and COM port
   - Upload the code
   - Monitor via serial for logs or use MQTT dashboard

4. 📡 MQTT CONFIGURATION:
   - Broker: mqtt-dashboard.com
   - Topics published:
     - hhh/herbert/pm1
     - hhh/herbert/pm25
     - hhh/herbert/pm4
     - hhh/herbert/pm10
     - hhh/herbert/voc
     - hhh/herbert/nox
     - hhh/herbert/hum
     - hhh/herbert/temp
   - Use **MQTT Explorer (desktop)** or **IoT MQTT Panel (mobile)** for real-time visualization

---

⚠️ FEATURES:
- Calculates average PM for alert logic
- Sends data to MQTT every 5 minutes
- Displays rolling sensor data on LCD
- Enters sleep mode after 10 minutes of inactivity
- Uses watchdog timer for fault recovery
- LED + buzzer alerts based on pollution level

---

📘 FUTURE WORK:
- Add EPA-based AQI calculation and display
- Enable cloud storage and mobile notification integration
- Use SD card for offline data backup
- Implement solar-powered version for energy autonomy

---

✉️ CONTACT:
Herbert Morrison Jr.  
Ashesi University – Capstone 2025  
Email: morrison29th@gmail.com
Link to GitHub Repository: 
