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

#include "sunset_slotted_csma.h"
#include<iostream>
#include <sunset_trace.h>
#include <sunset_common_pkt.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_SlottedCsmaClass : public TclClass 
{
public:
	Sunset_SlottedCsmaClass() : TclClass("Module/MMac/Sunset_Slotted_Csma") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_SlottedCsma());	
	}
	
} class_Sunset_SlottedCsma;


Sunset_SlottedCsma::Sunset_SlottedCsma() :  mhSend_(this), mhDefer_(this), mhSlot_(this) 
{
	
	use_ack = 1;
	pktACK_ = 0;	
	
	ACK_SIZE = 0;
	DATA_SIZE = 0;
	longRetryLimit = 0;
	
	slotTime_ = 0.0;
	
	// Get variables initialization from the Tcl script
	bind("use_ack_", &use_ack);
	bind("longRetryLimit", &longRetryLimit);
	bind("ACK_SIZE", &ACK_SIZE);
	bind("DATA_SIZE", &DATA_SIZE);
	bind("slotTime_", &slotTime_);
	
	distanceToNode.clear();
	timeSentToNode.clear();
	roundTripTime.clear();
	backoffSlotCount = 0;
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_SlottedCsma::CREATED");
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_SlottedCsma::command(int argc, const char*const* argv) 
{
	return Sunset_Mac::command(argc, argv);
}

/*!
 * 	@brief The transmit() function sends a packet to the lower layer and starts the timer to check if the transmission has been correctly completed.
 *	@param p The packet to be sent.
 */

void Sunset_SlottedCsma::transmit(Packet *p) 
{
	double newTimeout = 0.0;
	int pkt_size = 0;
	
	if (tx_active_) { // if another transmission is on going return
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::transmit but tx already active ERROR");
		return;
	}
	
	tx_active_ = 1;
	
	txAction(p, SUNSET_MAC_TX_ACTION_OK);
	
	Mac2PhyStartTx(p->copy()); 
	
	if (mhSend_.busy()) {   // if tx timer is running stop it
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::transmit timeout mhSend busy ERROR");
		mhSend_.stop();
	}
	
	pkt_size = macTiming->getPktSize(p);
	sunset_rateType rate = TIMING_CTRL_RATE;
	
	if (HDR_SUNSET_MAC(p)->dh_fc.fc_type == SUNSET_MAC_Type_Data) {
		
		rate = TIMING_DATA_RATE;
	}
	
	// compute the time to wait for a confirmation on the transmission (txDone)
	newTimeout = macTiming->overheadTime(pkt_size) 
	+ macTiming->getMaxPropagationDelay()
	+ macTiming->txtime(pkt_size, rate);
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::transmit timeout %f size %d", newTimeout, HDR_CMN(p)->size());
	
	mhSend_.start(newTimeout);
}


/*!
 * 	@brief The rx_resume() function is invoked when the node has finished to receive packets and it sets the MAC state to IDLE.
 */

void Sunset_SlottedCsma::rx_resume() 
{
	assert(pktRx_ == 0);
	setRxState(MAC_IDLE);
}

/*!
 *  @brief The deferHandler() function is invoked when the defer timer  expires to check if an ACK packet is waiting to be transmitted.
 */

void Sunset_SlottedCsma::deferHandler() 
{
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_SlottedCsma::deferHandler pktACK_ %d pktTx_ %d ", pktACK_, pktTx_);
	
	if (!use_ack) {
		
		return;
	}
	
	if (use_ack && check_pktACK() != 0) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::deferHandler but no ACK pkt ERROR");
		return;
	}
}

/*!
 *  @brief The getSlotTime() function returns the slot time.
 */

double Sunset_SlottedCsma::getSlotTime() 
{
	return slotTime_;
}

/*!
 *  @brief The getBackoffSlotCount() function returns the backoff slot counter.
 */

int Sunset_SlottedCsma::getBackoffSlotCount()
{
	int count = 0;
	
	count = Random::uniform(MAX(getTxRetry(), 1));
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_SlottedCsma::getBackoffSlotCount computed count %d", count);
	
	return count;
}

/*!
 * 	@brief The txDone() function is invoked when a packet transmission has been correctly completed.
 */

