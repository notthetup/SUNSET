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



#include <sunset_mac_pkt_converter.h>

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_PktMacConverterClass : public TclClass
{
public:
	
	Sunset_PktMacConverterClass() : TclClass("Sunset_PktConverter/Mac") {}
	
	TclObject* create(int, const char*const*) {
		return (new Sunset_MacPktConverter());
	}
	
} class_Sunset_PktMacConverterClass;

Sunset_MacPktConverter::Sunset_MacPktConverter() 
{ 
	use_source = 0;
	use_dest = 0;
	use_pktId = 0;
	
	TYPE_BITS = 2; 		//set according to MAC packet header
	SUBTYPE_BITS = 4;	//set according to MAC packet header
	
	bind("use_source", &use_source);
	bind("use_pktId", &use_pktId);
	bind("use_dest", &use_dest);
	
	Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter CREATED");
}

/*!
 * 	@brief The checkPktLevel function checks if the MAC packet header converter has to convert its header information in the specified packet. 
 *	@retval 0 The MAC packet header converter does not convert its header information in the specified packet.
 *	@retval 1 The MAC packet header converter has to convert its header information in the specified packet.
 */

int Sunset_MacPktConverter::checkPktLevel(Packet* p) 
{
	struct hdr_Sunset_Mac *mh = HDR_SUNSET_MAC(p);
	
	u_int8_t  type = mh->dh_fc.fc_type;
	u_int8_t  subtype = mh->dh_fc.fc_subtype;
	
	Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter::checkPktLevel");
	
	/* I check if the type and subtype of the packet are MAC types */
	
	if (type == SUNSET_MAC_Type_Control || type == SUNSET_MAC_Type_Data) {
		
		return 1;
	}
	
	return 0;
	
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_MacPktConverter::command( int argc, const char*const* argv ) 
{
	Tcl& tcl = Tcl::instance();
	
	if (argc == 3) {
		
		/* The "useSource" command is set to 1 if the source ID of the MAC header has to be used during packet conversion. The use of the MAC source ID is mandatory if the acoustic modem does not insert this information in the modem header. Otherwise modem information can be used instead of the MAC one to reduce the total packet size to be sent. */
		
		if (strcmp(argv[1], "useSource") == 0) {
			
			use_source = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter::command use_source %d", use_source);
			
			return TCL_OK;
		}
		
		/* The "useDest" command is set to 1 if the destination ID of the MAC header has to be used during packet conversion. The use of the MAC destination ID is mandatory if the acoustic modem does not insert this information in the modem header. Otherwise modem information can be used instead of the MAC one to reduce the total packet size to be sent. */
		
		if (strcmp(argv[1], "useDest") == 0) {
			
			use_dest = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter::command use_dest %d", use_dest);

			return TCL_OK;
		}
		
		/* The "usepktId" command is set to 1 if the pkt ID of the MAC header has to be used during packet conversion. The use of the MAC destination ID is mandatory if the acoustic modem does not insert this information in the modem header. Otherwise modem information can be used instead of the MAC one to reduce the total packet size to be sent. */
		
		if (strcmp(argv[1], "usePktId") == 0) {
			
			use_pktId = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter::command use_pktId %d", use_pktId);

			return TCL_OK;
		}
	}
	
	return Sunset_PktConverter::command(argc, argv);
}

/*!
 * 	@brief The getHeaderSizeBits function returns the total bits size of the MAC packet header.
 *	@retval size The total headers size.
 */

int Sunset_MacPktConverter::getHeaderSizeBits() 
{ 
	return getConvertedInfoLength((Packet*)0);
}

/*!
 * 	@brief The getConvertedInfoLength is used to compute the number of bits needed to convvert the MAC packet header.
 *	@param p The converted packet.
 * 	@retval len The bits size of the converted packet p.
 */

int Sunset_MacPktConverter::getConvertedInfoLength(Packet* p) 
{
	int len = 0;
	
	len += getLevelIdBits(); // packet header converter ID bits
	
	//control information in the mac packet header
	len += TYPE_BITS;
	len += SUBTYPE_BITS;
	
	// According to the used header values, the corresponding number of bits needed during the conversion process is computed 
	
	if (use_source) {
		
		len += getAddrBits(); //source id
	}
	
	if (use_dest) {
		
		len += getAddrBits(); //dest id
	}
	
	if (use_pktId) {
		
		len += getPktIdBits(); //pkt id
	}
	
	Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter::getConvertedInfoLength size %d", len);
	
	return len;
}

/*!
 * 	@brief The checkPktSubType checks if packet subtype belongs to the MAC header.
 * 	@retval 1 The packet subtype belongs to the MAC layer.
 * 	@retval 1 The packet subtype does not belong to the MAC layer.
 */


int Sunset_MacPktConverter::checkPktSubType(int pkt_sub_type) 
{
	switch(pkt_sub_type) {
			
		case SUNSET_MAC_Subtype_RTS:
			return 1;
			
		case SUNSET_MAC_Subtype_CTS:
			return 1;
			
		case SUNSET_MAC_Subtype_WARN:
			return 1;
			
		case SUNSET_MAC_Subtype_TONE:
			return 1;
			
		case SUNSET_MAC_Subtype_ACK:
			return 1;
			
		default:
			Sunset_Debug::debugInfo(-1, -1, "Sunset_MacPktConverter::checkPktSubType invalid MAC Control Subtype %d ERROR", pkt_sub_type);
			
			return 0;   //TODO: it's just to avoid exiting the simulation
	}
	
	return 0;
}

/*!
 * 	@brief The createMiniPkt function creates a short packet containing only the information on packet source, destination and subtype (Currently used for WHOI MicroModem).
 *	@param level The layer ID of the MAC packet header converter.
 *	@param p The packet containing the information.
 * 	@param m The packet header creating the mini packet.
 * 	@param src The packet source id.
 * 	@param dst The packet destination id.
 * 	@param pkt_sub_type The packet subtype.
 *	@retval p The created short packet.
 */

int Sunset_MacPktConverter::createMiniPkt(int level, Packet* p, sunset_header_type m, int src, int dst, int pkt_sub_type) 
{
	hdr_cmn* ch;
	struct hdr_Sunset_Mac *dh;
	int maxId = 0;
	
	// if the mini pkt to create is not a MAC mini packet exit
	
	if (m != UW_PKT_MAC) { 
		
		return 1;
	}
	
	ch = HDR_CMN(p);
	dh = (struct hdr_Sunset_Mac*)p->access(hdr_Sunset_Mac::offset_);
	
	ch->uid() = 0;
	ch->ptype() = PT_SUNSET_MAC;
	
	// dummy information added to MAC mini packet exit
	
	ch->size() = 10;
	ch->iface() = -2;
	ch->error() = 0;
	
	maxId = (int)pow(2.0, getAddrBits());
	
	if (src > maxId || dst > maxId) {
		
		Sunset_Debug::debugInfo(-1, -1, "Sunset_MacPktConverter::createMiniPkt ERROR src %d dst %d maxId %d", src, dst, maxId);
		
		ch->error() = 1;
	}
	
	if (checkPktSubType(pkt_sub_type) != 1) {
		
		Sunset_Debug::debugInfo(-1, -1, "Sunset_MacPktConverter::createMiniPkt ERROR pkt_sub_type %d", pkt_sub_type);	
		
		ch->error() = 1;
	}
	
	bzero(dh, MAC_HDR_LEN);
	
	dh->dh_fc.fc_protocol_version = SUNSET_MAC_ProtocolVersion;
 	dh->dh_fc.fc_type	= SUNSET_MAC_Type_Control;
 	dh->dh_fc.fc_subtype	= pkt_sub_type;
 	dh->dh_fc.fc_to_ds	= 0;
 	dh->dh_fc.fc_from_ds	= 0;
 	dh->dh_fc.fc_more_frag	= 0;
 	dh->dh_fc.fc_retry	= 0;
 	dh->dh_fc.fc_pwr_mgt	= 0;
 	dh->dh_fc.fc_more_data	= 0;
 	dh->dh_fc.fc_wep	= 0;
 	dh->dh_fc.fc_order	= 0;
	
	dh->dst = dst;
	dh->src = src;
	
	/* store ack tx time */
 	ch->txtime() = 1.0;
	HDR_CMN(p)->timestamp() = NOW;
	
	/* calculate ack duration */
 	dh->dh_duration = 0;	
	
	Sunset_Debug::debugInfo(2, -1, "Sunset_MacPktConverter::createMiniPkt");
	
	return 1;
}

/*!
 * 	@brief The isDataPkt function checks if the packet is a data packet according to the MAC packet header.
 *	@param level The layer ID of the MAC packet header converter.
 * 	@param p The packet.
 * 	@param m The packet header selected for checking.
 * 	@retval 1 if the packet header is the one has been selected according to m value, 0 otherwise.
 */

int Sunset_MacPktConverter::isDataPkt(int level, Packet* p, sunset_header_type m, int& result) 
{
	
	struct hdr_Sunset_Mac *dh;
	hdr_cmn* ch;
	
	// if it is not a MAC packet return 0
	
	if (m != UW_PKT_MAC) { 
		
		return 0;
	}
	
	ch = HDR_CMN(p);
	
	dh = (struct hdr_Sunset_Mac*)p->access(hdr_Sunset_Mac::offset_);
	
	if (dh->dh_fc.fc_type == SUNSET_MAC_Type_Data) {
		
		result = 1;
	}
	
	return 1;
}

/*!
 * 	@brief The isCtrlPkt function checks if the packet is a control packet according to the MAC packet header.
 *	@param level The layer ID of the MAC packet header converter.
 * 	@param p The packet.
 * 	@param m The packet header selected for checking
 * 	@param[out] result It is 1 if the packet is a control packet, 0 otherwise.
 * 	@retval 1 if the packet header is the one has been selected according to m value, 0 otherwise.
 */

int Sunset_MacPktConverter::isCtrlPkt(int level, Packet* p, sunset_header_type m, int& result) 
{
	hdr_cmn* ch;
	struct hdr_Sunset_Mac *dh;
	
	// if it is not a MAC packet return 0
	
	if (m != UW_PKT_MAC) { 
		
		return 0;
	}
	
	ch = HDR_CMN(p);
	
	dh = (struct hdr_Sunset_Mac*)p->access(hdr_Sunset_Mac::offset_);
	
	if (ch->ptype() == PT_SUNSET_MAC && dh->dh_fc.fc_type == SUNSET_MAC_Type_Control) {
		
		result = 1;
	}
	
	return 1;
}

/*!
 * 	@brief The useMiniPkt function checks if the packet can be sent using a short packet according to the MAC packet header.
 *	@param level The layer ID of the MAC packet header converter.
 * 	@param p The packet.
 * 	@param m The packet header selected for checking
 * 	@param[out] result It is 1 if the packet can be sent as mini packet, 0 otherwise.
 * 	@retval 1 if the packet header is the one has been selected according to m value, 0 otherwise.
 */

int Sunset_MacPktConverter::useMiniPkt(int level, Packet* p, sunset_header_type m, int& result) 
{
	
	hdr_cmn* ch;
	struct hdr_Sunset_Mac *dh;
	
	// if it is not a MAC packet return 0
	
	if (m != UW_PKT_MAC) { 
		
		return 0;
	}
	
	ch = HDR_CMN(p);
	
	dh = (struct hdr_Sunset_Mac*)p->access(hdr_Sunset_Mac::offset_);
	
	if (ch->ptype() == PT_SUNSET_MAC && dh->dh_fc.fc_type == SUNSET_MAC_Type_Control) {
		
		result = 1;
	}
	
	return 1;
}

/*!
 * 	@brief The pkt2Buffer function converts a MAC packet header in a stream of bytes.
 *	@param level The layer ID of the MAC packet header converter.
 *	@param p The packet containing the information to convert.
 * 	@param[out] buffer The converted buffer.
 * 	@param offset The offset used when starting to write into buffer.
 *	@retval size The buffer size in bits.
 */

int Sunset_MacPktConverter::pkt2Buffer(int level, Packet* p, char* buffer, int offset) 
{
	int aux = 0;
	int size = 0;
	
	aux = getConvertedInfoLength(p); // get the number of bits that have to be written by this packet header converter
	
	if (aux == 0) {
		
		return 0;
	}
	
	u_int8_t type = HDR_SUNSET_MAC(p)->dh_fc.fc_type;
	u_int8_t subtype = HDR_SUNSET_MAC(p)->dh_fc.fc_subtype;
	
	size = getLevelIdBits();
	
	// information about the packet header converter ID has to be always written to allow for correct conversion
	
	setBits(buffer, (char *)(&(level)), size, offset);
	
	setBits(buffer, (char *)(&type), TYPE_BITS, offset + size);
	size += TYPE_BITS;
	
	setBits(buffer, (char *)(&subtype), SUBTYPE_BITS, offset + size);
	size += SUBTYPE_BITS;
	
	// According to the used header values, the corresponding number of bits needed during the conversion process is written 
	
	if (use_source) {
		
		setBits(buffer, (char *)(&(HDR_SUNSET_MAC(p)->src)), getAddrBits(), offset + size);
		size += getAddrBits();
	}
	
	if (use_dest) {
		
		setBits(buffer, (char *)(&(HDR_SUNSET_MAC(p)->dst)), getAddrBits(), offset + size);
		size += getAddrBits();
	}
	
	if (use_pktId) {
		
		setBits(buffer, (char *)(&(HDR_SUNSET_MAC(p)->pktId)), getPktIdBits(), offset + size);
		size += getPktIdBits();
	}
	
	// if less bits are written w.r.t. the ones computed using the getConvertedInfoLength an error occurrs and 0 bits are added to the packet
	if (aux != size) {
		
		Sunset_Debug::debugInfo(-1, -1, "Sunset_AgtPktConverter::pkt2Buffer buffer filled ERROR aux %d size %d bits", aux, size);
		return -1;
		exit(1);
	}
	
	Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter::pkt2Buffer buffer filled size %d bits", size);
	Sunset_Debug::debugInfo(3, -1, "Sunset_MacPktConverter::pkt2Buffer MAC type %d subtype %d version %d duration %d src %d dst %d", HDR_SUNSET_MAC(p)->dh_fc.fc_type, HDR_SUNSET_MAC(p)->dh_fc.fc_subtype, HDR_SUNSET_MAC(p)->dh_fc.fc_protocol_version, HDR_SUNSET_MAC(p)->dh_duration, HDR_SUNSET_MAC(p)->src, HDR_SUNSET_MAC(p)->dst, HDR_SUNSET_MAC(p)->pktId);
	
	return size;
}

/*!
 * 	@brief The buffer2Pkt function converts a stream of bytes into a MAC packet header.
 *	@param level The layer ID of the MAC packet header converter.
 * 	@param p The packet that will contain the information taken from the buffer.
 * 	@param buffer The source buffer that contains the packet information of all the stack layers.
 * 	@param offset The offset used when starting to read from the buffer.
 * 	@param bits The number of bits to be read.
 *	@retval size The number of bits read from the buffer.
 */

int Sunset_MacPktConverter::buffer2Pkt(int level, Packet* p, char* buffer, int offset, int bits) 
{
	int info_level = 0;
	int size = getLevelIdBits();
	
	// check if there are enough bits to read the packet header converter ID
	
	if (bits < size) {
		
		Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter::buffer2Pkt check bits1 bits %d size %d", bits, size);
		return 0;
	}
	
	// read the packet header converter ID
	
	getBits(buffer, (char *)(&(info_level)), size, offset);
	
	// check if the packet header converter ID corresponds to my ID
	
	if (info_level != level) {
		
		Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter::buffer2Pkt info_level %d level %d level_bits %d", 
					info_level, level, getLevelIdBits());

		return 0;
	}
	
	// check if there are enough bits to read the packet header converter info
	
	if (bits < getHeaderSizeBits()) {
		
		Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter::buffer2Pkt check bits2 bits %d size %d", bits, getHeaderSizeBits());
		
		return -1;
	}
	
	HDR_SUNSET_MAC(p)->src = 0;
	HDR_SUNSET_MAC(p)->dst = 0;
	
	u_int8_t type = 0;
	HDR_SUNSET_MAC(p)->dh_fc.fc_type = 0;
	u_int8_t subtype = 0;
	HDR_SUNSET_MAC(p)->dh_fc.fc_subtype = 0;
	
	getBits(buffer, (char *)(&type), TYPE_BITS, offset + size);
	size += TYPE_BITS;
	HDR_SUNSET_MAC(p)->dh_fc.fc_type = type;
	
	getBits(buffer, (char *)(&subtype), SUBTYPE_BITS, offset + size);
	size += SUBTYPE_BITS;
	HDR_SUNSET_MAC(p)->dh_fc.fc_subtype = subtype;
	
	// According to the used header values, the corresponding number of bits needed during the conversion process is written 
	
	if (use_source) {
		
		getBits(buffer, (char *)(&(HDR_SUNSET_MAC(p)->src)), getAddrBits(), offset + size);
		size += getAddrBits();
	}
	
	if (use_dest) {
		
		getBits(buffer, (char *)(&(HDR_SUNSET_MAC(p)->dst)), getAddrBits(), offset + size);
		size += getAddrBits();
	}
	
	if (use_pktId) {
		
		getBits(buffer, (char *)(&(HDR_SUNSET_MAC(p)->pktId)), getPktIdBits(), offset + size);
		size += getPktIdBits();
	}
	
	Sunset_Debug::debugInfo(3, -1, "Sunset_MacPktConverter::buffer2PktMAC type %d subtype %d version %d duration %d src %d dst %d", HDR_SUNSET_MAC(p)->dh_fc.fc_type, HDR_SUNSET_MAC(p)->dh_fc.fc_subtype, HDR_SUNSET_MAC(p)->dh_fc.fc_protocol_version, HDR_SUNSET_MAC(p)->dh_duration, HDR_SUNSET_MAC(p)->src, HDR_SUNSET_MAC(p)->dst, HDR_SUNSET_MAC(p)->pktId);
	Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter::buffer2Pkt agent info for the packet have been set %d Bits", size);
	
	return size;
}

