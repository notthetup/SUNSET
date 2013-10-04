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

#include <sunset_modem2phy-clmsg.h>
#include <sunset_debug.h>

ClMessage_t CLMSG_MODEM2PHY_ENDTX;
ClMessage_t CLMSG_MODEM2PHY_STARTRX;
ClMessage_t CLMSG_MODEM2PHY_TXABORTED;

ClMsgModem2PhyTxAborted::ClMsgModem2PhyTxAborted(const Packet* p)
: pkt(p),
ClMessage(CLMSG_MODEM2PHY_VERBOSITY, CLMSG_MODEM2PHY_TXABORTED)
{
 	static int inited = 0;
 	if (!inited) {
		CLMSG_MODEM2PHY_TXABORTED = ClMessage::addClMessage();
		type_ = CLMSG_MODEM2PHY_TXABORTED;
		inited = 1;
	}
}

ClMessage* ClMsgModem2PhyTxAborted::copy()
{
	assert(0);
}


ClMsgModem2PhyEndTx::ClMsgModem2PhyEndTx(const Packet* p)
: pkt(p),
ClMessage(CLMSG_MODEM2PHY_VERBOSITY, CLMSG_MODEM2PHY_ENDTX)
{
 	static int inited = 0;
 	if (!inited) {
		CLMSG_MODEM2PHY_ENDTX = ClMessage::addClMessage();
		type_ = CLMSG_MODEM2PHY_ENDTX;
		inited = 1;
	}
}

ClMessage* ClMsgModem2PhyEndTx::copy()
{
	assert(0);
}



ClMsgModem2PhyStartRx::ClMsgModem2PhyStartRx(const Packet* p)
: pkt(p),

/* When running in emulation mode, packet p is just a place holder and does not have the information of the packet the node is currently receiving */ 
ClMessage(CLMSG_MODEM2PHY_VERBOSITY, CLMSG_MODEM2PHY_STARTRX)
{
 	static int inited = 0;
 	if (!inited) {
		CLMSG_MODEM2PHY_STARTRX = ClMessage::addClMessage();
		type_ = CLMSG_MODEM2PHY_STARTRX;
		inited = 1;
	}
}

ClMessage* ClMsgModem2PhyStartRx::copy()
{
	assert(0);
}

