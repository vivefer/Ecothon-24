//#include <Arduino.h>

#define FLOW_SENSOR_PIN 4 // Change this to the pin number you're using

volatile unsigned int pulseCount; // Number of pulses from the flow sensor
float flowRate; // Flow rate in liters per minute (L/min)
unsigned long lastTime;
float hourlyFlowRate; // Flow rate in liters per hour (L/h)
float dailyFlowRate;  // Flow rate in liters per day (L/d)
float weeklyFlowRate; // Flow rate in liters per week (L/w)
float monthlyFlowRate; // Flow rate in liters per month (L/m)
float yearlyFlowRate; // Flow rate in liters per year (L/y)
unsigned long lastHour;
unsigned long lastDay;
unsigned long lastWeek;
unsigned long lastMonth;
unsigned long lastYear;

void IRAM_ATTR flowSensorISR() {
  pulseCount++;
}

void setup() {
  Serial.begin(9600);
  pinMode(FLOW_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowSensorISR, RISING);
  pulseCount = 0;
  flowRate = 0.0;
  hourlyFlowRate = 0.0;
  dailyFlowRate = 0.0;
  weeklyFlowRate = 0.0;
  monthlyFlowRate = 0.0;
  yearlyFlowRate = 0.0;
  lastTime = 0;
  lastHour = 0;
  lastDay = 0;
  lastWeek = 0;
  lastMonth = 0;
  lastYear = 0;
}

void loop() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = currentTime - lastTime;

  if (elapsedTime > 1000) {
    detachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN));
    flowRate = (1000.0 / (elapsedTime / 1000.0)) * pulseCount/1000; // Flow rate in pulses per second
    flowRate /= 7.5; // Convert pulses per second to liters per minute
    
    // Update hourly flow rate
    if (currentTime - lastHour >= 3600000) {
      hourlyFlowRate = pulseCount / 60.0;
      lastHour = currentTime;
    }

    // Update daily flow rate
    if (currentTime - lastDay >= 86400000) {
      dailyFlowRate = pulseCount / 1440.0;
      lastDay = currentTime;
    }

    // Update weekly flow rate
    if (currentTime - lastWeek >= 604800000) {
      weeklyFlowRate = pulseCount / 10080.0;
      lastWeek = currentTime;
    }

    // Update monthly flow rate
    if (currentTime - lastMonth >= 2592000000) {
      monthlyFlowRate = pulseCount / 43800.0;
      lastMonth = currentTime;
    }

    // Update yearly flow rate
    if (currentTime - lastYear >= 31536000000) {
      yearlyFlowRate = pulseCount / 525600.0;
      lastYear = currentTime;
    }

    lastTime = currentTime;
    pulseCount = 0;
    attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR_PIN), flowSensorISR, RISING);

    Serial.print("Flow rate: ");
    Serial.print(flowRate);
    Serial.println(" L/min");
    Serial.print("Hourly flow rate: ");
    Serial.print(hourlyFlowRate);
    Serial.println(" L/h");
    Serial.print("Daily flow rate: ");
    Serial.print(dailyFlowRate);
    Serial.println(" L/d");
    Serial.print("Weekly flow rate: ");
    Serial.print(weeklyFlowRate);
    Serial.println(" L/w");
    Serial.print("Monthly flow rate: ");
    Serial.print(monthlyFlowRate);
    Serial.println(" L/m");
    Serial.print("Yearly flow rate: ");
    Serial.print(yearlyFlowRate);
    Serial.println(" L/y");
  }
}
