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



#ifndef __Sunset_MacPktConverter_h__
#define __Sunset_MacPktConverter_h__

#include <sunset_pkt_converter.h>
#include <sunset_mac_pkt.h>

/*!< \brief This class extends the packet converter class and it is responsable for the conversion of the MAC headers information into a stream of bytes for the external devices and vice versa. */

class Sunset_MacPktConverter : public Sunset_PktConverter {
	
public:
	Sunset_MacPktConverter();
	
	virtual int command( int argc, const char*const* argv );
	virtual int getHeaderSizeBits();
	virtual int getConvertedInfoLength(Packet* p);
	virtual int checkPktLevel(Packet* p);
	virtual int pkt2Buffer(int level, Packet* p, char* buffer, int offset);
	virtual int buffer2Pkt(int level, Packet* p, char* buffer, int offset, int bits);
	virtual int buffer2Pkt(int level, int src, int dst, Packet* p, char* buffer, int offset, int bits);
	virtual void erasePkt(int level, Packet* p) { };
	
	virtual int createMiniPkt(int level, Packet* p, sunset_header_type m, int src, int dst, int pkt_sub_type);
	virtual int isDataPkt(int level, Packet* p, sunset_header_type m, int& result);
	virtual int isCtrlPkt(int level, Packet* p, sunset_header_type m, int& result);
	virtual int useMiniPkt(int level, Packet* p, sunset_header_type m, int& result);
	
	virtual int getSrc(int level, Packet* p, sunset_header_type m, int& src);
	virtual int getDst(int level, Packet* p, sunset_header_type m, int& dst);
	virtual int getPktSubType(int level, Packet* p, sunset_header_type m, int& subType);
	virtual int getPktType(int level, Packet* p, sunset_header_type m, int& type);
	
	/*! @brief The getName prints the packet header converter name.*/
	virtual void getName() { Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter"); }
	
	virtual void start(); //initialize module variable and interaction with othe modules
	
protected:
	
	virtual int checkPktSubType(int pkt_sub_type);
	int use_source;
	int use_dest;
	int use_pktId;
	
	int TYPE_BITS;
	int SUBTYPE_BITS;
	
};

#endif
