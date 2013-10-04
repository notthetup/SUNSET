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

#include <sunset_phy2mac-clmsg.h>
#include <phymac-clmsg.h>


ClMessage_t CLMSG_PHY2MAC_TXABORTED;

ClMsgPhy2MacTxAborted::ClMsgPhy2MacTxAborted(const Packet* p)
: pkt(p),
ClMessage(CLMSG_PHY2MAC_VERBOSITY, CLMSG_PHY2MAC_TXABORTED)
{
	static int inited = 0;
	
	if (!inited) {
		
		CLMSG_PHY2MAC_TXABORTED = ClMessage::addClMessage();
		type_ = CLMSG_PHY2MAC_TXABORTED;
		inited = 1;
	}
}

ClMessage* ClMsgPhy2MacTxAborted::copy()
{
	assert(0);
}
