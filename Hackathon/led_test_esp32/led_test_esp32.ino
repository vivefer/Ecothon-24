// Include necessary libraries
//#include <Arduino.h>

// Define the pin connected to the flow sensor
#define FLOW_SENSOR_PIN 4 // Change this to the pin number you're using

// Define variables
volatile unsigned int pulseCount; // Number of pulses from the flow sensor
float flowRate; // Flow rate in liters per minute (L/min)
unsigned long lastTime;
unsigned long elapsedTime;

// Interrupt service routine for the flow sensor
void IRAM_ATTR flowSensorISR() {
  pulseCount++;
}

// Setup function
void setup() {
  Serial.begin(9600);
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP); // Set the pin as input
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowSensorISR, RISING); // Attach interrupt to the pin
  pulseCount = 0;
  flowRate = 0.0;
  lastTime = 0;
}

// Loop function
void loop() {
  // Calculate flow rate every second
  elapsedTime = millis() - lastTime;
  if (elapsedTime > 1000) {
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN)); // Stop interrupts while calculating
    flowRate = (1000.0 / (elapsedTime / 1000.0)) * pulseCount/1000; // Flow rate in pulses per second
    flowRate /= 7.5; // Convert pulses per second to liters per minute
    lastTime = millis();
    pulseCount = 0;
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowSensorISR, RISING); // Re-attach interrupt
    Serial.print("Flow rate: ");
    Serial.print(flowRate);
    Serial.println(" L/min");
  }
}
