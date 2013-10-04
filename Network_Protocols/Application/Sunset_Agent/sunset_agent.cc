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



#include <sunset_agent.h>

int Sunset_Agent::pkt_num = 0;

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_AgentTClass : public TclClass 
{
	
public:
	Sunset_AgentTClass() : TclClass("Module/Sunset_Agent") {}
	
	TclObject* create(int argc , const char*const* argv) {
		
		return(new Sunset_Agent());
	}
	
} class_Sunset_Agent;


Sunset_Agent::Sunset_Agent() : Module()
{
	port_number = -1;
	
	start_pktId = 0;
	
	bind("moduleAddress", &module_address);
	bind("runId", &run_id);
	bind("DATA_SIZE", &MAX_DATA_SIZE);
	bind("portNumber", &port_number);
	bind("start_pktId", &start_pktId);
	
	// variables used for statistics
	
	pkt_recv_ = 0;
	pkt_gen_ = 0;
	pktReceived.clear();
	pkt_num = start_pktId;
	
	//information dispatcher initialization
	sid_id = 0;
	sid = NULL;
	
	//statistics module initialization
	
	stat = NULL;
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::Sunset_Agent setup MAX_DATA_SIZE %d port_number %d", MAX_DATA_SIZE, port_number);
}


Sunset_Agent::~Sunset_Agent() {
}


/*!
 * 	@brief This function performs the initialization tasks of the Agent class. The start() function is called from the TCL scripts 
 */

void Sunset_Agent::start() 
{
	Sunset_Module::start();
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Agent::start");
	
	// if statistics info are used initialize the statistics module reference
	if (Sunset_Statistics::use_stat()) {
		stat = Sunset_Statistics::instance();   
	}
	
	// Initialize the information dispatcher module reference
	sid = Sunset_Information_Dispatcher::instance();
	
	// Register the agent module to the information dispatcher and collect the assigned ID
	sid_id = sid->register_module(getModuleAddress(), "AGT_EMU", this);
}

/*!
 * 	@brief The stop() function can be called from the TCL scripts to execute agent module operations when the simulation stops.
 */
