/*
Copyright (c) 2021 Jakub Mandula

Example of using one PZEM module with Software Serial interface.
================================================================

If only RX and TX pins are passed to the constructor, software 
serial interface will be used for communication with the module.

*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>

#if defined(ESP32)
#error "Software Serial is not supported on the ESP32"
#endif

/* Use software serial for the PZEM
 * Pin 2 Rx (Connects to the Tx pin on the PZEM)
 * Pin 3 Tx (Connects to the Rx pin on the PZEM)
*/
#if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
#define PZEM_RX_PIN 2
#define PZEM_TX_PIN 3
#endif

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1  // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


SoftwareSerial pzemSWSerial(PZEM_RX_PIN, PZEM_TX_PIN);
PZEM004Tv30 pzem(pzemSWSerial);


// Timing for transitions
unsigned long previousMillis = 0;
const long interval = 1000;  // 1-second interval
bool showFirstScreen = true;

void setup() {
  // Start Serial communication
  Serial.begin(9600);

  // Initialize OLED with detected I2C address (e.g., 0x3C or 0x3D)
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ;  // Halt if OLED initialization fails
  }

  display.clearDisplay();
  display.setTextSize(2);       // Set text size to 2
  display.setTextColor(WHITE);  // Set text color
}

void loop() {
  unsigned long currentMillis = millis();

  // Switch between screens every second
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    showFirstScreen = !showFirstScreen;  // Toggle screen
  }
  // Serial.print("Custom Address:");
  // Serial.println(pzem.readAddress(), HEX); // I do not need this line of code. It does print the I2C address the OLED is assigned to.

  // Read the data from the sensor
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  float pf = pzem.pf();

  Serial.println();

  // Oled display
  display.clearDisplay();

  if (showFirstScreen) {
    // Screen 1: Voltage, Current, and Power
    display.setCursor(0, 0);
    display.print("V:");
    display.print(voltage);
    display.println("V");

    display.setCursor(0, 20);
    display.print("C:");
    display.print(current);
    display.println("A");

    display.setCursor(0, 40);
    display.print("P:");
    display.print(power);
    display.println("W");
  } else {
    // Screen 2: Energy, Frequency, and Power Factor
    display.setCursor(0, 0);
    display.print("E:");
    display.print(energy, 3);
    display.println("kWh");

    display.setCursor(0, 20);
    display.print("F:");
    display.print(frequency, 1);
    display.println("Hz");

    display.setCursor(0, 40);
    display.print("PF:");
    display.print(pf);
  }

  // Update the OLED
  display.display();
  delay(1500);
}
