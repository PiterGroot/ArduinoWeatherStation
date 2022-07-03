#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include <SimpleDHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2); // set the LCD address to 0x27 for a 16 chars and 2 line display

SimpleDHT11 dht11;
#define DHT11_PIN 2

ESP8266WebServer server(80);

const char* ssid ="FritzGAMERS"; //Enter Wi-Fi SSID
const char* password ="69441935877053345791"; //Enter Wi-Fi Password

byte temperature = 0;
byte humidity = 0;
  
bool startServerOnAwake = true;
unsigned long previousMillis = 0; 
const long sensorInterval = 1000;  

void setup() {
  Serial.begin(115200);
  
  lcd.init(); // initialize the lcd
  lcd.backlight();
  
  if(!startServerOnAwake) return;
  startServer();
}

void loop() {
  server.handleClient(); //Handling of incoming client requests

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= sensorInterval) {
      previousMillis = currentMillis;
      handleSensorData();
  }
}

void startServer(){
  Serial.begin(115200); //Begin Serial at 115200 Baud
  WiFi.begin(ssid, password);  //Connect to the WiFi network
  
  while (WiFi.status() != WL_CONNECTED) {  //Wait for connection
      delay(500);
      Serial.println("Waiting to connect...");
  }
  
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //Print the local IP
  
  server.on("/", handle_OnConnect); //Handle Index page
  server.onNotFound(handleNotFound);
  
  server.begin(); //Start the server
  Serial.println("Server listening");

  lcd.backlight();
}

void handleSensorData(){
  Serial.println("SENSOR SAMPLING");
  
  if (dht11.read(DHT11_PIN, &temperature, &humidity, NULL)) {
    Serial.print("Read DHT11 failed."); //DHT11_PIN is not working
    return;
  }
  lcd.setCursor(0,0);
  String temp = String("TEMP = ");
  String c = String(" *C");
  String temperatureReading = String(temp + temperature + c);  
  
  lcd.print(temperatureReading);
  lcd.setCursor(0,1);
  
  String humid = String("HUMIDITY = ");
  String p = String(" %");
  String humidityReading = String(humid + humidity + p); 
  lcd.print(humidityReading + "e");
  Serial.println(temperatureReading + " " + humidityReading);
}
void handle_OnConnect() {
  //Print Hello at opening homepage
  server.send(200, "text/html", SendSensorDataHTML(temperature, humidity, "18:22", "02/07/2022"));
}

void handleNotFound(){
  server.send(404, "text/plain", "Not found");
}

int GetRandomNumber(){
  return random(100);
}

String SendSensorDataHTML(float TemperatureWeb, float HumidityWeb, String TimeWeb,String DateWeb){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>ESP8266 Global Server</title>\n";

  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<div id=\"webpage\">\n";
  ptr +="<h1>ESP8266 Global Server</h1>\n";

  ptr +="<p>Date: ";
  ptr +=(String)DateWeb;
  ptr +="</p>";
  ptr +="<p>Time: ";
  ptr +=(String)TimeWeb;
  ptr +="</p>";
  ptr +="<p>Temperature: ";
  ptr +=TemperatureWeb;
  ptr +="C</p>";
  ptr +="<p>Humidity: ";
  ptr +=HumidityWeb;
  ptr +="%</p>";
  
  ptr +="</div>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}