void Sunset_Agent::stop() 
{
	Sunset_Module::stop();
	
	stat = NULL;
	
	sid = NULL;
	
	sid_id = 0;
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param argc argc is a count of the arguments supplied to the command function.
 *	@param argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Agent::command(int argc, const char*const* argv)
{
	Tcl& tcl = Tcl::instance();
	
	if (argc == 2) {
		
		/* The "start" command starts the agent module */
		
		if (strcmp(argv[1], "start") == 0) {
			
			Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Agent::command start Module");
			start();
			
			return TCL_OK;
		}
		
		/* The "stop" command stops the agent module */
		
		if (strcmp(argv[1], "stop") == 0) {
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command stop Module");
			stop();
			
			return TCL_OK;
		}
	}
	else if ( argc == 3 ) {
		
		/* The "setModuleAddress" command sets the address of the agent module. */
		
		if (strcmp(argv[1], "setModuleAddress") == 0) {
			
			module_address = atoi(argv[2]);
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command setModuleAddress %d", getModuleAddress());
			
			return TCL_OK;
		}
		
		if (strcmp(argv[1], "setPortNumber") == 0) {
			
			/* When more than one agent module is used, when running in emulation mode, the port number is used to
			 identity the specific agent module transmitting and receiving packet. A different port number has to be 
			 assigned to the different agent modules created for a node in the network. 
			 Same modules on different nodes need to have the same port number values. */
			
			port_number = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command setPortNumber %d", getPortNumber());
			
			return TCL_OK;
		}
		
		/* The "setStartPktId" command sets the initial value for the agent packet id added to the data packets. */
		
		if (strcmp(argv[1], "setStartPktId") == 0) {
			
			start_pktId = atoi(argv[2]);
			pkt_num = start_pktId;
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command start_pktId %d", start_pktId);
			
			return TCL_OK;
		}

		/* The "setDataSize" command sets the maximal size of the data packets. */
		
		if (strcmp(argv[1], "setDataSize") == 0) {
			
			MAX_DATA_SIZE = atoi(argv[2]);
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command MAX_DATA_SIZE %d", MAX_DATA_SIZE);
			
			return TCL_OK;
		}
		
		/* When the "send" command is invoked, the agent module sends a data packets to a specified destination (argv[2]). */
		
		if (strcmp(argv[1], "send") == 0) {
			
			int dest = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command send to %d", dest);
			
			if (Sunset_Utilities::isSimulation()) {
				
				sendDataSim(dest, MAX_DATA_SIZE);
			}
			else {
				sendDataEmulation(dest);            
			}
			
			return TCL_OK;
		}
		
		// collect basic statistics information
		
		/* Number of  packets transmitted to dest node */
		
		if (strcmp(argv[1], "getTxPkt") == 0) {
			
			int dest = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command - getTxPkt %d", dest);
			tcl.resultf("%d", getTxPkt(dest));
			
			return TCL_OK;
		}
		
		/* Number of traversed hops for packets received from src node */
		
		if (strcmp(argv[1], "getPktHop") == 0) {
			
			int src = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command - getPktHop %d", src);
			tcl.resultf("%d", getPktHop(src));
			
			return TCL_OK;
		}
		
		/* Number of packets received from src node */
		
		if (strcmp(argv[1], "getRxPkt") == 0) {
			
			int src = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command - getRxPkt %d", src);
			tcl.resultf("%d", getRxPkt(src));
			
			return TCL_OK;
		}
		
		/* Packet latency for packets received from src node */
		
		if (strcmp(argv[1], "getDelay") == 0) {
			
			int src = atoi(argv[2]);
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command - getDelay %d", src);
			tcl.resultf("%f", getDelay(src));
			
			return TCL_OK;
		}
	}
	else if ( argc == 4 ) {
		
		/* The "setModuleAddress" command sets the address of the agent module. */
		
		if (strcmp(argv[1], "setModuleAddress") == 0) {
			
			module_address = atoi(argv[2]);
			run_id = atoi(argv[3]);
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command setModuleAddress %d runId %d", getModuleAddress(), getRunId());
			
			return TCL_OK;
		}
		
		/* When the "send" command is invoked and argc > 3, the agent layer sends a data 
		 a data packet containing the message "argv[3]" to a destination (argv[2]). This is used when running in emulation mode, since in simulation mode there is no need to actually allocate the packet payload */
		
		if (strcmp(argv[1], "send") == 0) {
			
			//if we are running in simulation mode we do not need to allocate memory for the packet, the other send function should be used
			if (Sunset_Utilities::isSimulation()) {
				
				return TCL_ERROR;            
			}
			
			int dest = atoi(argv[2]);
			int maxDataSize = Sunset_Utilities::get_max_pkt_size();
			char* msg = (char*) malloc (maxDataSize+1);
			
			if ( msg == NULL ) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Agent::command MALLOC ERROR");
				return TCL_ERROR;
			}
			
			memset(msg, '\0', maxDataSize+1);
			strncpy(msg, argv[3], maxDataSize);
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command send to %d msg %s", dest, msg);
			
			sendData(dest, msg);
			
			free(msg);
			
			return TCL_OK;
		}
	}
	
	else if ( argc == 6 ) {
		
		/* Command used to create and send a packet with random size (fixed_size + rand<min_size, max_size>) to dest. When running in emulation mode a random pyalod is created. */
		
		if (strcmp(argv[1], "sendRandomLength") == 0) {
			
			int dest = atoi(argv[2]);
			int fix_size = atoi(argv[3]);
			int min_size = atoi(argv[4]);
			int max_size = atoi(argv[5]);
			
			Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command sendRandom to %d fix %d min %d max %d", dest, fix_size, min_size, max_size);
			
			if (fix_size < 0 || min_size < 0 || max_size < 0 || max_size < min_size ) {
				
				return TCL_ERROR;
				
			}
			
			sendRandomLength(dest, fix_size, min_size, max_size);
			
			return TCL_OK;
		}
	}
	
	return Module::command(argc, argv);
}

/*!
 * 	@brief The isPktForMe function checks if the current node is the recipient of the data packet (using port number information).
 *	@param p The data packet received.
 *	@retval true I am the recipient.
 *	@retval false I am not the recipient.
 */

bool Sunset_Agent::isPktForMe(Packet* p) 
{
	struct hdr_ip* iph = HDR_IP(p);
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Agent::isPktForMe my_port %d port %d", getPortNumber(), iph->dport());
	
	if (getPortNumber() != -1 && iph->dport() != getPortNumber()) {
		
		return false;	
	}
	
	return true;
}

/*!
 * 	@brief The recvPkt function is invoked when a data packet is received from the lower layer for this agent module.
 @param p The received packet.
 */

