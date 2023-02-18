#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>
#include <LiquidCrystal_I2C.h>

#include <Hash.h>
#include "FS.h"
/* Put your SSID & Password */
const char* ssid = "team72";   //Enter SSID here
const char* password = "12345678";  //Enter Password here
Servo myServo ;
ESP8266WebServer server(80);
//left
int trigParkingArea1 = D5 ; 
int echoParkingArea1 = D6;
int ledParkingArea1 = D8 ;

//right
int trigParkingArea2 = D3; 
int echoParkingArea2 = D4;
int ledParkingArea2 = D0 ;
//controls

int numberOfCars = 0 ;
bool flag = false , flagServo = false ; 
int IR = 9 ;
int buzzer = 10 ;
float dis = 0 ;
unsigned int timeParking1 = 0 ,timeParking2 = 0 ,timeParking3 = 0 , timeOpeningServo = 0 ; 
int price1 = 0 , price2 = 0 , price3 = 0 ;


 
LiquidCrystal_I2C lcd(0x27, 16 ,2);  


void setup() {
  
  Serial.begin(115200);
  pinMode(trigParkingArea1 , OUTPUT );
  pinMode(echoParkingArea1 , INPUT );
  pinMode(trigParkingArea2 , OUTPUT );
  pinMode(echoParkingArea2 , INPUT );
  pinMode(ledParkingArea1 , OUTPUT );
  pinMode(ledParkingArea2 , OUTPUT );
  pinMode(IR , INPUT );
  pinMode(buzzer , OUTPUT );
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());
  delay(100);
  
  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  Serial.println("HTTP server started");
  timeParking1 = millis() ; 
  timeParking2 = millis() ;
  timeParking3 = millis() ;
  timeOpeningServo = millis() ;
  myServo.attach(D7);
  myServo.write(0);
  lcd.begin();
  lcd.backlight();
  lcd.clear();
}
void loop() {
  server.handleClient();
  float isDetect = digitalRead(IR);
  Serial.println(isDetect);
  numberOfCars = 0 ;
  flag = parking_Area( trigParkingArea1 , echoParkingArea1 , ledParkingArea1 ) ;
  if(flag){
    numberOfCars++ ;  
    timeParking1 = millis() ;
    price1 = 0 ;
    lcd.setCursor(8,1);
    lcd.print("L:Empty");
 
  }else{
    lcd.setCursor(8,1);
    lcd.print("L:Full ");
    if(millis() - timeParking1  >= 60000 ){
        price1 += 3 ;
        timeParking1 = millis() ; 
    }
  }
  flag = parking_Area( trigParkingArea2 , echoParkingArea2 , ledParkingArea2 ) ;
  if(flag){
    lcd.setCursor(0,1);
    lcd.print("R:Empty");
    numberOfCars++ ; 
    timeParking2 = millis() ;
    price2 = 0 ;
  }else{
    lcd.setCursor(0,1);
    lcd.print("R:Full ");
    if( millis()- timeParking2  >= 60000 ){
        price2 += 3 ;
        timeParking2 = millis() ; 
    }
  }

  if(isDetect == 0 && numberOfCars > 0 ){
    myServo.write(180);
    digitalWrite(buzzer , HIGH);
    delay(100);
    digitalWrite(buzzer , LOW);
    flagServo = true ;
    timeOpeningServo = millis() ;    
  }
  if(millis() - timeOpeningServo > 2000 &&  flagServo == true ){
     myServo.write(0);
  digitalWrite(buzzer , HIGH);
  delay(150);
  digitalWrite(buzzer , LOW);
     timeOpeningServo = millis() ;
     flagServo = false ;  
  }
   
  lcd.setCursor(1,0);
  lcd.print("have slot : ");
  lcd.print(numberOfCars);
  
  
}

void handle_OnConnect() {
 
  server.send(200, "text/html", SendHTML()); 
 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
  ptr +="<title>Parking</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;background-color:#212020 ; background-repeat: no-repeat; background-size: 100% 100%;} h1 {color: #f6d139 ;text-shadow: 10px 8px 8px #f8f7f8;margin: 50px auto 30px;} h2 {color: #f3f2f2;margin-bottom: 50px;}\n";

  ptr +="p {font-size: 26 px; color:#d8cccc ;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>Parking</h1>\n";
  ptr +="<p>Parking Area empty : ";
  ptr += numberOfCars ;
  ptr += "</p> \n " ; 
  ptr +="<h2>Parking Area 1</h2> <p>Price : ";
  ptr += price1 ;
  ptr += "</p>\n " ; 
  ptr +="<h2>Parking Area 2</h2>";
  ptr +="<p>Price : ";
  ptr += price2 ;
  ptr += "</p>\n " ;
  ptr +="</body>\n";
  ptr +="</html>\n";

  return ptr;
}
bool parking_Area(int trig , int echo , int ledPin ){
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  digitalWrite(trig,HIGH) ;
  delayMicroseconds(5);
  digitalWrite(trig, LOW);
  delayMicroseconds(2);
  float dur= pulseIn(echo , HIGH) ; 
  dis = dur/29/2 ; 
  delay(1000);
  if( dis>11 && dis < 14 ){
    myServo.write(180);
    flagServo = true ;
    timeOpeningServo = millis() ;   
  }
  if(dis > 15){
    digitalWrite(ledPin,HIGH);
    Serial.println(dis);
    return true ;
  }
  else{
    digitalWrite(ledPin,LOW) ;
    Serial.println(dis);
    return false ;
  }
  
}
