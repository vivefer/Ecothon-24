// Define the pin for the water flow sensor
#define FLOW_SENSOR_PIN 9

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  // Set up the water flow sensor pin as input
  pinMode(FLOW_SENSOR_PIN, INPUT);
}

void loop() {
  // Read the state of the water flow sensor pin
  int sensorValue = digitalRead(FLOW_SENSOR_PIN);
  Serial.println(sensorValue);
  delay(1000); // Delay 1 second
}
