/*
 * Main VW constructor
 * Process options
 * Tell the board to set it up
 */
var VW = function (options) {
  if (!options || !options.board) throw new Error('Must supply required options to VW');
  this.board = options.board;
  this.pin = this.board.normalizePin(options.pin || 10);
    
    //TO-DO add init to send a msg to arduino with the 433 MHz module pin
    
    this.board.on('data', function (message) {
        if(message[0] === '!' && message[message.length-2] === '?') {
            var m = message.slice(1, -2).split(':'),
                err = null,
                pin, type, data;

            if (!m.length) {
              return;
            }

            type = m[0];
            emitId = m[1];
            sensorId = m[2];
            data = m[3];
            this.board.emit('sensor_data', [emitId, sensorId, data]);
        }
    }.bind(this));
}

/*
 * Send vw message
 */
VW.prototype.sendMsg = function (receiver, msg) {
  var message = '93:' + this.board.rfId + ':' + receiver + ':' + msg;
  this.board.write(message);
}

/*
 * Set a RF pin's mode
 * val == out = 001
 * val == in  = 000
 */
VW.prototype.rfPinMode = function (receiver, pin, val) {
  pin = this.normalizePin(pin);
  val = (
    val == 'out' ?
    this.normalizeVal(1) :
    this.normalizeVal(0)
  );
  var msg = '00' + pin + val;
  this.sendMsg(receiver, msg);
}

/*
 * Send RF digitalWrite
 */
VW.prototype.rfDigitalWrite= function (receiver, pin, val) {
  var msg = '01' + this.normalizePin(pin) + this.normalizeVal(val);
  this.sendMsg(receiver, msg);  
}

/*
 * Send RF analogWrite
 */
VW.prototype.rfAnalogWrite= function (receiver, pin, val) {
  var msg = '03' + this.normalizePin(pin) + this.normalizeVal(val);
  this.sendMsg(receiver, msg);
}


module.exports = VW;
