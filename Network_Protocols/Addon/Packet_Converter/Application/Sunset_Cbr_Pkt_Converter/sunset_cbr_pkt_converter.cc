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

#include <sunset_cbr_pkt_converter.h>
#include <ip.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_PktCbrConverterClass : public TclClass {
public:
	Sunset_PktCbrConverterClass() : TclClass("Sunset_PktConverter/Cbr") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_CbrPktConverter());
	}
	
} class_Sunset_PktCbrConverterClass;

Sunset_CbrPktConverter::Sunset_CbrPktConverter() 
{ 
	use_timestamp = 0;
	
	PORT_BITS = 0;
	
	// Get variables initialization from the Tcl script
	bind("use_timestamp", &use_timestamp);
	bind("PORT_BITS", &PORT_BITS);
	
	Sunset_Debug::debugInfo(-1, -1, "Sunset_CbrPktConverter CREATO");
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_CbrPktConverter::command( int argc, const char*const* argv) 
{
	Tcl& tcl = Tcl::instance();
	
	if (argc == 3) {
		
		/* The "use_timestamp" command is set to 1 if the timestamp has to be considered during the conversion of the ns-2 packet into a stream of bytes for external devices transmissions. */
		if (strcmp(argv[1], "use_timestamp") == 0) {
			
			use_timestamp = atoi(argv[2]);
			
			return TCL_OK;
		}
		
		/* If "setPortBits" sets the maximum number of bits to express the portNumber information. */
		if (strcmp(argv[1], "setPortBits") == 0) {
			
			PORT_BITS = atoi(argv[2]);
			
			return TCL_OK;
		}
		
	}
	
	return Sunset_PktConverter::command(argc, argv);
}

/*!
 * 	@brief The checkPktLevel function checks if the CBR header converter has to convert its header information in the specified packet. 
 *	@param p The received packet.
 *	@retval 1 The packet contains the cbr header.
 *	@retval 0 The packet does not contain the cbr header.
 */

int Sunset_CbrPktConverter::checkPktLevel(Packet* p) 
{
	
	// if the packet type is PT_MCBR, the packet has been created by the cbr module
	if (HDR_CMN(p)->ptype() == PT_MCBR) {
		
		return 1;
	}
	
 	Sunset_Debug::debugInfo(5, -1, "Sunset_CbrPktConverter::checkPktLevel");
	
	return 0;
}

/*!
 * 	@brief The getHeaderSizeBits function returns the total bits size of CBR packet header.
 *	@retval size The total headers size in bits.
 */

int Sunset_CbrPktConverter::getHeaderSizeBits() 
{ 
	return getConvertedInfoLength((Packet*)0);
}

/*!
 * 	@brief The getConvertedInfoLength function returns the total bits size of the converted CBR packet header including the payload of a specific packet.
 *	@param p The considered packet.
 *	@retval len The total headers size.
 */

int Sunset_CbrPktConverter::getConvertedInfoLength(Packet* p) 
{
	int len = 0;
	
	len += getAddrBits(); //source id
	
	len += getAddrBits(); //dest id
	
	len += getPktIdBits(); // common header id
	
	len += getPktIdBits(); // sequence number
	
	len += getPortBits(); // src port number
	
	len += getPortBits(); // dst port number
	
	if (use_timestamp) {
		
		len += getTimeBits(); //timestamp 
	}
	
	len += getLevelIdBits(); // information about the packet header converter ID has to be always added to allow for correct conversion
	
	Sunset_Debug::debugInfo(3, -1, "Sunset_CbrPktConverter::getConvertedInfoLength lengthBit %d lenghtByte %d", len, (int)ceil((double)len/8.0));
	
	return len;
}

/*!
 * 	@brief The pkt2Buffer function copies all the necessary information from a ns-packet to a buffer at a specified offset.
 *	@param level The layer ID of the cbr header converter.
 * 	@param p A packet from which getting the information.
 * 	@param buffer The destination buffer that contains the packet information of all the stack layers.
 * 	@param offset The offset of buffer from which to start writing information.
 *	@retval size The bit size of the information written.
 */

int Sunset_CbrPktConverter::pkt2Buffer(int level, Packet* p, char* buffer, int offset) 
{
	int aux = 0;
	int size = 0;
	
	hdr_cmn* ch = hdr_cmn::access(p);
	hdr_ip* iph = hdr_ip::access(p);
	hdr_cbr* cbrh = HDR_CBR(p);
	
	aux = getConvertedInfoLength(p); // get the number of bits that have to be written by this packet header converter
	
	if (aux == 0) {
		return 0;
	}
	
	size = getLevelIdBits(); 
	
	// information about the packet header converter ID has to be always written to allow for correct conversion
	
	setBits(buffer, (char *)(&(level)), size, offset);
	
	setBits(buffer, (char *)(&(iph->saddr())), getAddrBits(), offset + size);
	size += getAddrBits();
	
	setBits(buffer, (char *)(&(iph->daddr())), getAddrBits(), offset + size);
	size += getAddrBits();
	
	setBits(buffer, (char *)(&(ch->uid())), getPktIdBits(), offset + size);
	size += getPktIdBits();
	
	setBits(buffer, (char *)(&(cbrh->sn)), getPktIdBits(), offset + size);
	size += getPktIdBits();
	
	setBits(buffer, (char *)(&(iph->dport())), getPortBits(), offset + size);
	size += getPortBits();
	
	setBits(buffer, (char *)(&(iph->sport())), getPortBits(), offset + size);
	size += getPortBits();
	
	if (use_timestamp) {
		
		setBits(buffer, (char *)(&(cbrh->ts)), getTimeBits(), offset + size);
		size += getTimeBits();
	}
	
	// if less bits are written w.r.t. the ones computed using the getConvertedInfoLength an error occurrs and 0 bits are added to the packet
	if (aux != size) {
		
		Sunset_Debug::debugInfo(-1, -1, "Sunset_CbrPktConverter::pkt2Buffer buffer filled ERROR aux %d size %d bits", aux, size);
		
		return 0;
		exit(1);
	}
	
	Sunset_Debug::debugInfo(5, -1, "Sunset_CbrPktConverter::pkt2Buffer buffer filled size %d bits", size);
	
	return size;
}

/*!
 * 	@brief The pkt2Buffer function copies all the necessary information from a buffer at a specified offset to a ns-packet.
 *	@param level The layer ID of the cbr header converter.
 * 	@param p The packet that will contain the information taken from the buffer.
 * 	@param buffer The source buffer that contains the packet information of all the stack layers.
 * 	@param offset The offset used when starting to read from the buffer.
 * 	@param bits The number of bits to be read.
 *	@retval size The number of bits read from the buffer.
 */

int Sunset_CbrPktConverter::buffer2Pkt(int level, Packet* p, char* buffer, int offset, int bits) 
{
	int length = 0;
	hdr_cmn* ch = hdr_cmn::access(p);
	hdr_ip* iph = hdr_ip::access(p);
	hdr_cbr* cbrh = HDR_CBR(p);
	int size = getLevelIdBits();
	int info_level = 0;
	nsaddr_t temp = 0;
	int temp1 = 0;
	double temp2 = 0.0;
	
	// check if there are enough bits to read the packet header converter ID
	if (bits < size) {
		
		Sunset_Debug::debugInfo(5, -1, "Sunset_CbrPktConverter::buffer2Pkt check bits1 bits %d size %d", bits, size);
		
		return 0;
	}
	
	// read the packet header converter ID
	
	getBits(buffer, (char *)(&(info_level)), size, offset);
	
	// check if the packet header converter ID corresponds to my ID
	
	if (info_level != level) {
		
		return 0;
	}
	
	// check if there are enough bits to read the packet header converter info
	
	if (bits < getHeaderSizeBits()) {
		
		Sunset_Debug::debugInfo(5, -1, "Sunset_CbrPktConverter::buffer2Pkt check bits2 bits %d size %d", bits, getHeaderSizeBits());
		
		return -1;
	}
	
	iph->saddr() = 0;
	iph->daddr() = 0;
	cbrh->sn = 0;
	iph->sport() = 0;
	iph->dport() = 0;
	cbrh->ts = 0.0;
	ch->uid() = 0;
	
	temp = 0;
	getBits(buffer, (char *)(&(temp)), getAddrBits(), offset + size);
	size += getAddrBits();
	
	iph->saddr() = temp;
	
	temp = 0;
	getBits(buffer, (char *)(&(temp)), getAddrBits(), offset + size);
	size += getAddrBits();
	
	iph->daddr() = temp;
	
	temp1 = 0;
	getBits(buffer, (char *)(&(temp1)), getPktIdBits(), offset + size);
	size += getPktIdBits();
	
	ch->uid() = temp1;
	
	temp1 = 0;
	getBits(buffer, (char *)(&(temp1)), getPktIdBits(), offset + size);
	size += getPktIdBits();
	
	cbrh->sn = temp1;
	
	temp = 0;
	getBits(buffer, (char *)(&(temp)), getPortBits(), offset + size);
	size += getPortBits();
	
	iph->dport() = temp;
	
	temp = 0;
	getBits(buffer, (char *)(&(temp)), getPortBits(), offset + size);
	size += getPortBits();
	
	iph->sport() = temp;
	
	if (use_timestamp) {
		
		temp2 = 0.0;
		getBits(buffer, (char *)(&(temp2)), getTimeBits(), offset + size);
		size += getTimeBits();
		
		cbrh->ts = temp2;
	}
	
	Sunset_Debug::debugInfo(5, -1, "Sunset_CbrPktConverter::buffer2Pkt cbr info for the packet have been set length %d", length);
	
	return size;
}

/*!	@brief The getSrc function is used to obtain the source ID for the CBR packet header. 
 *	@param level The layer ID of the cbr header converter.
 * 	@param p The packet.
 * 	@param m The packet header selected for checking.
 * 	@param[out] src The Cbr source ID is written by the packet header converter in the src variable.
 * 	@retval 1 if the packet header is the one has been selected according to m value, 0 otherwise.
 */
int Sunset_CbrPktConverter::getSrc(int level, Packet* p, sunset_header_type m, int& src) 
{ 
	hdr_ip* iph = hdr_ip::access(p);
	
	if (m == UW_PKT_CBR) { 
		
		src = iph->saddr();
		return 1;
	} 
	
	return 0;  
}

/*!	@brief The getDst function is used to obtain the destination ID for the CBR packet header. 
 *	@param level The layer ID of the cbr header converter.
 * 	@param p The packet.
 * 	@param m The packet header selected for checking.
 * 	@param[out] dst The Cbr destination ID is written by the packet header converter in the dst variable.
 * 	@retval 1 if the packet header is the one has been selected according to m value, 0 otherwise.
 */

int Sunset_CbrPktConverter::getDst(int level, Packet* p, sunset_header_type m, int& dst)
{ 
	hdr_ip* iph = hdr_ip::access(p);
	
	if (m == UW_PKT_CBR) { 
		
		dst = iph->daddr();
		return 1;
	} 
	
	return 0;  
}

/*!
 * 	@brief The start() function can be called from the TCL script to execute module operations when the emulation starts.
 */

void Sunset_CbrPktConverter::start() 
{	
	Sunset_PktConverter::start();
	assignNsPktType(PT_MCBR);
	Sunset_Debug::debugInfo(5, -1, "Sunset_CbrPktConverter::start");
}

