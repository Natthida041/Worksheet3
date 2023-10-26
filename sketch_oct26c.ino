#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

// Create a web server on port 80
ESP8266WebServer server(80);

const int ledPin = D6;
DHT dht11(D4, DHT11);



void initWifi(String ssid, String password) {
  Serial.println('WiFi PRASIT');
  // Initialize Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println('WiFi PRASIT');

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    Serial.println();
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  }

void setup() {
    Serial.begin(9600);
    initWifi("PRASIT_2G", "62221919");
    
    Serial.println("HTTP server started");
    pinMode(ledPin, OUTPUT);
    dht11.begin();

  // Set up routes
    server.on("/", HTTP_GET, handleRoot);
    server.on("/TurnOn", HTTP_GET, handleTurnOn);
    server.on("/TurnOff", HTTP_GET, handleTurnOff);
    server.on("/data", HTTP_GET, handleData);

    server.begin();
    Serial.println("HTTP server started");
}
void loop() {
    server.handleClient();
}
void handleRoot() {
  float currentTemperature = 0.0;
  float currentHumidity = 0.0;

    String html = "<html><body><div style='text-align: center;'>";
    html += "<p><h2 style='color: red;'>Web Server</h2>";
    html += "<p>LED Status: " + (digitalRead(ledPin) == HIGH ? String("<span style='color: #lightblue;'>ON</span>") : String("<span style='color: #lightcoral;'>OFF</span>")) + "</p>";
    html += "<form action='/TurnOn' method='get' style='display: inline;'>";
    html += "<input type='submit' value='Turn LED On' onclick='this.style.backgroundColor=\"lightblue\"; document.getElementById(\"ledStatus\").textContent=\"ON\"'>";
    html += "</form>";
    html += "<br><br>";
    html += "<form action='/TurnOff' method='get' style='display: inline;'>";
    html += "<input type='submit' value='Turn LED Off' onclick='this.style.backgroundColor=\"lightcoral\"; document.getElementById(\"ledStatus\").textContent=\"OFF\"'>";
    html += "</form>";
    html += "<p>Humidity: <span id='humidity'>" + String(currentHumidity) + " %</span></p>";
    html += "<p>Temperature: <span id='temperature'>" + String(currentTemperature) + " C</span></p>";
    html += "</div></body></html>";


    html += "<script>";
    html += "function updateData() {";
    html += "var xhr = new XMLHttpRequest();";
    html += "xhr.open('GET', '/data', true);";
    html += "xhr.onreadystatechange = function() {";
    html += "if (xhr.readyState == 4 && xhr.status == 200) {";
    html += "var data = JSON.parse(xhr.responseText);";
    html += "document.getElementById('temperature').textContent = data.temperature + 'C';";
    html += "document.getElementById('humidity').textContent = data.humidity + '%';";
    html += "}";
    html += "};";
    html += "xhr.send();";
    html += "}";
    html += "setInterval(updateData, 500);";
    html += "updateData();";
    html += "</script>";

    server.send(200, "text/html", html);
  }

  void handleTurnOn() {
    digitalWrite(ledPin, HIGH);
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  }

  void handleTurnOff() {
    digitalWrite(ledPin, LOW);
    server.sendHeader("Location", "/");
    server.send(302, "text/plain", "");
  }

  void handleData() {
    float temperature = dht11.readTemperature(0.0);
    float humidity = dht11.readHumidity(0.0);

    String data = "{\"temperature\": " + String(temperature) + ", \"humidity\": " + String(humidity) + "}";
    server.send(200, "application/json", data);
  }