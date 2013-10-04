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


#ifndef _SUNSET_PHY2MAC_CLMSG_H_
#define _SUNSET_PHY2MAC_CLMSG_H_

#include<packet.h>
#include<clmessage.h>
#include<phymac-clmsg.h> 

extern ClMessage_t CLMSG_PHY2MAC_TXABORTED;

class ClSAP;

/*! \brief This class defines a cross-layer messagge from the PHY layer to the MAC layer informing that the transmission of a packet p has not been correctly completed. */

class ClMsgPhy2MacTxAborted : public ClMessage {
	
public:
	ClMsgPhy2MacTxAborted(const Packet* p);
	ClMessage* copy();	// copy the message
	
	const Packet* pkt;
};


#endif
