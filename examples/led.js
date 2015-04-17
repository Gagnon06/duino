var arduino = require('../');

var board = new arduino.Board({
    debug: true,
    baudrate: 115200
});

var led = new arduino.Led({
    board: board,
    pin: "13"
});

board.on('ready', function(){
    led.blink();
});
