#include <WiFi.h>

// WiFi credentials
const char* ssid = "ECOTHON 224";
const char* password = "ku12345678";

// Define the pins connected to the flow sensors
#define CLEAN_FLOW_SENSOR_PIN 2
#define WASTE_FLOW_SENSOR_PIN 4

// Define variables for clean and waste flow sensors
volatile unsigned int cleanPulseCount;
volatile unsigned int wastePulseCount;
float cleanFlowRate;
float wasteFlowRate;
unsigned long lastTime;
unsigned long elapsedTime;
int counter;

// WiFi server port
WiFiServer server(80);

// Interrupt service routine for clean flow sensor
void IRAM_ATTR cleanFlowSensorISR() {
  cleanPulseCount++;
}

// Interrupt service routine for waste flow sensor
void IRAM_ATTR wasteFlowSensorISR() {
  wastePulseCount++;
}

// Setup function
void setup() {
  Serial.begin(9600);
  pinMode(CLEAN_FLOW_SENSOR_PIN, INPUT_PULLUP);
  pinMode(WASTE_FLOW_SENSOR_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(CLEAN_FLOW_SENSOR_PIN), cleanFlowSensorISR, RISING);
  attachInterrupt(digitalPinToInterrupt(WASTE_FLOW_SENSOR_PIN), wasteFlowSensorISR, RISING);
  
  cleanPulseCount = 0;
  wastePulseCount = 0;
  cleanFlowRate = 0.0;
  wasteFlowRate = 0.0;
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
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("To access Clean Water Flow Rate, go to: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/clean");
  Serial.println("To access Waste Water Flow Rate, go to: http://");
  Serial.print(WiFi.localIP());
  Serial.println("/waste");
}

// Function to serve Clean Water Flow Rate page
void serveCleanPage(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.println("<html>");
  client.println("<head>");
  client.println("<style>");
  client.println("body { font-family: Arial, sans-serif; background-color: #f0f0f0; }");
  client.println("h1 { color: #333333; }");
  client.println("p { color: #666666; }");
  client.println("</style>");
  client.println("</head>");
  client.println("<body>");
  client.print("<h1>Clean Water Flow Rate:</h1>");
  client.print("<p>Flow Rate: ");
  client.print(cleanFlowRate);
  client.println(" L/min</p>");
  client.println("</body>");
  client.println("</html>");
}

// Function to serve Waste Water Flow Rate page
void serveWastePage(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.println("<html>");
  client.println("<head>");
  client.println("<style>");
  client.println("body { font-family: Arial, sans-serif; background-color: #f0f0f0; }");
  client.println("h1 { color: #333333; }");
  client.println("p { color: #666666; }");
  client.println("</style>");
  client.println("</head>");
  client.println("<body>");
  client.print("<h1>Waste Water Flow Rate:</h1>");
  client.print("<p>Flow Rate: ");
  client.print(wasteFlowRate);
  client.println(" L/min</p>");
  client.println("</body>");
  client.println("</html>");
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

    // Serve different pages based on the request
    if (request.indexOf("/clean") != -1) {
      serveCleanPage(client);
    } else if (request.indexOf("/waste") != -1) {
      serveWastePage(client);
    } else {
      // If the requested URL is not recognized, return a 404 error
      client.println("HTTP/1.1 404 Not Found");
      client.println("Content-type:text/html");
      client.println();
      client.println("<html><body><h1>Error 404: Page Not Found</h1></body></html>");
    }

    delay(1);
    Serial.println("Client disconnected");
    client.stop();
  }

  // Calculate flow rates every second
  elapsedTime = millis() - lastTime;
  if (elapsedTime > 1000) {
    detachInterrupt(digitalPinToInterrupt(CLEAN_FLOW_SENSOR_PIN)); // Stop interrupts while calculating
    cleanFlowRate = (1000.0 / (elapsedTime / 1000.0)) * cleanPulseCount/1000; // Clean flow rate in pulses per second
    cleanFlowRate /= 7.5; // Convert pulses per second to liters per minute
    cleanPulseCount = 0;
    
    wasteFlowRate = (1000.0 / (elapsedTime / 1000.0)) * wastePulseCount/1000; // Waste flow rate in pulses per second
    wasteFlowRate /= 7.5; // Convert pulses per second to liters per minute
    wastePulseCount = 0;

    lastTime = millis();
    attachInterrupt(digitalPinToInterrupt(CLEAN_FLOW_SENSOR_PIN), cleanFlowSensorISR, RISING); // Re-attach interrupt
    attachInterrupt(digitalPinToInterrupt(WASTE_FLOW_SENSOR_PIN), wasteFlowSensorISR, RISING); // Re-attach interrupt

    Serial.print("Clean Flow rate: ");
    Serial.print(cleanFlowRate);
    Serial.println(" L/min");
    Serial.print("Waste Flow rate: ");
    Serial.print(wasteFlowRate);
    Serial.println(" L/min");
  }
}
