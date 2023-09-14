//-------------------------------------Libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>
#include <Hash.h>
#include <String.h>
#include <WiFiClient.h>
#include <SoftwareSerial.h>

//-------------------------------------PINS
#define mPin D8
#define led LED_BUILTIN
//-------------------------------------VARIABLES.

byte ones = 0;
byte modes = 0;
//default values
String curnt="2.2";
String volt="12";
String vspeed="0";
String loc="https://www.google.com/maps/search/amlidih+gprs+location/@21.2536351,81.638709,14z";
String btemp="30";
String mtemp="35";
String hack="NO";
String charge="100";
String old_value, value;

SoftwareSerial gprsSerial(D2,D3);//tx,rx
ESP8266WiFiMulti    WiFiMulti;
ESP8266WebServer    server(80);
WebSocketsServer    webSocket = WebSocketsServer(81);

//-------------------------------------WEBPAGE

char html_template[] PROGMEM = R"=====(
<html lang="en">
   <head>
      <meta charset="utf-8">
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <title>Access Point mode</title>
      <script>
        socket = new WebSocket("ws:/" + "/" + location.host + ":81");
        socket.onopen = function(e) {  console.log("[socket] socket.onopen "); };
        socket.onerror = function(e) {  console.log("[socket] socket.onerror "); };
        socket.onmessage = function(e) {  
            console.log("[socket] " + e.data);
            document.getElementById("a_value").innerHTML = e.data;
        };
      </script>
   </head>
   <body style="max-width:400px;margin: auto;font-family:Arial, Helvetica, sans-serif;text-align:center">
      <div><h1><br />WEBSOCKET ACCESS POINT</h1><hr></div>
      <div><p id="a_value" style="font-size:100px;margin:0"></p></div>
   </body>
</html>
)=====";

//-------------------------------------FUNCTIONS
void blinkD(short d, byte i){
  while(i>0)
  {
    digitalWrite(led, HIGH);
    delay(d);
    digitalWrite(led, LOW);
    delay(d);
    i--;
  }
}

//-------------------------------------------------------------Function*
// These are fuctions that will called on occurence of several events
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  
  switch (type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;

    case WStype_CONNECTED: {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);
        // send message to client
        webSocket.sendTXT(num, "0");
      }
      break;
// This is most important as it will be called upon when any text is to be transmitted or received.
    case WStype_TEXT:
      Serial.printf("[%u] get Text: %s\n", num, payload);
      // send message to client
      // webSocket.sendTXT(num, "message here");
      // send data to all connected clients
      // webSocket.broadcastTXT("message here");
      break;
      
    case WStype_BIN:
      Serial.printf("[%u] get binary length: %u\n", num, length);
      hexdump(payload, length);
      // send message to client
      // webSocket.sendBIN(num, payload, length);
      break;
  }

}


