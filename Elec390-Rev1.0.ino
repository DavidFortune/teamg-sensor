#include <FirebaseESP32.h>
#include <FirebaseESP32HTTPClient.h>
#include <FirebaseJson.h>
#include <jsmn.h>

#include <WiFi.h>          // Replace with WiFi.h for ESP32
#include <WebServer.h>     // Replace with WebServer.h for ESP32
#include <AutoConnect.h>
#include <Adafruit_AM2315.h> // for the humidity/temperature sensor


FirebaseData firebaseData;
WebServer Server;          // Replace with WebServer for ESP32
AutoConnect      Portal(Server);
Adafruit_AM2315 am2315; // define what our sensor object is
AutoConnectConfig  Config; //our default value
static String firebaseDB = "teamg-sensordata.firebaseio.com";
static String firebaseAUTH = "W83YT5I5zEdaYGJG6ixpiXu6H8OPp387i7y0hLo6";
static String uniqueID = "ABC123SENSOR1"; // the unique id for this sensor(microcontroller) FOR USE IN FIREBASE
static int soilMoisturePin = 39;
int rawSoilMoistureValue = 0;
static int solarPin = 32;
int rawSolarValue = 0;


void rootPage() {
  char content[] = "Hello, world";
  Server.send(200, "text/plain", content);
}


void setup() {

  /*
   * Config.apid = ESP.hostname();                 // Retrieve host name to SotAp identification
Config.apip = IPAddress(192,168,10,101);      // Sets SoftAP IP address
Config.gateway = IPAddress(192,168,10,1);     // Sets WLAN router IP address
Config.netmask = IPAddress(255,255,255,0);    // Sets WLAN scope
Config.autoReconnect = true;                  // Enable auto-reconnect
Config.autoSave = AC_SAVECREDENTIAL_NEVER;    // No save credential
Config.boundaryOffset = 64;                   // Reserve 64 bytes for the user data in EEPROM.
Config.portalTimeout = 60000;                 // Sets timeout value for the captive portal
Config.retainPortal = true;                   // Retains the portal function after timed-out
Config.homeUri = "/index.html";               // Sets home path of the sketch application
Config.title ="My menu";                      // Customize the menu title
Config.staip = IPAddress(192,168,10,10);      // Sets static IP
Config.staGateway = IPAddress(192,168,10,1);  // Sets WiFi router address
Config.staNetmask = IPAddress(255,255,255,0); // Sets WLAN scope
Config.dns1 = IPAddress(192,168,10,1);        // Sets primary DNS address
*/ //possible values for config

  delay(1000);
  Serial.begin(115200);
  Wire.begin(21,22);
  Serial.println();

 // Config.autoSave = AC_SAVECREDENTIAL_NEVER; //don't save credentials for demo
  Config.apid = "TeamG-ESP32";
  Config.title ="Wifi connection Configuration";
  Portal.config(Config); //set new configuration file 
  Server.on("/", rootPage);
  if (Portal.begin()) {
    Serial.println("WiFi connected: " + WiFi.localIP().toString());
  }
  if (! am2315.begin()) {
     Serial.println("Sensor not found, check wiring & pullups!");
    // while (1);
  }

  //setup firebase
  Firebase.begin(firebaseDB,firebaseAUTH);
 // Firebase.reconnectWiFi(true);
  Firebase.setReadTimeout(firebaseData, 1000 * 60);
  Firebase.setwriteSizeLimit(firebaseData, "tiny");

}

void loop() {
  float temperature, humidity;
    rawSoilMoistureValue = analogRead(soilMoisturePin);
    Serial.print("RawSoil val:  ");Serial.println(rawSoilMoistureValue);
    rawSolarValue = analogRead(solarPin);
    Serial.print("RawSOLAR val:  ");Serial.println(rawSolarValue);
    

  while (! am2315.readTemperatureAndHumidity(&temperature, &humidity)) {
   // Serial.println("Failed to read data from AM2315"); // will show this while it's reading information too, as long as it doesn't have a complete value yet
    delay(250); //reading the AM2315 sensor
  }
  Serial.print("Temp *C: "); Serial.println(temperature);
  Serial.print("Hum %: "); Serial.println(humidity);

  Portal.handleClient();
  delay(500); // need 2 second at least delay between AM2315 readings thus we give 3 seconds.
  Serial.println("Sending data to firebase");



  Firebase.setString(firebaseData,"/sensors/"+uniqueID+"/rawSoilMoistureValue",String(rawSoilMoistureValue));
  Firebase.setString(firebaseData,"/sensors/"+uniqueID+"/rawSolarValue",String(rawSolarValue));
  Firebase.setString(firebaseData,"/sensors/"+uniqueID+"/rawTemperature",String(temperature));
  Firebase.setString(firebaseData,"/sensors/"+uniqueID+"/rawHumidity",String(humidity));
  Serial.println("data DONE");
  //Begin sending data to firebase

  delay(500);
  
    
}
