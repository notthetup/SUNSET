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


#include "sunset_tdma.h"
#include <sunset_trace.h>
#include <sunset_common_pkt.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_TdmaClass : public TclClass 
{
public:
	
	Sunset_TdmaClass() : TclClass("Module/MMac/Sunset_Tdma") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_Tdma());	
	}
	
} class_Sunset_Tdma;

Sunset_Tdma::Sunset_Tdma():  mhSend_(this), mhDefer_(this), mhSlot_(this) 
{
	slot_offset_ = 0;
	slotCount_ = 0;
	
	use_ack = 1;
	pktACK_ = 0;	
	
	ACK_SIZE = 0;
	DATA_SIZE = 0;
	longRetryLimit = 0;
	
	slotTime_ = 0.0;
	logical_id = getModuleAddress();
	
	// Get variables initialization from the Tcl script
	bind("slot_per_frame_", &slot_per_frame_);
	bind("slot_offset_", &slot_offset_);
	bind("use_ack_", &use_ack);
	bind("longRetryLimit", &longRetryLimit);
	bind("ACK_SIZE", &ACK_SIZE);
	bind("DATA_SIZE", &DATA_SIZE);
	bind("slotTime_", &slotTime_);
	bind("logical_id", &logical_id);
	
	distanceToNode.clear();
	timeSentToNode.clear();
	roundTripTime.clear();
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Tdma::CREATED slot_per_frame %d slot_offset_ %d", slot_per_frame_, slot_offset_);
}


/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Tdma::command(int argc, const char*const* argv) 
{
	Tcl& tcl = Tcl::instance();
	
	if (argc == 3) {
		
		// assign the logical ID to the node
		if (strcmp(argv[1], "setLogicalId") == 0) {
			
			Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Tdma::command setLogicalId %d", atoi(argv[2]));
			logical_id = atoi(argv[2]);
			
			return TCL_OK;
		}
	}
	
	return Sunset_Mac::command(argc, argv);
}

/*!
 * 	@brief The slotHandler() function is invoked when the slot timer timeout is over.
 */

void Sunset_Tdma::slotHandler() 
{
	int slotId = 0;
	int my_slot = 0;
	
	slotId = (slotCount_ % slot_per_frame_); // determine the node ID for the current slot
	my_slot = logical_id - slot_offset_;     // determine teh ID of the node
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Tdma::slotHandler slotHandler %f slotId %d my_slot %d", getSlotTime(), slotId, my_slot);
	
	slotCount_++;
	
	if (slotId != my_slot) {  // this is not my slot
		
		mhSlot_.start(getSlotTime());
		return;
	}
	
	// It is my slot
	if (pktTx_ == 0) {  // no pkt waiting to be transmitted
		
		mhSlot_.start(getSlotTime());
		return;
	}
	
	// I have to send a data packet
	mhSlot_.start(getSlotTime());
	
	if (backoffSlotCount == 0) { // no back off currently on going
		
		if(is_idle()) {
			
			if (mhDefer_.busy() == 0 && pktACK_ == 0) {
				
				/* We can try to transmit the data packet. */				 
				
				check_pktTx();
			}
			else {
				
				/* If we are already deferring, we are currently trying to transmit an ACK packet. */
				
				txAction(pktTx_, SUNSET_MAC_TX_ACTION_BUSY);
				
				RetransmitDATA();
			}
			
		} else {
			
			/* If the medium is NOT IDLE, then we start the backoff timer. */
			
			txAction(pktTx_, SUNSET_MAC_TX_ACTION_BUSY);
			
			RetransmitDATA();
		}
	}
	else {
		
		backoffSlotCount--;  // decrease the number of back off slots
	}
}

/*!
 *  @brief The getBackoffSlotCount() function returns the backoff slot counter.
 */

int Sunset_Tdma::getBackoffSlotCount()
{
	int count = 0;
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Tdma::getBackoffSlotCount computed count %d", count);
	
	return count;
}


/*!
 * 	@brief The transmit() function sends a packet to the lower layer.
 *	@param p The packet to be sent.
 */

