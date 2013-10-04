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

#include "sunset_csma_aloha.h"
#include <sunset_trace.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_Csma_AlohaClass : public TclClass 
{
public:
	Sunset_Csma_AlohaClass() : TclClass("Module/MMac/Sunset_Csma_Aloha") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Csma_Aloha());	
	}
	
} class_Sunset_Csma_Aloha;


Sunset_Csma_Aloha::Sunset_Csma_Aloha() : mhBackoff_(this) 
{
	DATA_SIZE = 0;
	longRetryLimit = 0;
	
	// Get variables initialization from the Tcl script
	bind("longRetryLimit", &longRetryLimit);
	bind("DATA_SIZE", &DATA_SIZE);
	
	DATA_SIZE += MAC_HDR_SIZE;
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Csma_Aloha::CREATED hdr %d data %d", MAC_HDR_SIZE, DATA_SIZE);
}

/*!
 * 	@brief The transmit() function sends a packet to the lower layer and starts the timer for completing the transmission.
 *	@param p The packet to be sent.
 */

void Sunset_Csma_Aloha::transmit(Packet *p) 
{
	// if another transmission is on going return
	
	if (tx_active_) { 
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Csma_Aloha::transmit but tx is already active ERROR");
		
		return;  //TODO: it's just to avoid exiting the simulation
		exit(8);
	}
	
	tx_active_ = 1;
	
	txAction(p, SUNSET_MAC_TX_ACTION_OK);
	
	Mac2PhyStartTx(p->copy()); 
}


/*!
 * 	@brief The rx_resume() function is invoked when the node has finished to receive packets and it sets the MAC state to IDLE.
 */
void Sunset_Csma_Aloha::rx_resume() 
{
	assert(pktRx_ == 0);
	setRxState(MAC_IDLE);
}


/*!
 *  @brief The backoffHandler() function is invoked when the backoff timer expires to check if a data packet is waiting to be transmitted.
 */

void Sunset_Csma_Aloha::backoffHandler() 
{
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Csma_Aloha::backoffHandler pktTx_ %p ", pktTx_);
	
	if(pktTx_ && check_pktTx() != 0) {
		
		return;
	}
}


double Sunset_Csma_Aloha::getBackoffTime()
{
	double time = Random::uniform(MAX(getTxRetry(), 1) * (getMinBackoff() + macTiming->txtime(macTiming->getPktSize(DATA_SIZE+getMacHdrSize()))));
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Csma_Aloha::getBackoffTime computed time %f", time);
	
	return time;
}

/*!
 * 	@brief The txDone() function is invoked when a packet transmission has been correctly completed.
 * 	@param p The packet transmitted.
 */

void Sunset_Csma_Aloha::txDone(Packet* p) 
{
	if (tx_active_ == 0) {  // if a transmission is not currently on going return
		
		Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());

		return;
	}
	
	tx_active_ = 0;
	
	int removePkt = 0;

	switch(tx_state_) {
			
		case MAC_SEND:
			
			Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Csma_Aloha::txDone DATA TRANSMITTED %p", pktTx_);
			
			if (p != pktTx_ && p != 0) {
		
				removePkt = 1;
			
			}
			
			if(pktTx_ != 0) {
				
				Packet* aux = pktTx_->copy();
				
				txAction(pktTx_, SUNSET_MAC_TX_ACTION_DONE);
				
				Sunset_Utilities::erasePkt(pktTx_, getModuleAddress()); 
				
				pktTx_ = 0;
				resetTxRetry();
				
				if (mhBackoff_.busy()) {
					
					mhBackoff_.stop();
				}
				
				setTxState(MAC_IDLE);
				
				if (aux != 0) {
					
					Mac2RtgPktTransmitted(aux);
				
				}

				anotherTransmission();
			}
			
			break;
			
		default:
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Csma_Aloha::txDone DATA TRANSMITTED tx_state_ ERROR %d", tx_state_);

	}	
	
	if (p != 0 && removePkt) {
	
		Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());	
		
	}
}

