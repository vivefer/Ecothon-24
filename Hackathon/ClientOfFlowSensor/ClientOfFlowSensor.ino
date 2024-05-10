#include <WiFi.h>

// WiFi credentials
const char* ssid = "ECOTHON 224";
const char* password = "ku12345678";

// Define the pin connected to the flow sensor
#define FLOW_SENSOR_PIN 4 // Change this to the pin number you're using

// Define variables
volatile unsigned int pulseCount; // Number of pulses from the flow sensor
float flowRate; // Flow rate in liters per minute (L/min)
unsigned long lastTime;
unsigned long elapsedTime;
int counter;

// WiFi server port
WiFiServer server(80);

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
  counter = 0;

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  // Start the server
  server.begin();
  Serial.println("Server started");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP());
}

// Loop function
void loop() {
  counter++;
  // Check if a client has connected
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New client connected");
    // Read the request
    String request = client.readStringUntil('\r');
    Serial.println(request);
    client.flush();

    // Send the response header
    client.println("HTTP/1.1 200 OK");
    client.println("Content-type:text/html");
    client.println();

    // Send the HTML content with flow rate
    client.print("<html><body><h1>Flow Rate:</h1><p>");
    client.print(flowRate);
    client.print("Counter: ");
    client.print(counter);
    client.print("</p></body></html>");

    delay(1);
    Serial.println("Client disconnected");
    client.stop();
  }

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
