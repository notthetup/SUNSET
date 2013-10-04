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



#ifndef __Sunset_Csma_Aloha_h__
#define __Sunset_Csma_Aloha_h__

#include "sunset_csma_aloha_timers.h"
#include <sunset_mac.h>
#include <sunset_timing.h>

#define CSMA_INFO_TIME 30

/*! @brief This class implements the slotted CSMA Aloha protocol. It extends the Sunset_Mac class. 
 * @see class Sunset_Mac
 */

class Sunset_Csma_Aloha : public Sunset_Mac {
	
	friend class Sunset_Csma_Aloha_Backoff_Timer;   // Backoff timer
	
public:
	
	Sunset_Csma_Aloha();
	
	/*! @brief Function called when packet p has been correctly transmitted. */
	virtual void txDone(Packet* p);
	
	/*! @brief Function called when an error occurs during packet p transmission. */
	virtual void txAborted(Packet* p);
	
protected:
	
	/*! @brief Function called when the backoff timer exprires. */
	virtual void backoffHandler(void);
	
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
	
	/*! @brief Function called to control if a data packet can be transmitted. */
	virtual int check_pktTx();
	
	/*! @brief Function called to control if a data packet has to be retransmitted. */
	virtual void RetransmitDATA();
	
	/*! @brief Function called when receiving a DATA packet. */
	virtual void recvDATA(Packet *p);
	
	void rx_resume(void);
	
	/*! @brief Function called when a packet is received from the lower layer. */
	virtual void recvPkt ();
	
	/*! @brief Function called if there are packets waiting to be transmitted. */
	virtual void anotherTransmission();
	
	/*! @brief Function called when a packet p has to be transmitted. */
	virtual void transmit(Packet *p);
	
	virtual double getBackoffTime();
	
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
	
	Sunset_Csma_Aloha_Backoff_Timer mhBackoff_;     // backoff timer
	
	/* ============================================================
	 Internal MAC State
	 ============================================================ */
	
	int DATA_SIZE;      /*!< @brief Average length assumed for a data packet - used for backoff computation. */
	
	int longRetryLimit; /*!< @brief Maximun number of retransmission for a data packet. */
	
	void setLongRetryLimit(int lRtx) { longRetryLimit = lRtx; }
	int getLongRetryLimit() { return longRetryLimit; }
	
	/*! @brief Function called to compute the minimal value for backoff computation. */
	virtual inline double getMinBackoff() 
	{ 
		return(2 * macTiming->getMaxPropagationDelay() + macTiming->getDeviceDelay() + macTiming->getModemDelay());
	}
	
	/*! @brief Function called  by the information dispatcher to notify to the agent if a value is changed among the ones the agent has registered itsef for. */
	virtual int notify_info(list<notified_info> linfo);
	
};

#endif
