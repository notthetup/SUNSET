/* SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
 *
 * Copyright (C) 2012 Regents of UWSN Group of SENSES Lab <http://reti.dsi.uniroma1.it/SENSES_lab/>
 *
 * Author: Roberto Petroccia - petroccia@di.uniroma1.it
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License as published
 * at http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANATBILITY or FITNESS FOR A PARTICULAR PURPOSE. See the Creative Commons
 * Attribution-NonCommercial-ShareAlike 3.0 Unported License for more details.
 *
 * You should have received a copy of the Creative Commons Attribution-NonCommercial-ShareAlike 3.0 Unported License
 * along with this program. If not, see <http://creativecommons.org/licenses/by-nc-sa/3.0/legalcode>.
 */


#ifndef __Sunset_SlottedCsma_h__
#define __Sunset_SlottedCsma_h__

#include "sunset_mac_pkt.h"
#include "sunset_slotted_csma_timers.h"

#include "sunset_mac.h"

/*! @brief This class implements the slotted CSMA MAC protocol. It extends the Sunset_Mac class. 
 * @see class Sunset_Mac
 */

class Sunset_SlottedCsma : public Sunset_Mac 
{
	
	friend class Sunset_SlottedCsma_Defer_Timer;    // Defer timer
 	friend class Sunset_SlottedCsma_Slot_Timer;     // Slot timer
	friend class Sunset_SlottedCsma_Tx_Timer;       // Tx timer
	
public:
	
	Sunset_SlottedCsma();
	
	/*! @brief Function called when packet p has been correctly transmitted. */
	virtual void txDone(Packet* p);
	
	/*! @brief Function called when an error occurs during packet p transmission. */
	virtual void txAborted(Packet* p);
	
	
protected:
	
	/*! @brief Function handling the tx timer expiration. */
	virtual void sendHandler(void);
	
	/*! @brief Function called when the slot timer expires. */
	virtual void slotHandler(void);
	
	/*! @brief Function called when the tx defer expires. */
	virtual void deferHandler(void);	
	
protected:
	
	virtual int command(int argc, const char*const* argv);
	
	virtual void start();	
	virtual void stop();	
	
	/*! @brief Function called when a packet is received from the upper layer. */
	virtual void recvFromUpperLayers(Packet* p);
	
	// functions used for the interaction with the lower layer
	//--------------------------------------
	virtual void Phy2MacEndTx(const Packet* p);
	virtual void Phy2MacTxAborted(const Packet* p);	
	virtual void Phy2MacStartRx(const Packet* p);
	virtual void Phy2MacEndRx(Packet* p);
	//--------------------------------------
	
	/*! @brief Function called when the node starts the operation to transmit packet p. */
	virtual void send(Packet* p);
	
	/*! @brief Function called to compute the timeuout for packet p. */
	virtual double getTimeout(Packet* p);
	
	/*! @brief Function called when the tx timer expires. */
	virtual void send_timer(void);
	
	/*! @brief Function called to control if an ACK packet can be transmitted. */
	virtual int check_pktACK();
	
	/*! @brief Function called to control if a data packet can be transmitted. */
	virtual int check_pktTx();
	
	/*! @brief Function called to create an ACK packet for dst node. */
	virtual void createACK(int dst);
	
	/*! @brief Function called to control if a data packet has to be retransmitted. */
	virtual void RetransmitDATA();
	
	/*! @brief Function called when receiving an ACK packet. */
	virtual void recvACK(Packet *p);
	
	/*! @brief Function called when receiving a DATA packet. */
	virtual void recvDATA(Packet *p);
	
	void rx_resume(void);	
	
	/*! @brief Function called to collect the propagation delay to node dst, which is upated according to Rount Trip Time for DATA-ACK. */
	virtual double getPropagationDelay(int dst);
	
	/*! @brief Function called when a packet is received from the lower layer. */
	virtual void recvPkt ();
	
	/*! @brief Function called if there are packets waiting to be transmitted. */
	virtual void anotherTransmission();
	
	/*! @brief Function called when a packet p has to be transmitted. */
	virtual void transmit(Packet *p);
	
	/*! @brief Function called to collect the time duration of each slot. */
	virtual double getSlotTime();
	
	/*! @brief Function called to compute the number of slots to wait when the node has to back off. */
	virtual int getBackoffSlotCount();
	
    	/*! @brief Exponential computation of the retry window size. */
	inline u_int32_t getTxRetry() 
	{ 
		return (u_int32_t)pow(2.0, (double)tx_retry);
	}
	
	virtual inline void resetTxRetry() { tx_retry = 0; }
	
	/*! @brief Function called when a transmission operation is perfomed to check if an action has to be taken*/
	virtual void txAction(Packet* p, mac_action_type mct);
	
	/*! @brief Function called when a reception operation is perfomed to check if an action has to be taken*/
	virtual void rxAction(Packet* p, mac_action_type mct);
	
protected:
	
	Sunset_SlottedCsma_Tx_Timer 	mhSend_;		/*!< @brief outgoing packets */
	Sunset_SlottedCsma_Defer_Timer	mhDefer_;		/*!< @brief defer timer */
 	Sunset_SlottedCsma_Slot_Timer	mhSlot_;		/*!< @brief slot timer */
	
	Packet *pktACK_;	/*!< @brief outgoing ACK packet */
	
	map<int, double> roundTripTime;     /*!< @brief RTT to other nodes in meters */
	map<int, double> distanceToNode;	/*!< @brief distance to other nodes in meters */
	
	map<int, double> timeSentToNode;	/*!< @brief time a node A sends a packet to node B to calculate the distance between A eand B  */
	
	int use_ack;    //  if 1 acknowledgment packets are used, 0 otherwise
	
	int ACK_SIZE;   // size for the ACK packet to be used in simulation
	
	void setAckSize(int ackS) { ACK_SIZE = ackS; }
	int getACK_Size() { return macTiming->getPktSize(ACK_SIZE); }
	
	int DATA_SIZE;      // average length assumed for a data packet - used for backoff computation
	
	int getDATA_Size() { return macTiming->getPktSize(DATA_SIZE); }
	
	int longRetryLimit; //maximun number of retransmission for a data packet
	
	void setLongRetryLimit(int lRtx) { longRetryLimit = lRtx; }
	int getLongRetryLimit() { return longRetryLimit; }
	
	
	int backoffSlotCount;   // contains the current number of slots to wait
	
	double slotTime_;       // contains the duration of each slot
	
	/*! @brief Function called  by the information dispatcher to notify to the agent if a value is changed among the ones the agent has registered itsef for. */
	virtual int notify_info(list<notified_info> linfo);
	
};

#endif