void Sunset_Agent::recvPkt(Packet *p) 
{ 
	struct hdr_Sunset_Agent *gh = hdr_Sunset_Agent::access(p); 
	u_int8_t  type = gh->ac.ac_type;
	u_int8_t  subtype = gh->ac.ac_subtype;
	
	Sunset_Debug::debugInfo(0, getModuleAddress(), "Sunset_Agent::recv pkt src %d type %d subtype %d dest %d", gh->srcId(), type, subtype, gh->dstId());
	
	/* If I am the sender of the packet discard it */
	
	if (getModuleAddress() == gh->srcId()) {
		
		Sunset_Utilities::erasePkt(p, getModuleAddress());
		
		return;
	}
	
	/* If the packet is not addressed to me discard it */
	
	if (getModuleAddress() != gh->dstId() && gh->dstId() != Sunset_Address::getBroadcastAddress()) {
		
		Sunset_Utilities::erasePkt(p, getModuleAddress());
		
		return;
	}
	else {

		hdr_cmn *cmh; 
		hdr_ip *iph; 
		
		int src = 0;
		int dst = 0;
		int id = 0;
		
		cmh = HDR_CMN(p);
		iph = HDR_IP(p);
		src = (int)iph->saddr();
		dst = (int)iph->daddr();
		id = cmh->uid();
		
		if (gh->srcId() < 0 || pktReceived.find(src) != pktReceived.end() && (pktReceived[src]).find(id) != (pktReceived[src]).end()) {
			
			rxAction(p, SUNSET_AGT_RX_DUPLICATED_ACTION);

			Sunset_Trace::print_info("stat - (%.3f) Node:%d - Receives duplicated DATA message from %d id %d hop %d\n", Sunset_Utilities::get_now(), getModuleAddress(), gh->srcId(), id, HDR_CMN(p)->num_forwards());

			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Agent::recvPkt pkt from %d to %d already processed DISCARD", src, dst);
			
			Sunset_Utilities::erasePkt(p, getModuleAddress());
			
			return;
		}
		
		rxAction(p, SUNSET_AGT_RX_ACTION);

		pktReceived[src].insert(id);
		
		switch(type) {
				
			/* If the packet type and subtype are DATA I call the recvData() function */
				
			case SUNSET_AGT_Type_Data:
				
				switch(subtype) {
						
					case SUNSET_AGT_Subtype_Data:
						
						recvData(p);
						
						break;
						
					default:
						Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Agent::recv recv_timer3:Invalid MAC Data Subtype %x ERROR", subtype);
						
						Sunset_Utilities::erasePkt(p, getModuleAddress());
						
						return;
				}
				
				break;
				
				/* If I receive a control packet an error occurs somewhere, since no control packets are used by this agent layer */
				
			case SUNSET_AGT_Type_Control:
				
				switch(subtype) {
					default:
						Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Agent::recv recv_timer3:Invalid MAC Control Subtype %x ERROR",	subtype);
						
						Sunset_Utilities::erasePkt(p, getModuleAddress());
						
						return;
				}
				
				break;	
				
			default:
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Agent::recv recv_timer4:Invalid MAC Type %x ERROR", subtype);
				
				Sunset_Utilities::erasePkt(p, getModuleAddress());
				
				return;
		}
	}
}


/*!
 * 	@brief The recv function receives a packet from the upper/lower layer.
 @param p The received packet.
 */

void Sunset_Agent::recv(Packet *p) 
{ 
	struct hdr_Sunset_Agent *gh = hdr_Sunset_Agent::access(p); 
	struct hdr_cmn* cmh = HDR_CMN(p);
	u_int8_t  type = gh->ac.ac_type;
	u_int8_t  subtype = gh->ac.ac_subtype;
	struct hdr_ip* iph = HDR_IP(p);
	
	if (cmh->direction() == hdr_cmn::DOWN) {
		
		/* I have received a packet from the upper layer, it has to be updated and sent to the lower layer*/
		
		struct hdr_ip* iph = HDR_IP(p);
		
		if (Sunset_Utilities::isSimulation()) {
			
			sendDataSim(iph->daddr(), MAX_DATA_SIZE);
		}
		else {
			sendDataEmulation(iph->daddr());            
		}
		
		return;
	}		
	
	/* I have received a packet from the lower layer */
	
	if (!isPktForMe(p)) {
		
		/* I am not the recipient of the data packet I discard it but I do not free the payload since it could be be read from another module */
		
		Sunset_Utilities::eraseOnlyPkt(p, getModuleAddress());
		
		return;
	}
	
	recvPkt(p);
	
}

