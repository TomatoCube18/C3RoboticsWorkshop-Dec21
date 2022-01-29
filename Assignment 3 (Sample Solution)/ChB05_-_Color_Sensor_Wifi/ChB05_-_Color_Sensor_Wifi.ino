/*
  WebSocket LED

  ESP32-C3 Running  WebSocket Server Interfacing to the onBoard
  Color Sensor Exercise
  
  LEDs
  --  IO2
  --  PA7 (3)(ATTINY1614)

  IR_RECV
  --  IO3

  I2C
  --  IO18 (SCL) <-> PB0 (ATTINY1614)
  --  IO19 (SDA) <-> PB1 (ATTINY1614)
  
*/

#include <WiFi.h>
#include <WiFiMulti.h>
#include <WiFiClientSecure.h>

#include <WebSocketsServer.h>
#include <WebServer.h>

WiFiMulti WiFiMulti;
WebSocketsServer webSocket = WebSocketsServer(81);
WebServer server(80);


#include <Wire.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"
#include <Adafruit_NeoPixel.h>

#include "Adafruit_TCS34725.h"
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_614MS, TCS34725_GAIN_1X);

#define OLED_I2CAADR 0x3C

SSD1306AsciiWire oled;

// i2c PCF8574(A) Port expander
// PCF8574  -> 0 0 1 0  0 A2 A1 A0    0 0 1 1
// PCF8574A -> 0 0 1 1  1 A2 A1 A0    1 0 1 1

#define PEXP_I2CADDR 0x23
//#define PEXP_A_I2CADDR 0x3B

// i2c Slave Co-processor
#define I2CADDR 0x13
// I2C address  - MainBoard
#define I2CADDR_B 0x12

#define I2C_SDA 19
#define I2C_SCL 18
#define LED 2
#define IR_RECV 4
#define NEO_PIXEL 5
#define LED_COUNT 3

// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";


bool ledState = 0;


const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP Web Server</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
  html {
    font-family: Arial, Helvetica, sans-serif;
    text-align: center;
  }
  </style>
<title>ESP Web Server</title>
<meta name="viewport" content="width=device-width, initial-scale=1">
<link rel="icon" href="data:,">
</head>
<body>
  <div class="topnav">
    <h1>C3 CoreModule - CockroachBot WebSocket Server</h1>
  </div>
  <div class="content">
    <div class="card">
      <h2>LED - GPIO 2</h2>
      <p class="state">state: <span id="state">-</span></p>
      <p><button id="button" class="button">Toggle</button></p>
    </div>
  </div>
<script>
  var gateway = `ws://${window.location.hostname}:81`;
  var websocket;
  window.addEventListener('load', onLoad);
  function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage; // <-- add this line
  }
  function onOpen(event) {
    console.log('Connection opened');
  }
  function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
  }
  function onMessage(event) {
    var state;
    if (event.data == "LED=1"){
      state = "ON";
    }
    else if (event.data == "LED=0"){
      state = "OFF";
    }
    document.getElementById('state').innerHTML = state;
  }
  function onLoad(event) {
    initWebSocket();
    initButton();
  }
  function initButton() {
    document.getElementById('button').addEventListener('click', toggle);
  }
  function toggle(){
    websocket.send('toggle');
  }
</script>
</body>
</html>
)rawliteral";

//void notifyClients() {
//  ws.textAll(String(ledState));
//}

//
//String processor(const String& var){
//  Serial.println(var);
//  if(var == "STATE"){
//    if (ledState){
//      return "ON";
//    }
//    else{
//      return "OFF";
//    }
//  }
//  return String();
//}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}




void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            Serial.printf("[%u] Disconnected!\n", num);
            break;
        case WStype_CONNECTED:
            {
                IPAddress ip = webSocket.remoteIP(num);
                Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

        // send message to client
        webSocket.sendTXT(num, "Connected");
        webSocket.sendTXT(num, ledState? "LED=1": "LED=0");
            }
            break;
        case WStype_TEXT:
            Serial.printf("[%u] get Text: %s\n", num, payload);
            if (strcmp((char*)payload, "toggle") == 0) {
              ledState = !ledState;
              webSocket.broadcastTXT(ledState? "LED=1": "LED=0");
            }
            
            // send message to client
            // webSocket.sendTXT(num, "message here");

            // send data to all connected clients
            // webSocket.broadcastTXT("message here");
            break;
        case WStype_BIN:
            // send message to client
            // webSocket.sendBIN(num, payload, length);
            break;
    case WStype_ERROR:      
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
      break;
    }

}

void setup(){
  Wire.begin(I2C_SDA, I2C_SCL);    
  oled.begin(&Adafruit128x64, OLED_I2CAADR); 
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  delay(2000);

  Serial.begin(115200);
  oled.setFont(Adafruit5x7);
  oled.clear();
  oled.println("WebSocket Server");
  
  tcs.begin();
  
  WiFiMulti.addAP(ssid, password);
  
  while(WiFiMulti.run() != WL_CONNECTED) {
      delay(100);
  }
  // Print ESP Local IP Address
  Serial.println(WiFi.localIP());
  oled.println(WiFi.localIP());
  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);

  // handle index
  server.on("/", []() {
      // send index.html
      server.send(200, "text/html", index_html);
  });
  
  server.onNotFound(handleNotFound);

  server.begin();
}

unsigned long lastColSensorRead = 0;

void loop() {
  webSocket.loop();
  server.handleClient();
  digitalWrite(LED, ledState);

  if (millis() - lastColSensorRead >= 1000) {
//      uint16_t r, g, b, c, colorTemp, lux;
      float red, green, blue;
      char colorBuff[20]; //We just need 12... but let's add more just in case
//      tcs.getRawData(&r, &g, &b, &c);
    
      oled.setCursor(0, 3); 
//      oled.print("R: "); oled.print(r, DEC); oled.print(" ");
//      oled.print("G: "); oled.print(g, DEC); oled.println("   ");
//      oled.print("B: "); oled.print(b, DEC); oled.print(" ");
//      oled.print("C: "); oled.print(c, DEC); oled.println("   ");
      tcs.getRGB(&red, &green, &blue);

      Serial.print("R:\t"); Serial.print(int(red)); 
      Serial.print("\tG:\t"); Serial.print(int(green)); 
      Serial.print("\tB:\t"); Serial.println(int(blue));
  
      sprintf(colorBuff, "Color=%02X%02X%02X", (int)red,(int)green,(int)blue);
      oled.print(colorBuff); oled.println("   ");
      webSocket.broadcastTXT(colorBuff);
      lastColSensorRead = millis();
  }
  
  delay(2);//allow the cpu to switch to other tasks
}
