# LASS
以Linkit 7688 duo為基礎之空氣品質偵測系統 (第二版)
(程式均來自開源) 

目標: 上傳空氣品質數據至LASS及Thingspeak

需要硬體
Linkit Smart 7688 duo
Grove Breakout for LinkIt Smart 7688 Duo
PMS3003(G3) 粉塵傳感器
DHT 22數位溫濕度感測器模組
Arduino I2C 1602 LCD 藍色背光液晶模塊

//設定pin腳
SoftwareSerial seG3(8,10); - G3
DHT dht(4, DHT22); - DHT22
LCD (其中vcc請接5v, 其它直接使用I2C)

Arduino 使用 lass.ino (請自行下載使用), 並上傳至Linkit 7688 duo

請自行先去ThingSpeak註冊 (記下Write API Key, 另外Channel Settings四個, 分別是温度, 溼度, pm2.5及pm10)

使用ssh登入7688, 下指令
opkg update
opkg install mosquitto mosquitto-client libmosquitto
npm install mqtt@1.7.0 --save
npm install ntp-client –save
opkg install coreutils-nohup
opkg install nano
Linkit 7688 duo 使用iot1.js及things.py (use ssh)

Iot1.js 請自行下載使用, 需要修改地方有
deviceId: "xxx_xxx",//自己命名

lat : xx.xxxx,//使用google map查詢

lng : xxx.xxxx,


讓系統開機可以自動work
vim /etc/rc.local 
#!/bin/sh -e

node /root/iot1.js

exit 0 

things.py請自行下載使用,
這支程式目的是上傳至Thingspeak (程式原理是訂閱LASS資料, 並上傳至Thingspeak)

使用ssh登入, 並執行以下指令

nohup python things.py LASS_DEVICE_ID ThingSpeak_API_KEY > /dev/null 2>&1 &

其中LASS_DEVICE_ID請改成你的LASS DEVICE ID, ThingSpeak_API_KEY改成你自己的KEY

後面的> /dev/null 2>&1 &表示是背景執行, 並將結果導向null, 經親身經驗, 若不導向null, Linkit 7688 duo晶片空間會被感測資料灌爆 !

可使用top指令查看這二支程式(iot1.js and things.py) be working

共三支程式, 可至以下網址下載
https://github.com/chinlunghsu/LASS

經實際測試, 可以正常work !
