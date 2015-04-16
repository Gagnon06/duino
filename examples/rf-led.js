var arduino = require('../')
    board, rf;

var board = new arduino.Board({
  debug: true,
  baseCode: "123"
});

var led = new arduino.Led({
	board: board,
	pin: "10",
	rf: true,
	receiver: "00"
});

board.on('ready', function(){
  led.blink();
  
});
