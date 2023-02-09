#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <Servo.h>

const char* ssid = "NIMBARGI"; 
const char* password = "samarth@2506";

int servoPin25 = 25;
int servoPin26 = 26;
int servoPin27 = 27;

const char* serverName = "http://192.168.0.102:5000/control-get";

Servo servo;

int position = 0;

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);



  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");  
}

void loop() {
  // put your main code here, to run repeatedly:
  JsonObject& control = getControl();
  int servoAddress = int(control["servoAddress"]);

  Serial.println(position);


  if(servoAddress !=0 ){
    servoRotate(servoAddress);     
  }
  else{
    servo.detach();
  } 
}

void servoRotate(int servoAddress){

  if(servoAddress != 0){
    if(servoAddress < 0){
        servo.attach(25);
        servo.write(180);         
      }
      else{
          servo.attach(25);
          servo.write(0);
    }
  }
}

int convertToInt(char* a, int size)  // from string to int
{
    int i;
    String s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s.toInt();
}

JsonObject& getControl(){           // GET request to the server. Used a function just to make the code more readable.

  StaticJsonBuffer<200> jsonBuffer;


  String controlJson = httpGETRequest(serverName);
  JsonObject& root = jsonBuffer.parseObject(controlJson);

  return root;
}

String httpGETRequest(const char* serverName) {  // ye code chapa hai
  WiFiClient client;
  HTTPClient http;
    
  http.begin(client, serverName);
  
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }

  http.end();

  return payload;
}

int httpPOSTRequest(const char* serverName, String requestData){
  WiFiClientSecure client;
  HTTPClient http;

  http.begin(client, serverName);
  http.addHeader("Content-Type", "application/json");

  int httpResponseCode = http.POST(requestData);

  return httpResponseCode;
}
