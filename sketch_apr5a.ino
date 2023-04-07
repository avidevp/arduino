#include <SoftwareSerial.h>
// #include <ArduinoJson.h>
#define RX 10
#define TX 11
#define led_pin 2
#define trigPin 4
#define echoPin 3
// defines variables
long duration;
int distance;
String AP = "Avinash";       // CHANGE ME
String PASS = "BABA9930"; // CHANGE ME
String HOST = "api.thingspeak.com";
int countTrueCommand;
int countTimeCommand; 
boolean found = false; 
SoftwareSerial esp8266(RX,TX); 

unsigned long lastTimeMillis = 0;
String response="";
void setup() {
  
  Serial.begin(9600);
  esp8266.begin(9600);
  pinMode(led_pin, OUTPUT);
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
  digitalWrite(led_pin, HIGH);
  sendCommand("AT",5,"OK");
  sendCommand("AT+UART_DEF=9600,8,1,0,0",5,"OK");
  sendCommand("AT+CWMODE=1",5,"OK");
  sendCommand("AT+CWJAP=\""+ AP +"\",\""+ PASS +"\"",20,"OK");
  sendCommand("AT+CIPMUX=1",10,"OK");
}
void printResponse() {
  while (esp8266.available()) {
    Serial.println(esp8266.readStringUntil('\n')); 
  }
}

void loop() {
  // if (millis() - lastTimeMillis > 30000) {
    // Serial.println(millis());
    distance=getDistance();    
    Serial.println(distance);
    

    esp8266.println("AT+CIPSTART=4,\"TCP\",\"192.168.137.1\",80");
    delay(100);
    printResponse();
    String cmd = "GET /water-tank/bulb/read_data2.php HTTP/1.1\r\nHost:192.168.137.1\r\nConnection: close\r\n\r\n";
    // String cmd = "POST /water-tank/insert_data.php HTTP/1.1\r\nHost:192.168.0.6\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 7\r\nConnection: close\r\n\r\ndist=12\r\n";
    esp8266.println("AT+CIPSEND=4," + String(cmd.length() + 4));
    delay(100);

    esp8266.println(cmd);
    delay(100);
    esp8266.println(""); 
    // esp8266.println("AT+CIPCLOSE");
  // }
  // delay(10000);
  if (esp8266.available()) {
    // Serial.println(esp8266.read());
    // Serial.write(esp8266.readString());
    response=esp8266.readString();
    if(response.lastIndexOf("<div>0</div>")>0)
    {
      
  digitalWrite(led_pin, LOW);
    }
    else if(response.lastIndexOf("<div>1</div>")>0)
    {
      
  digitalWrite(led_pin, HIGH);
    }
        
  }
  esp8266.println("AT+CIPCLOSE=5");
  delay(100);
   esp8266.println("AT+CIPSTART=4,\"TCP\",\"192.168.137.1\",80");
   delay(300);
  //   delay(100);
    printResponse();
  cmd = "POST /water-tank/insert_data.php HTTP/1.1\r\nHost:192.168.137.1\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: 7\r\nConnection: close\r\n\r\ndist="+String(distance)+"\r\n";
    esp8266.println("AT+CIPSEND=4," + String(cmd.length() + 4));
    delay(300);

    esp8266.println(cmd);
    delay(100);
    esp8266.println(""); 
  // // }
  // delay(1000);
  if (esp8266.available()) {
    // Serial.println(esp8266.read());
    // Serial.write(esp8266.readString());
    response=esp8266.readString();
    // Serial.println(response);
    
  }
  
  delay(1000);
}

int getDistance()
{
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  duration = pulseIn(echoPin, HIGH);
  // Calculating the distance
  int d = duration * 0.034 / 2;
  return d;

}

int getSensorData(){
  return random(1000); // Replace with 
}
void sendCommand(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while(countTimeCommand < (maxTime*1))
  {
    esp8266.println(command);//at+cipsend
    if(esp8266.find(readReplay))//ok
    {
      found = true;
      break;
    }
  
    countTimeCommand++;
  }
  
  if(found == true)
  {
    Serial.println("OYI");
    countTrueCommand++;
    countTimeCommand = 0;
  }
  
  if(found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }
  
  found = false;
 }