//Enum for all the msg types

enum MsgType {
	SM = 0, //set mode
	DW, //digitalWrite
	DR, //digitalRead
	AW, //analogWrite
	AR, //analogRead

	RF = 93,
	RCDecimal,
	IR,
	RCTristate,
	Ping,
	Servo,
	Debug
};
