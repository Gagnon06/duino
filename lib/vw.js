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
        var m = message.slice(0, -1).split('::'),
            err = null,
            pin, type, data;

        if (!m.length) {
          return;
        }

        pin = m[0];
        type = m[1];
        data = m.length === 3 ? m[2] : null;

    }.bind(this));
}

/*
 * Send vw message
 */
VW.prototype.sendMsg = function (receiver, msg) {
  var msg = '9300' + this.boad.rfId + receiver + msg;
  this.board.write(msg);	
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