/*!
 * 	@brief The sendPkt function creates a new data packet and sends it to the lower layer. A real payload is allocated. It is used when running in emulation mode.
 *	@param dest The destination ID.
 *	@param msg The payload of the data packet.
 *	@param sizeMsg The payload size of the data packet.
 *	@param type The packet type (i.e. SUNSET_AGT_Type_Data).
 *	@param subType The packet subtype (i.e. SUNSET_AGT_Subtype_Data).
 */

void Sunset_Agent::sendPkt(int dest, char* msg, int sizeMsg, int type, int subType) 
{	
	
	// if I am running in simulation mode exit
	if (Sunset_Utilities::isSimulation()) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Agent::sendPkt to node:%d but running in simulation mode ERROR", dest);
		
		return;
	}
	
	Packet *p;
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Agent::sendPkt to node:%d len %d len2 %d text: %s", dest, sizeMsg, strlen(msg), msg);
	
	// if dest = -1, conventionally used by ns2 for BRAODCAST set dest to  Sunset_Address::getBroadcastAddress()
	
	if (dest == -1) {
		
		dest = Sunset_Address::getBroadcastAddress();
	}
	
	//Create the pkt
	
	p = createPacket(dest, msg, sizeMsg, type, subType);
	
	Sunset_Utilities::sync_time();
	
	//update statistics information about pkt transmission
	if (pktSent_.find(dest) == pktSent_.end()) {
		
		pktSent_[dest] = 0;
	}
	
	pktSent_[dest]++;
	
	txAction(p, SUNSET_AGT_TX_ACTION);
	
	if (dest == Sunset_Address::getBroadcastAddress()) {
		
		Sunset_Debug::debugInfo(0, getModuleAddress(), "send BROADCAST DATA len %d id %d message: %s", HDR_SUNSET_AGT(p)->dataSize(), HDR_CMN(p)->uid(), HDR_SUNSET_AGT(p)->getData());
		
	} else {
		
		Sunset_Debug::debugInfo(0, getModuleAddress(), "send DATA message to %d len %d id %d msg: %s", dest, HDR_SUNSET_AGT(p)->dataSize(), HDR_CMN(p)->uid(), HDR_SUNSET_AGT(p)->getData());
	}
	
	sendDown(p);
	
	return;
}

/*!
 * 	@brief The sendDataRandomLength function sends a data packet containing a random length message to a specified node.
 *	@param destId The destination ID.
 *	@param fix_size Number of bytes that have to necessary be in the message.
 *	@param min_size Minimum number of bytes to add to the fixed length.
 *	@param max_size Maximun number of bytes to add to the fixed length.
 */

int Sunset_Agent::sendRandomLength(int destId, int fix_size, int min_size, int max_size) 
{
	Sunset_Utilities::sync_time();
	
	if (Sunset_Utilities::isSimulation()) {
		
		int rand_len = fix_size + (int)(Random::uniform(min_size, max_size));
		
		sendDataSim(destId, rand_len);
		
		return 1;
	}
	else {
		int dest = destId;
		int maxDataSize = Sunset_Utilities::get_max_pkt_size();
		char* msg = (char*) malloc (maxDataSize+1);
		int rand_len = 0;
		int len = 0;
		int j = 0;
		int k = 0;
		
		if ( msg == NULL ) {
			
			Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Agent::sendRandomLength MALLOC ERROR");
			return 0;
		}
		
		memset(msg, '\0', maxDataSize+1);
		
		rand_len = maxDataSize;
		
		if (fix_size + min_size < maxDataSize) {
			
			rand_len = fix_size + (int)(Random::uniform(min_size, min(max_size, maxDataSize - fix_size)));
		}
		
		//we try to add to the payload basic informatio to identify the sender, receiver, pkt id and pkt creation timestamp
		len = snprintf(msg, rand_len, "Tx%d-%d:%d__%f__", getModuleAddress(), dest, pkt_num, Sunset_Utilities::get_now());
		
		for (j = strlen(msg), k = 0; j < rand_len; j++, k++) {
			
			msg[j] = k;
		}
		
		
		Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::command sendRandomLength to %d msg %s", dest, msg);
		
		sendPkt(dest, msg, rand_len, SUNSET_AGT_Type_Data, SUNSET_AGT_Subtype_Data);
		
		//free the allcoated memory
		free(msg);
	}
	
	return 1;
}

