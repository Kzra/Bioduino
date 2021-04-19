/*
Arduino Wifi Naturalist Display v 1.0 - Alpha 

MIT (C) Kzra 2021 
*/

// Libraries 
#include <SPI.h>
#include <WiFiNINA.h>
#define ARDUINOJSON_ENABLE_COMMENTS 1 
#include <ArduinoJson.h> // used to parse the api resposne
#include "arduino_secrets.h" 
#include "Adafruit_GFX.h"// Hardware-specific library
// MCUFRIEND is the library that provides support for Arduino TFT shields
// MCUFRIEND_kbv inherits all the methods from the Adafruit_GFX class: https://learn.adafruit.com/adafruit-gfx-graphics-library/overview 
// and Print class: https://www.arduino.cc/en/Serial/Print
// so you can use Adafruit_GFX commands to contorl your TFT display 
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID; // your network SSID (name)
char pass[] = SECRET_PASS; // your network password (use for WPA, or use as key for WEP)
double myLat = SECRET_LAT; // your latitutde in decimal
double myLon = SECRET_LON; // your longitude in decmial
// char ebirdToken[] = SECRET_EBIRD; //your eBird API token - not part of v 1.0 

//Wifi Variables 
int keyIndex = 0;            // your network key Index number (needed only for WEP)
int status = WL_IDLE_STATUS;
// Initialize the Ethernet client library
// with the IP address and port of the server
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client; // this is a wifi client, fot http
WiFiSSLClient secureClient; // this is a secure wifi client, for https: you need to add the ssl certificate for inaturalist.org to the arduino
// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
char iNatServer[] = "api.inaturalist.org";    // name address for inaturalist (using DNS)
char timeServer[] = "worldtimeapi.org";     // name and address for world time api.orf

// API varibles 
const int radius = 20; // radius of observations in km 
const int requestTime = 10; // time to leave between http requests (min 0 - 60)
boolean fetched = false; //boolean to ensure we only make one inat request

// LCD TFT Display Variables 
// Assign human-readable names to some common 16-bit color values:
#define  BLACK   0x0000
#define RED     0xF800
#define GREEN   0x2747 //pastel green 
#define WHITE   0xFFFF

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0
#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// PROGMEM stores the inaturalist logo in FLASH memory. This is the same memory location as the sketch, meaning it is non-volatile. 
// It is not lost when the Arduino is reset. 
// It also means that we can't change iNatLogo in the code. It is "burned" into memory. 
// https://playground.arduino.cc/Learning/Memory/
const unsigned char iNatLogo[] PROGMEM = {
  // 'inat logo, 84x84px
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xf7, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xf1, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xf0, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 
  0xf8, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x0f, 0xff, 0xff, 0xff, 
  0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x07, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xf8, 0x1f, 
  0xff, 0xfc, 0x03, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xfc, 0x01, 0xff, 0xfc, 0x01, 0xff, 0xff, 
  0xff, 0xff, 0xf0, 0xff, 0xfc, 0x00, 0x1f, 0xfe, 0x00, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xfe, 
  0x00, 0x03, 0xfe, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x7f, 
  0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0x80, 0x00, 0x1f, 0x80, 0x3f, 0xff, 0xff, 0xff, 0xf0, 0xff, 
  0xff, 0xe0, 0x00, 0x07, 0xc0, 0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xf0, 0x00, 0x01, 0xe0, 
  0x1f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xfc, 0x00, 0x00, 0xf8, 0x0f, 0xff, 0xff, 0xff, 0xf0, 
  0xff, 0xff, 0xfe, 0x00, 0x00, 0x3c, 0x0f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0x80, 0x00, 
  0x1e, 0x0f, 0xfc, 0x00, 0x7f, 0xf0, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x07, 0x07, 0xf0, 0x00, 0xff, 
  0xf0, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x03, 0x87, 0xe1, 0xc1, 0xff, 0xf0, 0xff, 0xff, 0xf0, 0x00, 
  0x00, 0x01, 0xe7, 0xc1, 0xc3, 0xff, 0xf0, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0xf7, 0x81, 0xc3, 
  0xff, 0xf0, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x77, 0x00, 0x07, 0xff, 0xf0, 0xff, 0xff, 0xfe, 
  0x00, 0x00, 0x00, 0x3e, 0x00, 0x07, 0xff, 0xf0, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x1e, 0x00, 
  0x0f, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x0c, 0x00, 0x0f, 0xff, 0xf0, 0xff, 0xff, 
  0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 
  0x00, 0x0f, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xf0, 0xff, 
  0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 
  0x00, 0x00, 0x1f, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xf0, 
  0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xf8, 0x00, 
  0x00, 0x00, 0x00, 0x3f, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 
  0xf0, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 
  0x80, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x7f, 
  0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xfe, 0x00, 0x00, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x01, 
  0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x01, 0xff, 0xff, 0xf0, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xf8, 0x00, 0x03, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 
  0x07, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x0f, 0xff, 0xff, 0xf0, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x1f, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 
  0x00, 0x3f, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xf0, 
  0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x01, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xf8, 
  0x00, 0x03, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x0f, 0xff, 0xff, 0xff, 
  0xf0, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 
  0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
  0xf0, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0
};

