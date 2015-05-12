/*
 * RFCom.cpp
 *
 *  Created on: 2015-03-02
 *      Author: Gagnon
 *      Description: Class that handle RF com with VirtualWire
 *      library and the custom protocol for command
 *
 */

#include "RFCom.h"

RFCom::RFCom(unsigned const int rx_pin, unsigned const int tx_pin, unsigned const int baseId, unsigned const int id) {
	m_rx_pin = rx_pin;
	m_tx_pin = tx_pin;
	m_baseId = baseId;
	m_id = id;
}

RFCom::~RFCom() {
	// TODO Auto-generated destructor stub
}

//Init the VirtualWire library
void RFCom::init() {
	vw_setup(2000);
	if(m_tx_pin != 0)
		vw_set_tx_pin(m_tx_pin);
	if(m_rx_pin != 0) {
		vw_set_rx_pin(m_rx_pin);
		vw_rx_start();
	}
}

/**
 * Receive a message and return it in the RfMessage form
 */
RfMessage RFCom::receiveMessage() {
	RfMessage msg;
	uint8_t msgLen = sizeof(msg);

	if(vw_get_message((uint8_t *)(&msg), &msgLen)) {
	  if(msg.baseId == m_baseId && msg.recvId == m_id) {
		  msg.valid = true;
	  } else {
		  msg.valid = false;
	  }
	  return msg;

	} else {
		msg.valid = false;
		return msg;
	}
}

/**
 * Send message
 */
void RFCom::sendMessage(RfMessage msg, int delayms = 200) {
	msg.id = random(0, 2000000000);
	msg.baseId = m_baseId;
	msg.emitId = m_id;
	vw_send((uint8_t *)(&msg), sizeof(msg));
	vw_wait_tx();
	delay(delayms);
}


/*
 * Get all the important values
 */
int RFCom::getBase() { return m_baseId; }
int RFCom::getRecv() { return m_id; }