void Sunset_SlottedCsma::txDone(Packet* p) 
{
	
	if (tx_active_ == 0) { // if a transmission is not currently on going return
		
		Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
		
		return;
	}
	
	if (mhSend_.busy()) {   // stop the timer waiting for the confirmation on the correct transmission
		
		mhSend_.stop();
	}
	
	tx_active_ = 0;
	
	switch(tx_state_) {
			
		// An ACK packet has been correctly transmitted
			
		case MAC_ACK:
			
			Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::txDone ACK transmitted");
			
			if (!use_ack) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::txDone but we are not using ACK pkt ERROR");
				return;	
			}
			
			// reset ACK variables
			if (pktACK_ != 0) {
				
				txAction(pktACK_, SUNSET_MAC_TX_ACTION_DONE);
				
				Sunset_Utilities::erasePkt(pktACK_, getModuleAddress()); 
				
				pktACK_ = 0;
			}
			
			pktACK_ = 0;
			
			//reset tx status
			setTxState(MAC_IDLE);
			
			Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
			
			// check if other data packets are waiting to be transmitted 
			anotherTransmission();
			
			return;
			
		case MAC_SEND:
			
			// A DATA packet has been correctly transmitted
			
			Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::txDone DATA transmitted %p", pktTx_);
			
			if (pktTx_ != 0) {
				
				txAction(pktTx_, SUNSET_MAC_TX_ACTION_DONE);
				
				timeSentToNode[HDR_SUNSET_MAC(pktTx_)->dst] = NOW;  // set tx time to node dst used for RTT computation
				
				// check if no ACK is expected
				if (HDR_SUNSET_MAC(pktTx_)->dst == getBroadcastAddress() || !use_ack) {
					
					Packet * aux = pktTx_->copy();
					
					// reset tx variables
					Sunset_Utilities::erasePkt(pktTx_, getModuleAddress()); 
					
					pktTx_ = 0;
					resetTxRetry();
					
					//reset tx status
					setTxState(MAC_IDLE);
					
					// check if other data packets are waiting to be transmitted 
					if (aux != 0) {
						
						Mac2RtgPktTransmitted(aux);

						Sunset_Utilities::eraseOnlyPkt(aux, getModuleAddress());
						
					}
					
					anotherTransmission();
				}
				else {
					// Waiting to receive the ACK packet
					
					//compute the timeout for the ACK packet
					double timeout = getTimeout(pktTx_);
					
					Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::txDone timeout at %f pktSize %d", NOW + timeout, HDR_CMN(pktTx_)->size());
					
					// start the timer
					mhSend_.start(timeout);	
				}
			}
			
			break;
			
		default:
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::txDone tx_state_ ERROR %d", tx_state_);
	}	
	
	Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());

	return;
}

/*!
 * 	@brief The txAborted() function is called when a packet transmission has been aborted because of a timeout or an 
 *	error message received from the modem. All the status and temporary data structures are cleared from the information 
 *	regarding the aborted transmission.
 */

void Sunset_SlottedCsma::txAborted(Packet* p) {
	
	if (tx_active_ == 0) { // if a transmission is not currently on going return
		
		Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
		
		return;
	}
	
	if (mhSend_.busy()) { // stop the timer waiting for the confirmation on the correct transmission
		
		mhSend_.stop();
	}
	
	tx_active_ = 0;
	
	switch(tx_state_) {
			
		case MAC_ACK:
			
			// ERROR on an ACK packet transmission
			
			Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::txAborted ACK tranmission ABORTED");
			
			if (!use_ack) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::txAborted but we are not using ACK pkt ERROR");
				
				Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
				
				return;	
			}
			
			//reset ACK variables
			if (pktACK_ != 0) {
				
				txAction(pktACK_, SUNSET_MAC_TX_ACTION_ABORTED);
				
				Sunset_Utilities::erasePkt(pktACK_, getModuleAddress()); 
				
				pktACK_ = 0;
			}
			
			pktACK_ = 0;
			
			//reset tx status
			setTxState(MAC_IDLE);
			
			// check if other data packets are waiting to be transmitted 
			anotherTransmission();
			
			break;
			
		case MAC_SEND:
			
			// ERROR on an DATA packet transmission
			
			Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::txAborted DATA transmission ABORTED pktTx == 0 (%d)", pktTx_ == 0);
			
			setTxState(MAC_IDLE);
			
			if (pktTx_ != 0) {
				
				txAction(pktTx_, SUNSET_MAC_TX_ACTION_ABORTED);
                                
				// retransmit the DATA packet 
				RetransmitDATA();
			}
			
			break;
			
		default:
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::txAborted tx_state_ ERROR %d", tx_state_);
	}
	
	Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());

	return;	
}

/*!
 * 	@brief The send_timer() function is invoked when the tx timeout (mhSend) expires while waiting for correct transmission or ACK packet reception.
 */

void Sunset_SlottedCsma::send_timer() 
{
	Sunset_Debug::debugInfo(4, getModuleAddress(), "unset_SlottedCsma::send_timer  pktACK_ %d pktTx_ %d tx_state %d", pktACK_, pktTx_, tx_state_);
	
	if (tx_active_) {   // if the transmission is active no txDone message has been received
		
		txAborted((Packet*) 0); // There has been an ERROR on the packet transmission
		return;
	}
	
	// Otherwise tx has been correctly completed
	
	switch(tx_state_) {
			
			// No ACK has been received
			
		case MAC_SEND:
			
			if(pktTx_ != 0 && HDR_SUNSET_MAC(pktTx_)->dst != getBroadcastAddress() && use_ack) {
				
				txAction (pktTx_, SUNSET_MAC_TX_ACTION_ERROR);
				
				// retransmit the DATA packet 
				
				RetransmitDATA();
			}	
			
			break;
			
		case MAC_ACK:
			
			/* Sent an ACK, we have to just resume the transmission status. */
			
			if (!use_ack) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::send_timer but we are not using ACK pkt ERROR");
				break;	
			}
			
			break;
			
		case MAC_IDLE:
			
			break;
			
		default:
			assert(0);
	}
	
	setTxState(MAC_IDLE);
}