void Sunset_Tdma::transmit(Packet *p) 
{
	double newTimeout = 0.0;
	int pkt_size = 0;
	
	if (tx_active_) { // if another transmission is on going return
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::transmit but tx already active ERROR");
		return;
	}
	
	tx_active_ = 1;
	
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::transmit src %d dest %d pktId %d", HDR_SUNSET_MAC(p)->src, HDR_SUNSET_MAC(p)->dst, HDR_SUNSET_MAC(p)->pktId);
	
	txAction(p, SUNSET_MAC_TX_ACTION_OK);
	
	Mac2PhyStartTx(p->copy()); 
	
	if (mhSend_.busy()) {   // if tx timer is running stop it
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::transmit timeout mhSend busy ERROR");
		
		mhSend_.stop();
	}
	
	pkt_size = macTiming->getPktSize(p);
	sunset_rateType rate = TIMING_CTRL_RATE;
	
	if (HDR_SUNSET_MAC(p)->dh_fc.fc_type == SUNSET_MAC_Type_Data) {
	
		rate = TIMING_DATA_RATE;
	}
	
	// compute the time to wait for a confirmation on the transmission (txDone)
	newTimeout = macTiming->overheadTime(pkt_size) + macTiming->getMaxPropagationDelay() + macTiming->txtime(pkt_size, rate);
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::transmit timeout %f size %d", newTimeout, HDR_CMN(p)->size());
	
	mhSend_.start(newTimeout);
}

/*!
 * 	@brief The rx_resume() function is invoked when the node has finished to receive packets and it sets the MAC state to IDLE.
 */

void Sunset_Tdma::rx_resume() 
{
	assert(pktRx_ == 0);
	setRxState(MAC_IDLE);
}

/*!
 *  @brief The deferHandler() function is invoked when the defer timer timeout expires anc check if an ACK packet is waiting to be transmitted.
 */

void Sunset_Tdma::deferHandler() 
{
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Tdma::deferHandler pktACK_ %d pktTx_ %d ", pktACK_, pktTx_);
	
	if (!use_ack) {
		
		return;
	}
	
	if (use_ack && check_pktACK() != 0) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::deferHandler but no ACK pkt ERROR");
		return;
	}
}

/*!
 *  @brief The getSlotTime() function returns the slot time.
 */

double Sunset_Tdma::getSlotTime() 
{
	return slotTime_;
}

/*!
 * 	@brief The txDone() function is invoked when a packet transmission has been correctly completed.
 */

void Sunset_Tdma::txDone(Packet* p) 
{
	
	if (tx_active_ == 0) { // if a transmission is not currently on going return
		
		if ( p != 0) {
			
			Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
			
		}

		return;
	}
	
	if (mhSend_.busy()) {   // stop the timer waiting for the confirmation on the correct transmission
		
		mhSend_.stop();
	}
	
	tx_active_ = 0;
	
	int removePkt = 0;
	
	switch(tx_state_) {
			
			// An ACK packet has been correctly transmitted
			
		case MAC_ACK:
			
			Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::txDone ACK transmitted");
			
			if (!use_ack) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::txDone but we are not using ACK pkt ERROR");
				
				if (p != 0) {
					
					Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
					
				}
				
				return;	
			}
			
			// reset ACK variables
			
			if ( p != pktACK_ && p != 0) {
			
				removePkt = 1;

			}

			if (pktACK_ != 0) {
				
				txAction(pktACK_, SUNSET_MAC_TX_ACTION_DONE);
				
				Sunset_Utilities::erasePkt(pktACK_, getModuleAddress()); 
				
				pktACK_ = 0;
			}
			
			pktACK_ = 0;
			
			//reset tx status
			setTxState(MAC_IDLE);
			
			// check if other data packets are waiting to be transmitted 
			anotherTransmission();
			
			if (p != 0 && removePkt) {
				
				Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
				
			}
			
			return;
			
		case MAC_SEND:
			
			// A DATA packet has been correctly transmitted
			
			Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::txDone DATA transmitted %p", pktTx_);
			
			if ( p != pktTx_ && p != 0) {
			
				removePkt = 1;

			}

			if (pktTx_ != 0) {
				
				txAction(pktTx_, SUNSET_MAC_TX_ACTION_DONE);
				
				timeSentToNode[HDR_SUNSET_MAC(pktTx_)->dst] = NOW; // set tx time to node dst used for RTT computation
				
				// check if no ACK is expected
 				if (HDR_SUNSET_MAC(pktTx_)->dst == getBroadcastAddress() || !use_ack) {
					
					Packet* aux = pktTx_->copy();

					Sunset_Utilities::erasePkt(pktTx_, getModuleAddress()); 
					
					// reset tx variables
					pktTx_ = 0;
					resetTxRetry();
					
					//reset tx status
					setTxState(MAC_IDLE);
					
					if (aux != 0) {
						
						Mac2RtgPktTransmitted(aux);
					
					}

					// check if other data packets are waiting to be transmitted 
					anotherTransmission();
				}
				else {
					// Waiting to receive the ACK packet
					
					//compute the timeout for the ACK packet
					double timeout = getTimeout(pktTx_);
					
					Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::txDone timeout at %f pktSize %d", NOW + timeout, HDR_CMN(pktTx_)->size());
					
					// start the timer
					mhSend_.start(timeout);	
				}
			}
			
			break;
			
		default:
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::txDone tx_state_ ERROR %d", tx_state_);

	}	
	
	if (p != 0 && removePkt) {
		
		Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
		
	}

	return;
}

