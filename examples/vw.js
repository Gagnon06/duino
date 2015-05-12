var arduino = require('../')
    board, vw;

var board = new arduino.Board({
  debug: true
});

var vw = new arduino.VW({
  board: board,
  pin: "10"
});

board.on('ready', function(){
  board.write('93:123:1:1:0000');
});