/*!
 * 	@brief The sendHandler() function is invoked when the backoff timeout (mhSend) expires.
 */

void Sunset_SlottedCsma::sendHandler() 
{
	send_timer();
}

/*!
 * 	@brief The getTimeout() function returns the timeout associated with the type of the packet p.
 */

double Sunset_SlottedCsma::getTimeout(Packet* p) 
{
	struct hdr_Sunset_Mac *mh;
	double timeout = 0.0;
	int dst = 0, src = 0;
	
	mh = HDR_SUNSET_MAC(p);
	
	dst = (mh->dst);
	src = (mh->src);
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_SlottedCsma::getTimeout src %d dst %d type %d subType %d", src, dst, mh->dh_fc.fc_type, mh->dh_fc.fc_subtype);
	
	if (mh->dh_fc.fc_type == SUNSET_MAC_Type_Control) {
		
		switch(mh->dh_fc.fc_subtype) {
				
			case SUNSET_MAC_Subtype_ACK:
				
				/* if the packet type is ACK ... */	
				
				if (!use_ack) {
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::getTimeout but we are not using ACK pkt ERROR");
					
					return 10.0;
				}
				
				// There is no need to wait after ACK packet transmission
				timeout = 0.0; 
				
				Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::getTimeout send ACK to %d timeout at %f", dst, NOW + timeout);
				
				return timeout;
				
			default:
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::getTimeout SubTypeCtrl ERROR %d", mh->dh_fc.fc_subtype);
				
				return 10.0;
		}
	}
	else if (mh->dh_fc.fc_type == SUNSET_MAC_Type_Data) {
		
		/* if the packet type is DATA ... */
		
		if (mh->dh_fc.fc_subtype == SUNSET_MAC_Subtype_Data) {
			
			int pkt_ack_size = macTiming->getPktSize(getACK_Size());
			if(mh->dst != getBroadcastAddress() && use_ack) {
				
				// Compute the timeout to  wait for an ACK packet considering also additional delays related to modem and device operations
				
				timeout = getPropagationDelay(mh->dst) 
				+ macTiming->txtime(pkt_ack_size)
				+ macTiming->getDeviceDelay()
				+ macTiming->getModemDelay()
				+ macTiming->transfertTime(pkt_ack_size)
				+ getPropagationDelay(mh->dst) 
				+ EPSILON_DELAY;
			}
			else {
				// There is no need to wait after DATA packet transmission in broadcast or with no ACK ... wait EPSILON
				timeout = 0.001;
			}
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_SlottedCsma::getTimeout propagation %f time %f dDelay %f mDelay %f tTime %f maxPropDelay %f ACK_SIZE %d DATA_SIZE %d", macTiming->getMaxPropagationDelay(), macTiming->txtime(pkt_ack_size), macTiming->getDeviceDelay(), macTiming->getModemDelay(), macTiming->transfertTime(pkt_ack_size), getPropagationDelay(mh->dst), pkt_ack_size, macTiming->getPktSize(getDATA_Size()));
		}
		else {
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::getTimeout SubTypeData ERROR %d", mh->dh_fc.fc_subtype);
			
			return 10.0;
		}
	}
	else {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::getTimeout Type ERROR %d", mh->dh_fc.fc_type);
		
		return 10.0;
	}
	
	// if timeout is longer than slot time, the time needed for the communication requires more than one slot and this is not allowed
	if (timeout > getSlotTime()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::getTimeout %f slotTime %f ERROR", timeout, getSlotTime());
		
		return 10.0;
	}
	
	// if timeout is longer than the remaining time for the current slot, the time needed for the communication requires more than one slot and this is not allowed
	if (timeout > mhSlot_.expire()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::getTimeout %f slotTime expire %f ERROR", timeout, mhSlot_.expire());
		
		return 10.0;
	}
	
	return timeout;
}

/*!
 * 	@brief The check_pktACK() function transmits an ACK packet (after checking if the transmission can actually occurs).
 *	@retval 0 All is done.
 *	@retval -1 An error occured.
 */

int Sunset_SlottedCsma::check_pktACK() 
{
	struct hdr_Sunset_Mac *mh;
	int dst = 0, src = 0;
	
	if (!use_ack) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::check_pktACK but we are not using ACK pkt ERROR");
		
		return -1;
	}
	
	if (mhDefer_.busy() != 0) {
		
		return -1;
	}
	
	if (pktACK_ == 0) {
		
		return -1;
	}
	
	if (tx_state_ == MAC_ACK) {
		
		return -1;
	}
	
	mh = HDR_SUNSET_MAC(pktACK_);
	
	dst = (mh->dst);
	src = (mh->src);
	
	setTxState(MAC_ACK);
	transmit(pktACK_);
	
	return 0;
}

/*!
 * 	@brief The check_pktTx() function transmits a DATA packet (after checking if the transmission can actually occurs).
 *	@retval 0 All is done.
 *	@retval -1 An error occured.
 */

