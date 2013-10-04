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


#ifndef __Sunset_Common_PktConverter_h__
#define __Sunset_Common_PktConverter_h__

#include <sunset_pkt_converter.h>
#include <packet.h>


#define DISTANCE_PRECISION 10
#define DELAY_PRECISION 10000

/*! \brief This class extends the packet converter class and it is responsable for the conversion of the SUNSET common packet header information into a stream of bytes for the external devices and vice versa.*/

class Sunset_Common_PktConverter : public Sunset_PktConverter {
	
public:
	
	Sunset_Common_PktConverter();
	
	virtual int command( int argc, const char*const* argv );
	
	/*!< \brief It returns the number of bits needed when converting the SUNSET COMMON HEADER information. */	
	virtual int getHeaderSizeBits();
	
	/*! @brief Function returning the size (in bits) of the stream of bytes for the SUNSET COMMON HEADER information conversion. */
	virtual int getConvertedInfoLength(Packet* p);
	
	/*! @brief Function returning 1 if the SUNSET COMMON HEADER information of packet p have to be converted, 0 otherwise. */
	virtual int checkPktLevel(Packet* p);
	
	/*! @brief Function to convert SUNSET COMMON HEADER information of packet p in a stream of bytes. */
	virtual int pkt2Buffer(int level, Packet* p, char* buffer, int offset);
	
	/*! @brief Function to convert a stream of bytes into SUNSET COMMON HEADER information for packet p. */
	virtual int buffer2Pkt(int level, Packet* p, char* buffer, int offset, int bits);
	
	/*! @brief Function to delete memory allocated for SUNSET COMMON HEADER information of packet p. */
	virtual void erasePkt(int level, Packet* p) { };
	
	/*! @brief Function to convert a stream of bytes into SUNSET COMMON HEADER information for packet p when source and destination IDs are provided by the modoem. */
	virtual int buffer2Pkt(int level, int src, int dst, Packet* p, char* buffer, int offset, int bits) {
		// call conversion  without using the IDs
		return buffer2Pkt(level, p, buffer, offset, bits);
	}
	
	/*! @brief The getName prints the packet header converter name. */	
	virtual void getName() { Sunset_Debug::debugInfo(5, -1, "Sunset_Common_PktConverter"); }
	
protected:
	
	/*! @brief The getDistanceBits returns the maximum number of bits to express the distance information. */
	int getDistanceBits() { return DISTANCE_BITS; }
	
	/*! @brief The getDelayBits returns the maximum number of bits to express the delay information. */
	int getDelayBits() { return DELAY_BITS; }
	
	int usePropagationDelay_;
	int useProcessingDelay_;
	int useDistance_;
	int useBackSeat_;
	
	int useResponseBit_;
	
	int DISTANCE_BITS;
	int DELAY_BITS;
};

#endif
