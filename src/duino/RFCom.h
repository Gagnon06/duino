/*
 * RFCom.h
 *
 *  Created on: 2015-03-02
 *      Author: Gagnon
 */

#ifndef RFCOM_H_
#define RFCOM_H_
#include "VirtualWire.h"
#include "typesDefine.h"

typedef struct RfMessage {
	uint32_t id = 0;
	int baseId = 0;
	int emitId = 0;
	int recvId = 0;
	MsgType type = (MsgType)0;
	int param = 0;
	double value = 0;
	bool valid = false;
};

class RFCom {
public:
	RFCom(unsigned const int rx_pin, unsigned const int tx_pin, unsigned const int base, unsigned const int recv);
	virtual ~RFCom();

	void init();
	int getBase();
	int getRecv();
	RfMessage receiveMessage();
	void sendMessage(RfMessage, int);

private:
	unsigned int m_rx_pin;
	unsigned int m_tx_pin;
	unsigned int m_baseId;
	unsigned int m_id;
};

#endif /* RFCOM_H_ */