int Sunset_SlottedCsma::check_pktTx() {
	
	struct hdr_Sunset_Mac *mh;
	int dst, src;
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_SlottedCsma::check_pktTx entering ...");
	
	if(pktTx_ == 0) {
		
		return -1;
	}
	
	mh = HDR_SUNSET_MAC(pktTx_);
	
	switch(mh->dh_fc.fc_subtype) {
			
		case SUNSET_MAC_Subtype_Data:
			
			if(!is_idle() || mhDefer_.busy() || mhSend_.busy() || tx_active_ || pktACK_) {
				
				Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_SlottedCsma::check_pktTx channel is busy");
				
				txAction(pktTx_, SUNSET_MAC_TX_ACTION_BUSY);
				
				RetransmitDATA();
				
				return 0;
			}
			
			setTxState(MAC_SEND);
			
			break;
			
		default:
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::check_pktTx check_pktTx:Invalid MAC Control subtype ERROR");
			
			return -1;
	}
	
	dst = (mh->dst);
	src = (mh->src);
	
	transmit(pktTx_);
	
	return 0;
}

/*!
 * 	@brief The createACK() function creates an ACK packet.
 *	@param[in] dst The destination address.
 */

void Sunset_SlottedCsma::createACK(int dst) 
{
	Packet *p = Packet::alloc();
	hdr_cmn* ch = HDR_CMN(p);
	struct sunset_ack_frame *af;
	
	af = (struct sunset_ack_frame*)p->access(hdr_Sunset_Mac::offset_);
	
	if (pktACK_ != 0) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::createACK but pktACK_ is not zero ERROR");
		return;
	}
	
	if (!use_ack) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::createACK but we are not using ACK pkt ERROR");
		return;
	}
	
	ch->uid() = 0;
	ch->ptype() = PT_SUNSET_MAC;
	
	ch->size() = getACK_Size();
	ch->iface() = -2;
	ch->error() = 0;
	
	memset(af, 0x0, MAC_HDR_LEN);
	
	af->af_fc.fc_protocol_version = SUNSET_MAC_ProtocolVersion;
 	af->af_fc.fc_type	= SUNSET_MAC_Type_Control;
 	af->af_fc.fc_subtype	= SUNSET_MAC_Subtype_ACK;
 	af->af_fc.fc_to_ds	= 0;
 	af->af_fc.fc_from_ds	= 0;
 	af->af_fc.fc_more_frag	= 0;
 	af->af_fc.fc_retry	= 0;
 	af->af_fc.fc_pwr_mgt	= 0;
 	af->af_fc.fc_more_data	= 0;
 	af->af_fc.fc_wep	= 0;
 	af->af_fc.fc_order	= 0;
	
	af->dst = dst;
	af->src = (int)getModuleAddress();
	
	SUNSET_HDR_CMN(p)->response = 1;

	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_SlottedCsma::createACK size %d rate %d", ch->size(), TIMING_CTRL_RATE);
	
	/* store ack tx time */
 	
 	ch->txtime() = macTiming->txtime(macTiming->getPktSize(p), TIMING_CTRL_RATE);
	
	HDR_CMN(p)->timestamp() = Sunset_Utilities::getRealTime();
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_SlottedCsma::createACK txTime %f timeStamp %f", ch->txtime(), HDR_CMN(p)->timestamp());
	
	/* calculate ack duration */
	
 	af->af_duration = 0;	
	
	pktACK_ = p;
	
	if (mhDefer_.busy() != 0) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::createACK has to send ACK pkt to node:%d but timer is running ERROR", dst);
		
		return;
	}
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_SlottedCsma::createACK node:%d to node:%d time %f pType %d", getModuleAddress(), dst, HDR_CMN(p)->timestamp(), (u_int8_t)ch->ptype());
}


/*!
 * 	@brief The RetransmitDATA() function is invoked when a DATA packet must be retransmitted.
 */