/*!
 * 	@brief The txAborted() function is invoked when a packet transmission has been aborted because of a timeout or an 
 *	error message received from the modem. All the status and temporary data structures are cleared from the information 
 *	regarding the aborted transmission.
 */

void Sunset_Tdma::txAborted(Packet* p) 
{
	
	if (tx_active_ == 0) { // if a transmission is not currently on going return
		
		if (p != 0) {
			
			Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
			
		}
		
		return;
	}
	
	if (mhSend_.busy()) { // stop the timer waiting for the confirmation on the correct transmission
		
		mhSend_.stop();
	}
	
	tx_active_ = 0;
	
	int removePkt = 0;
	
	switch(tx_state_) {
			
		// ERROR on an ACK packet transmission
			
		case MAC_ACK:
			
			Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::txAborted ACK tranmission ABORTED");
			
			if (!use_ack) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::txAborted but we are not using ACK pkt ERROR");

				if (p != 0) {
					
					Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
					
				}
				
				return;	
			}
			
			if (p != 0 && p != pktACK_) {
			
				removePkt = 1;
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
			
			Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::txAborted DATA transmission ABORTED pktTx == 0 (%d)", pktTx_ == 0);
			
			setTxState(MAC_IDLE);
			
			if (p != 0 && p != pktTx_) {
				
				removePkt = 1;
				
			}
			
			if (pktTx_ != 0) {
				
				txAction(pktTx_, SUNSET_MAC_TX_ACTION_ABORTED);
				
				// retransmit the DATA packet 
				RetransmitDATA();
			}
			
			break;
			
		default:
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::txAborted tx_state_ ERROR %d", tx_state_);
	}
	
	if (p != 0 && removePkt) {
		
		Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
		
	}

	return;	
}

/*!
 * 	@brief The send_timer() function is invoked when the tx timeout (mhSend) expires.
 */

