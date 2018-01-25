var mcs = require('mcsjs');  
  var com = require("serialport");
var s = new com.SerialPort("/dev/ttyS0", {
    baudrate: 57600});
require('events').EventEmitter.prototype._maxListeners = 100;


var myApp = mcs.register({
     deviceId: 'xxxxxx',
     deviceKey: 'xxxxxxx',
});

myApp.on('Relay_Control', function(data, time) {
 if(Number(data) === 1){
     console.log("Relay on");   //顯示訊息
     s.write("0")     //繼電器啟動
 } else {
     console.log("Relay off");
     s.write("1")
 }
});