/*!
 * 	@brief The txAborted() function is called when a packet transmission has been aborted because of a timeout or an error message received from the modem. All the status and temporary data structures are cleared from the information regarding the aborted transmission.
 *  	@param p The packet aborted.
 */

void Sunset_Csma_Aloha::txAborted(Packet* p) 
{
	if (tx_active_ == 0) {

		if (p != 0) {
				
			Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
			
		}

		return;
	}
	
	tx_active_ = 0;
	
	int removePkt = 0;
	
	switch(tx_state_) {
		
		case MAC_SEND:
		
			Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Csma_Aloha::txAborted DATA TRANSMISSION ABORTED pktTx == 0 (%d)", pktTx_ == 0);
			
			setTxState(MAC_IDLE);
			
			if (p != 0 && p != pktTx_) {
			
				removePkt = 1;
				
			}
			
			if(pktTx_ != 0) {
				
				txAction(pktTx_, SUNSET_MAC_TX_ACTION_ABORTED);
				
				RetransmitDATA();
			}
			
			break;
			
		default:
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Csma_Aloha::txAborted DATA TX ABORTED tx_state_ ERROR %d", tx_state_);
	}	

	if (p != 0 && removePkt) {
	
		Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
	
	}
}


/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Csma_Aloha::command(int argc, const char*const* argv) 
{
	return Sunset_Mac::command(argc, argv);
}

/*!
 * 	@brief The check_pktTx() function transmits a DATA packet (after checking if the transmission can actually occurs).
 *	@retval 0 All is done.
 *	@retval -1 An error occured.
 */

int Sunset_Csma_Aloha::check_pktTx() {
	
	struct hdr_Sunset_Mac *mh;
	double timeout;
	int dst, src;
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Csma_Aloha::check_pktTx");
	
	if(mhBackoff_.busy() != 0) {
		
		return -1;
	}
	
	if(pktTx_ == 0) {
		
		return -1;
	}
	
	mh = HDR_SUNSET_MAC(pktTx_);
	
	switch(mh->dh_fc.fc_subtype) {

		case SUNSET_MAC_Subtype_Data:
		
			if(!is_idle() || tx_active_) {
			
				Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Csma_Aloha::check_pktTx channel is busy backoff");
				
				txAction(pktTx_, SUNSET_MAC_TX_ACTION_BUSY);
				
				RetransmitDATA();
				
				return 0;
			}
			
			setTxState(MAC_SEND);
			
			break;
			
		default:
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Csma_Aloha::check_pktTx check_pktTx:Invalid MAC Control subtype ERROR");
			
			return -1;
	}

	dst = (mh->dst);
	src = (mh->src);
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Csma_Aloha::check_pktTx send DATA to %d timeout at %f", dst, NOW + timeout);
	
	transmit(pktTx_);
	
	return 0;
}

/*!
 * 	@brief The RetransmitDATA() function is invoked when a DATA packet must be retransmitted.
 */

