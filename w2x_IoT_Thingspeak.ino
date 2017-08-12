#include <SoftwareSerial.h>

#define DEBUG true

int LED = 13;   // make LED as an indicator, when the data been push to thingSpeak

// replace with your channel's thingspeak API key
String apiKey = "IIIVD9S34ME4JWLU";

SoftwareSerial esp8266(2, 3); // make RX Arduino line is pin 2, make TX Arduino line is pin 3.
                              // This means that you need to connect the TX line from the esp to the Arduino's pin 2
                              // and the RX line from the esp to the Arduino's pin 3

void setup(){
  pinMode(LED, OUTPUT);    

  // enable debug serial
  Serial.begin(9600); 
  // enable software serial
  esp8266.begin(9600);
  
  // set ESP8266 as client
  sendData("AT+RST\r\n",2000,DEBUG); // reset module
  sendData("AT+CWMODE=1\r\n",1000,DEBUG); // configure as access point
  sendData("AT+CWJAP=\"MyDD2017\",\"mydd@2017\"\r\n",10000,DEBUG); // configure as client
  sendData("AT+CIFSR\r\n",1000,DEBUG); // get ip address
  sendData("AT+CIPMUX=1\r\n",1000,DEBUG); // configure for multiple connections
  Serial.println("Server ready");
}

void loop(){
  
  // TEMPERATURE DATA
  int sensorValue = analogRead(A0);

    // read the analog in value:
  // sensorValue = analogRead(analogInPin);
  // map it to the range of the analog out:
  int outputValue = map(sensorValue, 0, 1023, 0, 100);
  // change the analog out value:
//  analogWrite(analogOutPin, outputValue);


  //float tempValue = sensorValue*1;

  //Convert float to character Array
  char buff[16];
  String Level = dtostrf(outputValue, 4, 1, buff);
  
  // TCP CONNECTION
  String getThingSpeak = "AT+CIPSTART=4,\"TCP\",\"";
  getThingSpeak += "52.7.7.190"; // api.thingspeak.com
  getThingSpeak += "\",80";
  esp8266.println(getThingSpeak);
  
  // wait 2 second to establish connection between thingSpeak and ESP8266
  delay(100);
   
  if(esp8266.find("Error")){
    Serial.println("AT+CIPSTART error");
    return;
  }
  
  // UPDATE THINGSPEAK CHANNEL WITH DATA STRING
  String dataToPush = "GET /update?api_key=";
  dataToPush += apiKey;
  dataToPush += "&field1=";
  dataToPush += String(Level);
  dataToPush += "\r\n\r\n";

  // SEND DATA LENGTH
  String sendData = "AT+CIPSEND=4,";
  sendData += String(dataToPush.length());
  Serial.println(sendData);
  esp8266.println(sendData);
  // wait 2 second to establish connection between thingSpeak and ESP8266
  delay(2000);

  if(esp8266.find(">")){
    esp8266.print(dataToPush);
  }
  else{
    esp8266.println("AT+CIPCLOSE");
    // alerting closing connection
    Serial.println("AT+CIPCLOSE");
  }

  Serial.print("Level = ");
  Serial.println(Level);
  Serial.print("TCP Connection > ");
  Serial.println(getThingSpeak);
  Serial.print("API Update > ");
  Serial.println(dataToPush);
      
  // thingSpeak needs minimum 15 sec delay between updates
  delay(10000);  
}

/*
* Name: sendData
* Description: Function used to send data to Serial.
* Params: command - the data/command to send; timeout - the time to wait for a response; debug - print to Serial window?(true = yes, false = no)
* Returns: The response from the Serial1 (if there is a reponse)
*/
String sendData(String command, const int timeout, boolean debug)
{
    String response = "x";
    
    esp8266.print(command); // send the read character to the esp8266
    
    long int time = millis();
    
    while( (time+timeout) > millis())
    {
      while(esp8266.available())
      {
        
        // The esp has data so display its output to the serial window 
        char c = esp8266.read(); // read the next character.
        response+=c;
      }  
    }
   
    if(debug)
    {
      Serial.print(response);
    }
    
    return response;
}