// timekeeping variables 
int currentMin = 0;
int currentHour = 0;
int currentDay = 1;
int currentMonth = 1;
int currentYear = 2021;
int daysinMonth[] = {31,28,31,30,31,30,31,31,30,31,30,31};
unsigned long previousTime = 0;

void setup() {
  //Initialize serial and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  } 
  // initialise the LCD display 
  uint16_t ID = tft.readID(); 
  tft.begin(ID); //initialise the display
  tft.setRotation(1); // set landscape rotation
  tft.setTextWrap(false); //turn on text wrapping
  tft.fillScreen(GREEN);// the default background is green 
  //  Add the title 
  tft.setTextColor(WHITE, BLACK);
  tft.setCursor(0, 0);
  tft.setTextSize(3);
  tft.println("Arduino Wifi        \n iNaturalist Display");
  // Add the iNat Logo 
  tft.drawBitmap(396,236,iNatLogo,84,84,BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, 312);
  // Add the signature
  tft.println("(C) Kzra 2021");
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true);
  }
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }
  // attempt to connect to WiFi network:
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    // wait 10 seconds for connection:
    delay(10000);
  }
  Serial.println("Connected to wifi");
  printWiFiStatus();
  // get and display the time
  fetch_worldtime();
  // get and display the last three inaturalist observations
  fetch_inaturalist();
}

void loop() {
  // time keeping
  unsigned long currentTime = millis();
  // calculate minutes, hours, days, months and years
  if (currentTime - previousTime > 60000){
      currentMin++;   
      previousTime = currentTime;
  }
  if (currentMin >= 60){
    currentMin = 0;
    currentHour ++;
  }
  if (currentHour >= 24){
    currentHour = 0;
    currentDay ++;
  }
  if (currentDay >= daysinMonth[currentMonth]){
    currentDay = 1;
    currentMonth ++;
  }
  if (currentMonth >= 13){
    currentMonth = 1;
    currentYear ++;
  }
  // Display the current time 
  tft.setTextSize(2);
  tft.setCursor(0,255);
  tft.setTextColor(RED,GREEN); //set the background to green to overwrite previous text
  tft.print(" ");
  if (currentDay < 10){
    tft.print("0");
    tft.print(currentDay);
  } else {
  tft.print(currentDay); 
  }
  tft.print("/");
  if (currentMonth < 10){
    tft.print("0");
    tft.print(currentMonth);
  } else {
  tft.print(currentMonth);
  }
  tft.print("/");
  tft.print(currentYear);
  tft.print("  ");
  if (currentHour < 10){
    tft.print("0");
    tft.print(currentHour);
  } else {
  tft.print(currentHour); 
  }
  tft.print(":");
  if (currentMin < 10){
    tft.print("0");
    tft.print(currentMin);
  } else {
  tft.print(currentMin);
  }
  tft.println("    ");
  // Display the search parameters
  tft.print(" lat=");
  tft.print(myLat);
  tft.print(" lon=");
  tft.println(myLon);
  tft.print(" radius=");
  tft.print(radius);
  tft.print(" km");
  //fetch three most recent observations in your area from iNaturalist every ten minutes 
  if(currentMin % requestTime == 0 and fetched == false){
  fetch_inaturalist();
  fetched = true;
  } 
  if(currentMin % requestTime != 0){
   fetched = false;
   //Serial.println("Not requesting...");
  }
}


