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

#ifndef __Sunset_CbrPktConverter_h__
#define __Sunset_CbrPktConverter_h__

#include <cbr-module.h>
#include <sunset_pkt_converter.h>

/*!< \brief This class is responsable for the conversion of the CBR header information into a stream of bytes. */

class Sunset_CbrPktConverter : public Sunset_PktConverter {
	
public:
	Sunset_CbrPktConverter();
	
	virtual int command( int argc, const char*const* argv );
	
	/*!< \brief It returns the number of bits needed when converting the CBR HEADER information. */	
	virtual int getHeaderSizeBits();
	
	/*! @brief Function returning the size (in bits) of the stream of bytes for the CBR HEADER information conversion. */
	virtual int getConvertedInfoLength(Packet* p);
	
	/*! @brief Function returning 1 if the CBR HEADER information of packet p have to be converted, 0 otherwise. */      
	virtual int checkPktLevel(Packet* p);
	
	/*! @brief Function to convert CBR HEADER information of packet p in a stream of bytes. */
	virtual int pkt2Buffer(int level, Packet* p, char* buffer, int offset);
	
	/*! @brief Function to convert a stream of bytes into CBR HEADER information for packet p. */
	virtual int buffer2Pkt(int level, Packet* p, char* buffer, int offset, int bits);
	
	/*! @brief Function to convert a stream of bytes into CBR HEADER information for packet p when source and destination IDs are provided by the modoem. */
	virtual int buffer2Pkt(int level, int src, int dst, Packet* p, char* buffer, int offset, int bits) {
		// call conversion  without using the IDs
		return buffer2Pkt(level, p, buffer, offset, bits);
	}
	
	/*! @brief The getSrc function is used to obtain the source ID for the CBR packet header. */
	virtual int getSrc(int level, Packet* p, sunset_header_type m, int& src);
	
	/*! @brief The getDst function is used to obtain the destination ID for the CBR packet header. */
	virtual int getDst(int level, Packet* p, sunset_header_type m, int& dst);
	
	/*! @brief The getName prints the packet header converter name.*/
	virtual void getName() { Sunset_Debug::debugInfo(3, -1, "Sunset_CbrPktConverter"); }
	
	/*! @brief The getPortBits returns the maximum number of bits to express the portNumber information. */
	int getPortBits() { return PORT_BITS; }
	
	virtual void start(); //initialize module variable and interaction with othe modules
	
protected:
	
	int use_timestamp;
	
	int PORT_BITS;
	
};


#endif
