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
#define hackP D6
#define led LED_BUILTIN
#define Hall_Sensor_D D2
//-------------------------------------VARIABLES.

byte ones = 0;
byte modes = 0;
//default values

float vref = 3;
float resolution = vref/1023;
//---


String curnt="2.2";
String volt="12";
String vspeed="0";
String loc="https://www.google.com/maps/search/amlidih+gprs+location/@18.6470489,73.5858729,14z";
String btemp="27.14";
String mtemp="27.14";
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
      <style>
        .boxcol{
            background-color: #1363Df;
            color: white;
            border: 3px;
            padding: 5px;
        }
      </style>
      <script>
        socket = new WebSocket("ws:/" + "/" + location.host + ":81");
//          webSocket.broadcastTXT(curnt, volt, vspeed, hack, mtemp, btemp, charge);
        socket.onopen = function(e, f, g, h, i, j ,k) {  console.log("[socket] socket.onopen "); };
        socket.onerror = function(e, f, g, h, i, j ,k) {  console.log("[socket] socket.onerror "); };
        socket.onmessage = function(e, f, g, h, i, j ,k) {  
            console.log("[socket] " + e.data);
            document.getElementById("a_value").innerHTML = e.data;
          
        };
      </script>
   </head>
   <body style="background-color: #DFF6FF;">
      <p class="boxcol">Battery Charging   :<u id="charging">100%</u></p>
      <p class="boxcol">Speed              :<u id="speed">0 Km/h</u></p>
      <p class="boxcol">Motor Temperature  :<u id="mtemp">27.4 C</u></p>
      <p class="boxcol">Battery Temperature:<u id="btemp">27.4 C</u></p>
      <p class="boxcol">Is Tempered?       : <u id="a_value">NO</u></p>
      <p class="boxcol">Battery Current    : <u id="current">2.2A</u></p>
      <p class="boxcol">Battery Voltage    : <u id="volt">12V</u></p>
      
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
  if(digitalRead(mPin)==HIGH){
  gprsSerial.println("AT+CREG?");
  delay(100);
   if(digitalRead(mPin)==HIGH){
  gprsSerial.println("AT+CGATT?");
  delay(100);
    if(digitalRead(mPin)==HIGH){
  gprsSerial.println("AT+CIPSHUT");
  delay(100);
    if(digitalRead(mPin)==HIGH){
  gprsSerial.println("AT+CIPSTATUS");
  delay(200);
    if(digitalRead(mPin)==HIGH){
  gprsSerial.println("AT+CIPMUX=0");
  delay(200);
 
  ShowSerialData();
    if(digitalRead(mPin)==HIGH){
  gprsSerial.println("AT+CSTT=\"airtelgprs.com\"");//start task and setting the APN,
  delay(1000);
 
  ShowSerialData();
    if(digitalRead(mPin)==HIGH){
  gprsSerial.println("AT+CIICR");//bring up wireless connection
  delay(3000);
 
  ShowSerialData();
    if(digitalRead(mPin)==HIGH){
  gprsSerial.println("AT+CIFSR");//get local IP adress
  delay(2000);
 
  ShowSerialData();
    if(digitalRead(mPin)==HIGH){
  gprsSerial.println("AT+CIPSPRT=0");
  delay(300);
 
  ShowSerialData();
     if(digitalRead(mPin)==HIGH){
  gprsSerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");//start up the connection
  delay(600);
 
  ShowSerialData();
    if(digitalRead(mPin)==HIGH){
  gprsSerial.println("AT+CIPSEND");//begin send data to remote server
  delay(400);
  ShowSerialData();
     if(digitalRead(mPin)==HIGH){
  String str="GET https://api.thingspeak.com/update?api_key=7GXCB6IG0PDLP0JV&field1=0" + curnt+"&field2="+volt+"&field3="+vspeed+"&field4="+loc+"&field5="+btemp+"&field6="+btemp+"&field7="+hack+"&field8="+charge;
  Serial.println(str);
  gprsSerial.println(str);//begin send data to remote server
  
  delay(400);
  ShowSerialData();
    if(digitalRead(mPin)==HIGH){
  gprsSerial.println((char)26);//sending
  delay(500);//waitting for reply, important! the time is base on the condition of internet 
  gprsSerial.println();
 
  ShowSerialData();
    if(digitalRead(mPin)==HIGH){
  gprsSerial.println("AT+CIPSHUT");//close the connection
  delay(100);
  ShowSerialData();
  }}}}}}}}}}}}}}
  Serial.println("You can now change mode");
  blinkD(200,7);
  delay(1000);
}
//-------------------------------------WIFI(AP)
void loopWifi(){
  Serial.println("LOOP WIFI");
  webSocket.loop();
  server.handleClient();

  webSocket.broadcastTXT(hack);
//  delay(300);
}
//-------------------------------------HOTSPOT
void loopHotspot(){
  Serial.println("LOOP HOTSPOT");
  webSocket.loop();
  server.handleClient();
  webSocket.broadcastTXT(hack);
//  delay(100);
}
//-------------------------------------CALCULATE
void calc(){
 btemp = (String)calcTemp();
 delay(100);
 mtemp = (String)calcTemp();
 delay(100);
 vspeed = (String)speedCalc();
 hack = isTempered();
 charge = charge="100";
 curnt="2.2";
 volt="12";
}

float calcTemp(){
 float temperature = analogRead(A0);
 temperature = (temperature*resolution);
 temperature = (temperature*100) - 273.15;
 Serial.println(temperature);
 return temperature;
}

String isTempered(){
  byte isHack = digitalRead(hackP);
  if(isHack==1){
    return "NO";
  }else{
    return "YES";
  }
}

float calcVolt(){
  int sensorI;
  float voltage;
  while(voltage<=11){
    sensorI=analogRead(A0);
    voltage = map(sensorI, 0, 1024,0, 12); 
  }
 return voltage;
}

float getCurrent()
{
  
  float VRMS = 0;
  float AmpsRMS = 0;
  
  float result;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 1024;          // store min value here
  
   uint32_t start_time = millis();

   while((millis()-start_time) < 1000) //sample for 1 Sec
   {
       readValue = analogRead(A0);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
    }
   
   // Subtract min from max
  float Voltage = ((maxValue - minValue) * 5)/1024.0;
   VRMS = (Voltage/2.0) *0.707; // sq root
   AmpsRMS = 2+((VRMS * 1000)/66);
   return AmpsRMS;
}


float speedCalc(){
  int startTime = millis();
  int endTime = startTime;
  
  byte count = 0;
  byte Val1=0, nVal=1;

   while((endTime - startTime)<=1000){
     Val1=digitalRead(Hall_Sensor_D);
     if(Val1==0){
      if(Val1 != nVal){
        count++;
        nVal = Val1;
      }
     }else{
      nVal+=Val1;
     }
     endTime = millis();
   }
   return (0.001885*count*120*12);
}
//-------------------------------------MAIN SETUP
void setup() {
  pinMode(mPin, INPUT);
  pinMode(led, OUTPUT);
  pinMode(A0, INPUT);
  
  gprsSerial.begin(9600); 
  Serial.begin(9600);
  delay(300);
}
//-------------------------------------MAIN LOOP
void loop() {
  if(digitalRead(mPin)==1){                     // Checking if switch is on
    calc();
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