void Sunset_SlottedCsma::RetransmitDATA() 
{
	struct hdr_Sunset_Mac *dh;
	int dst, src;
	
	if (pktTx_ == 0 ) {
		
		return;
	}
	
	dh = HDR_SUNSET_MAC(pktTx_);
	
	setTxState(MAC_IDLE);
	
	dst = (dh->dst);
	src = (dh->src);
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::RetransmitDATA to %d retry %d", dst, tx_retry);
	
	tx_retry += 1;
	
	if (tx_retry >= getLongRetryLimit()) {
		
		/* I have reached the data packet retry limit  discard the data packet and notify to upper layer */
		
		txAction(pktTx_, SUNSET_MAC_ACTION_PKT_DISCARDED);
		
		hdr_cmn *ch = HDR_CMN(pktTx_);
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::RetransmitDATA DISCARD too many retransmissions");
		
		Packet* aux = pktTx_->copy();
		
		discard(pktTx_); 
		pktTx_ = 0;
		resetTxRetry();
		backoffSlotCount = 0;
		
		setTxState(MAC_IDLE);
		
		if (aux != 0) {
			
			Mac2RtgPktDiscarded(aux);

			Sunset_Utilities::eraseOnlyPkt(aux, getModuleAddress());
			
		}
		
		anotherTransmission();
		
		return;
		
	} else {
		
		struct hdr_Sunset_Mac *dh;
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::RetransmitDATA start retransmission process");
		
		dh = HDR_SUNSET_MAC(pktTx_);
		dh->dh_fc.fc_retry = 1;
		
		// compute the number of slots to back off
		backoffSlotCount = getBackoffSlotCount();
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::RetransmitDATA backoff Time %f backoffSlotCount %d", backoffSlotCount * getSlotTime(), backoffSlotCount);
		
		if (Sunset_Statistics::use_stat() && stat != NULL) {
			
			stat->logStatInfo(SUNSET_STAT_MAC_BACKOFF, getModuleAddress(), pktTx_, HDR_CMN(pktTx_)->timestamp(), "%f\n", backoffSlotCount * getSlotTime());
		}
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::RetransmitDATA to %d retry %d count %d", dst, tx_retry, backoffSlotCount);
}


/*!
 * 	@brief The send() function associates a packet to the packet pointer to be transmitted.
 *	@param p The packet to be transmitted.
 */

void Sunset_SlottedCsma::send(Packet *p) 
{
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::send size %d", HDR_CMN(p)->size());
	
	if (pktTx_ != 0) {
		
		return;
	}
	
	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_MAC_NEW, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
	}
	
	pktTx_ = p;
}

/*!
 * 	@brief The slotHandler() function is invoked when the slot timer timeout is over.
 */

void Sunset_SlottedCsma::slotHandler() 
{
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_SlottedCsma::slotHandler %f backoffSlotCount %d", getSlotTime(), backoffSlotCount);
	
	if (pktTx_ == 0) { // check if there is no data packet waiting to be transmitted
		
		mhSlot_.start(getSlotTime());
		
		if(backoffSlotCount != 0) {  // check if back off is running and decrease the number of back off slots
		  	
		  	backoffSlotCount--;
		}
		
		return;
	}
	
	// A data packet is waiting to be transmitted
	
	mhSlot_.start(getSlotTime());
    	
	if (backoffSlotCount <= 0) { // no back off currently running
		
		if (is_idle()) {
			
			if (mhDefer_.busy() == 0 && pktACK_ == 0) {
				
				/* We can try to transmit the data packet. */				 
				check_pktTx();
			}
			else {
				
				/* If we are already deferring, we are currently trying to transmit an ACK packet. */
				RetransmitDATA();
				
			}
			
		} else {
			
			/* If the medium is NOT IDLE, then we need to retransmit the data packet */
			
			RetransmitDATA();
		}
	}
	else {
		
		backoffSlotCount--; // decrease the number of back off slots
	}
}

/*!
 * 	@brief The recvFromUpperLayers() function handles a packet coming from upper layers.
 *	@param p The received packet.
 */

void Sunset_SlottedCsma::recvFromUpperLayers(Packet *p) 
{
	struct hdr_cmn *hdr = HDR_CMN(p);
	
	checkQueue(p);
}

/*!
 * 	@brief The recvPkt() function is invoked when a new packet is received.
 */

