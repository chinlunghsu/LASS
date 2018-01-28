# LASS

Based on Linkit 7688 duo, the final version of air quality management system be released (Jan, 2018). The purpose of this system is to upload air-related data including temperature, humidity, PM2.5 and PM10 to cloud platform such as LASS and Thingspeak. Besides, the system can control air purifier by MCS apps on mobile devices. The hardware of system includes Linkit smart 7688 duo, Grove Breakout for LinkIt Smart 7688 Duo, PMS3003(G3), DHT22 module, OLED Display, relay and RTC time module. The software requires four revised programs such as lass.ino, iot1.js, app.js and things.py which are based on LASS open sources and MCS open sources.  

以Linkit 7688 duo為基礎之空氣品質偵測系統 (final version, 2018/1/28)
(程式均來自開源) 

目標: 上傳空氣品質數據至LASS及Thingspeak, 並可使用手機回控繼電器, 開關空氣清靜機.

需要硬體
Linkit Smart 7688 duo
Grove Breakout for LinkIt Smart 7688 Duo
PMS3003(G3) 粉塵傳感器
DHT 22數位溫濕度感測器模組
Grove - OLED Display
5V 1路 繼電器模組
RTC 時間模組

//設定pin腳
SoftwareSerial seG3(8,10); - G3
DHT dht(4, DHT22); - DHT22
DS1302 rtc(A3, A2, A1); //時間PIN
int relay=7; // 繼電器PIN
OLED Display IC2

ps. 5v slot不足需自行破線, 再使用熱縮套管結合, 另外, 繼電器外接110v電源線需小心, 需斷電再work.

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

things.py請自行下載使用,
這支程式目的是上傳至Thingspeak (程式原理是訂閱LASS資料, 並上傳至Thingspeak, 更新頻率約一分鐘)

雲端回控
至mcs (https://mcs.mediatek.com/) 雲端平台上申請帳號,並記下deviceId與 deviceKey, 以利app.js使用.

雲端回控 (app.js)需安裝下列模組

opkg update

madir app
 
cd app

npm init

問答輸入後按enter, 會產生個json檔.

npm install mcsjs 

安裝forever

npm install forever -g

讓系統開機可以自動執行3支程式
vim /etc/rc.local 
#!/bin/sh -e
nohup node /root/iot1.js > /dev/null 2>&1 &
forever start -c python /root/things.py LASS_DEVICE_ID ThingSpeak_API_KEY 
nohup node /root/app/app.js > /dev/null 2>&1 &

exit 0 


其中LASS_DEVICE_ID請改成你的LASS DEVICE ID, ThingSpeak_API_KEY改成你自己的KEY

後面的> /dev/null 2>&1 &表示是背景執行, 並將結果導向null, 經親身經驗, 若不導向null, Linkit 7688 duo晶片空間會被感測資料灌爆 !

可使用top指令查看程式(iot1.js, things.py and app.js) 


經實際測試, 可以正常work !

2018 the final version.
系統功能有:
1. 同時將温溼度感測資料上傳雲端LASS及ThingSpeak (iot1.js and things.py)
2. 手機遠端遙控系統開和關, 以利啟動和關閉空清機 (app.js)
3. 自動判斷晚上十二點至早上六點關閉空清機, 其它時間則依據PM2.5濃度智慧啟閉空清機（窗形控制，20以上開空清機，10以下關空清機）
4. 計算當天PM2.5 最大值及最小值


