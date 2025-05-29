#define TINY_GSM_MODEM_SIM800

#include <SoftwareSerial.h>
#include <TinyGsmClient.h>
#include <PubSubClient.h>
#include <SensirionI2CSen5x.h>
#include <LiquidCrystal_I2C.h>
#include "Timer.h"
#include <LowPower.h> // Library for sleep mode
#include <avr/wdt.h>   // Include watchdog timer library

// Device Pins
int redLED = A2;
int blueLED = A3;
int greenLED = A1;
int buzzerPin = 4;

LiquidCrystal_I2C lcd(0x27, 16, 2);
SoftwareSerial SerialAT(8, 9); // RX, TX

// Sensor and Communication Variables
float massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0, massConcentrationPm10p0;
float ambientHumidity, ambientTemperature, vocIndex, noxIndex;
const char apn[]  = "internet";
const char user[] = "";
const char pass[] = "";
const char* broker = "mqtt-dashboard.com";
const char* topics[] = {
  "hhh/herbert/pm10", "hhh/herbert/pm1", "hhh/herbert/pm25", "hhh/herbert/pm4",
  "hhh/herbert/voc", "hhh/herbert/hum", "hhh/herbert/temp", "hhh/herbert/nox"
};

TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);
SensirionI2CSen5x sen5x;
Timer t;
char myBuffer[20];
long lastReconnectAttempt = 0;

unsigned long lastReadingTime = 0;
unsigned long sleepThreshold = 600000; // Sleep after 10 minutes in ms

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.print(F("System start"));

  // Initialize LEDs and Buzzer
  pinMode(blueLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  Serial.begin(9600);
  SerialAT.begin(9600);
  setupSensor();
  t.every(60000, FiveMinutes);

  // Watchdog timer setup (reset if not refreshed in 8 seconds)
  wdt_enable(WDTO_8S);

  modem.restart();
  if (!modem.waitForNetwork()) while (true);
  lcd.setCursor(0, 1);
  lcd.print(F("Connected"));

  if (!modem.gprsConnect(apn, user, pass)) while (true);
  lcd.clear();
  lcd.print(F("GPRS OK"));

  mqtt.setServer(broker, 1883);
  mqtt.setCallback(mqttCallback);
  if (!mqttConnect()) lcd.print(F("MQTT Fail"));

  mqtt.publish(topics[1], "Ashesi Pollution");
  lcd.setCursor(0, 1);
  lcd.print(F("MQTT Up"));
}

void loop() {
  t.update();
  pollutionLoop();

  // Watchdog timer reset
  wdt_reset();

  // Check if it's time to go to sleep
  if (millis() - lastReadingTime > sleepThreshold) {
    enterSleepMode();  // Enter sleep mode after the specified inactivity time
  }

  if (mqtt.connected()) {
    mqtt.loop();
  } else {
    unsigned long t = millis();
    if (t - lastReconnectAttempt > 10000L) {
      lastReconnectAttempt = t;
      if (mqttConnect()) lastReconnectAttempt = 0;
    }
  }
}

boolean mqttConnect() {
  const char* clientID = "99928829292";
  return mqtt.connect(clientID);
}

void mqttCallback(char* topic, byte* payload, unsigned int len) {
  // Optional MQTT receive logic
}

void setupSensor() {
  Wire.begin();
  sen5x.begin(Wire);
  sen5x.deviceReset();
  sen5x.startMeasurement();
}

void pollutionLoop() {
  if (sen5x.readMeasuredValues(
    massConcentrationPm1p0, massConcentrationPm2p5, massConcentrationPm4p0,
    massConcentrationPm10p0, ambientHumidity, ambientTemperature, vocIndex, noxIndex) != 0)
    return;

  float avg = (massConcentrationPm10p0 + massConcentrationPm1p0 + 
               massConcentrationPm2p5 + massConcentrationPm4p0) / 4.0;

  if (avg <= 10) {
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED, LOW);
    digitalWrite(blueLED, LOW);
    noTone(buzzerPin);  // Buzzer OFF
  } 
  else if (avg <= 100) {
    digitalWrite(greenLED, HIGH);
    digitalWrite(redLED, LOW);
    digitalWrite(blueLED, LOW);
    noTone(buzzerPin);  // Buzzer OFF
  } 
  else {
    // Blue LED flashes rapidly, Buzzer beeps in sync
    digitalWrite(greenLED, LOW);
    digitalWrite(redLED, LOW);
    for (int i = 0; i < 20; i++) { // Flashes for 2 seconds (20 x 100ms)
      digitalWrite(blueLED, HIGH);
      tone(buzzerPin, 1000); // Buzzer ON
      delay(100);
      digitalWrite(blueLED, LOW);
      noTone(buzzerPin);     // Buzzer OFF
      delay(100);
    }
  }

  displayReadings();
  lastReadingTime = millis();  // Update the last reading time after every data cycle
}

void displayReadings() {
  lcd.setCursor(0, 0);
  lcd.print(F("PM1:")); lcd.print(massConcentrationPm1p0, 1);
  lcd.setCursor(0, 1);
  lcd.print(F("PM2.5:")); lcd.print(massConcentrationPm2p5, 1);
  delay(1500);
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(F("PM4:")); lcd.print(massConcentrationPm4p0, 1);
  lcd.setCursor(0, 1);
  lcd.print(F("PM10:")); lcd.print(massConcentrationPm10p0, 1);
  delay(1500);
  lcd.clear();
}

void FiveMinutes() {
  float values[] = {
    massConcentrationPm10p0, massConcentrationPm1p0,
    massConcentrationPm2p5, massConcentrationPm4p0,
    vocIndex, ambientHumidity, ambientTemperature, noxIndex
  };

  for (byte i = 0; i < 8; i++) {
    dtostrf(values[i], 3, 2, myBuffer);
    mqtt.publish(topics[i], myBuffer, true);
    delay(100);
  }
}

// Function to enter sleep mode
void enterSleepMode() {
  lcd.clear();
  lcd.print(F("Entering Sleep Mode"));

  // Enter sleep mode for 8 seconds (using LowPower library)
  LowPower.powerDown(SLEEP_8S, ADC_OFF, BOD_OFF);  // Sleep for 8 seconds

  // After sleep, wake up and resume normal operations
  setupSensor();  // Reinitialize the sensor after waking up
  lcd.clear();
  lcd.print(F("Waking up"));
}