/*!
 * 	@brief The sendData function sends a data packet containing a specified message to a specified node.
 *	@param dest The destination ID.
 *	@param msg The payload of the data packet.
 */

void Sunset_Agent::sendData(int dest, char* msg) 
{
	Sunset_Utilities::sync_time();
	
	sendPkt(dest, msg, strlen(msg), SUNSET_AGT_Type_Data, SUNSET_AGT_Subtype_Data);
}

/*!
 * 	@brief The sendDataSim function creates and sends a packet to a specified destination when running in simulation mode.
 *	@param dest The intended packet destination.
 *	@param len The packet size in bytes.
 */

void Sunset_Agent::sendDataSim(int dest, int len) 
{
	
	Sunset_Utilities::sync_time();
	
	Packet *p;
	
	/* Create a DATA packet */
	
	p = createPacket(dest, len, SUNSET_AGT_Type_Data, SUNSET_AGT_Subtype_Data);
	
	txAction(p, SUNSET_AGT_TX_ACTION);
	
	//update statistics information about pkt transmission
	if (pktSent_.find(dest) == pktSent_.end()) {
		
		pktSent_[dest] = 0;
	}
	
	pktSent_[dest]++;
	
	if (dest == -1 || dest == Sunset_Address::getBroadcastAddress()) {
		
		Sunset_Debug::debugInfo(0, getModuleAddress(),  "send BROADCAST DATA message id %d", HDR_SUNSET_AGT(p)->pktId());
		
	}
	else {
		Sunset_Debug::debugInfo(0, getModuleAddress(),  "send DATA message to %d id %d size %d - count %d", dest, HDR_SUNSET_AGT(p)->pktId(), HDR_CMN(p)->size(), pktSent_[dest]);
	}
	
	sendDown(p);
}


/*!
 * 	@brief The sendData function sends a generic data packet to a specified destination node, a dummy payload is used.
 *	@param dest The destination ID.
 */

void Sunset_Agent::sendDataEmulation(int dest) 
{
	Sunset_Utilities::sync_time();
	
	char* dData = 0;
	int maxDataSize = Sunset_Utilities::get_max_pkt_size();
	
	int pkt_size = MAX_DATA_SIZE; //getPktSize();
	int max_pkt_size = maxDataSize;
	
	int len = 0;
	
	Sunset_Debug::debugInfo(4, getModuleAddress(), "Sunset_Agent::sendDataEmulation - %d - %d", max_pkt_size, pkt_size);
	
	//allocate the memory for the packet payload without exceedign the maximum allowed payload size
	if (pkt_size < max_pkt_size) {
		
		max_pkt_size = pkt_size;
	}
	
	if (max_pkt_size > maxDataSize) {
		
		max_pkt_size = maxDataSize;
	}
	
	dData = (char*)malloc(max_pkt_size+1);
	
	if ( dData == NULL ) {
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Agent::sendDataEmulation MALLOC ERROR");
		
		return;
	}
	
	memset(dData, '\0', max_pkt_size+1);
	
	len = snprintf(dData, max_pkt_size, "Tx%d-%d:%d__%f__ prova1 prova2 prova3 prova4 prova5 prova6 end", getModuleAddress(), dest, pkt_num, Sunset_Utilities::get_now());
	
	if ( len < max_pkt_size ) {
		
		memset(dData+len, 0x01, max_pkt_size-len);
	}
	
	sendPkt(dest, dData, strlen(dData), SUNSET_AGT_Type_Data, SUNSET_AGT_Subtype_Data);
	
	free(dData);
}

/*!
 * 	@brief The recvData function is invoked when a DATA packet is received by the packet final destination.
 *	@param pkt The received packet.
 */

