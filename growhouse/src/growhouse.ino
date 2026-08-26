/*
 * Automated Growhouse
 * Agris Miezitis
 *
 * Three control loops on one Arduino: irrigation, ventilation and lighting.
 * Nothing in loop() blocks, so all three keep running at the same time.
 *
 * Hardware:
 *   Soil moisture - capacitive sensor v2.0 on A1 (reads high when dry)
 *   Temperature   - DHT11 on D2
 *   Pump          - 5 V pump on D7
 *   Fan           - 12 V fan on D3
 *   Vent hatch    - servo on D4
 *   Grow lamp     - 230 V lamp through a relay on D8
 *   Display       - SSD1306 OLED over I2C
 */

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <Servo.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
DHT dht(2, DHT11);
Servo ventServo;

int fanPin = 3;
int ventPin = 4;
int waterPumpPin = 7;
int ledPin = 8;
int soilMoisturePin = A1;

/* Pump timing. The lockout is the important part: soil that has just been
 * watered still reads dry, because the water has not soaked in yet. Without
 * the wait the pump would start again on every reading and drown the plant. */
const unsigned long pumpRunDuration = 5000;        // 5 seconds
const unsigned long pumpLockoutDuration = 300000;  // 5 minutes

const unsigned long halfDay = 12UL * 60 * 60 * 1000;
const unsigned long sensorInterval = 1000;         // the DHT11 cannot be read faster

/* Soil sensor calibration, measured with this sensor. A capacitive sensor
 * gives a high reading in dry soil and a low one in wet soil. */
const int dry = 330;
const int wet = 196;
const int sensorMargin = 50;       // a reading outside this band means a broken sensor
const int moistureThreshold = 50;  // water when moisture drops to this or below

enum State {
  WAITING,
  PUMP_ON,
  PUMP_OFF
};

State currentState = WAITING;
unsigned long pumpTimer = 0;
unsigned long lightTimer = 0;
unsigned long sensorTimer = 0;

float temperature = NAN;
int percentageMoisture = 0;
bool soilSensorOK = false;
bool displayWorking = true;

void setup() {
  Serial.begin(9600);

  /* If the display does not start, carry on without it. Watering and
   * ventilation matter more than the screen. */
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 not found, running without the display"));
    displayWorking = false;
  } else {
    display.display();
    delay(2000);
    display.clearDisplay();
  }

  pinMode(fanPin, OUTPUT);
  pinMode(waterPumpPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  dht.begin();
  ventServo.attach(ventPin);   // the servo library takes care of this pin
  digitalWrite(ledPin, HIGH);
}

void loop() {
  unsigned long currentMillis = millis();

  // ---------- Read the sensors once a second ----------
  if (currentMillis - sensorTimer >= sensorInterval) {
    sensorTimer = currentMillis;
    readSoil();
    readTemperature();
    updateDisplay();
  }

  // ---------- Irrigation ----------
  switch (currentState) {
    case WAITING:
      // Only water if the sensor reading makes sense and the soil is dry.
      if (soilSensorOK && percentageMoisture <= moistureThreshold) {
        currentState = PUMP_ON;
        digitalWrite(waterPumpPin, HIGH);
        pumpTimer = currentMillis;
      }
      break;

    case PUMP_ON:
      if (currentMillis - pumpTimer >= pumpRunDuration) {
        currentState = PUMP_OFF;
        digitalWrite(waterPumpPin, LOW);
        pumpTimer = currentMillis;
      }
      break;

    case PUMP_OFF:
      if (currentMillis - pumpTimer >= pumpLockoutDuration) {
        currentState = WAITING;
      }
      break;
  }

  // ---------- Ventilation ----------
  /* The 2 degree gap between the two limits is deliberate. With one setpoint
   * the fan would switch on and off constantly around that temperature. */
  if (!isnan(temperature)) {
    if (temperature > 27) {
      digitalWrite(fanPin, HIGH);
      ventServo.write(45);
    } else if (temperature < 25) {
      digitalWrite(fanPin, LOW);
      ventServo.write(0);
    }
  }

  // ---------- Grow lamp, 12 hours on and 12 hours off ----------
  if (currentMillis - lightTimer >= halfDay) {
    digitalWrite(ledPin, !digitalRead(ledPin));
    lightTimer = currentMillis;
  }
}

void readSoil() {
  int sensorVal = analogRead(soilMoisturePin);

  /* An unplugged sensor floats to one end of the range. Without this check
   * it reads as bone dry and the pump runs on every cycle. */
  soilSensorOK = (sensorVal >= wet - sensorMargin && sensorVal <= dry + sensorMargin);

  if (!soilSensorOK) {
    Serial.print(F("Soil sensor reading out of range ("));
    Serial.print(sensorVal);
    Serial.println(F("), not watering. Check the sensor."));
    return;
  }

  // dry maps to 0 % and wet maps to 100 %, so this really is moisture.
  percentageMoisture = constrain(map(sensorVal, dry, wet, 0, 100), 0, 100);

  Serial.print(F("Soil moisture: "));
  Serial.print(percentageMoisture);
  Serial.println(F("%"));
}

void readTemperature() {
  temperature = dht.readTemperature();

  if (isnan(temperature)) {
    Serial.println(F("Error reading temperature. Check the DHT sensor."));
    return;
  }

  Serial.print(F("Temperature: "));
  Serial.println(temperature);
}

void updateDisplay() {
  if (!displayWorking) {
    return;
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 0);
  display.print(F("SILTUMNICA"));

  display.setCursor(0, 20);
  display.print(F("T: "));
  if (isnan(temperature)) {
    display.println(F("--"));
  } else {
    display.println(temperature);
  }

  display.setCursor(0, 40);
  display.print(F("M: "));
  if (soilSensorOK) {
    display.println(percentageMoisture);   // no % sign, matching the build photo
  } else {
    display.println(F("--"));
  }

  display.display();
}