void Sunset_SlottedCsma::recvPkt() 
{
	hdr_cmn *ch = HDR_CMN(pktRx_);
	struct hdr_Sunset_Mac *mh = HDR_SUNSET_MAC(pktRx_);
	int dst = (mh->dst);
	int src = mh->src; 
	u_int8_t  type = mh->dh_fc.fc_type;
	u_int8_t  subtype = mh->dh_fc.fc_subtype;
	
	assert(pktRx_);
	
	if( src == getModuleAddress() ) { // I am the src of the packet received from lower layer discard it
		
		Sunset_Utilities::erasePkt(pktRx_, getModuleAddress());
		
		goto done;
	}
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_SlottedCsma::recvPkt recv_timer %d->%d", src, dst);
	
	if( ch->error() ) {  // packet received with error discard it
		
		if (pktRx_ != 0) {
			
			rxAction(pktRx_, SUNSET_MAC_RX_ACTION_ERROR);
			
		}
		
		Sunset_Utilities::erasePkt(pktRx_, getModuleAddress());
		
		goto done;
	}
	
	// packet reception completer --> reset tx status
	
	if (pktRx_ != 0) {
		
		rxAction(pktRx_, SUNSET_MAC_RX_ACTION_OK);
		
	}
	
	setRxState(MAC_IDLE);
	
	switch(type) {
			
		case SUNSET_MAC_Type_Control:
			
			switch(subtype) {
					
				case SUNSET_MAC_Subtype_ACK:
					
					recvACK(pktRx_);    //recv an ACK packet
					
					break;
					
				default:
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::recvPkt Invalid MAC Control Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		case SUNSET_MAC_Type_Data:
			
			switch(subtype) {
					
				case SUNSET_MAC_Subtype_Data:
					
					recvDATA(pktRx_);   // recv a DATA packet
					
					break;
					
				default:
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::recvPkt Invalid MAC Data Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		default:
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::recvPkt Invalid MAC Type %x ERROR", subtype);
			return;
	}
	
done:
	pktRx_ = 0;
	rx_resume();	
	
	return;
}

/*!
 * 	@brief The recvDATA() function is invoked when a new DATA packet is received.
 *	@param[in] p The received DATA packet.
 */

void Sunset_SlottedCsma::recvDATA(Packet *p) 
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
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::recvDATA from %d to %d size %d timestamp %f", src, dst, ch->size(), HDR_CMN(p)->timestamp());
	
	/* it is a UNICAST pkt but I am not the destination */
	
	if (dst != getBroadcastAddress() && dst != getModuleAddress()) {
		
		discard(p);
		return;
	}
	
	if (dst != getBroadcastAddress() && use_ack) {
		
		// packet is addressed to this node
		
		if(pktACK_ || tx_state_ != MAC_IDLE || mhSend_.busy() || tx_active_) {
			
			//if the node is busy in other operations discard the packet
			discard(p);
			return;
		}
		
		if (!mhSend_.busy() && !tx_active_) {
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_SlottedCsma::recvDATA from %d to %d  createACK", src, dst);
			
			// packet for me prepare an ACK packet in response
			createACK(src);
			
		}
		else {
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_SlottedCsma::recvDATA from %d to %d  createACK mhSend %d tx_Active %d ERROR", src, dst, mhSend_.busy(), tx_active_);
			
			return;
		}
	}	
	
	rxAction(p, SUNSET_MAC_RX_ACTION_DONE);
	
	// provide the packet to the upper layer
	sendUp(p);	
	
	if (dst != getBroadcastAddress() && use_ack) {
		
		if (mhDefer_.busy()) {
			
			mhDefer_.stop();
			
		}
		
		// start the timer to send the ACK packet
		mhDefer_.start(macTiming->getSIFS());
	}
    	
	return;
}

/*!
 * 	@brief The recvACK() function  is invoked when a new ACK packet is received.
 *	@param[in] p The received ACK packet.
 */

void Sunset_SlottedCsma::recvACK(Packet *p) 
{
	
	struct hdr_Sunset_Mac *dh = HDR_SUNSET_MAC(p);
	int dst, src;
	struct hdr_cmn *ch = HDR_CMN(p);
	int check = 0;
	
	// we are not using ack packets
	if (!use_ack) {
		discard(p);
		return;
	}
	
	dst = (dh->dst);
	src = (dh->src);
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_SlottedCsma::recvACK from %d to %d", src, dst);
	
	// ACK packet is not for me
	if (dst != getModuleAddress()) {
		
		discard(p);
		return;
	}
	
	// recv ACK packet for me but node busy in other operations
	
	if(tx_state_ != MAC_SEND) {
		
		discard(p);
		return;
	}
	
	if (pktTx_ == 0) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::recvACK from %d but no pktTx is present ERROR", src);
		
		discard(p);
		
		return;
	}
	
	Packet * aux = pktTx_->copy();
	
	if ((mhSend_.busy())) { // stop waiting for the ACK packet
		
		check = 1;	//I was waiting for the ACK
		mhSend_.stop();
	}	
	
	if  (check) {	// if I was waiting for the ACK - I have received now and can reset the tx variable, otherwise this ACK is not expected
		
		//check if I have to estimate the propagation delay (distance) or some other module is providing this information
		
		if (sid == NULL || sid->is_provided(getModuleAddress(), sid_id, "NODE_PROPAGATION_DELAY") == 0) { 
			
			//no other module are providing the propagation delay information
			
			map<int,double>::iterator it = timeSentToNode.find((int)src); 
			
			if (it != timeSentToNode.end()) {
				
				double time = (NOW - macTiming->txtime(ch->size())) - 
				macTiming->getDeviceDelay() -
				macTiming->getModemDelay() - it->second; // compute RTT time 
				
				if (time > 0.0) {
					
					if (Sunset_Utilities::compareDouble(time, 0.0, SUNSET_DOUBLE_PRECISION_HIGH) <= 0) {
						
						time = 2 * macTiming->getMaxPropagationDelay();
					}
					
					roundTripTime[(int)src] = time;
					distanceToNode[(int)src] = (time * macTiming->getSoundSpeedInWater())/2.0;
				}
				else {
					time = 2 * macTiming->getMaxPropagationDelay();
					
					roundTripTime[(int)src] = time;
					
					distanceToNode[(int)src] = (time * macTiming->getSoundSpeedInWater())/2.0;				
				}
			}
		}
		
		resetTxRetry();
	}
	
	Sunset_Utilities::erasePkt(pktTx_, getModuleAddress()); 
	pktTx_ = 0;
	
	discard(p);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_SlottedCsma::recvACK recvACK");
	Sunset_Debug::debugInfo(0, getModuleAddress(), "Sunset_SlottedCsma::recvACK Transmission COMPLETED to %d", src);
	
	// reset tx status
	setTxState(MAC_IDLE);

	if (aux != 0) {
		
		Mac2RtgPktTransmitted(aux);
	
		Sunset_Utilities::eraseOnlyPkt(aux, getModuleAddress());
		
	}
	
	// check if other data packets are waiting to be transmitted     
	anotherTransmission();
}