void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}


void fetch_worldtime(){
    // get the time from world time api  
  if (client.connect(timeServer, 80)) {
    Serial.println("connected to world time server");
    // Make a HTTP request:
    client.println("GET /api/ip HTTP/1.1"); // request the time based on the wifi IP 
    client.println("Host: worldtimeapi.org");
    client.println("Connection: close");
    client.println();
    delay(5000);
    // Check HTTP status
    char status[32] = {0};
    client.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
      Serial.print(F("Unexpected response: "));
      Serial.println(status);
      client.stop();
      return;
    }
    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!client.find(endOfHeaders)) {
      Serial.println(F("Invalid response"));
      client.stop();
      return;
    }
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, client);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    const char* datetime = doc["datetime"]; // e.g. "2021-04-08T18:12:23.982549+01:00"
    Serial.println(datetime); 
    String yearString = "0000";
    for (int i = 0; i <4; i++){
      yearString[i] = datetime[i];
    }
    String monthString = "00";
    for (int i = 5; i <7; i++){
      monthString[i-5] = datetime[i];
    }    
    String dayString = "00";
    for (int i = 8; i <10; i++){
      dayString[i-8] = datetime[i];
    }
    String hourString = "00";
    for (int i = 11; i <13; i++){
      hourString[i-11] = datetime[i];
    }    
    String minuteString = "00";
    for (int i = 14; i <16; i++){
      minuteString[i-14] = datetime[i];
    }
    currentYear = yearString.toInt();
    currentMonth = monthString.toInt();
    currentDay = dayString.toInt();
    currentHour = hourString.toInt();
    currentMin = minuteString.toInt();         
  } 
}

void fetch_inaturalist(){
  DynamicJsonDocument filter(300);
  DynamicJsonDocument data(2048);
  Serial.println("\nStarting connection to iNaturalist server...");
  // if you get a connection, report back via serial:
  if (secureClient.connect(iNatServer, 443)) {
    Serial.println("connected to iNaturalist server");
    // Make a HTTP request:
    secureClient.print("GET /v1/observations?page=1&per_page=3&quality_grade=research&order_by=observed_on&order=desc&lat=");
    secureClient.print(myLat);
    secureClient.print("&lng=");
    secureClient.print(myLon);
    secureClient.print("&radius=");
    secureClient.print(radius);
    secureClient.println(" HTTP/1.1");
    secureClient.println("Host: api.inaturalist.org");
    secureClient.println("Connection: close");
    secureClient.println();
    if (secureClient.println() == 0) {
    Serial.println(F("Failed to send request"));
    secureClient.stop();
    return;
    } 
    delay(7000); // delay 7 seconds before reading response
    // Check HTTP status
    char status[32] = {0};
    secureClient.readBytesUntil('\r', status, sizeof(status));
    if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
      Serial.print(F("Unexpected response: "));
      Serial.println(status);
      secureClient.stop();
      return;
    }
    // Skip HTTP headers
    char endOfHeaders[] = "\r\n\r\n";
    if (!secureClient.find(endOfHeaders)) {
      Serial.println(F("Invalid response"));
      secureClient.stop();
      return;
    }
    //while (secureClient.available()){
    //   char c = secureClient.read();
    //   Serial.write(c);
    //}
    Serial.println("Deserialising iNaturalist json");
    filter["results"][0]["taxon"]["name"] = true;
    filter["results"][0]["taxon"]["preferred_common_name"] = true;
    filter["results"][0]["observed_on_string"] = true;
    filter["results"][0]["place_guess"] = true;
    DeserializationError error = deserializeJson(data, secureClient, DeserializationOption::Filter(filter));
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }
    Serial.println("Deserialization success");
    //Print the three most recent observations to the screen 
    //Include the common and scientific name 
    //The time and date observed
    //And the place it was observed
    tft.println();
    tft.setCursor(0,55);
    for (int i = 0; i <3; i++){
    JsonObject results_0 = data["results"][i];
    JsonObject results_0_taxon = results_0["taxon"];
    int results_0_taxon_id = results_0_taxon["id"]; // 0
    const char* taxon_name = results_0_taxon["name"]; // "string"
    if (taxon_name == NULL){
      Serial.println("Null taxon");
      break; //if taxon name is empty break. the api request hasn't worked. 
    }
    const char* preferred_common_name = results_0_taxon["preferred_common_name"];
    const char* observed_on_string = results_0["observed_on_string"]; // "string"
    const char* place_guess = results_0["place_guess"];
    tft.setTextColor(BLACK,GREEN);
    tft.setTextSize(2);
    tft.print(" ");
    tft.print(preferred_common_name);
    tft.print(" (");
    tft.print(taxon_name);
    tft.println(")");
    tft.print(" ");
    uint8_t comma = 0;
    int c = 0;
    while (comma < 2){
      if(place_guess[c] == ','){
        comma++;
      }
      if (comma < 2){
      tft.print(place_guess[c]);
      }
      c++;
    }
    tft.println();
    tft.print(" ");
    for (int c = 0; c <25; c++){
      tft.print(observed_on_string[c]);
    }
    tft.println();
    tft.println();
   }
  }
}

  // Here is some code for eBird support. I never heard back from my request for an API token
  // so v 1.0 of Arduino Wifi Naturalist Display doesn't use it. 