void Sunset_Tdma::send_timer() 
{
	Sunset_Debug::debugInfo(4, getModuleAddress(), "unset_SlottedTdma::send_timer pktACK_ %d pktTx_ %d tx_state %d", pktACK_, pktTx_, tx_state_);
	
	if (tx_active_) {  // if the transmission is active no txDone message has been received
		
		txAborted((Packet*) 0); // There has been an ERROR on the packet transmission
		return;
	}
	
	// Otherwise tx has been correctly completed
	
	switch(tx_state_) {
			
		// No ACK has been received
			
		case MAC_SEND:
			
			if(pktTx_ != 0 && HDR_SUNSET_MAC(pktTx_)->dst != getBroadcastAddress() && use_ack) {
				
				txAction(pktTx_, SUNSET_MAC_TX_ACTION_ERROR);
				
				// retransmit the DATA packet 
				
				RetransmitDATA();
			}	
			
			break;
			
		case MAC_ACK:
			
			/* Sent an ACK, we have to just resume the transmission status. */
			
			if (!use_ack) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::send_timer but we are not using ACK pkt ERROR");
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

void Sunset_Tdma::sendHandler() 
{
	send_timer();
}

/*!
 * 	@brief The getTimeout() function returns the timeout associated with the type of the packet p.
 */

double Sunset_Tdma::getTimeout(Packet* p) 
{
	struct hdr_Sunset_Mac *mh;
	double timeout = 0.0;
	int dst = 0, src = 0;
	
	mh = HDR_SUNSET_MAC(p);
	
	dst = (mh->dst);
	src = (mh->src);
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Tdma::getTimeout src %d dst %d type %d subType %d", src, dst, mh->dh_fc.fc_type, mh->dh_fc.fc_subtype);
	
	if (mh->dh_fc.fc_type == SUNSET_MAC_Type_Control) {
		
		switch(mh->dh_fc.fc_subtype) {
				
			case SUNSET_MAC_Subtype_ACK:
				
				/* if the packet type is ACK ... */	
				
				if (!use_ack) {
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::getTimeout but we are not using ACK pkt ERROR");
										
					return 10.0;
				}
				
				// There is no need to wait after ACK packet transmission
				timeout = 0.0; 
				
				Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::getTimeout send ACK to %d timeout at %f", dst, NOW + timeout);
				
				return timeout;
				
			default:
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::getTimeout SubTypeCtrl ERROR %d", mh->dh_fc.fc_subtype);
				
				return 10.0;
		}
	}
	else if (mh->dh_fc.fc_type == SUNSET_MAC_Type_Data) {
		
		/* if the packet type is DATA ... */
		
		int pkt_ack_size = macTiming->getPktSize(getACK_Size());
		if (mh->dh_fc.fc_subtype == SUNSET_MAC_Subtype_Data) {
			
			// Compute the timeout to  wait for an ACK packet considering also additional delays related to modem and device operations
			
			if(mh->dst != getBroadcastAddress() && use_ack) {
				
				timeout = getPropagationDelay(mh->dst) + macTiming->txtime(pkt_ack_size) + macTiming->getDeviceDelay() + macTiming->getModemDelay() + macTiming->transfertTime(pkt_ack_size) + getPropagationDelay(mh->dst) + EPSILON_DELAY;
			}
			else {
				
				// There is no need to wait after DATA packet transmission in broadcast or with no ACK ... wait EPSILON
				timeout = 0.001;
			}
			
			Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Tdma::getTimeout mDelay %f time %f dDelay %f mDelay %f tTime %f maxPropDelay %f ACK_SIZE %d DATA_SIZE %d", macTiming->getMaxPropagationDelay(), macTiming->txtime(pkt_ack_size), macTiming->getDeviceDelay(), macTiming->getModemDelay(), macTiming->transfertTime(pkt_ack_size), getPropagationDelay(mh->dst), pkt_ack_size, macTiming->getPktSize(getDATA_Size()));
		}
		else {
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::getTimeout SubTypeData ERROR %d", mh->dh_fc.fc_subtype);
			
			return 10.0;
		}
	}
	else {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::getTimeout Type ERROR %d", mh->dh_fc.fc_type);
		
		return 10.0;
	}
	
	// if timeout is longer than slot time, the time needed for the communication requires more than one slot and this is not allowed
	if (timeout > getSlotTime()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::getTimeout %f slotTime %f ERROR", timeout, getSlotTime());
		
		return 10.0;
	}
	
	// if timeout is longer than the remaining time for the current slot, the time needed for the communication requires more than one slot and this is not allowed
	if (timeout > mhSlot_.expire()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::getTimeout %f slotTime expire %f ERROR", timeout, mhSlot_.expire());
		
		return 10.0;
	}
	
	return timeout;
}

/*!
 * 	@brief The check_pktACK() function transmits an ACK packet (after checking if the transmission can actually occurs).
 *	@retval 0 All is done.
 *	@retval -1 An error occured.
 */

int Sunset_Tdma::check_pktACK() 
{
	struct hdr_Sunset_Mac *mh;
	int dst = 0, src = 0;
	
	if (!use_ack) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::check_pktACK but we are not using ACK pkt ERROR");
		
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

int Sunset_Tdma::check_pktTx() {
	
	struct hdr_Sunset_Mac *mh;
	int dst, src;
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Tdma::check_pktTx entering ...");
	
	if(pktTx_ == 0) {
		
		return -1;
	}
	
	mh = HDR_SUNSET_MAC(pktTx_);
	
	switch(mh->dh_fc.fc_subtype) {
			
		case SUNSET_MAC_Subtype_Data:
			
			if(!is_idle() || mhDefer_.busy() || mhSend_.busy() || tx_active_ || pktACK_) {
				
				Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Tdma::check_pktTx channel is busy");
				
				txAction(pktTx_, SUNSET_MAC_TX_ACTION_BUSY);
				
				if (mhDefer_.busy()) {
					mhDefer_.stop();
				} 
				
				if (mhSend_.busy()) {
					mhSend_.stop();
				}
				
				RetransmitDATA();
				
				return 0;
			}
			
			setTxState(MAC_SEND);
			
			break;
			
		default:
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::check_pktTx check_pktTx:Invalid MAC Control subtype ERROR");
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

void Sunset_Tdma::createACK(int dst) 
{
	Packet *p = Packet::alloc();
	hdr_cmn* ch = HDR_CMN(p);
	struct sunset_ack_frame *af;
	
	
	af = (struct sunset_ack_frame*)p->access(hdr_Sunset_Mac::offset_);
	
	if (pktACK_ != 0) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::createACK but pktACK_ is not zero ERROR");
		return;
	}
	
	if (!use_ack) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::createACK but we are not using ACK pkt ERROR");
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

	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Tdma::createACK size %d rate %d", ch->size(), TIMING_CTRL_RATE);
	
	/* store ack tx time */
 	
 	ch->txtime() = macTiming->txtime(macTiming->getPktSize(p), TIMING_CTRL_RATE);
	
	HDR_CMN(p)->timestamp() = Sunset_Utilities::getRealTime();
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Tdma::createACK txTime %f timeStamp %f", ch->txtime(), HDR_CMN(p)->timestamp());
	
	/* calculate ack duration */
	
 	af->af_duration = 0;	
	
	pktACK_ = p;
	
	if (mhDefer_.busy() != 0) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::createACK has to send ACK pkt to node:%d but timer is running ERROR", dst);
		
		return;
	}
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Tdma::createACK node:%d to node:%d time %f pType %d", getModuleAddress(), dst, HDR_CMN(p)->timestamp(), (u_int8_t)ch->ptype());
}


/*!
 * 	@brief The RetransmitDATA() function is invoked when a DATA packet must be retransmitted.
 */

void Sunset_Tdma::RetransmitDATA() 
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
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::RetransmitDATA() to %d retry %d", dst, tx_retry);
	
	tx_retry += 1;
	
	if (tx_retry >= getLongRetryLimit()) {
		
		/* I have reached the data packet retry limit discard the data packet and notify to upper layer */
		
		txAction(pktTx_, SUNSET_MAC_ACTION_PKT_DISCARDED);
		
		hdr_cmn *ch = HDR_CMN(pktTx_);
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::RetransmitDATA() DISCARD too many retransmissions");
		
		Packet* aux = pktTx_->copy();

		discard(pktTx_); 
		pktTx_ = 0;
		resetTxRetry();
		backoffSlotCount = 0;
		
		setTxState(MAC_IDLE);
		
		if (aux != 0) {
			
			Mac2RtgPktDiscarded(aux);
		
		}
		
		anotherTransmission();
		
		return;
		
	} else {
		
		struct hdr_Sunset_Mac *dh;
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::RetransmitDATA() start retransmission process");
		
		dh = HDR_SUNSET_MAC(pktTx_);
		dh->dh_fc.fc_retry = 1;
		
		backoffSlotCount = getBackoffSlotCount();
		
		Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::RetransmitDATA() backoff Time %f", time);
		
		if (Sunset_Statistics::use_stat() && stat != NULL) {
			
			stat->logStatInfo(SUNSET_STAT_MAC_BACKOFF, getModuleAddress(), pktTx_, HDR_CMN(pktTx_)->timestamp(), "%d\n", backoffSlotCount * getSlotTime());
		}
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::RetransmitDATA() to %d retry %d count %d",
				dst, tx_retry, backoffSlotCount);
}


/*!
 * 	@brief The send() function associates a packet to the packet pointer to be transmitted.
 *	@param p The packet to be transmitted.
 */

void Sunset_Tdma::send(Packet *p) 
{
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::send size %d", HDR_CMN(p)->size());
	
	if (pktTx_ != 0) {
		
		return;
	}
	
	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_MAC_NEW, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
		
	}
	
	pktTx_ = p;
}

/*!
 * 	@brief The recvFromUpperLayers() function handles a packet coming from the upper layer.
 *	@param p The received packet.
 */

void Sunset_Tdma::recvFromUpperLayers(Packet *p) 
{
	struct hdr_cmn *hdr = HDR_CMN(p);
	
	checkQueue(p);
}

/*!
 * 	@brief The recvPkt() function is invoked when a new packet is received from the lower layer.
 */

void Sunset_Tdma::recvPkt() 
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
        
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Tdma::recvPkt recv_timer %d->%d", src, dst);
	
	if( ch->error() ) {  // packet received with error discard it
				
		Sunset_Utilities::erasePkt(pktRx_, getModuleAddress());
		
		goto done;
	}
	
	Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::recvPkt src %d dest %d pktId %d", src, dst, HDR_SUNSET_MAC(pktRx_)->pktId);
	
	
	if (pktRx_ != 0) {
		
		rxAction(pktRx_, SUNSET_MAC_RX_ACTION_OK);
		
	}
	
	// packet reception completer --> reset tx status
	setRxState(MAC_IDLE);
	
	switch(type) {
			
		case SUNSET_MAC_Type_Control:
			
			switch(subtype) {
					
				case SUNSET_MAC_Subtype_ACK:
					
					recvACK(pktRx_);   //recv an ACK packet
					
					break;
					
				default:
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::recvPkt Invalid MAC Control Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		case SUNSET_MAC_Type_Data:
			
			switch(subtype) {
					
				case SUNSET_MAC_Subtype_Data:
					
					recvDATA(pktRx_); // recv a DATA packet
					
					break;
					
				default:
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::recvPkt Invalid MAC Data Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		default:
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::recvPkt Invalid MAC Type %x ERROR", subtype);
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

void Sunset_Tdma::recvDATA(Packet *p) 
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
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::recvDATA from %d to %d size %d timestamp %f", src, dst, ch->size(), HDR_CMN(p)->timestamp());
	
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
			
			Sunset_Debug::debugInfo(4, getModuleAddress(), "MAC_TDMA recvDATA from %d to %d  createACK", src, dst);
			
			// packet for me prepare an ACK packet in response
			createACK(src);
		}
		else {
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "ERROR MAC_TDMA recvDATA from %d to %d  createACK mhSend %d tx_Active %d ERROR", src, dst, mhSend_.busy(), tx_active_);
			
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
 * 	@brief The recvACK() function is invoked when a new ACK packet is received.
 *	@param[in] p The received ACK packet.
 */

void Sunset_Tdma::recvACK(Packet *p) 
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
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Tdma::recvACK from %d to %d", src, dst);
	
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
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::recvACK from %d but no pktTx is present ERROR", src);
		
		discard(p);
		
		return;
	}
	
	Packet* aux = pktTx_->copy();
	
	if ((mhSend_.busy())) { // stop waiting for the ACK packet
		
		if (pktTx_ == 0) {
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::recvACK from %d but no pktTx is present ERROR2", src);
			
			discard(p);
			
			return;
		}
		
		check = 1;
		mhSend_.stop();
	}	
	
	if (check) {  // compute RTT time to update propagation delay to the src node of the ACK packet
		
		map<int,double>::iterator it = timeSentToNode.find((int)src);
		
		if (it != timeSentToNode.end()) {
			
			double time = (NOW - ch->txtime()) - it->second;
			
			if (time > 0.0) {
				
				if (Sunset_Utilities::compareDouble(time, 0.0, SUNSET_DOUBLE_PRECISION_HIGH) <= 0) {
	
					time = 2 * macTiming->getMaxPropagationDelay();
				}
				
				roundTripTime[(int)src] = time;
				distanceToNode[(int)src] = (time * macTiming->getSoundSpeedInWater())/2.0;
			}
		}
		
		resetTxRetry();
	}
	
	Sunset_Utilities::erasePkt(pktTx_, getModuleAddress()); 
	pktTx_ = 0;
	
	discard(p);
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Tdma::recvACK recvACK");
	Sunset_Debug::debugInfo(0, getModuleAddress(), "Sunset_Tdma::recvACK Transmission COMPLETED to %d", src);
	
	// reset tx status
	setTxState(MAC_IDLE);
	
	if (aux != 0) {
		
		Mac2RtgPktTransmitted(aux);

	}
	
	// check if other data packets are waiting to be transmitted     
	anotherTransmission();
}


/*!
 * 	@brief The start() function is called when the tdma module starts and it computes 
 * the duration of the slot in a conservative way if it has not been defined by the user using the tcl script.
 */

void Sunset_Tdma::start() 
{
	
	Sunset_Mac::start(); // call the start function of Sunset_Mac for class variables initialization
	
	int pkt_ack_size = macTiming->getPktSize(getACK_Size());
	int pkt_data_size = macTiming->getPktSize(getDATA_Size() + getMacHdrSize());
	
	// if no slot time is provided in the Tcl scripts compute the slot time considering if ack packets are used or not
	if (use_ack) {
		
		if ( slotTime_ == 0.0 ) {
			
			slotTime_ = (2 * macTiming->getMaxPropagationDelay()) + (2 * macTiming->getDeviceDelay()) + (2 * macTiming->getModemDelay()) + macTiming->transfertTime(pkt_ack_size) + macTiming->txtime(pkt_ack_size) + macTiming->transfertTime(pkt_data_size) + macTiming->txtime(pkt_data_size) + EPSILON_DELAY;
		}
	}
	else {
		
		if ( slotTime_ == 0.0 ) {
			
			slotTime_ = (macTiming->getMaxPropagationDelay())  + (macTiming->getDeviceDelay()) + (macTiming->getModemDelay()) + macTiming->transfertTime(pkt_data_size) + macTiming->txtime(pkt_data_size) + EPSILON_DELAY;
		}
	}
	
	// register this mac to the information dispatcher
	if (sid != NULL) {
		
		sid_id = sid->register_module(getModuleAddress(), "MAC_TDMA", this);
		
		sid->provide(getModuleAddress(), sid_id, "MAC_SLOT_COUNT");
	}
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Tdma::start slotTime %f", getSlotTime());
	
	mhSlot_.start(getSlotTime());
}

/*!
 * 	@brief The stop() function is called when the tdma module stops.
 */

void Sunset_Tdma::stop() 
{      
	Sunset_Mac::stop(); // call the stop function of Sunset_Mac
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Tdma::stop macQueue_->length() = %d pktTx %d pktRx %d pktAck %d tx_active %d tx_state %d rx_state %d backoff %d", macQueue_->length(), pktTx_, pktRx_, pktACK_, tx_active_, tx_state_, rx_state_, backoffSlotCount);
	
	if (mhSlot_.busy()) {
		
		mhSlot_.stop();
	}
	
	return;
}

/*!
 * 	@brief The anotherTransmission() function checks if a new transmission has to be done.
 */

void Sunset_Tdma::anotherTransmission() 
{
	if (pktTx_ != 0) { // if pktTx is already set return
		
		return;
	}
	
	if (macQueue_ != 0 && macQueue_->length() > 0) { // check the queue, if any
		
		Packet* p2;
		
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Tdma::anotherTransmission macQueue_->length() = %d", macQueue_->length());
		
		p2 = macQueue_->deque();
		
		send(p2);
	}
	else {
		Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Tdma::anotherTransmission macQueue_->length() = 0");
   	}
}

/*!
 * 	@brief The Phy2MacTxAborted() function handles the abort-of-PHY-transmission event.
 *	@param p The aborted packet.
 */

void Sunset_Tdma::Phy2MacTxAborted(const Packet* p) 
{
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Tdma::Phy2MacTxAborted");
	txAborted((Packet*)p);
//	Sunset_Utilities::eraseOnlyPkt((Packet *)p, getModuleAddress());
}

/*!
 * 	@brief The Phy2MacEndTx() function handles the end-of-PHY-transmission event.
 *	@param p The transmitted packet.
 */

void Sunset_Tdma::Phy2MacEndTx(const Packet* p) 
{
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Tdma::Phy2MacEndTx %p %p", p, pktTx_);
	txDone((Packet*)p);
//	Sunset_Utilities::eraseOnlyPkt((Packet *)p, getModuleAddress());
}

/*!
 * 	@brief The Phy2MacStartRx() function handles the detected-start-of-PHY-reception event.
 *	@param p The packet under reception.
 */

void Sunset_Tdma::Phy2MacStartRx(const Packet* p) 
{
	Sunset_Debug::debugInfo(5, getModuleAddress(),  "Sunset_Tdma::Phy2MacStartRx");
	
	setRxState(MAC_RECV);
	
	if (Sunset_Utilities::isSimulation()) {
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Tdma::Phy2MacStartRx src %d dst %d",  HDR_SUNSET_MAC(p)->src, HDR_SUNSET_MAC(p)->dst);
	}
}

/*!
 *  @brief The Phy2MacEndRx() function handles the end-of-PHY-reception event.
 *	@param p The received packet.
 */

void Sunset_Tdma::Phy2MacEndRx(Packet* p) 
{
	if ( p == 0 ) {
		
		setRxState(MAC_IDLE);
		
		return;

	}

	hdr_cmn* ch = HDR_CMN(p);
	
	Sunset_Debug::debugInfo(5, getModuleAddress(), "Sunset_Tdma::Phy2MacEndRx size %d", ch->size());
	
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
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Tdma::Phy2MacEndRx src %d dst %d TOO MUCH NOISE", HDR_SUNSET_MAC(p)->src, HDR_SUNSET_MAC(p)->dst);
		}
		
		Sunset_Utilities::erasePkt(p, getModuleAddress());
		setRxState(MAC_IDLE);
	}
	else {
		
		if (HDR_SUNSET_MAC(p)->dst == getModuleAddress()) {
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Tdma::Phy2MacEndRx src %d dst %d CORRECT", HDR_SUNSET_MAC(p)->src, HDR_SUNSET_MAC(p)->dst);
		}
		
		pktRx_ = p;
		recvPkt();
	}
}

