#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <Servo.h>
#include <ArduinoJson.h>

const char* ssid = "NIMBARGI"; 
const char* password = "samarth@2506";

int servoPin1 = 25;
int servoPin2 = 26;
int servoPin3 = 27;

int directions[3];


const char* serverName = "http://192.168.0.102:5000/control-get";

Servo servo;
Servo servo2;
Servo servo3;

int position = 0;

void setup() {
  // put your setup code here, to run once:
  
  Serial.begin(115200);

  servo.attach(servoPin1);
  servo2.attach(servoPin2);
  servo3.attach(servoPin3);



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
  JsonObject control = getControl();

  directions[0] = int(control["1"]);
  directions[1] = int(control["2"]);
  directions[2] = int(control["3"]);


  // for(int i=1; i<=3; i++){
  //   String j = String(i);
  //   directions[i-1] = int(control[j]);
  //   delay(0);
  // }
    Serial.println(directions[0]);


  //int servoAddress = int(control["servoAddress"]);


  servoRotateNew(directions[0], directions[1], directions[2]); 

}

void servoRotateNew(int direction1, int direction2, int direction3){
  int rotate1;
  int rotate2;
  int rotate3;

  rotate1 = getRotationValue(direction1);
  rotate2 = getRotationValue(direction2);
  rotate3 = getRotationValue(direction3);

  servo.write(rotate1);
  servo2.write(rotate2);
  servo3.write(rotate3);
}

int getRotationValue(int direction){
  if(direction == 1){
    return 0;
  }
  else if(direction == -1){
    return 180;
  }
  else if(direction == 0){
    return 90;
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

// int servoMApping(int servoAddress){
//   if(servoAddress == 1){
//     return servoPin1;
//   }
//   else if(servoAddress == 2){
//     return servoPin2;
//   }
//   else if(servoAddress == 3){
//     return servoPin3;
//   }

//   return 0;
// }

int convertToInt(char* a, int size)  // from string to int
{
    int i;
    String s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s.toInt();
}

JsonObject getControl(){           // GET request to the server. Used a function just to make the code more readable.

  StaticJsonDocument<200> jsonBuffer;


  String controlJson = httpGETRequest(serverName);
  deserializeJson(jsonBuffer, controlJson);
  JsonObject control = jsonBuffer.as<JsonObject>();
  //JsonObject& root = jsonBuffer.parseObject(controlJson);

  return control;

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
