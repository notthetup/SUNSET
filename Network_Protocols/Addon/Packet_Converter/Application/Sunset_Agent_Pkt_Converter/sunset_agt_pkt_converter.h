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


#ifndef __Sunset_AgtPktConverter_h__
#define __Sunset_AgtPktConverter_h__

#include <sunset_agent_pkt.h>
#include <sunset_pkt_converter.h>

/*!< \brief This class is responsable for the conversion of the Agent header information into a stream of bytes. */

class Sunset_AgtPktConverter : public Sunset_PktConverter {
	
public:
	Sunset_AgtPktConverter();
	
	virtual int command( int argc, const char*const* argv );
	virtual int getHeaderSizeBits();
	virtual int getConvertedInfoLength(Packet* p);
	virtual int checkPktLevel(Packet* p);
	virtual int pkt2Buffer(int level, Packet* p, char* buffer, int offset);
	virtual int buffer2Pkt(int level, Packet* p, char* buffer, int offset, int bits);
	virtual int buffer2Pkt(int level, int src, int dst, Packet* p, char* buffer, int offset, int bits);
	
	virtual int getSrc(int level, Packet* p, sunset_header_type m, int& src);
	virtual int getDst(int level, Packet* p, sunset_header_type m, int& dst);
	virtual int getPktSubType(int level, Packet* p, sunset_header_type m, int& subType);
	virtual int getPktType(int level, Packet* p, sunset_header_type m, int& type);
	
	virtual void erasePkt(int level, Packet* p);
	
	/*! @brief The getName prints the packet header converter name.*/
	virtual void getName() { Sunset_Debug::debugInfo(3, -1, "Sunset_AgtPktConverter"); }
	
	int data_copy(int level, Packet *p, Packet* copy);
	
	virtual void start(); //initialize module variable and interaction with othe modules

	/*!< \brief Method to check if the packet type in input corresponds to an application packet, it can be extended by each packet header converter module. */
	virtual int isApplicationPkt(int level, int pktType, int& result) { 

		if (pktType == PT_SUNSET_AGT) {
		
			result = 1; 
		
		}
		
		return 1;
	}
	
protected:
	
	int use_source;
	int use_dest;
	int use_pktId;
	int use_data;
	
};


#endif