void Sunset_Agent::recvData(Packet *pkt) 
{
	struct hdr_Sunset_Agent *gh = hdr_Sunset_Agent::access(pkt);
	
	/* If it is a duplicated packet discard it */
	
	if (pktRx_.find((int)(gh->srcId())) == pktRx_.end()) {
		
		pktRx_[(int)(gh->srcId())] = 0;
	}
	
	pktRx_[(int)(gh->srcId())]++;
	pktHop_[(int)(gh->srcId())] += HDR_CMN(pkt)->num_forwards();
	
	if (pktLatency_.find((int)(gh->srcId())) == pktLatency_.end()) {
		
		pktLatency_[(int)(gh->srcId())] = 0.0;
	}
	
	pktLatency_[(int)(gh->srcId())] += Sunset_Utilities::getRealTime() - HDR_CMN(pkt)->timestamp();
	
	if (Sunset_Utilities::isSimulation()) {
		Sunset_Debug::debugInfo(0, getModuleAddress(), "receives DATA message from %d id %d hop %d latency %f", gh->srcId(), gh->pktId(), HDR_CMN(pkt)->num_forwards(), Sunset_Utilities::getRealTime() - HDR_CMN(pkt)->timestamp());
	}
	else {
		if (HDR_CMN(pkt)->timestamp() != 0) {
			
			Sunset_Debug::debugInfo(0, getModuleAddress(), "receives DATA message from %d id %d  hop %d latency %f: %s", gh->srcId(), gh->pktId(), HDR_CMN(pkt)->num_forwards(), Sunset_Utilities::getRealTime() - HDR_CMN(pkt)->timestamp(), gh->getData());
			
			if (Sunset_Utilities::getRealTime() - HDR_CMN(pkt)->timestamp() < 0.0) {
				
				Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Agent::recvData pktLatency_ %f < 0 ERROR", Sunset_Utilities::getRealTime() - HDR_CMN(pkt)->timestamp()); 
			}
		}
		else {
			
			Sunset_Debug::debugInfo(0, getModuleAddress(), "receives DATA message from %d id %d  hop %d: %s", gh->srcId(), gh->pktId(), HDR_CMN(pkt)->num_forwards(), gh->getData());
		}
		
	}
		
	if (HDR_CMN(pkt)->ptype() == PT_SUNSET_AGT) {
		
		Sunset_Utilities::erasePkt(pkt, getModuleAddress());	
		return;
	}
	else {
		if (Sunset_Utilities::isEmulation) {
			Sunset_Utilities::eraseData(pkt, getModuleAddress());
		}
	}
	
	// send the packet to the upper layer 
	sendUp(pkt);
}

/*!
 * 	@brief The getAgtHeaderSize function returns the agent header size in bytes, it is used when running in simulation mode.
 *	@retval len The size of the agent header.
 */

int Sunset_Agent::getAgtHeaderSize() 
{
	int len = 0;
	
	len += sizeof(struct sunset_agt_control); //control information in the agent header
	len += sizeof(u_int8_t); //source id
	len += sizeof(u_int8_t); //destination id
	len += sizeof(u_int16_t); //pkt ID
	len += sizeof(u_int16_t); //size of the payload
	
	return len;
}

/*!
 * 	@brief The createPacket function creates and initializes a new data packet without payload information.
 *	@param dest The packet destination address.
 *	@param pktType The packet type (i.e. SUNSET_AGT_Type_Data).
 *	@param pktSubType The packet subtype (i.e. SUNSET_AGT_Subtype_Data).
 *	@retval p The packet allocated and initialized.
 */

Packet* Sunset_Agent::createPacket(int dest, int len, int pktType, int pktSubType) 
{
	Packet *p = Packet::alloc();
	
	initPacket(p, dest, len, pktType, pktSubType);
	
	return p;
}

/*!
 * 	@brief The createPacket function creates and initializes a new data packet, using a message as data payload.
 *	@param dest The destination ID.
 *	@param msg The payload of the data packet.
 *	@param len The length of the payload.
 *	@param pktType The packet type (i.e. SUNSET_AGT_Type_Data).
 *	@param pktSubType The packet subtype (i.e. SUNSET_AGT_Subtype_Data).
 *	@retval p The packet allocated and initialized.
 */

