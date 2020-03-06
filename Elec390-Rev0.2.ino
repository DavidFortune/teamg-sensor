#include <WiFi.h>          // Replace with WiFi.h for ESP32
#include <WebServer.h>     // Replace with WebServer.h for ESP32
#include <AutoConnect.h>
#include <Adafruit_AM2315.h> // for the humidity/temperature sensor


WebServer Server;          // Replace with WebServer for ESP32
AutoConnect      Portal(Server);
Adafruit_AM2315 am2315; // define what our sensor object is

static int soilMoisturePin = 39;
int rawSoilMoistureValue = 0;
static int solarPin = 32;
int rawSolarValue = 0;
void rootPage() {
  char content[] = "Hello, world";
  Server.send(200, "text/plain", content);
}


void setup() {
  delay(1000);
  Serial.begin(115200);
  Wire.begin(21,22);
  Serial.println();

  Server.on("/", rootPage);
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }
  if (! am2315.begin()) {
     Serial.println("Sensor not found, check wiring & pullups!");
    // while (1);
  }
}

void loop() {
  float temperature, humidity;
    rawSoilMoistureValue = analogRead(soilMoisturePin);
    Serial.print("RawSoil val:  ");Serial.println(rawSoilMoistureValue);
    rawSolarValue = analogRead(solarPin);
    Serial.print("RawSOLAR val:  ");Serial.println(rawSolarValue);
    Portal.handleClient();

  while (! am2315.readTemperatureAndHumidity(&temperature, &humidity)) {
   // Serial.println("Failed to read data from AM2315"); // will show this while it's reading information too, as long as it doesn't have a complete value yet
    delay(250); //reading the AM2315 sensor
  }
  Serial.print("Temp *C: "); Serial.println(temperature);
  Serial.print("Hum %: "); Serial.println(humidity);

  Portal.handleClient();
  delay(500); // need 2 second at least delay between AM2315 readings thus we give 3 seconds.

  
    
}
