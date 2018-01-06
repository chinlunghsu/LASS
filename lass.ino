#include "DHT.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include <LCD.h>
#include "LiquidCrystal_I2C.h"
#include <SoftwareSerial.h>

//設定 pin 腳
SoftwareSerial seG3(8,10);

DHT dht(4, DHT22);
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

long pmcf10=0;
long pmcf25=0;
long pmcf100=0;
long pmat10=0;
long pmat25=0;
long pmat100=0;
float hum; //Stores humidity value
float temp; //Stores temperature value
char buf[50];
int cc=0;//counter

void setup() {
  lcd.begin(16, 2); // 初始化 LCD，一行 16 的字元，共 2 行，預設開啟背光
// 閃爍
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
  delay(200);
}

void loop() {
  int count = 0;
  unsigned char c;
  unsigned char high;
  hum = dht.readHumidity();
  temp = dht.readTemperature();
   if (isnan(hum) || isnan(temp)) {
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("DHT sensor fail");
   delay(5000);
   lcd.setCursor(0, 1);
   lcd.print("PM2.5=");
   lcd.print(pmat25);
   lcd.setCursor(9, 1);
   lcd.print("PM10=");
   lcd.print(pmat10);
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
delay(2000);  // Delays 2 secods, as the DHT22 sampling rate is 0.5Hz
lcd.clear();
Serial.flush();
Serial1.flush();

}