void Sunset_Csma_Aloha::RetransmitDATA() 
{
	if (pktTx_ == 0 || mhBackoff_.busy()) {
		return;
	}
	
	setTxState(MAC_IDLE);
	struct hdr_Sunset_Mac *dh = HDR_SUNSET_MAC(pktTx_);
	int dst, src;
	
	dst = (dh->dst);
	src = (dh->src);
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Csma_Aloha::RetransmitDATA to %d retry %d broadcast %d", dst, tx_retry, getBroadcastAddress());
	
	tx_retry += 1;
	
	if(tx_retry >= getLongRetryLimit()) {
		
		hdr_cmn *ch = HDR_CMN(pktTx_);
		
		/* tell the callback the send operation failed before discarding the packet */
		
		Packet* aux = pktTx_->copy();
		
		txAction(pktTx_, SUNSET_MAC_ACTION_PKT_DISCARDED);
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Csma_Aloha::RetransmitDATA DISCARD too many retransmissions");
		
		discard(pktTx_);
		
		pktTx_ = 0;
		resetTxRetry();
		
		setTxState(MAC_IDLE);
	
		if (aux != 0) {
			
			Mac2RtgPktDiscarded(aux);
		
		}
		
		anotherTransmission();
		
		return;
		
	} else {
		
		struct hdr_Sunset_Mac *dh;
		double time = getBackoffTime();
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Csma_Aloha::RetransmitDATA start retransmission process");
		
		dh = HDR_SUNSET_MAC(pktTx_);
		dh->dh_fc.fc_retry = 1;
		
		if ( mhBackoff_.busy() ) {
		
			mhBackoff_.stop();
		}
		
		mhBackoff_.start(time);
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Csma_Aloha::RetransmitDATA backoff Time %f", time);
		
		if (Sunset_Statistics::use_stat() && stat != NULL) {
			
			stat->logStatInfo(SUNSET_STAT_MAC_BACKOFF, getModuleAddress(), pktTx_, HDR_CMN(pktTx_)->timestamp(), "%f\n", time);
		}
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Csma_Aloha::RetransmitDATA to %d retry %d time %f", dst, tx_retry, NOW + mhBackoff_.expire());	
}

/*!
 * 	@brief The send() function associates a packet to the packet pointer to be transmitted.
 *	@param p The packet to be transmitted.
 */

void Sunset_Csma_Aloha::send(Packet *p) 
{
	if (pktTx_ != 0) {
		
		return;
	}
	
	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_MAC_NEW, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
	}
	
	pktTx_ = p;
	
	if(mhBackoff_.busy() == 0) {
	
		if(is_idle()) {
			
			backoffHandler();
			
		} else {
			/*
			 * If the medium is NOT IDLE, then we start
			 * the backoff timer.
			 */
			mhBackoff_.start(getBackoffTime());
		}
	}
	
	
	if (mhBackoff_.busy()) {
	
		if (Sunset_Statistics::use_stat() && stat != NULL) {
			
			stat->logStatInfo(SUNSET_STAT_MAC_BACKOFF, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "%f", mhBackoff_.expire());	
		}
	}

	return;
}

/*!
 * 	@brief The recvFromUpperLayers() function handles a packet coming from upper layers.
 *	@param p The received packet.
 */

void Sunset_Csma_Aloha::recvFromUpperLayers(Packet *p) 
{
	struct hdr_cmn *hdr = HDR_CMN(p);

	checkQueue(p);
}

/*!
 * 	@brief The recvPkt() function is invoked when a new packet is received.
 */

