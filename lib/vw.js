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
                type, emitId, itemId, data;

            if (!m.length) {
              return;
            }

            type = m[0];
            if (type === '40') {
                emitId = m[1];
                itemId = m[2];
                data = m[3];
                this.board.emit('sensor_data', [emitId, itemId, data]);
            }
        }
    }.bind(this));
}

/*
 * Send vw message
 */
VW.prototype.sendMsg = function (receiver, type, body) {
  var message = '93:' + receiver + ':' + type + ':' + body;
  this.board.write(message);
}



module.exports = VW;