// Functions to be called when we vist a website.
void handleMain() {
  server.send_P(200, "text/html", html_template ); 
}
void handleNotFound() {
  server.send(404,   "text/html", "<html><body><p>404 Error</p></body></html>" );
}
//  GPRS function to show serial data
void ShowSerialData()
{
  while(gprsSerial.available()!=0)
  Serial.write(gprsSerial.read());
  delay(6000); 
  
}
//-------------------------------------MODE SETUP FUNCTIONS
//-------------------------------------INTERNET
void setupInternet(){
  Serial.flush();
  Serial.end();
  Serial.begin(9600);
  Serial.println("INTERNET MODE");
  blinkD(500, 2);
  WiFi.mode(WIFI_OFF);
  Serial.begin(9600);
  delay(100);
}
//-------------------------------------WIFI(AP)
void setupWifi(){
  Serial.flush();
  Serial.end();
  Serial.begin(115200);
  
  Serial.println("WIFI MODE");
  blinkD(500, 3);

  Serial.println();
  Serial.print("Configuring access point...");
  WiFi.softAP("ESPap", "thereisnospoon");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  server.on("/", handleMain);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Websocket server started");
}
//-------------------------------------HOTSPOT
void setupHotspot(){
  WiFi.mode(WIFI_OFF);
  Serial.println("HOTSPOT MODE");
  blinkD(500, 4);
  WiFiMulti.addAP("realme 7", "1234567890");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print('.');
    delay(100);
  }
  Serial.println(WiFi.localIP());

  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
  server.on("/", handleMain);
  server.onNotFound(handleNotFound);
  server.begin();
}
//-------------------------------------MODE LOOP FUNCTIONS
//-------------------------------------INTERNET
void loopInternet(){
  Serial.println("LOOP INTERNET");
  if (gprsSerial.available()){
    Serial.write(gprsSerial.read());
  }

  gprsSerial.println("AT+CREG?");
  delay(100);
 
  gprsSerial.println("AT+CGATT?");
  delay(100);
 
  gprsSerial.println("AT+CIPSHUT");
  delay(100);
 
  gprsSerial.println("AT+CIPSTATUS");
  delay(200);
 
  gprsSerial.println("AT+CIPMUX=0");
  delay(200);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CSTT=\"airtelgprs.com\"");//start task and setting the APN,
  delay(1000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIICR");//bring up wireless connection
  delay(3000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIFSR");//get local IP adress
  delay(2000);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIPSPRT=0");
  delay(300);
 
  ShowSerialData();
  
  gprsSerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  delay(600);
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIPSEND");//begin send data to remote server
  delay(400);
  ShowSerialData();
  
  String str="GET https://api.thingspeak.com/update?api_key=7GXCB6IG0PDLP0JV&field1=0" + curnt+"&field2="+volt+"&field3="+loc+"&field4="+btemp+"&field5="+mtemp+"&field6="+hack+"&field7="+charge+"&field8="+volt;
  Serial.println(str);
  gprsSerial.println(str);//begin send data to remote server
  
  delay(400);
  ShowSerialData();
 
  gprsSerial.println((char)26);//sending
  delay(500);//waitting for reply, important! the time is base on the condition of internet 
  gprsSerial.println();
 
  ShowSerialData();
 
  gprsSerial.println("AT+CIPSHUT");//close the connection
  delay(100);
  ShowSerialData();
  Serial.println("You can now change mode");
  blinkD(200,7);
  delay(1000);
}
//-------------------------------------WIFI(AP)
void loopWifi(){
  Serial.println("LOOP WIFI");
  webSocket.loop();
  server.handleClient();
  value = (String) analogRead(A0);
  webSocket.broadcastTXT(value);
  delay(300);
}
//-------------------------------------HOTSPOT
void loopHotspot(){
  Serial.println("LOOP HOTSPOT");
  webSocket.loop();
  server.handleClient();
  value = (String) analogRead(A0);
  webSocket.broadcastTXT(value);
  delay(300);
}
//-------------------------------------MAIN SETUP
void setup() {
  pinMode(mPin, INPUT);
  pinMode(led, OUTPUT);
  gprsSerial.begin(9600); 
  Serial.begin(9600);
  delay(300);
}
//-------------------------------------MAIN LOOP
void loop() {
  if(digitalRead(mPin)==1){                     // Checking if switch is on
    if(ones==0){                                // For running once
      if(modes<3){                              // Inrementin modes such as it is 1, 2 or 3
        modes++;
        Serial.println(modes);
      }else{
        modes = 0;                              // If modes>3 them make modes = 0
      }
                                                
      if(modes==1){                             // If modes is 1
        setupInternet();
      }else if(modes==2){                       // If modes is 2
        setupWifi();
      }else if(modes==3){                       // If modes is 3
        setupHotspot();
      }
      ones++;                                   // Incrementing ones so that it runs only once
    }else{                                      // Codes that will exeute until switch is pressed (LOOPs)
      if(modes==1){                             // If modes is 1 then loop of mode 1
        loopInternet();
      }else if(modes==2){                       // If modes is 2 then loop of mode 2
        loopWifi();
      }else if(modes==3){                       // If modes is 3 then loop of mode 3
        loopHotspot();
      }
    }
  }else{                                        // If switch is off then make ones = 0, means do not enter any mode
      ones = 0;
  }
}
