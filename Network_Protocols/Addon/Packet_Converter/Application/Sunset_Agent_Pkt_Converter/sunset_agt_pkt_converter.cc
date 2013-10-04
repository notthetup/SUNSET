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



#include <sunset_agt_pkt_converter.h>
#include <ip.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_PktAgtConverterClass : public TclClass {
public:
	Sunset_PktAgtConverterClass() : TclClass("Sunset_PktConverter/Agent") {}
	
	TclObject* create(int, const char*const*) {
		
		return (new Sunset_AgtPktConverter());
	}
	
} class_Sunset_PktAgtConverterClass;

Sunset_AgtPktConverter::Sunset_AgtPktConverter() 
{ 
	use_source = 0;
	use_dest = 0;
	use_pktId = 0;
	use_data = 0;
	
	bind("use_source", &use_source);
	bind("use_dest", &use_dest);
	bind("use_pktId", &use_pktId);
	bind("use_data", &use_data);
	
	Sunset_Debug::debugInfo(-1, -1, "Sunset_AgtPktConverter CREATED");
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_AgtPktConverter::command( int argc, const char*const* argv) 
{
	Tcl& tcl = Tcl::instance();
	
	if (argc == 3) {
		
		/* The "useSource" command is set to 1 if the source ID has to be considered during the conversion of the ns-2 packet into a stream of bytes for external devices transmissions. */
		if (strcmp(argv[1], "useSource") == 0) {
			
			use_source = atoi(argv[2]);
			
			return TCL_OK;
		}
		
		/* The "useDest" command is set to 1 if the destination ID has to be considered during the conversion of the ns-2 packet into a stream of bytes for external devices transmissions. */
		if (strcmp(argv[1], "useDest") == 0) {
			
			use_dest = atoi(argv[2]);
			
			return TCL_OK;
		}
		
		/* The "usePktId" command is set to 1 if the packet ID has to be considered during the conversion of the ns-2 packet into a stream of bytes for external devices transmissions. */
		if (strcmp(argv[1], "usePktId") == 0) {
			
			use_pktId = atoi(argv[2]);
			
			return TCL_OK;
			
		}
		/* The "useData" command is set to 1 if the data payload has to be considered during the conversion of the ns-2 packet into a stream of bytes for external devices transmissions. */
		if (strcmp(argv[1], "useData") == 0) {
			
			use_data = atoi(argv[2]);
			
			return TCL_OK;
		}
	}
	
	return Sunset_PktConverter::command(argc, argv);
}

/*!
 * 	@brief The checkPktLevel function checks if the agent header converter has to convert its header information in the specified packet. 
 *	@param p The received packet.
 *	@retval 1 The packet contains the agent header.
 *	@retval 0 The packet does not contain the agent header.
 */

int Sunset_AgtPktConverter::checkPktLevel(Packet* p) 
{
	struct hdr_Sunset_Agent *gh = hdr_Sunset_Agent::access(p);
	
	u_int8_t  type = gh->ac.ac_type;
	u_int8_t  subtype = gh->ac.ac_subtype;
	
	// if the packet type is PT_SUNSET_AGT, the packet has been created by an agent module and data conversion is needed
	if (HDR_CMN(p)->ptype() == PT_SUNSET_AGT) {
		return 1;
	}
	
	// if the packet type is not PT_SUNSET_AGT but agent information have been set data conversion is needed
	
	switch(type) {
			
		case SUNSET_AGT_Type_Data:
			
			switch(subtype) {
					
				case SUNSET_AGT_Subtype_Data:
					
					return 1;
					
				case SUNSET_AGT_Subtype_Request:
					
					return 1;
					
				case SUNSET_AGT_Subtype_Response:
					
					return 1;
					
				default:
					Sunset_Debug::debugInfo(3, -1, "Sunset_Agent::getBufferInfoLength DATA type %d subtype %d", type, subtype);
					
					return 0;
			}
			
			break;
			
		default:
			
			Sunset_Debug::debugInfo(3, -1, "Sunset_Agent::getBufferInfoLength default type %d", type);
			
			return 0;
	}
	
	Sunset_Debug::debugInfo(5, -1, "Sunset_AgtPktConverter::checkPktLevel");
	
	return 0;
}

/*!
 * 	@brief The erasePkt erases the packet and the data payload stored in the agent header.
 *	@param level The layer ID of the agent header converter.
 *	@param p The packet to be erased.
 */

void Sunset_AgtPktConverter::erasePkt(int level, Packet* p) 
{
	if ( checkPktLevel(p) ) {
		
		if (use_data && HDR_SUNSET_AGT(p)->getData() != NULL) {	
			
			//remove the memory allocated for the packet payload
			
			free(HDR_SUNSET_AGT(p)->getData());
			
			HDR_SUNSET_AGT(p)->data = NULL;
		}
	}	
}

/*!
 * 	@brief The getHeaderSizeBits returns the bits size of the agent layer header for the converted packet. The data payload is not considered.
 */

int Sunset_AgtPktConverter::getHeaderSizeBits() 
{ 
	int len = 0;
	
	len += ((int)(sizeof(struct sunset_agt_control))) * 8; //control information in the agent header
	
	// According to the used header values, the corresponding number of bits needed during the conversion process is computed 
	
	if (use_source) {  
		
		len += getAddrBits(); //source id
	}
	
	if (use_dest) {
		
		len += getAddrBits(); //dest id
	}
	
	if (use_pktId) {
		
		len += getPktIdBits(); // packet id
	}
	
	if (use_data) {
		
		len += getDataBits(); //source id
	}
	
	len += getLevelIdBits(); // information about the packet header converter ID has to be always added to allow for correct conversion
	
	Sunset_Debug::debugInfo(3, -1,  "Sunset_AgtPktConverter::getHeaderSizeBits length %d", len);
	
	return len;	
}

/*!
 * 	@brief The getConvertedInfoLength function returns the total bits size of the converted agent packet header including the payload of a specific packet.
 *	@param p The considered packet.
 *	@retval len The total headers size.
 */

int Sunset_AgtPktConverter::getConvertedInfoLength(Packet* p) 
{
	int len = 0;
	
	len += ((int)sizeof(struct sunset_agt_control)) * 8; //control information in the agent header
	len += getLevelIdBits(); // packet header converter ID bits
	
	// According to the used header values, the corresponding number of bits needed during the conversion process is computed 
	
	if (use_source) {
		
		len += getAddrBits(); //source id
	}
	
	if (use_dest) {
		
		len += getAddrBits(); //dest id
	}
	
	if (use_pktId) {
		
		len += getPktIdBits(); 
	}
	
	if (use_data) {
		
		len += getDataBits(); /* bits to store the size of the payload */
		
		Sunset_Debug::debugInfo(3, -1, "Sunset_AgtPktConverter::getConvertedInfoLength data %d dataSizeB %d", len, HDR_SUNSET_AGT(p)->dataSize() * 8);
		
		len += HDR_SUNSET_AGT(p)->dataSize() * 8; /* data payload */
	}
	
	Sunset_Debug::debugInfo(3, -1, "Sunset_AgtPktConverter::getConvertedInfoLength lengthBit %d lenghtByte %d data %d", len, (int)ceil((double)len/8.0), HDR_SUNSET_AGT(p)->dataSize());
	
	return len;
}

/*!
 * 	@brief The pkt2Buffer function copies all the necessary information from a ns-packet to a buffer at a specified offset.
 *	@param level The layer ID of the agent header converter.
 * 	@param p A packet from which getting the information.
 * 	@param buffer The destination buffer that contains the packet information of all the stack layers.
 * 	@param offset The offset of buffer from which to start writing information.
 *	@retval size The bit size of the information written.
 */

int Sunset_AgtPktConverter::pkt2Buffer(int level, Packet* p, char* buffer, int offset) 
{
	int aux = 0;
	int size = 0;
	
	aux = getConvertedInfoLength(p); // get the number of bits that have to be written by this packet header converter
	
	if (aux == 0) {
		return 0;
	}
	
	size = getLevelIdBits(); 
	
	// information about the packet header converter ID has to be always written to allow for correct conversion
	
	setBits(buffer, (char *)(&(level)), size, offset);
	setBits(buffer, (char *)(&(HDR_SUNSET_AGT(p)->ac)), (int)((sizeof(sunset_agt_control)) * 8), offset + size);
	
	size += (int)((sizeof(sunset_agt_control)) * 8);
	
	// According to the used header values, the corresponding number of bits needed during the conversion process is written 
	
	if (use_source) {
		
		setBits(buffer, (char *)(&(HDR_SUNSET_AGT(p)->srcId())), getAddrBits(), offset + size);
		size += getAddrBits();
	}
	
	if (use_dest) {
		
		setBits(buffer, (char *)(&(HDR_SUNSET_AGT(p)->dstId())), getAddrBits(), offset + size);
		size += getAddrBits();
	}
	
	if (use_pktId) {
		
		setBits(buffer, (char *)(&(HDR_SUNSET_AGT(p)->pktId())), getPktIdBits(), offset + size);
		size += getPktIdBits();
	}
	
	if (use_data) {
		
		setBits(buffer, (char *)(&(HDR_SUNSET_AGT(p)->dataSize())), getDataBits(), offset + size);
		size += getDataBits();
		
		if ((HDR_SUNSET_AGT(p)->dataSize()) > 0) {
			setBits(buffer, (HDR_SUNSET_AGT(p)->getData()), (HDR_SUNSET_AGT(p)->dataSize() * 8), offset + size);
			size += (HDR_SUNSET_AGT(p)->dataSize() * 8);

			Sunset_Debug::debugInfo(5, -1, "Sunset_AgtPktConverter::pkt2Buffer AGENT data len %d msg:%s", (HDR_SUNSET_AGT(p)->dataSize()), HDR_SUNSET_AGT(p)->data);
		}
		else {
			
			Sunset_Debug::debugInfo(5, -1, "Sunset_AgtPktConverter::pkt2Buffer AGENT data NO DATA");
		}
	}
	
	// if less bits are written w.r.t. the ones computed using the getConvertedInfoLength an error occurrs and 0 bits are added to the packet
	if (aux != size) {
		
		Sunset_Debug::debugInfo(-1, -1, "Sunset_AgtPktConverter::pkt2Buffer buffer filled ERROR aux %d size %d bits", aux, size);
		
		return 0;
		exit(1);
	}
	
	Sunset_Debug::debugInfo(5, -1, "Sunset_AgtPktConverter::pkt2Buffer type %d subtype %d version %d src %d dst %d pktId %d dataSize %d", HDR_SUNSET_AGT(p)->ac.ac_type, HDR_SUNSET_AGT(p)->ac.ac_subtype, HDR_SUNSET_AGT(p)->ac.ac_protocol_version, HDR_SUNSET_AGT(p)->srcId(), HDR_SUNSET_AGT(p)->dstId(), HDR_SUNSET_AGT(p)->pktId(), HDR_SUNSET_AGT(p)->dataSize());
	
	Sunset_Debug::debugInfo(5, -1, "AGENT data %s", HDR_SUNSET_AGT(p)->data);
	Sunset_Debug::debugInfo(5, -1, "Sunset_AgtPktConverter::pkt2Buffer buffer filled size %d bits", size);
	
	return size;
}

/*!
 * 	@brief The pkt2Buffer function copies all the necessary information from a buffer at a specified offset to a ns-packet.
 *	@param level The layer ID of the agent header converter.
 * 	@param p The packet that will contain the information taken from the buffer.
 * 	@param buffer The source buffer that contains the packet information of all the stack layers.
 * 	@param offset The offset used when starting to read from the buffer.
 * 	@param bits The number of bits to be read.
 *	@retval size The number of bits read from the buffer.
 */

int Sunset_AgtPktConverter::buffer2Pkt(int level, Packet* p, char* buffer, int offset, int bits) 
{
	int length = 0;
	struct hdr_ip* iph = HDR_IP(p);
	int size = getLevelIdBits();
	int info_level = 0;
	
	// check if there are enough bits to read the packet header converter ID
	if (bits < size) {
		
		Sunset_Debug::debugInfo(5, -1, "Sunset_AgtPktConverter::buffer2Pkt check bits1 bits %d size %d", bits, size);
		
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
		
		Sunset_Debug::debugInfo(5, -1, "Sunset_AgtPktConverter::buffer2Pkt check bits2 bits %d size %d", bits, getHeaderSizeBits());
		
		return -1;
	}
	
	HDR_SUNSET_AGT(p)->srcId() = 0;
	HDR_SUNSET_AGT(p)->dstId() = 0;
	HDR_SUNSET_AGT(p)->pktId() = 0;
	HDR_SUNSET_AGT(p)->dataSize() = 0;
	
	getBits(buffer, (char *)(&(HDR_SUNSET_AGT(p)->ac)), (int)((sizeof(sunset_agt_control)) * 8), offset + size);
	size += (int)((sizeof(sunset_agt_control)) * 8);
	
	// According to the used header values, the corresponding number of bits needed during the conversion process is written 
	
	if (use_source) {
		
		int srcId = 0;
		
		getBits(buffer, (char *)(&(srcId)), getAddrBits(), offset + size);
		size += getAddrBits();
		
		HDR_SUNSET_AGT(p)->srcId() = srcId;
		iph->saddr() = HDR_SUNSET_AGT(p)->srcId();
	}
	
	if (use_dest) {
		
		int dstId = 0;
		
		getBits(buffer, (char *)(&(dstId)), getAddrBits(), offset + size);
		size += getAddrBits();
		
		HDR_SUNSET_AGT(p)->dstId() = dstId;
		iph->daddr() = HDR_SUNSET_AGT(p)->dstId();
	}
	
	if (use_pktId) {
		
		int pktId = 0;
		
		getBits(buffer, (char *)(&(pktId)), getPktIdBits(), offset + size);
		size += getPktIdBits();
		
		HDR_SUNSET_AGT(p)->pktId() = pktId;
	}
	
	else {
	
		HDR_SUNSET_AGT(p)->pktId() = HDR_CMN(p)->uid();
	}
	
	if (use_data) {
		
		int dataSize = 0;
		
		getBits(buffer, (char *)(&(dataSize)), getDataBits(), offset + size);
		size += getDataBits();
		
		HDR_SUNSET_AGT(p)->dataSize() = dataSize;
		length += (HDR_SUNSET_AGT(p)->dataSize());
		
		HDR_CMN(p)->size() += length;
		
		if ((HDR_SUNSET_AGT(p)->dataSize()) > 0 ) {
			
			HDR_SUNSET_AGT(p)->data = (char*)(malloc((int)(HDR_SUNSET_AGT(p)->dataSize()) + 1));
			
			memset(HDR_SUNSET_AGT(p)->data, '\0', (int)(HDR_SUNSET_AGT(p)->dataSize() + 1));
			
			getBits(buffer, (HDR_SUNSET_AGT(p)->data), HDR_SUNSET_AGT(p)->dataSize() * 8, offset + size);
			size += HDR_SUNSET_AGT(p)->dataSize() * 8;
			
			//HDR_SUNSET_AGT(p)->getData() = HDR_SUNSET_AGT(p)->data;
			
			Sunset_Debug::debugInfo(5, -1, "Sunset_AgtPktConverter::buffer2Pkt AGENT data len %d msg:%s", (HDR_SUNSET_AGT(p)->dataSize()), HDR_SUNSET_AGT(p)->data);
		}
		else {
			
			Sunset_Debug::debugInfo(5, -1, "Sunset_AgtPktConverter::buffer2Pkt AGENT data NO DATA");
		}
	}
	
	Sunset_Debug::debugInfo(5, -1, "Sunset_AgtPktConverter::buffer2Pkt type %d subtype %d version %d src %d dst %d pktId %d dataSize %d length %d", HDR_SUNSET_AGT(p)->ac.ac_type, HDR_SUNSET_AGT(p)->ac.ac_subtype, HDR_SUNSET_AGT(p)->ac.ac_protocol_version, HDR_SUNSET_AGT(p)->srcId(), HDR_SUNSET_AGT(p)->dstId(), HDR_SUNSET_AGT(p)->pktId(), HDR_SUNSET_AGT(p)->dataSize(), length);
	Sunset_Debug::debugInfo(5, -1, "Sunset_AgtPktConverter::buffer2Pkt agent info for the packet have been set length %d", length);
	
	return size;
}

/*!
 * 	@brief The buffer2Pkt function converts a stream of bytes into an agent packet header when link source and destination are specified in the input.
 *	@param level The layer ID of the agent header converter.
 *	@param src The link source ID.
 *	@param dst The link destination ID.
 * 	@param p The packet that will contain the information taken from the buffer.
 * 	@param buffer The source buffer that contains the packet information of all the stack layers.
 * 	@param offset The offset used when starting to read from the buffer.
 * 	@param bits The number of bits to be read.
 *	@retval size The number of bits read from the buffer.
 */

int Sunset_AgtPktConverter::buffer2Pkt(int level, int src, int dst, Packet* p, char* buffer, int offset, int bits) 
{
	return buffer2Pkt(level, p, buffer, offset, bits);
}

/*!	@brief The getSrc function is used to obtain the source ID for the agent packet header. 
 *	@param level The layer ID of the agent header converter.
 * 	@param p The packet.
 * 	@param m The packet header selected for checking.
 * 	@param[out] src The Agent source ID is written by the packet header converter in the src variable.
 * 	@retval 1 if the packet header is the one has been selected according to m value, 0 otherwise.
 */
int Sunset_AgtPktConverter::getSrc(int level, Packet* p, sunset_header_type m, int& src) 
{ 
	if (m == UW_PKT_AGENT) { 
		
		src = HDR_SUNSET_AGT(p)->srcId();
		return 1;
	} 
	
	return 0;  
}

/*!	@brief The getDst function is used to obtain the destination ID for the agent packet header. 
 *	@param level The layer ID of the agent header converter.
 * 	@param p The packet.
 * 	@param m The packet header selected for checking.
 * 	@param[out] dst The Agent destination ID is written by the packet header converter in the dst variable.
 * 	@retval 1 if the packet header is the one has been selected according to m value, 0 otherwise.
 */

int Sunset_AgtPktConverter::getDst(int level, Packet* p, sunset_header_type m, int& dst)
{ 
	if (m == UW_PKT_AGENT) { 
		
		dst = HDR_SUNSET_AGT(p)->dstId();
		return 1;
	} 
	
	return 0;  
}

/*!	@brief The getPktSubType function is used to obtain the packet subtype for the agent packet header. 
 *	@param level The layer ID of the agent header converter.
 * 	@param p The packet.
 * 	@param m The packet header selected for checking.
 * 	@param[out] subType The agent packet subtype is written by the packet header converter in the subType variable.
 * 	@retval 1 if the packet header is the one has been selected according to m value, 0 otherwise.
 */

int Sunset_AgtPktConverter::getPktSubType(int level, Packet* p, sunset_header_type m, int& subType) 
{ 
	if ( m == UW_PKT_AGENT ) { 
		
		subType = HDR_SUNSET_AGT(p)->ac.ac_subtype; 
		
		return 1;
	} 
	
	return 0;  
}

/*! @brief The getPktType function is used to obtain the packet type for the agent packet header.
 *	@param level The layer ID of the agent header converter.
 * 	@param p The packet.
 * 	@param m The packet header selected for checking.
 * 	@param[out] type The agent packet type is written by the packet header converter in the type variable.
 * 	@retval 1 if the packet header is the one has been selected according to m value, 0 otherwise.
 */

int Sunset_AgtPktConverter::getPktType(int level, Packet* p, sunset_header_type m, int& type)
{ 
	if ( m == UW_PKT_AGENT ) { 
		
		type = HDR_SUNSET_AGT(p)->ac.ac_subtype; 
		
		return 1;  
	} 
	
	return 0;
}

/*!
 * 	@brief The data_copy function copies packet p information in packet copy allocating additional memory for the packet payload. 
 * 	@param p The packet to copy.
 * 	@param copy The copied packet.
 */

int Sunset_AgtPktConverter::data_copy(int  level, Packet *p, Packet* copy) 
{ 
	
	if (use_data && (HDR_SUNSET_AGT(p)->dataSize()) > 0) {		

		int length = (HDR_SUNSET_AGT(p)->dataSize());		
		
		if (length > 0) {
			
			HDR_SUNSET_AGT(copy)->data = (char*)(malloc( length + 1));		
			memset(HDR_SUNSET_AGT(copy)->data, '\0', length + 1);
			memcpy(HDR_SUNSET_AGT(copy)->data, HDR_SUNSET_AGT(p)->data, length); 
			
			Sunset_Debug::debugInfo(5, -1, "AGENT pkt_copy data %s len %d", HDR_SUNSET_AGT(copy)->data, length);
		}
		else {
			Sunset_Debug::debugInfo(5, -1, "AGENT pkt_copy ERROR");		
			
			return 0;
		}
	}
	
	return 1;
}

/*!
 * 	@brief The start() function can be called from the TCL script to execute module operations when the emulation starts.
 */

void Sunset_AgtPktConverter::start() 
{	
	Sunset_PktConverter::start();
	assignNsPktType(PT_SUNSET_AGT);

	Sunset_Debug::debugInfo(5, -1, "Sunset_AgtPktConverter::start");
}