//fetch_ebird(){  
  // Fetch the top 3 documents for eBird
  //filter.clear();
  //DynamicJsonDocument filter(300);
  //DynamicJsonDocument data(2048);

  //Serial.println("\nStarting connection to eBird server...");
  // if you get a connection, report back via serial:
  //if (client.connect(eBirdServer, 443)) {
    //Serial.println("connected to eBird server");
    // Make a HTTP request:
    //client.println("GET /v2/data/obs/geo/recent/?lat=55.9&lng=-3.1&maxResults=3&dist=10");
    //client.println("Host: api.ebird.org");
    //client.print("X-eBirdApiToken: ");
    //client.println(ebirdToken);
    //client.println("Connection: close");
    //client.println();
    //if (client.println() == 0) {
    //Serial.println(F("Failed to send request"));
    //client.stop();
    //return;
    //} 
    //delay(5000); // delay 5 seconds before reading response
    // Check HTTP status
    //char status[32] = {0};
    //client.readBytesUntil('\r', status, sizeof(status));
    //if (strcmp(status, "HTTP/1.1 200 OK") != 0) {
    //  Serial.print(F("Unexpected response: "));
    //  Serial.println(status);
    //  client.stop();
    //  return;
    //}
    // Skip HTTP headers
    //char endOfHeaders[] = "\r\n\r\n";
    //if (!client.find(endOfHeaders)) {
    //  Serial.println(F("Invalid response"));
    //  client.stop();
    //  return;
    //}
    //Serial.println("Deserialising eBird json");
    //DeserializationError error = deserializeJson(data, client, DeserializationOption::Filter(filter));
    //if (error) {
    //  Serial.print(F("deserializeJson() failed: "));
    //  Serial.println(error.f_str());
    //  return;
    //}
    //for (JsonObject elem : data.as<JsonArray>()) {
    //const char* speciesCode = elem["speciesCode"]; // "domgoo1", "cangoo", "wooduc"
    //const char* comName = elem["comName"]; // "Domestic goose sp. (Domestic type)", "Canada Goose", "Wood ...
    //const char* sciName = elem["sciName"]; // "Anser sp. (Domestic type)", "Branta canadensis", "Aix sponsa"
    //const char* locId = elem["locId"]; // "L99381", "L1150539", "L1150931"
    //const char* locName = elem["locName"]; // "Stewart Park", "Hanshaw Rd. fields", "Danby - Bald Hill ...
    //const char* obsDt = elem["obsDt"]; // "2017-08-23 08:14", "2017-08-23 20:05", "2017-08-23 19:05"
    //int howMany = elem["howMany"]; // 2, 30, 12
    //double lat = elem["lat"]; // 42.4613413, 42.4663513, 42.3361988
    //double lng = elem["lng"]; // -76.5054578, -76.4531064, -76.4980173
    //bool obsValid = elem["obsValid"]; // true, true, true
    //bool obsReviewed = elem["obsReviewed"]; // false, false, false
    //bool locationPrivate = elem["locationPrivate"]; // false, false, true
    //Serial.println(comName);
    //Serial.println(locName);
    // }
    // } } }