/*!
 * 	@brief The start() function  is called when the slotted csma module starts and it computes the duration of the slot in a conservative way if it has not been
 *	defined by the user using the tcl script.
 */

void Sunset_SlottedCsma::start() 
{
	
	Sunset_Mac::start(); // call the start function of Sunset_Mac for class variables initialization
	
	double aux = 0.0;
	int data_pkt_size = macTiming->getPktSize(getDATA_Size() + getMacHdrSize());
	int ack_pkt_size = macTiming->getPktSize(getACK_Size());
	
	// if no slot time is provided in the Tcl scripts compute the slot time considering if ack packets are used or not
	if (use_ack) {
		
		if ( slotTime_ == 0.0 ) {
			
			slotTime_ = (2 * macTiming->getMaxPropagationDelay()) + (2 * macTiming->getDeviceDelay()) + (2 * macTiming->getModemDelay()) + macTiming->transfertTime(ack_pkt_size) + macTiming->txtime(ack_pkt_size) + macTiming->transfertTime(data_pkt_size) + macTiming->txtime(data_pkt_size) + EPSILON_DELAY;
		}
	}
	else {
		if ( slotTime_ == 0.0 ) {
			
			slotTime_ = (macTiming->getMaxPropagationDelay()) + (macTiming->getDeviceDelay()) + (macTiming->getModemDelay()) + macTiming->transfertTime(data_pkt_size) + macTiming->txtime(data_pkt_size) + EPSILON_DELAY;
		}
	}
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_SlottedCsma::start slotTime %f", getSlotTime());
	
	aux = Random::uniform((5 + getModuleAddress())%10);
	
	mhSlot_.start(aux + getSlotTime());
	
	// register this mac to the information dispatcher
	if (sid != NULL) {
		
		sid_id = sid->register_module(getModuleAddress(), "MAC_SLOTTED_CSMA", this);
		sid->define(getModuleAddress(), sid_id, "NODE_PROPAGATION_DELAY");
		sid->subscribe(getModuleAddress(), sid_id, "NODE_PROPAGATION_DELAY");
	}
}

/*!
 * 	@brief The stop() function  is called when the slotted csma module stops.
 */

void Sunset_SlottedCsma::stop() 
{   
	Sunset_Mac::stop(); // call the stop function of Sunset_Mac
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_SlottedCsma::stop macQueue_->length() = %d pktTx %d pktRx %d pktAck %d tx_active %d tx_state %d rx_state %d backoff %d", macQueue_->length(), pktTx_, pktRx_, pktACK_, tx_active_, tx_state_, rx_state_, backoffSlotCount);
	
	if (mhSlot_.busy()) {
		
		mhSlot_.stop();
	}
	
	return;
}

/*!
 * 	@brief The anotherTransmission() function checks if a new transmission has to be done.
 */

void Sunset_SlottedCsma::anotherTransmission() 
{
	if (pktTx_ != 0) { // if pktTx is already set return
		
		return;
	}
	
	if (macQueue_ != 0 && macQueue_->length() > 0) { // check the queue, if any
		
		Packet* p2;
		
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_SlottedCsma::anotherTransmission macQueue_->length() = %d", macQueue_->length());
		
		p2 = macQueue_->deque();
		
		send(p2);
	}
	else {
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_SlottedCsma::anotherTransmission macQueue_->length() = 0");
   	}
}

/*!
 * 	@brief The Phy2MacTxAborted() function handles the abort-of-PHY-transmission event.
 *	@param p The aborted packet.
 */

void Sunset_SlottedCsma::Phy2MacTxAborted(const Packet* p) 
{
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_SlottedCsma::Phy2MacTxAborted");
	txAborted((Packet*)p);
//	Sunset_Utilities::eraseOnlyPkt((Packet *)p, getModuleAddress());
}

/*!
 * 	@brief The Phy2MacEndTx() function handles the end-of-PHY-transmission event.
 *	@param p The transmitted packet.
 */

void Sunset_SlottedCsma::Phy2MacEndTx(const Packet* p) 
{
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_SlottedCsma::Phy2MacEndTx %p %p", p, pktTx_);
	txDone((Packet*)p);
//	Sunset_Utilities::eraseOnlyPkt((Packet *)p, getModuleAddress());
}

/*!
 * 	@brief The Phy2MacStartRx() function handles the detected-start-of-PHY-reception event.
 *	@param p The packet under reception.
 */

void Sunset_SlottedCsma::Phy2MacStartRx(const Packet* p) 
{
	Sunset_Debug::debugInfo(5, getModuleAddress(),  "Sunset_SlottedCsma::Phy2MacStartRx");
	
	setRxState(MAC_RECV);
	
	if (Sunset_Utilities::isSimulation()) {
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_SlottedCsma::Phy2MacStartRx src %d dst %d",  HDR_SUNSET_MAC(p)->src, HDR_SUNSET_MAC(p)->dst);
	}
}

/*!
 *  @brief The Phy2MacEndRx() function handles the end-of-PHY-reception event.
 *	@param p The received packet.
 */

