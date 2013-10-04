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



#ifndef __Sunset_RtgPktConverter_h__
#define __Sunset_RtgPktConverter_h__

#include <sunset_pkt_converter.h>

/*!< \brief This class is responsable for the conversion of the routing headers information into a stream of bytes. This class is a place holder at the moment, since no routing header packets have been implemented yet when running in emulation mode. */

class Sunset_RtgPktConverter : public Sunset_PktConverter {
	
public:
	Sunset_RtgPktConverter() { }
	
	virtual int getHeaderSizeBits();
	virtual int getConvertedInfoLength(Packet* p);
	virtual int checkPktLevel(Packet* p);
	virtual int pkt2Buffer(int level, Packet* p, char* buffer, int offset);
	virtual int buffer2Pkt(int level, Packet* p, char* buffer, int offset, int bits);
	virtual void erasePkt(int level, Packet* p) { };
	virtual int buffer2Pkt(int level, int src, int dst, Packet* p, char* buffer, int offset, int bits);
	
	/*! @brief The getName prints the packet header converter name. */
	virtual void getName() { Sunset_Debug::debugInfo(5, -1, "Sunset_RtgPktConverter"); }
	
	virtual void start(); //initialize module variable and interaction with othe modules
	
protected:
	
};

#endif