void Sunset_Csma_Aloha::recvPkt() 
{	
  	assert(pktRx_);
	
	hdr_cmn *ch = HDR_CMN(pktRx_);
	struct hdr_Sunset_Mac *mh = HDR_SUNSET_MAC(pktRx_);
	int dst = (mh->dst);
	int src = mh->src; 
	u_int8_t  type = mh->dh_fc.fc_type;
	u_int8_t  subtype = mh->dh_fc.fc_subtype;
		
	if( src == getModuleAddress() ) {
		
		Sunset_Utilities::erasePkt(pktRx_, getModuleAddress());
		goto done;
	}
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Csma_Aloha::recvPkt %d->%d error %d type %d subType %d", src, dst, ch->error(), type, subtype);
	
	if( ch->error() ) {
		
		if (pktRx_ != 0) {
			
			Sunset_Utilities::erasePkt(pktRx_, getModuleAddress());
			
		}
		
		goto done;
	}
	
	if (pktRx_ != 0) {
		
		rxAction(pktRx_, SUNSET_MAC_RX_ACTION_OK);
	}
	
	setRxState(MAC_IDLE);
	
	switch(type) {
			
		case SUNSET_MAC_Type_Data:
	
			switch(subtype) {
			
				case SUNSET_MAC_Subtype_Data:
				
					recvDATA(pktRx_);
					break;
					
				default:
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Csma_Aloha::recvPkt Invalid MAC Data Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		default:
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Csma_Aloha::recvPkt Invalid MAC Type %x ERROR", subtype);
			
			return;
	}
	
done:
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Csma_Aloha::recv_timer recv completed");
	pktRx_ = 0;
	rx_resume();
	
	return;
}

/*!
 * 	@brief The recvDATA() function is invoked when a new DATA packet is received.
 *	@param[in] p The received DATA packet.
 */

void Sunset_Csma_Aloha::recvDATA(Packet *p) 
{
	struct hdr_Sunset_Mac *dh = HDR_SUNSET_MAC(p);
	int dst, src, size;
	struct hdr_cmn *ch = HDR_CMN(p);
	
	dst = (dh->dst);
	src = (dh->src);
	size = ch->size();
	
	/* Adjust the MAC packet size - ie; strip off the mac header */
	
	ch->size() -= getMacHdrSize();
	ch->num_forwards() += 1;
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Csma_Aloha::recvDATAsize %d from %d to %d  timestamp %f broadcast %d", HDR_CMN(p)->size(), src, dst, HDR_CMN(p)->timestamp(), getBroadcastAddress());
	
	//it is a UNICAST pkt but I am not the destination
	
	if(dst != getBroadcastAddress() && dst != getModuleAddress()) {
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Csma_Aloha::recvDATA size %d from %d to %d  timestamp %f DISCARD", HDR_CMN(p)->size(), src, dst, HDR_CMN(p)->timestamp());
		
		discard(p);
		
		return;
	}
	
	if(dst != getBroadcastAddress()) {
		
		if( tx_state_ != MAC_IDLE || tx_active_) {
			
			discard(p);
			
			return;
		}
	}	
	
	rxAction(p, SUNSET_MAC_RX_ACTION_DONE);
	
	sendUp(p);
}

/*!
 * 	@brief The start() function  is called when the slotted csma module starts and it computes the duration of the slot in a conservative way if it has not been
 *	defined by the user using the tcl script.
 */

void Sunset_Csma_Aloha::start() 
{	
	Sunset_Mac::start();
	
	if (sid != NULL) {
		
		sid_id = sid->register_module(getModuleAddress(), "MAC_CSMA_ALOHA", this);
	}
	
	return;
}

/*!
 * 	@brief The stop() function  is called when the slotted csma module stops.
 */

void Sunset_Csma_Aloha::stop() 
{
	
	Sunset_Mac::stop();
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Csma_Aloha::stop macQueue_->length() = %d pktTx %d pktRx %d pktAck %d tx_active %d tx_state %d rx_state %d backoff %d", macQueue_->length(), pktTx_, pktRx_, tx_active_, tx_state_, rx_state_, mhBackoff_.busy() );
	
	return;
}

/*!
 * 	@brief The anotherTransmission() function checks if a new transmission has to be done.
 */

void Sunset_Csma_Aloha::anotherTransmission() 
{
	if (pktTx_ != 0) {
		
		return;
	}
	
	if (macQueue_ != 0 && macQueue_->length() > 0) {
		
		Packet* p2;
		
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Csma_Aloha::anotherTransmission macQueue_->length() = %d", macQueue_->length());
		
		p2 = macQueue_->deque();
		
		send(p2);
	}
	else {
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Csma_Aloha::anotherTransmission macQueue_->length() = 0");
	}
	
}

/*!
 * 	@brief The Phy2MacTxAborted() function handles the abort-of-PHY-transmission event.
 *	@param p The aborted packet.
 */

void Sunset_Csma_Aloha::Phy2MacTxAborted(const Packet* p) 
{
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Csma_Aloha::Phy2MacTxAborted");
	txAborted((Packet*)p);
	Sunset_Utilities::eraseOnlyPkt((Packet *)p, getModuleAddress());	
}

/*!
 * 	@brief The Phy2MacEndTx() function handles the end-of-PHY-transmission event.
 *	@param p The transmitted packet.
 */

void Sunset_Csma_Aloha::Phy2MacEndTx(const Packet* p) 
{
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Csma_Aloha::Phy2MacEndTx %p %p", p, pktTx_);
	txDone((Packet*)p);
	Sunset_Utilities::eraseOnlyPkt((Packet *)p, getModuleAddress());	
}

/*!
 * 	@brief The Phy2MacStartRx() function handles the detected-start-of-PHY-reception event.
 *	@param p The packet under reception.
 */

void Sunset_Csma_Aloha::Phy2MacStartRx(const Packet* p) 
{
	Sunset_Debug::debugInfo(5, getModuleAddress(),  "Sunset_Csma_Aloha::Phy2MacStartRx");
	setRxState(MAC_RECV);
	
	if (Sunset_Utilities::isSimulation()) {
		
		Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Csma_Aloha::Phy2MacStartRx src %d dst %d",  HDR_SUNSET_MAC(p)->src, HDR_SUNSET_MAC(p)->dst);
	}
}

/*!
 *  @brief The Phy2MacEndRx() function handles the end-of-PHY-reception event.
 *	@param p The received packet.
 */

void Sunset_Csma_Aloha::Phy2MacEndRx(Packet* p) 
{

	if ( p == 0 ) {
		
		setRxState(MAC_IDLE);
		
		return;

	}
	
	hdr_cmn* ch = HDR_CMN(p);
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Csma_Aloha::Phy2MacEndRx size %d error %d src %d dst %d", ch->size(), ch->error(), HDR_SUNSET_MAC(p)->src, HDR_SUNSET_MAC(p)->dst);
	
	if (ch->error()) {
		
		if (ch->size() == 0) {
		
			Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
			
			setRxState(MAC_IDLE);
			
			return;
		}
		
		if (HDR_SUNSET_MAC(p)->dst == getModuleAddress() || HDR_SUNSET_MAC(p)->dst == getBroadcastAddress()) {
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Csma_Aloha::Phy2MacEndRx src %d dst %d TOO MUCH NOISE %d", HDR_SUNSET_MAC(p)->src, HDR_SUNSET_MAC(p)->dst, ch->size());
		}
		
		if (p != 0) {
			
			rxAction(p, SUNSET_MAC_RX_ACTION_ERROR);
		}

		Sunset_Utilities::erasePkt(p, getModuleAddress());
		setRxState(MAC_IDLE);
	}
	else {
		
		if (HDR_SUNSET_MAC(p)->dst == getModuleAddress()  || HDR_SUNSET_MAC(p)->dst == getBroadcastAddress()) {
		
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Csma_Aloha::Phy2MacEndRx src %d dst %d CORRECT", HDR_SUNSET_MAC(p)->src, HDR_SUNSET_MAC(p)->dst);
		}
		
		pktRx_ = p;
		recvPkt();
	}
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Csma_Aloha::Phy2MacEndRx EXIT");
}

/*!
 * 	@brief The notify_info function is called from the information dispatcher when a value the CSMA-Aloha module is registered 
 *  for has been updated. This CSMA-Aloha is currently not registered for any variables, this method can be extend by other application layers if interested in any variables shared with the other modules
 *	@param linfo The list of variables shared with the other modules the CSMA-Aloha module is interested in.
 */

int Sunset_Csma_Aloha::notify_info(list<notified_info> linfo) 
{ 
	Sunset_Debug::debugInfo(-1, getModuleAddress(), " Sunset_Csma_Aloha::notify_info NOTHING TO DO");
	
	return Sunset_Mac::notify_info(linfo); 
} 

/*! @brief Function called when a transmission operation is performed to check if an action has to be taken
 *	@param[in] p   The packet the action is related to.
 *	@param[in] mct The transmission action that has been executed.
 */

void Sunset_Csma_Aloha::txAction(Packet* p, mac_action_type mct) 
{
	
	Sunset_Mac::txAction(p, mct);
	
	if (p == 0) {
		
		return;	
	}
	
	hdr_cmn *ch = HDR_CMN(p);
	hdr_ip *ip = HDR_IP(p);
	struct hdr_Sunset_Mac *mh = HDR_SUNSET_MAC(p);
	int dst = (mh->dst);
	int src = mh->src; 
	u_int8_t  type = mh->dh_fc.fc_type;
	u_int8_t  subtype = mh->dh_fc.fc_subtype;
	string pktType = "";
	string actionType = "";
	
	switch(type) {
		
		case SUNSET_MAC_Type_Control:
		
			switch(subtype) {
			
				case SUNSET_MAC_Subtype_ACK:
					pktType = "ACK";
					break;
				
				default:
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Csma_Aloha::txAction Invalid MAC Control Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		case SUNSET_MAC_Type_Data:
			
			switch(subtype) {
			
				case SUNSET_MAC_Subtype_Data:
				
					pktType = "DATA";
					break;
				
				default:
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Csma_Aloha::txAction Invalid MAC Data Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		default:
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Csma_Aloha::txAction Invalid MAC Type %x ERROR", subtype);
			
			return;
	}
	
	switch (mct) {
		
		case SUNSET_MAC_TX_ACTION_OK:
			
			actionType = "TX";
			
			break;
			
		case SUNSET_MAC_TX_ACTION_DONE:
                        
			actionType = "TX COMPLETED";
			
			break;
			
		case SUNSET_MAC_TX_ACTION_ABORTED:
			
			actionType = "ABORTED";
			
			break;
			
		case SUNSET_MAC_TX_ACTION_BUSY:
			
			actionType = "BUSY";
			
			break;
			
		case SUNSET_MAC_TX_ACTION_ERROR:
			
			actionType = "ERROR";
			
			break;
			
		case SUNSET_MAC_ACTION_PKT_DISCARDED:
			
			actionType = "DISCARDED";
			
			break;
			
		default:
			
			actionType = "ERROR";
			
			break;
	}
	
	Sunset_Trace::print_info("mac - (%f) Node:%d - CSMA_ALOHA %s %s source %d destination %d src %d dst %d\n", NOW, getModuleAddress(), actionType.c_str(), pktType.c_str(), ip->saddr(), ip->daddr(), src, dst);

	return;
}

/*! @brief Function called when a reception operation is performed to check if an action has to be taken
 *	@param[in] p   The packet the action is related to.
 *	@param[in] mct The reception action that has been executed.
 */

void Sunset_Csma_Aloha::rxAction(Packet* p, mac_action_type mct) 
{
	
	Sunset_Mac::rxAction(p, mct);
	
	if (p == 0) {
		
		return;
	}
	
	hdr_cmn *ch = HDR_CMN(p);
	hdr_ip *ip = HDR_IP(p);
	struct hdr_Sunset_Mac *mh = HDR_SUNSET_MAC(p);
	int dst = (mh->dst);
	int src = mh->src; 
	u_int8_t  type = mh->dh_fc.fc_type;
	u_int8_t  subtype = mh->dh_fc.fc_subtype;
	string pktType = "";
	string actionType = "";
	
	switch(type) {
		
		case SUNSET_MAC_Type_Control:
		
			switch(subtype) {
			
				case SUNSET_MAC_Subtype_ACK:
				
					pktType = "ACK";
					break;
				
				default:
				
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Csma_Aloha::rxAction Invalid MAC Control Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		case SUNSET_MAC_Type_Data:
			
			switch(subtype) {
				
				case SUNSET_MAC_Subtype_Data:
				
					pktType = "DATA";
					break;
					
				default:
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Csma_Aloha::rxAction Invalid MAC Data Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		default:
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Csma_Aloha::rxAction Invalid MAC Type %x ERROR", subtype);
			
			return;
	}
	
	switch (mct) {
		
		case SUNSET_MAC_RX_ACTION_OK:
			
			actionType = "RX";
			
			break;
			
		case SUNSET_MAC_RX_ACTION_DONE:
			
			actionType = "RX_DONE";
			
			break;
			
		case SUNSET_MAC_RX_ACTION_ERROR:
			
			actionType = "RX_ERROR";
			
			break;
			
		default:
			
			actionType = "RX_ERROR";
			break;
	}
	
	
	Sunset_Trace::print_info("mac - (%f) Node:%d - CSMA_ALOHA %s %s source %d destination %d src %d dst %d\n", NOW, getModuleAddress(), actionType.c_str(), pktType.c_str(), ip->saddr(), ip->daddr(), src, dst);
	
}