/*!
 * 	@brief The buffer2Pkt function converts a stream of bytes into a MAC packet header when link source and destination are specified in the input.
 *	@param level The layer ID of the MAC packet header converter.
 *	@param src The link source ID.
 *	@param dst The link destination ID.
 * 	@param p The packet that will contain the information taken from the buffer.
 * 	@param buffer The source buffer that contains the packet information of all the stack layers.
 * 	@param offset The offset used when starting to read from the buffer.
 * 	@param bits The number of bits to be read.
 *	@retval size The number of bits read from the buffer.
 */

int Sunset_MacPktConverter::buffer2Pkt(int level, int src, int dst, Packet* p, char* buffer, int offset, int bits) 
{
	int size = 0;
	
	size = buffer2Pkt(level, p, buffer, offset, bits);
	
	// set src and dest ID received from the modem driver in the MAC packet header
	HDR_SUNSET_MAC(p)->src = src;
	HDR_SUNSET_MAC(p)->dst = dst;
	
	Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter::buffer2PktMAC_2 type %d subtype %d version %d duration %d src %d dst %d", HDR_SUNSET_MAC(p)->dh_fc.fc_type, HDR_SUNSET_MAC(p)->dh_fc.fc_subtype, HDR_SUNSET_MAC(p)->dh_fc.fc_protocol_version, HDR_SUNSET_MAC(p)->dh_duration, HDR_SUNSET_MAC(p)->src, HDR_SUNSET_MAC(p)->dst);
	Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter::buffer2PktMAC_2 agent info for the packet have been set %d Bits", size);
	
	return size;
}