Packet* Sunset_Agent::createPacket(int dest, char* msg, int len, int pktType, int pktSubType) 
{
	Packet *p;
	p = Sunset_Agent::createPacket(dest, len, pktType, pktSubType);
	struct hdr_Sunset_Agent *gh = hdr_Sunset_Agent::access(p); 
	struct hdr_cmn* cmh = HDR_CMN(p);
	int lenData = len;
	int maxDataSize = Sunset_Utilities::get_max_pkt_size();
	
	if (lenData > maxDataSize) {
		
		lenData = maxDataSize;
	}
	
	HDR_IP(p)->dport() = getPortNumber();
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::createPacket src %d dst %d uid %d pktId %d port %d my_port %d", getModuleAddress(), dest, cmh->uid(), gh->pktId(), HDR_IP(p)->dport(), getPortNumber());

	gh->dataSize() = lenData; //PKT_SIZE;
	gh->data = (char*)malloc(sizeof(char) * ((int)(gh->dataSize()) + 1));
	
	if ( gh->data == NULL ) { 
		
		Sunset_Debug::debugInfo(-1, getModuleAddress(), "Sunset_Agent::createPacket MALLOC ERROR");
		
		return p;
		exit(1);
	}
	
	memset(gh->data, '\0', ((int)(gh->dataSize()) + 1));
	
	memcpy(gh->getData(), msg, (int)(gh->dataSize()));
	
	Sunset_Debug::debugInfo(3, getModuleAddress(), "Sunset_Agent::createPacket len %d max_lenData %d msg_len %d msg: %s", len, maxDataSize, gh->dataSize(), gh->data);

	cmh->size() = getAgtHeaderSize() + lenData;
	cmh->direction() == hdr_cmn::DOWN;
	
	return p;
}

/*!
 * 	@brief The notify_info function is called from the information dispatcher when a value the agent module is registered 
 *  for has been updated. This agent is currently not registered for any variables, this method can be extend by other 
 *  application layers if interested in any variables shared with the other modules
 *	@param linfo The list of variables shared with the other modules the agent module is interested in.
 */

int Sunset_Agent::notify_info(list<notified_info> linfo) 
{ 
	return Sunset_Module::notify_info(linfo); 
} 

/*!
 * 	@brief The initPacket function initializes a new data packet. It is used when a packet from the upper layer is received.
 *	@param p The packet to be initialized.
 */

void Sunset_Agent::initPacket(Packet* p, int dest, int size, int pktType, int pktSubType) 
{
	struct hdr_Sunset_Agent *gh = hdr_Sunset_Agent::access(p);
	struct hdr_cmn* cmh = HDR_CMN(p);
	struct hdr_ip* iph = HDR_IP(p);	
	iph->daddr() = dest;
	
	/* If packet destination is a ns-2 broadcast address set dest using the framework broadcast address */
	
	// header initialitation
	if ( iph->daddr() == -1 || iph->daddr() == IP_BROADCAST) {
		iph->daddr() = Sunset_Address::getBroadcastAddress();
	}
	/* header initialitation */
	
	gh->ac.ac_protocol_version = SUNSET_AGT_ProtocolVersion;
	gh->ac.ac_type = pktType;
	gh->ac.ac_subtype = pktSubType;
	
	iph->saddr() = getModuleAddress();
	gh->srcId() = getModuleAddress();
	gh->dstId() = iph->daddr();
	cmh->ptype() = PT_SUNSET_AGT;
	cmh->num_forwards() = 0;
	cmh->timestamp() = Sunset_Utilities::getRealTime(); //s.clock();
	cmh->addr_type() = NS_AF_ILINK;
	
	cmh->error() = 0; /* pkt not corrupt to start with */
	
	SUNSET_HDR_CMN(p)->init();

	HDR_CMN(p)->prev_hop_ = getModuleAddress();
	cmh->uid() = pkt_num;
	gh->pktId() = pkt_num;
	
	if ( gh->dstId() == Sunset_Address::getBroadcastAddress()) {
		
		cmh->next_hop_  = Sunset_Address::getBroadcastAddress();
		iph->dst_.addr_ = Sunset_Address::getBroadcastAddress();

	} else {
		
		cmh->next_hop_  = gh->dstId();
		iph->dst_.addr_ = dest;
	}
	
	cmh->direction() = hdr_cmn::DOWN;
	
	Sunset_Debug::debugInfo(1, getModuleAddress(), "Sunset_Agent::initPacket for node:%d", dest);
	
	gh->data = 0;
	gh->dataSize() = 0;
	
	cmh->size() = size + getAgtHeaderSize();
	
	pkt_num++;
	
	return;
}

/*!
 * 	@brief The getTxPkt function returns the number of transmitted packet to a specific destination node.
 *	@param dest The destination ID.
 *	@retval pktSent_[dest] The number of packet sent to dest.
 */

