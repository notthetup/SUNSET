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



#include <sunset_routing_pkt_converter.h>
#include <sunset_routing_pkt.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_PktRtgConverterClass : public TclClass 
{
public:
	
	Sunset_PktRtgConverterClass() : TclClass("Sunset_PktConverter/Routing") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_RtgPktConverter());
	}
	
} class_Sunset_PktRtgConverterClass;

/*!
 * 	@brief The getHeaderSizeBits function returns the total bits size of the Routing packet header.
 *	@retval size The total headers size.
 */

int Sunset_RtgPktConverter::getHeaderSizeBits() 
{ 
	Sunset_Debug::debugInfo(5, -1, "Sunset_RtgPktConverter::getHeaderSizeBits return 0");
	
	return 0; 
}

/*!
 * 	@brief The getConvertedInfoLength is used to compute the number of bits needed to convert the ns-2 packet.
 *	@param p The converted packet.
 * 	@retval size The byte size of the converted packet p.
 */

int Sunset_RtgPktConverter::getConvertedInfoLength(Packet* p) 
{ 
	return 0; 
}

/*!
 * 	@brief The checkPktLevel function checks if the Routing packet header converter has to convert its header information in the specified packet. 
 *	@retval 0 The Routing packet header converter does not convert its header information in the specified packet.
 *	@retval 1 The Routing packet header converter has to convert its header information in the specified packet.
 */

int Sunset_RtgPktConverter::checkPktLevel(Packet* p) 
{ 
	return 0; 
}

/*!
 * 	@brief The pkt2Buffer function converts a Routing packet header in a stream of bytes.
 *	@param level The layer ID of the PHY packet header converter.
 *	@param p The packet containing the information to convert.
 * 	@param[out] buffer The converted buffer.
 * 	@param offset The offset used when starting to write into buffer.
 *	@retval size The buffer size in bits.
 */

int Sunset_RtgPktConverter::pkt2Buffer(int level, Packet* p, char* buffer, int offset) 
{
	return 0;
}

/*!
 * 	@brief The buffer2Pkt function converts a stream of bytes into a Routing packet header.
 *	@param level The layer ID of the PHY packet header converter.
 * 	@param p The packet that will contain the information taken from the buffer.
 * 	@param buffer The source buffer that contains the packet information of all the stack layers.
 * 	@param offset The offset used when starting to read from the buffer.
 * 	@param bits The number of bits to be read.
 *	@retval size The number of bits read from the buffer.
 */

int Sunset_RtgPktConverter::buffer2Pkt(int level, Packet* p, char* buffer, int offset, int bits) 
{
	return 0;
}

/*!
 * 	@brief The buffer2Pkt function converts a stream of bytes into a Routing packet header when link source and destination are specified in the input.
 *	@param level The layer ID of the PHY packet header converter.
 *	@param src The link source ID.
 *	@param dst The link destination ID.
 * 	@param p The packet that will contain the information taken from the buffer.
 * 	@param buffer The source buffer that contains the packet information of all the stack layers.
 * 	@param offset The offset used when starting to read from the buffer.
 * 	@param bits The number of bits to be read.
 *	@retval size The number of bits read from the buffer.
 */

int Sunset_RtgPktConverter::buffer2Pkt(int level, int src, int dst, Packet* p, char* buffer, int offset, int bits) 
{
	return buffer2Pkt(level, p, buffer, offset, bits);
}

/*!
 * 	@brief The start() function can be called from the TCL script to execute module operations when the emulation starts.
 */

void Sunset_RtgPktConverter::start() 
{	
	Sunset_PktConverter::start();
	assignNsPktType(PT_SUNSET_RTG);
	Sunset_Debug::debugInfo(5, -1, "Sunset_RtgPktConverter::start");
}