/*!	@brief The getSrc function is used to obtain the source ID for the MAC packet header. 
 *	@param level The layer ID of the MAC packet header converter.
 * 	@param p The packet.
 * 	@param m The packet header selected for checking.
 * 	@param[out] src The MAC source ID is written by the packet header converter in the src variable.
 * 	@retval 1 if the packet header is the one has been selected according to m value, 0 otherwise.
 */

int Sunset_MacPktConverter::getSrc(int level, Packet* p, sunset_header_type m, int& src) 
{ 
	// check if this is a  MAC packet
	if (m == UW_PKT_MAC) { 
		
		src = HDR_SUNSET_MAC(p)->src;
		return 1;
	} 
	
	return 0;  
}

/*!	@brief The getDst function is used to obtain the destination ID for the MAC packet header. 
 *	@param level The layer ID of the MAC packet header converter.
 * 	@param p The packet.
 * 	@param m The packet header selected for checking.
 * 	@param[out] dst The MAC destination ID is written by the packet header converter in the dst variable.
 * 	@retval 1 if the packet header is the one has been selected according to m value, 0 otherwise.
 */

int Sunset_MacPktConverter::getDst(int level, Packet* p, sunset_header_type m, int& dst)
{ 
	// check if this is a  MAC packet
	if (m == UW_PKT_MAC) { 
		
		dst = HDR_SUNSET_MAC(p)->dst;
		return 1;
	} 
	
	return 0;  
}

