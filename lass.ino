#include "DHT.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include <LCD.h>
#include "LiquidCrystal_I2C.h"
#include <SoftwareSerial.h>
#include <SeeedOLED.h> //載入SeeedOLED函式庫
#include <DS1302.h>

//設定 pin 腳
SoftwareSerial seG3(8,10); 

DHT dht(4, DHT22);  //温溼度PIN
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
DS1302 rtc(A3, A2, A1);


// #define PINOUT 7 // 繼電器PIN
#define LEASTONTIME 300000
#define ONvalue 25
#define OFFvalue 15

int relay=7;
long pmcf10=0;
long pmcf25=0;
long pmcf100=0;
long pmat10=0;
long pmat25=0;
long pmat100=0;
long lastOnTime;
long heartbeat;
float hum; //Stores humidity value
float temp; //Stores temperature value
char buf[50];
int cc=0;//counter
String dw="";
String tw = "";
String hh = "";
float max25=0;
float min25=300;
float temph=0;

void setup() {
  rtc.halt(false);
  rtc.writeProtect(true);
  // rtc.setDate(20, 1, 2018);
  // rtc.setTime(9,28,00);
  dw = rtc.getDateStr();
  pinMode(relay,OUTPUT);
  SeeedOled.init();  
  SeeedOled.clearDisplay();  //清除螢幕
  SeeedOled.setNormalDisplay(); //設定螢幕為正常模式(非反白)
  SeeedOled.setPageMode();  //設定尋址模式頁模式
  SeeedOled.setTextXY(0,0); //設定啟始坐標
  SeeedOled.putString("Hsu_001 station");
  lcd.begin(16, 2); // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光
  lcd.backlight(); // 開啟背光
  delay(200);
  lcd.noBacklight(); // 關閉背光
  delay(200);
  lcd.backlight();
  lcd.setCursor(0, 0); // 設定游標位置在第一行行首
  lcd.print("Hello, world!");
  seG3.begin(9600);//G3
  Serial.begin(9600);
  Serial1.begin(57600);//給 MPU 訊息
  delay(1000);
  temph = dht.readTemperature();
  long lastOnTime;
}

void loop() {
  dw = rtc.getDateStr();
  int count = 0;
  unsigned char c;
  unsigned char high;
  hum = dht.readHumidity();
  temp = dht.readTemperature();
  if (isnan(hum) || isnan(temp) || abs(temph-temp) > 2)
   {
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("DHT sensor fail");
   lcd.setCursor(0, 1);
   lcd.print("PM2.5=");
   lcd.print(pmat25);
   lcd.setCursor(9, 1);
   lcd.print("PM10=");
   lcd.print(pmat10);
   delay(2000);
   return;
   }    
cc++;
  while (seG3.available()) {
    c = seG3.read();
    if((count==0 && c!=0x42) || (count==1 && c!=0x4d)){
      Serial.println("check failed");
      break;
    }
    if(count > 15){
      Serial.println("complete");
      break;
    }
    else if(count == 4 || count == 6 || count == 8 || count == 10 || count == 12 || count == 14) {
      high = c;
    }
    else if(count == 5){
      pmcf10 = 256*high + c;
      Serial.print("CF=1, PM1.0=");
      Serial.print(pmcf10);
      Serial.println(" ug/m3");
    }
    else if(count == 7){
      pmcf25 = 256*high + c;
      Serial.print("CF=1, PM2.5=");
      Serial.print(pmcf25);
      Serial.println(" ug/m3");
    }
    else if(count == 9){
      pmcf100 = 256*high + c;
      Serial.print("CF=1, PM10=");
      Serial.print(pmcf100);
      Serial.println(" ug/m3");
    }
    else if(count == 11){
      pmat10 = 256*high + c;
      Serial.print("atmosphere, PM1.0=");
      Serial.print(pmat10);
      Serial.println(" ug/m3");
    }
    else if(count == 13){
      pmat25 = 256*high + c;
      Serial.print("atmosphere, PM2.5=");
      Serial.print(pmat25);
      Serial.println(" ug/m3");
    }
    else if(count == 15){
      pmat100 = 256*high + c;
      Serial.print("atmosphere, PM10=");
      Serial.print(pmat100);
      Serial.println(" ug/m3");
    }
    count++;
}
    
  while(seG3.available()) seG3.read();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(temp);
  lcd.print(" *C");
  lcd.setCursor(9, 0);
  lcd.print(hum);
  lcd.print(" %");
  lcd.setCursor(0, 1);
  lcd.print("PM2.5=");
  lcd.print(pmat25);
  lcd.setCursor(9, 1);
  lcd.print("PM10=");
  lcd.print(pmat10);


//將溫濕度與 PM2.5 資訊傳給 MPU
Serial1.print("|s_t0=");//Temperature=
Serial1.print(temp);
Serial1.print("|s_h0=");//Humi=
Serial1.print(hum);
Serial1.print("|s_d0=");//PM2.5=
Serial1.print(pmat25);
Serial1.print("|s_d1=");//PM10=
Serial1.println(pmat10);

SeeedOled.setTextXY(1, 0);
SeeedOled.putString("Date: ");
SeeedOled.putString(rtc.getDateStr());
SeeedOled.setTextXY(2, 0);
SeeedOled.putString("Time: ");
SeeedOled.putString(rtc.getTimeStr());

if (dw == rtc.getDateStr())
   {
     if (max25 <= pmat25)
       {
         SeeedOled.setTextXY(3, 0);
         SeeedOled.putString("max: ");
         SeeedOled.putFloat(pmat25);
         max25 = pmat25;
       }
      else if (min25 >= pmat25)
       {
          SeeedOled.setTextXY(5, 0);
          SeeedOled.putString("min: ");
          SeeedOled.putFloat(pmat25);
          min25 = pmat25;
       } 
   }
if (dw !=  rtc.getDateStr())
      {
         max25 =0;
         min25 =300;
         dw = rtc.getDateStr();
      }
   
//設定繼電器開關
tw = rtc.getTimeStr();
hh = tw.substring(0,2); 
long nowtime = millis();
if(pmat25 > ONvalue && (hh != "22") && (hh != "23") && (hh != "00") && (hh != "01") && (hh != "02") && (hh != "03") && (hh != "04") && (hh != "05"))
  {
    digitalWrite(relay,LOW); //Turn ON
    delay(1000);
    lastOnTime =nowtime;   
  }
if(pmat25 < OFFvalue && (nowtime - lastOnTime) > LEASTONTIME)
  {
    digitalWrite(relay,HIGH); //Turn OFF
    delay(1000);
  }

if((hh == "22") || (hh == "23") || (hh == "00") || (hh == "01") || (hh == "02") || (hh == "03") || (hh == "04") || (hh == "05"))
  {
    digitalWrite(relay,HIGH); //Turn OFF
    delay(1000);
  }

SeeedOled.setTextXY(4, 0);
SeeedOled.putString("now:       ");
SeeedOled.putFloat(pmat25);
SeeedOled.setTextXY(7, 0);
SeeedOled.putString("C ");
SeeedOled.putFloat(temp);
SeeedOled.setTextXY(7, 8);
SeeedOled.putString(" % ");
SeeedOled.putFloat(hum);

temph = temp;
delay(10000);
}


