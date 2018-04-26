//using Adafruit Feather HUZZAH w/integrated ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>

int sensorPin = 14;  //Optomax Digital Liquid Level Sensor - LLC200D3SH-LLPK1
bool sensorValue;
bool lastMessage;

const char* ssid = "NETWORK_NAME"; 
const char* password = "PASSWORD"; 

const char* host = "api.pushbullet.com";
const int httpsPort = 443;

const char* accessToken = "YOUR_ACCESS_TOKEN"; //access token via your pushbullet account 
String url = "/v2/pushes";

int initCount = 36; //{"body":,"title":,"type":"note"}""""

void setup() {
  // put your setup code here, to run once:
  pinMode(sensorPin, INPUT);               //declaration for sensor pin
  Serial.begin(9600);
  Serial.println("Starting Board"); 

  //connect to wifi network

  Serial.print("Connecting to network: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); 
   
  while (WiFi.status() != WL_CONNECTED) //waiting for connection
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  lastMessage = ReadSensor(); //initial reading of liquid sensor
  PrintSensorData(lastMessage);
}

void loop() 
{  
 sensorValue = ReadSensor();     //reads the sensor
 if (lastMessage == sensorValue) //do nothing if the signal remains the same
 {
 }

 else if (lastMessage!= sensorValue) //if new signal does not match first signal then...
 {
  PrintSensorData(sensorValue);      //print new signal message

  if (sensorValue)                   //if the signal shows that water has been detected,
  {                                 // then send that as push notification
    PushSensorData("ELLA FOOD NOTIFICATION!", "FOOD OVERFLOW DETECTED!");
     
  }
  lastMessage = sensorValue; //change the last recorded value. 
 }                          //The loop will track CHANGES in sensor value and print those changes in the 
                            //serial monitor, but will only send notifications if water is detected
}                           //only tracking a change in sensor reading prevents notification spamming
bool ReadSensor()
{
  int val = digitalRead(sensorPin);
  if (val == HIGH)  //when sensor is not in liquid, returns a HIGH signal
  {
    return false;
  }
  if (val == LOW) //when sensor is submerged, returns a LOW signal
  {
    return true;
  }
}

void PrintSensorData(bool response) //this function lets you track sensor value changes
{                                   //in the serial monitor
  if (response)
  {
    Serial.println("WATER DETECTED!");
  }
  else
  {
    Serial.println("NO WATER DETECTED");
  }
}

void PushSensorData(const char* messageTitle, const char* messageBody)               //send data to pushbullet, becoming a push notification. this function needs to be                                  
{
    // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  Serial.print("connecting to ");
  Serial.println(host);
  
  if (!client.connect(host, httpsPort)) 
  {
    Serial.println("connection failed");   //diagnose disconnect 
    return;
  }
  Serial.print("requesting URL: "); Serial.println(url);

  int messageLength = initCount;

  messageLength = messageLength + strlen(messageTitle);
  messageLength = messageLength + strlen(messageBody);

  Serial.println(messageLength);

  client.print(String("POST ") + url + " HTTP/1.1\r\n" +            //push protocol
         "Host: " + host + "\r\n" +
         "User-Agent: ESP8266\r\n" +
         "Access-Token: " + accessToken + "\r\n" +
         "Content-length: " + messageLength + "\r\n"                                  
         "Content-Type: application/json\r\n" +                                     
         "Connection: close\r\n\r\n" +
         "{\"body\":\"" + messageBody + "\",\"title\":\"" + messageTitle + "\",\"type\":\"note\"}");
         
  Serial.println("Notification pushed!");
}