/*!	@brief The getPktSubType function is used to obtain the packet subtype of the MAC packet header. 
 *	@param level The layer ID of the MAC packet header converter.
 * 	@param p The packet.
 * 	@param m The packet header selected for checking.
 * 	@param[out] subType The MAC packet subtype is written by the packet header converter in the subType variable.
 * 	@retval 1 if the packet header is the one has been selected according to m value, 0 otherwise.
 */

int Sunset_MacPktConverter::getPktSubType(int level, Packet* p, sunset_header_type m, int& subType) 
{ 
	// check if this is a  MAC packet
	if (m == UW_PKT_MAC) { 
		
		subType = HDR_SUNSET_MAC(p)->dh_fc.fc_subtype; 
		return 1;
	} 
	
	return 0;  
}

/*!	@brief The getPktType function is used to obtain the packet type of the MAC packet header. 
 *	@param level The layer ID of the MAC packet header converter.
 * 	@param p The packet.
 * 	@param m The packet header selected for checking.
 * 	@param[out] type The MAC packet type is written by the packet header converter in the type variable.
 * 	@retval 1 if the packet header is the one has been selected according to m value, 0 otherwise.
 */

int Sunset_MacPktConverter::getPktType(int level, Packet* p, sunset_header_type m, int& type)
{ 
	// check if this is a  MAC packet
	if (m == UW_PKT_MAC) { 
		
		type = HDR_SUNSET_MAC(p)->dh_fc.fc_type; 
		return 1;  
	} 
	
	return 0;
}

/*!
 * 	@brief The start() function can be called from the TCL script to execute module operations when the emulation starts.
 */

void Sunset_MacPktConverter::start() 
{	
	Sunset_PktConverter::start();
	assignNsPktType(PT_SUNSET_MAC);
	Sunset_Debug::debugInfo(5, -1, "Sunset_MacPktConverter::start");
}