/*!
 *  @brief The Phy2getPropagationDelay() function returns the estimated propagation delay between the current node and a destination node.
 *	@param dst The destination node address.
 */

double Sunset_Tdma::getPropagationDelay(int dst) 
{
	map<int, double>::iterator i = roundTripTime.find(dst);
	
	if (i == roundTripTime.end()) { // if no information on node dst is available return the maximum propagation delay
		
		return macTiming->getMaxPropagationDelay();
	}
	
	return MIN(i->second/2.0, macTiming->getMaxPropagationDelay());
}

/*! @brief Function called when a transmission operation is performed to check if an action has to be taken
 *	@param[in] p   The packet the action is related to.
 *	@param[in] mct The transmission action that has been executed.
 */

void Sunset_Tdma::txAction(Packet* p, mac_action_type mct) 
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
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::txAction Invalid MAC Control Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		case SUNSET_MAC_Type_Data:
			
			switch(subtype) {
				
				case SUNSET_MAC_Subtype_Data:
				
					pktType = "DATA";
					break;
				
				default:
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::txAction Invalid MAC Data Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		default:
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::txAction Invalid MAC Type %x ERROR", subtype);
			
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
	
	Sunset_Trace::print_info("mac - (%f) Node:%d - TDMA %s %s source %d destination %d src %d dst %d\n", NOW, getModuleAddress(), actionType.c_str(), pktType.c_str(), ip->saddr(), ip->daddr(), src, dst);
	
}

/*! @brief Function called when a reception operation is performed to check if an action has to be taken
 *	@param[in] p   The packet the action is related to.
 *	@param[in] mct The reception action that has been executed.
 */

void Sunset_Tdma::rxAction(Packet* p, mac_action_type mct) 
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
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::rxAction Invalid MAC Control Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		case SUNSET_MAC_Type_Data:
			
			switch(subtype) {
			
				case SUNSET_MAC_Subtype_Data:
				
					pktType = "DATA";
					break;
				
				default:
					
					Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::rxAction Invalid MAC Data Subtype %x ERROR", subtype);
					
					return;
			}
			
			break;
			
		default:
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Tdma::rxAction Invalid MAC Type %x ERROR", subtype);
			
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
	
	
	Sunset_Trace::print_info("mac - (%f) Node:%d - TDMA %s %s source %d destination %d src %d dst %d\n", NOW, getModuleAddress(), actionType.c_str(), pktType.c_str(), ip->saddr(), ip->daddr(), src, dst);	
}