int Sunset_Agent::getTxPkt(int dest) 
{
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Agent::getTxPkt %d", dest);
	
	if ( pktSent_.find(dest) == pktSent_.end() ) {
		
		return 0;
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Agent::getTxPkt %d value %d", dest, pktSent_[dest]);
	
	return pktSent_[dest];
}

/*!
 * 	@brief The getPktHop function returns the number of hops traversed by data packets received from a specified source node.
 *	@param src The packet source node ID.
 *	@retval pktHop_[src] The number of hop traversed.
 */

int Sunset_Agent::getPktHop(int src) 
{
	
	if (pktHop_.find(src) == pktHop_.end()) {
		
		return 0;
	}
	
	return pktHop_[src];
}

/*!
 * 	@brief The getRxPkt function returns the number of  packets correctly received which have ben generated by a specified source node.
 *	@param src The source ID.
 *	@retval pktRx_[src] The number of packet correctly received from src.
 */

int Sunset_Agent::getRxPkt(int src) 
{
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Agent::getRxPkt %d", src);
	
	if (pktRx_.find(src) == pktRx_.end()) {
		
		return 0;
	}
	
	Sunset_Debug::debugInfo(2, getModuleAddress(), "Sunset_Agent::getRxPkt %d value %d", src, pktRx_[src]);
	
	return pktRx_[src];
}

/*!
 * 	@brief The getDelay function returns the overall end-to-end latency for the packets correctly received which have ben generated by a specified source node.
 *	@param src The source ID.
 *	@retval pktLatency_[src] The overall end-to-end latency for packets generated by src.
 */

double Sunset_Agent::getDelay(int src) 
{
	
	if (pktRx_.find(src) == pktRx_.end() || pktRx_.find(src) == pktRx_.end()) {
		
		return 0.0;
	}
	
	if (pktRx_[src] == 0.0) {
		
		return 0.0;
	}
	
	return pktLatency_[src];
}


/*! @brief Function called when a transmission operation is performed to check if an action has to be taken.
 *	@param[in] p   The packet the action is related to.
 *	@param[in] mct The transmission action that has been executed.
 */

void Sunset_Agent::txAction(Packet* p, agt_action_type mct) 
{
	if (p == 0) {
		
		return;
	}

	hdr_cmn *ch = HDR_CMN(p);
	hdr_ip *ip = HDR_IP(p);

	if (Sunset_Statistics::use_stat() && stat == NULL) {
		
		stat = Sunset_Statistics::instance(); 	
	}

	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_AGENT_TX, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");
	}
	
	return;
}

/*! @brief Function called when a reception operation is performed to check if an action has to be taken.
 *	@param[in] p   The packet the action is related to.
 *	@param[in] mct The reception action that has been executed.
 */

void Sunset_Agent::rxAction(Packet* p, agt_action_type mct) 
{
	if (p == 0) {
		
		return;
	}
	
	hdr_cmn *ch = HDR_CMN(p);
	hdr_ip *ip = HDR_IP(p);
	
	char aux_msg[AGT_MAX_BUFFER_SIZE];
	memset (aux_msg, 0x0, AGT_MAX_BUFFER_SIZE);
	
	if (Sunset_Statistics::use_stat() && stat == NULL) {
		
		stat = Sunset_Statistics::instance(); 
	}

	if (Sunset_Statistics::use_stat() && stat != NULL) {
		
		stat->logStatInfo(SUNSET_STAT_AGENT_RX, getModuleAddress(), p, HDR_CMN(p)->timestamp(), "");	
	}
	
	if ( HDR_SUNSET_AGT(p)->dataSize() > 0 && HDR_SUNSET_AGT(p)->getData() != NULL) {
		
		snprintf(aux_msg, AGT_MAX_BUFFER_SIZE, "%s", HDR_SUNSET_AGT(p)->getData());
	}
	
	if (mct == SUNSET_AGT_RX_ACTION) {
		
		Sunset_Trace::print_info("agt - (%f) Node:%d - RX PACKET source %d destination %d id %d hops %d msg:\n", NOW, getModuleAddress(), ip->saddr(), ip->daddr(), ch->uid(), ch->num_forwards());
	}
	else if (mct == SUNSET_AGT_RX_DUPLICATED_ACTION) {
		
		Sunset_Trace::print_info("agt - (%f) Node:%d - RX DUPLICATED PACKET source %d destination %d id %d hops %d msg\n", NOW, getModuleAddress(), ip->saddr(), ip->daddr(), ch->uid(),  ch->num_forwards());
	}
	else {	
		
		Sunset_Trace::print_info("agt - (%f) Node:%d - RX ACTION NOT DEFINED  source %d destination %d id %d\n", NOW,  getModuleAddress(), ip->saddr(), ip->daddr(), ch->uid());
	}
	
	return;
}
