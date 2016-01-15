#include "DHT.h"


#define DHTPIN 4     // what digital pin we're connected to
#define LEDPIN 5  
#include <ESP8266WiFi.h>
#include <Wire.h>
#include "SSD1306.h"
#include "SSD1306Ui.h"
 
const char* ssid     = "YOURSSID";
const char* password = "YOURPASSWORD";
 
const char* host = "api.thingspeak.com";

const char* hosts3 = "yourbucket.s3.amazonaws.com";
const char* writekey = "yourthingspeakwriteAPI";
// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
// Note that older versions of this library took an optional third parameter to
// tweak the timings for faster processors.  This parameter is no longer needed
// as the current DHT reading algorithm adjusts itself to work on faster procs.
DHT dht(DHTPIN, DHTTYPE);
SSD1306   display(0x3c, D5, D6);
SSD1306Ui ui     ( &display );

void setup() {
  Serial.begin(9600);
  Serial.println();
  Serial.println();

display.init();
display.displayOn();
display.flipScreenVertically();
display.drawString(5,5,"Room temp and humidity");
display.drawString(5,20,"Loading...");
display.display();

Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  

  
  Serial.println("DHTxx test!");

  dht.begin();
}

void loop() {
  // Wait a few seconds between measurements.
  
  WiFiClient client;
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");
  String displaytemp =  ("Current temp " + String(f,2) + "*F");
  display.clear();
  display.drawString(2,15,displaytemp);
  String displayhum = ("Current humidity " + String (h,2) + "%");
  display.drawString(2,30,displayhum);
display.display();
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hic);
  Serial.print(" *C ");
  Serial.print(hif);
  Serial.println(" *F");

  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // We now create a URI for the request
  String url = "/update?key=";
  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + writekey + "&field3=" + t + "&field2=" + h + "&field4=" + f + " HTTP/1.1\r\n" +
              "Host: " + host + "\r\n" + 
             "Connection: close\r\n\r\n");
  delay(500);
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
   Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");
  

  Serial.print("connecting to ");
  Serial.println(hosts3);
  String urls3upload = "/currenttemp?yoururlposturl";
  Serial.print("Requesting AWS URL: ");
  Serial.println(urls3upload);
    if (!client.connect(hosts3, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  client.print(String("PUT ") + urls3upload + " HTTP/1.1\r\n" +
               "Host: " + hosts3 + "\r\n" + 
               "Accept: */*\r\nUser-Agent: curl/7.43.0\r\nContent-Length: 2\r\nExpect: 100-continue\r\n\r\n");
  delay(500); // wait for 100 continue
  client.print(f);
 
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");

   Serial.print("connecting to ");
  Serial.println(hosts3);
  String urls3get = "/destemp";
  Serial.print("Requesting AWS URL: ");
  Serial.println(urls3get);
    if (!client.connect(hosts3, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  client.print(String("GET ") + urls3get + " HTTP/1.1\r\n" +
               "Host: " + hosts3 + "\r\n" + 
               "Accept: */*\r\nUser-Agent: curl/7.43.0\r\n\r\n");
  delay(500); 
  client.print(f);
 
  String destemp = "";
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
    destemp = line; //last line is desired temp
  }
  
  Serial.println();
  Serial.println("closing connection");
  destemp.trim(); //get rid of whitespace
  Serial.println(destemp);
  Serial.println(destemp.length());
  String displaydestemp = ("Desired temp " +  destemp + "*F");
  display.drawString(2,45,displaydestemp);
  display.display();

  
pinMode(LEDPIN, OUTPUT);
digitalWrite(LEDPIN, HIGH);
float destempfloat = destemp.toFloat();
if (destempfloat > f) {
  digitalWrite(LEDPIN, HIGH);
  Serial.println("High");
  Serial.println(destempfloat);
  Serial.println(f);
} else
{
  digitalWrite(LEDPIN, LOW);
  Serial.println("Low");
  Serial.println(destempfloat);
  Serial.println(f);
}
  delay(120000); //wait 2 mins
               
}