void Sunset_SlottedCsma::Phy2MacEndRx(Packet* p) 
{
	if ( p == 0 ) {
		
		setRxState(MAC_IDLE);
		
		return;

	}

	hdr_cmn* ch = HDR_CMN(p);
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_SlottedCsma::Phy2MacEndRx size %d", ch->size());
	
	if (ch->error()) {
		
		if (ch->size() == 0) {
			
			Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
			setRxState(MAC_IDLE);
			
			return;
		}

		if (p != 0) {
			
			rxAction(p, SUNSET_MAC_RX_ACTION_ERROR);
			
		}

		if (HDR_SUNSET_MAC(p)->dst == getModuleAddress()) {
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_SlottedCsma::Phy2MacEndRx src %d dst %d TOO MUCH NOISE", HDR_SUNSET_MAC(p)->src, HDR_SUNSET_MAC(p)->dst);
		}
		
		Sunset_Utilities::erasePkt(p, getModuleAddress());
		setRxState(MAC_IDLE);
	}
	else {
		
		if (HDR_SUNSET_MAC(p)->dst == getModuleAddress()) {
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_SlottedCsma::Phy2MacEndRx src %d dst %d CORRECT", HDR_SUNSET_MAC(p)->src, HDR_SUNSET_MAC(p)->dst);
		}
		
		pktRx_ = p;
		recvPkt();
	}
}

/*!
 *  @brief The Phy2getPropagationDelay() function returns the estimated propagation delay between the current node and a destination node.
 *	@param dst The destination node address.
 */

double Sunset_SlottedCsma::getPropagationDelay(int dst) 
{
	map<int, double>::iterator i = roundTripTime.find(dst);
	
	if (i == roundTripTime.end()) { // if no information on node dst is available return the maximum propagation delay
		
		return macTiming->getMaxPropagationDelay();
	}
	
	return MIN(i->second/2.0, macTiming->getMaxPropagationDelay());
}

/*!
 * 	@brief The notify_info function is called from the information dispatcher when a value the Slotted CSMA module is registered for has been updated.
 *	@param linfo The list of variables shared with the other modules the Slotted CSMA module is interested in.
 */

int Sunset_SlottedCsma::notify_info(list<notified_info> linfo) 
{ 
	list<notified_info>::iterator it = linfo.begin();
	notified_info ni;
	string s;
	char msg[300];
	memset(msg, '\0', 300);
	double val = 0.0;
	
	for (; it != linfo.end(); it++) {
		
		ni = *it;
		s = "NODE_PROPAGATION_DELAY";
		
		if (strncmp((ni.info_name).c_str(), s.c_str(), strlen(s.c_str())) == 0 ) {
			
			sid->get_value(&val, ni);
			
			roundTripTime[ni.node_id] = 2.0 * val;
			
			distanceToNode[ni.node_id] = (val * macTiming->getSoundSpeedInWater());				
			
			Sunset_Debug::debugInfo(1, getModuleAddress(), " Sunset_SlottedCsma::notify_info NODE_PROPAGATION_DELAY to %d val %f", ni.node_id, val);
			
			return 1;
		}
	}
	
	Sunset_Debug::debugInfo(-1, getModuleAddress(), " Sunset_SlottedCsma::notify_info NOTHING TO DO");
	
	return Sunset_Mac::notify_info(linfo); 
} 

/*! @brief Function called when a transmission operation is performed to check if an action has to be taken.
 *	@param[in] p   The packet the action is related to.
 *	@param[in] mct The transmission action that has been executed.
 */

void Sunset_SlottedCsma::txAction(Packet* p, mac_action_type mct) 
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
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::txAction Invalid MAC Control Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		case SUNSET_MAC_Type_Data:
			
			switch(subtype) {
				
				case SUNSET_MAC_Subtype_Data:
					
					pktType = "DATA";
					break;
					
				default:
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::txAction Invalid MAC Data Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		default:
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::txAction Invalid MAC Type %x ERROR", subtype);
			
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
	
	Sunset_Trace::print_info("mac - (%f) Node:%d - SLOTTED_CSMA %s %s source %d destination %d src %d dst %d\n", NOW, getModuleAddress(), actionType.c_str(), pktType.c_str(), ip->saddr(), ip->daddr(), src, dst);
	
}

/*! @brief Function called when a reception operation is performed to check if an action has to be taken.
 *	@param[in] p   The packet the action is related to.
 *	@param[in] mct The reception action that has been executed.
 */

void Sunset_SlottedCsma::rxAction(Packet* p, mac_action_type mct) 
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
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::rxAction Invalid MAC Control Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		case SUNSET_MAC_Type_Data:
			
			switch(subtype) {
				
				case SUNSET_MAC_Subtype_Data:
					
					pktType = "DATA";
					break;
					
				default:
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::rxAction Invalid MAC Data Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		default:
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_SlottedCsma::rxAction Invalid MAC Type %x ERROR", subtype);
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
	
	Sunset_Trace::print_info("mac - (%f) Node:%d - SLOTTED_CSMA %s %s source %d destination %d src %d dst %d\n", NOW, getModuleAddress(), actionType.c_str(), pktType.c_str(), ip->saddr(), ip->daddr(), src, dst);
	
}
