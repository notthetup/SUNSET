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

#include <sunset_mac2rtg-clmsg.h>

ClMessage_t CLMSG_MAC2RTG_PKT_TRANSMITTED;
ClMessage_t CLMSG_MAC2RTG_PKT_DISCARDED;


ClMsgMac2RtgPktTransmitted::ClMsgMac2RtgPktTransmitted(const Packet* p)
: pkt(p),
ClMessage(CLMSG_MAC2RTG_VERBOSITY, CLMSG_MAC2RTG_PKT_TRANSMITTED)
{
 	static int inited = 0;
 	if (!inited) {
		CLMSG_MAC2RTG_PKT_TRANSMITTED = ClMessage::addClMessage();
		type_ = CLMSG_MAC2RTG_PKT_TRANSMITTED;
		inited = 1;
	}
}

ClMessage* ClMsgMac2RtgPktTransmitted::copy()
{
	assert(0);
}



ClMsgMac2RtgPktDiscarded::ClMsgMac2RtgPktDiscarded(const Packet* p)
: pkt(p),
ClMessage(CLMSG_MAC2RTG_VERBOSITY, CLMSG_MAC2RTG_PKT_DISCARDED)
{
 	static int inited = 0;
 	if (!inited) {
		CLMSG_MAC2RTG_PKT_DISCARDED = ClMessage::addClMessage();
		type_ = CLMSG_MAC2RTG_PKT_DISCARDED;
		inited = 1;
	}
}

ClMessage* ClMsgMac2RtgPktDiscarded::copy()
{
	assert(0);
}


