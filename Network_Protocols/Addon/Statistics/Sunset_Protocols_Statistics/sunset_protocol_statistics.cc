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


#include "sunset_protocol_statistics.h"
#include <cbr-module.h>

char Sunset_Protocol_Statistics::fileOut[FILE_NAME] = "";
ofstream Sunset_Protocol_Statistics::outFile;

/*!
 * 	@brief This static class is a hook class used to instantiate a C++ object from the TCL script. 
 *	It also allows to define parameter values using the bind function in the class constructor.
 */

static class Sunset_Protocol_StatisticsClass : public TclClass 
{
public:
	Sunset_Protocol_StatisticsClass() : TclClass("Sunset_Protocol_Statistics") {}

	TclObject* create(int, const char*const*) {
	
		return (new Sunset_Protocol_Statistics());
	}
	
} class_Sunset_Protocol_StatisticsClass;


Sunset_Protocol_Statistics::Sunset_Protocol_Statistics() : Sunset_Statistics() 
{
	pthread_mutex_init(&mutex_stat, NULL);
	
	single = this;
	preamble_size = 0;
	start_traffic = 0.0;
	run_id = 0;
	binaryOutput = 0;
	
	stop_time = 0.0;
	start_time = 0.0;
	
	max_node_id = 0;
	
	bind("binaryOutput_", &binaryOutput);
}

/*!
 * 	@brief The command() function is a TCL hook for all the classes in ns-2 which allows C++ functions to be called from a TCL script 
 *	@param[in] argc argc is a count of the arguments supplied to the command function.
 *	@param[in] argv argv is an array of pointers to the strings which are those arguments.
 *	@retval TCL_OK the command has been correctly executed. 
 *	@retval TCL_ERROR the command has NOT been correctly executed. 
 */

int Sunset_Protocol_Statistics::command( int argc, const char*const* argv ) 
{
	Tcl& tcl = Tcl::instance();

	if ( argc == 2 ) {
	
		//to collect network Packet Delivery Ratio. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getPDR") == 0) {	
			
			tcl.resultf("%f", getPDR());
			
			return TCL_OK;
		}

		//to collect the number of generated data packets. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getGeneratedPacket") == 0) {
			
			tcl.resultf("%f", getGeneratedPacket());
			
			return TCL_OK;
		}

		//to collect the number of data packets transmitted at the MAC layer. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getMacDataPacketTransmissions") == 0) {
			
			tcl.resultf("%f", getMacDataPacketTransmissions());
			
			return TCL_OK;
		}

		//to collect the number of data packets recevied at the MAC layer. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getMacDataPacketReceptions") == 0) {
			
			tcl.resultf("%f", getMacDataPacketReceptions());
			
			return TCL_OK;
		}
		
		//to collect the number of ctrl packets transmitted at the MAC layer. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getMacCtrlPacketTransmissions") == 0) {
			
			tcl.resultf("%f", getMacCtrlPacketTransmissions());
			
			return TCL_OK;
		}

		//to collect the number of ctrl packets recevied at the MAC layer. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getMacCtrlPacketReceptions") == 0) {
			
			tcl.resultf("%f", getMacCtrlPacketReceptions());
			
			return TCL_OK;
		}

		//to collect the number of delivered data packets. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getDeliveredPacket") == 0) {
			
			tcl.resultf("%f", getDeliveredPacket());
			
			return TCL_OK;
		}

		//to collect the throughput at the application layer. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getApplicationThroughput") == 0) {
			
			tcl.resultf("%f", getApplicationThroughput());
			
			return TCL_OK;
		}

		//to collect the average end-to-end packet latency. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getPacketLatency") == 0) {
			
			tcl.resultf("%f", getPacketLatency());
			
			return TCL_OK;
		}

		//to collect the average route length. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getRouteLength") == 0) {
			
			tcl.resultf("%f", getRouteLength());
			
			return TCL_OK;
		}

		//to collect the number of different routes used in the network. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getNumRoutes") == 0) {
			
			tcl.resultf("%f", getNumRoutes());
			
			return TCL_OK;
		}

		//to collect the maximal route length used in the network. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getMaxRouteLength") == 0) {
			
			tcl.resultf("%f", getMaxRouteLength());
			
			return TCL_OK;
		}

		//to collect the amount of overhead needed to deliver one bit of information. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getOverheadPerBit") == 0) {
			
			tcl.resultf("%f", getOverheadPerBit());
			
			return TCL_OK;
		}

		//to collect the throughpout at the MAC layer from the Tcl file over time
		if (strcmp(argv[1], "getMacThroughput") == 0) {
			
			tcl.resultf("%f", getMacThroughput());
			
			return TCL_OK;
		}

		//to collect the average number of data packet retrsnamissions at the MAC layer. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getMacDataRetransmissions") == 0) {
			
			tcl.resultf("%f", getMacDataRetransmissions());
			
			return TCL_OK;
		}
	
		//to collect the experiment duration. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getExperimentTime") == 0) {
			
			tcl.resultf("%f", getExperimentTime());
			
			return TCL_OK;
		}
	
		//to collect the  number of data packet discarded at the MAC layer. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getMacDataPacketDiscarded") == 0) {
			
			tcl.resultf("%d", getMacDataPacketDiscarded());
			
			return TCL_OK;
		}
	
		//to collect the  number of control packet discarded at the MAC layer. It can be done from the Tcl file over time
		if (strcmp(argv[1], "getMacCtrlPacketDiscarded") == 0) {
			
			tcl.resultf("%d", getMacCtrlPacketDiscarded());
			
			return TCL_OK;
		}
		
		/* The "getResidualEnergy" returns the residual energy in the network. */
		
		if (strcmp(argv[1], "getResidualEnergy") == 0) {
			
			tcl.resultf("%f", getResidualEnergy());
			
			return TCL_OK;
		}
		
		/* The "getIdleTime" returns the time (in sec.) spent by the network in idle. */
		
		if (strcmp(argv[1], "getIdleTime") == 0) {
			
			tcl.resultf("%f", getIdleTime());
			
			return TCL_OK;
		}
		
		/* The "getRxTime" returns the time (in sec.) spent by the network in receiving mode. */
		
		if (strcmp(argv[1], "getRxTime") == 0) {
			
			tcl.resultf("%f", getRxTime());
			
			return TCL_OK;
		}
		
		/* The "getTotTxTime" returns the time (in sec.) spent by the network while transmitting. */
		
		if (strcmp(argv[1], "getTotTxTime") == 0) {
			
			tcl.resultf("%f", getTotTxTime());
			
			return TCL_OK;
		}
		
		/* The "getIdleConsumption" returns energy consumed by the network in idle. */
		
		if (strcmp(argv[1], "getIdleConsumption") == 0) {
			
			tcl.resultf("%f", getIdleConsumption());
			
			return TCL_OK;
		}
		
		/* The "getRxConsumption" returns the energy consumed by the network in receiving mode. */
		
		if (strcmp(argv[1], "getRxConsumption") == 0) {
			
			tcl.resultf("%f", getRxConsumption());
			
			return TCL_OK;
		}
		
		/* The "getTotTxConsumption" returns the energy consumed by the network while transmitting. */
		
		if (strcmp(argv[1], "getTotTxConsumption") == 0) {
			
			tcl.resultf("%f", getTotTxConsumption());
			
			return TCL_OK;
		}
	
	}
	else if ( argc == 3 ) {
		
		if (strcmp(argv[1], "setOutputFile") == 0) {
		
			memset(fileOut, '\0', FILE_NAME);
			sprintf(fileOut, "%s", argv[2]);
			
			return TCL_OK;
		}	
		
		if (strcmp(argv[1], "setLogFile") == 0) {
		
			memset(fileOut2, '\0', FILE_NAME);
			sprintf(fileOut2, "%s", argv[2]);
			
			return TCL_OK;
		}	
		
		if (strcmp(argv[1], "setPhyPreambleSize") == 0) {
		
			preamble_size = atoi(argv[2]);
			
			return TCL_OK;
		}	
		
		if (strcmp(argv[1], "setStartTraffic") == 0) {
		
			start_traffic = atof(argv[2]);
			
			return TCL_OK;
		}	
		
		if (strcmp(argv[1], "setRunId") == 0) {
		
			run_id = atoi(argv[2]);
			
			return TCL_OK;
		}	
		
		if (strcmp(argv[1], "setMaxNodeId") == 0) {
		
			max_node_id = atoi(argv[2]);
			
			return TCL_OK;
		}	

		if (strcmp(argv[1], "setBinaryOutput") == 0) {
			
			binaryOutput = atoi(argv[2]);
			
			return TCL_OK;
		}
		
		if (strcmp(argv[1], "setTotalEnergy") == 0) {
		
			totalEnergy = atof(argv[2]);
			
			return TCL_OK;
		}

		/* to collect the  number of data packet generated in the network by a given node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getGeneratedPacket") == 0) {
			
			int src = atoi(argv[2]);
			
			tcl.resultf("%f", getGeneratedPacket(src));
			
			return TCL_OK;
		}

		/* to collect the  number of data packet delivered in the network by a given node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getDeliveredPacket") == 0) {
			
			int src = atoi(argv[2]);
			
			tcl.resultf("%f", getDeliveredPacket(src));
			
			return TCL_OK;
		}
		
		/* The "getTxTime" returns the time (in sec.) spent by a given node while transmitting using a given tx power. */
		
		if (strcmp(argv[1], "getTxTime") == 0) {
			
			tcl.resultf("%f", getTxTime(atof(argv[2])));
			
			return TCL_OK;
		}
		
		/* The "getTxConsumption" returns energy power by a given node while transmitting using a given tx power. */
		
		if (strcmp(argv[1], "getTxConsumption") == 0) {
			
			tcl.resultf("%f", getTxConsumption(atof(argv[2])));
			
			return TCL_OK;
		}
		
		/* The "getResidualEnergy" returns the residual energy of a given node. */
		
		if (strcmp(argv[1], "getResidualEnergy") == 0) {
			
			int id = atoi(argv[2]);
			
			tcl.resultf("%f", getResidualEnergy(id));
			
			return TCL_OK;
		}
		
		/* The "getRxTime" returns the time (in sec.) spent by a given node in receiving mode. */
		
		if (strcmp(argv[1], "getRxTime") == 0) {
			
			int id = atoi(argv[2]);
			
			tcl.resultf("%f", getRxTime(id));
			
			return TCL_OK;
		}
		
		/* The "getIdleConsumption" returns energy consumed by a given node in idle. */
		
		if (strcmp(argv[1], "getIdleConsumption") == 0) {
			
			int id = atoi(argv[2]);
			
			tcl.resultf("%f", getIdleConsumption(id));
			
			return TCL_OK;
		}
		
		/* The "getIdleConsumption" returns energy consumed by a given node in idle. */
		
		if (strcmp(argv[1], "getIdleTime") == 0) {
			
			int id = atoi(argv[2]);
			
			tcl.resultf("%f", getIdleTime(id));
			
			return TCL_OK;
		}
		
		/* The "getTotTxTime" returns the time (in sec.) spent by a given node while transmitting. */
		
		if (strcmp(argv[1], "getTotTxTime") == 0) {
			
			int id = atoi(argv[2]);
			
			tcl.resultf("%f", getTotTxTime(id));
			
			return TCL_OK;
		}

		
	}
	else if ( argc == 4 ) {
		
		int src = atoi(argv[2]);
		int dst = atoi(argv[3]);

		/* to collect the  Packet delivery ratio between node src and node dst. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getPDR") == 0) {
			
			tcl.resultf("%f", getPDR(src, dst));
			
			return TCL_OK;
		}

		/* to collect the  number of data packet generated by src node to dst node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getGeneratedPacket") == 0) {
			
			tcl.resultf("%d", getGeneratedPacket(src, dst));
			
			return TCL_OK;
		}

		/* to collect the  number of data packet delivered by src node to dst node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getDeliveredPacket") == 0) {
			
			tcl.resultf("%d", getDeliveredPacket(src, dst));
			
			return TCL_OK;
		}
		
		/* to collect the  number of duplicated data packet delivered by src node to dst node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getDeliveredDuplicatedPacket") == 0) {
			
			tcl.resultf("%d", getDeliveredDuplicatedPacket(src, dst));
			
			return TCL_OK;
		}

		/* to collect the  throughput at the application layer between src node and dst node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getApplicationThroughput") == 0) {
			
			tcl.resultf("%f", getApplicationThroughput(src, dst));
			
			return TCL_OK;
		}

		/* to collect the average end-to-end packet latency between src node and dst node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getPacketLatency") == 0) {
			
			tcl.resultf("%f", getPacketLatency(src, dst));
			
			return TCL_OK;
		}

		/* to collect the average route length between src node and dst node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getRouteLength") == 0) {
			
			tcl.resultf("%f", getRouteLength(src, dst));
			
			return TCL_OK;
		}

		/* to collect the number of different routes used between src node and dst node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getNumRoutes") == 0) {
			
			tcl.resultf("%d", getNumRoutes(src, dst));
			
			return TCL_OK;
		}

		/* to collect the maximal route length used between src node and dst node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getMaxRouteLength") == 0) {
			
			tcl.resultf("%d", getMaxRouteLength(src, dst));
			
			return TCL_OK;
		}

		/* to collect the amount of overhead information needed to deliver one bit of data 
		between src node and dst node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getOverheadPerBit") == 0) {
			
			tcl.resultf("%f", getOverheadPerBit(src, dst));
			
			return TCL_OK;
		}

		/* to collect the throughput at the MAC layer on the link between src node and dst node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getMacThroughput") == 0) {
			
			tcl.resultf("%f", getMacThroughput(src, dst));
			
			return TCL_OK;
		}

		/* to collect the average number of data packet retransmissions at the MAC layer 
		between src node and dst node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getMacDataRetransmissions") == 0) {
			
			tcl.resultf("%f", getMacDataRetransmissions(src, dst));
			
			return TCL_OK;
		}
	
		/* to collect the  number of data packet discarded at the MAC layer 
		between src node and dst node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getMacDataPacketDiscarded") == 0) {
			
			tcl.resultf("%d", getMacDataPacketDiscarded(src, dst));
			
			return TCL_OK;
		}
	
		/* to collect the  number of control packet discarded at the MAC layer 
		between src node and dst node. 
		It can be done from the Tcl file over time */
		if (strcmp(argv[1], "getMacCtrlPacketDiscarded") == 0) {
			
			tcl.resultf("%d", getMacCtrlPacketDiscarded(src, dst));
			
			return TCL_OK;
		}
		
		/* The "getTxTime" returns the time (in sec.) spent by a given node while transmitting using a given tx power. */
		
		if (strcmp(argv[1], "getTxTime") == 0) {
			
			tcl.resultf("%f", getTxTime(atoi(argv[2]), atof(argv[3])));
			
			return TCL_OK;
		}
		
		/* The "getTxConsumption" returns energy power by a given node while transmitting using a given tx power. */
		
		if (strcmp(argv[1], "getTxConsumption") == 0) {
			
			tcl.resultf("%f", getTxConsumption(atoi(argv[2]), atof(argv[3])));
			
			return TCL_OK;
		}
	
	}
	
	return Sunset_Statistics::command(argc, argv);
}

/*!
 * 	@brief The start() function can be called from the TCL script to execute statistics module operations 
 *	when the simulation/emulation experiment starts.
 */

void Sunset_Protocol_Statistics::start() 
{
	if (!useStat) {
		
		return;
	}

	outFile.open(fileOut, ios::out );
	outFile.setf( ios::left, ios::adjustfield );    	
	
	if (!outFile.is_open()) {
		
		Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::start outputFile %s cannot be opened ERROR", fileOut);

		return;
	}
	
	Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::start useStat %d path %s", useStat, fileOut);
	
	if (Sunset_Utilities::isSimulation()) {

		start_time = Sunset_Utilities::get_now();
	}
}

/*!
 * 	@brief The stop() function can be called from the TCL script to execute statistics module operations 
 *	when the simulation/emulation experiment ends to provide the processed information which are also logged
 *	on the file defined by the user.
 */
void Sunset_Protocol_Statistics::stop() 
{
	Sunset_Statistics::stop();
	
	if (!useStat) {
		
		return;
	}
	
	if (Sunset_Utilities::isEmulation()) {
		
		return;
	}
	
	if (!outFile.is_open()) {
		
		Sunset_Debug::debugInfo(-1, -1,"Sunset_Protocol_Statistics::stop outputFile %s was not opened ERROR", fileOut);
		
		return;
	}

	outFile.close();
	Sunset_Debug::debugInfo(5, -1, "Sunset_Protocol_Statistics::stop useStat %d path %s isOpen %d", useStat, fileOut, outFile.is_open());
	
	if (Sunset_Utilities::isSimulation()) {
		
		stop_time = Sunset_Utilities::get_now();
		show_data();
	}
}



/*!
 * 	@brief The pktType function returns the statistic packet type associated to the input packet.
 *	@param p The input packet we want to know the statistic packet associated to
 *	@retval Statistic packet type
 */

sunset_statisticPktType Sunset_Protocol_Statistics::pktType(Packet *p) 
{
	sunset_statisticPktType sPktType;
	packet_t pType = HDR_CMN(p)->ptype();
	u_int8_t  type;
	u_int8_t  subtype;
	
	if (pType == PT_SUNSET_AGT) {
	
		if (HDR_SUNSET_AGT(p)->ac.ac_type == SUNSET_AGT_Type_Data) {
		
			sPktType = SUNSET_STAT_DATA;
		}
		else if (HDR_SUNSET_AGT(p)->ac.ac_type == SUNSET_AGT_Type_Control) {
		
			sPktType = SUNSET_STAT_CTRL;
		}	
		
		return SUNSET_STAT_DATA;
	}
	
	else if(pType == PT_SUNSET_MAC) {
		
		type = HDR_SUNSET_MAC(p)->dh_fc.fc_type;
		subtype = HDR_SUNSET_MAC(p)->dh_fc.fc_subtype;
		
		switch(type) {					
		
			case SUNSET_MAC_Type_Control:
			
				switch(subtype) {
					
					case SUNSET_MAC_Subtype_RTS:
						sPktType = SUNSET_STAT_RTS;
						break;
					
					case SUNSET_MAC_Subtype_CTS:
						sPktType = SUNSET_STAT_CTS;
						break;
					
					case SUNSET_MAC_Subtype_WARN:
						sPktType = SUNSET_STAT_WARNING;
						break;
					
					case SUNSET_MAC_Subtype_TONE:
						sPktType = SUNSET_STAT_TONE;
						break;
					
					case SUNSET_MAC_Subtype_ACK:
						sPktType = SUNSET_STAT_ACK;
						break;
					
					default:
						Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::pktType recvTimer2:Invalid MAC Control Subtype %x ERROR", subtype);
						
						return SUNSET_STAT_NONE;
				}
				
				break;
				
			case SUNSET_MAC_Type_Data:
				return SUNSET_STAT_DATA;
			
			default:
				Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::pktType recvTimer3:Invalid MAC Control Subtype %x ERROR", subtype);
				
				return SUNSET_STAT_NONE;
		}
	}
	else if(pType == PT_SUNSET_RTG) {
		
		sPktType = SUNSET_STAT_CTRL;
	}
	else if(pType == PT_MCBR) {
		
		sPktType = SUNSET_STAT_DATA;
	}
	else if (pType == -1) {
		
		Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::pktType STAT_NONE pkt");
		
		return SUNSET_STAT_NONE;
	}
	else {
		Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::pktType recvTimer4:Invalid MAC Control Subtype %x ERROR", pType);
		
		return SUNSET_STAT_NONE;
	}
	
	return sPktType;
}


/*!
 * 	@brief The logStatInfo function collects the statisitcal information according to the parameters in input.
 *		It updates all the different data structures involved in the reported action and store the data (binary 
 *		or ASCII format) in the file defined by the user.
 *	@param sType The type of the performed action. The list of the supported actions is in the sunset_statistics.h file where te Sunset_Statistics class is defined.
 *	@param node The ID of the node reporting the actions.
 *	@param p The packet containing the informaiton related to the action, it is zero if the action is not related to any packet. 
 *	@param time The time when the action occurred.
 *	@param fmt Additional information related to the current action, if any.
 */
void Sunset_Protocol_Statistics::logStatInfo(sunset_statisticType sType, u_int16_t node, Packet*p, double time, const char *fmt, ...) 
{
	char command[STAT_MAX_BUF];
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int agt_src = 0;
	int agt_dst = 0;
	int agt_pktId = 0;
	int src = 0;
	int dst = 0;
	int pkt_size = 0;
	int num_hop = 0;
	double lq = -10;
	double rssi = 0;
	double realTime = 0.0;
	statInfo st;

	Tcl& tcl = Tcl::instance();

	memset(command, '\0', STAT_MAX_BUF);
	
	if (!useStat) {
		
		return;
	}
	
	Sunset_Utilities::sync_time();

	if ((!useStat || !outFile.is_open())) {

		return;
	}
	
	if (Sunset_Utilities::isSimulation() && sType == SUNSET_STAT_MAC_BACKOFF) {
		return;
	}
	
	if (sType == SUNSET_STAT_MODEM_INFO) {
	
		if (strcmp(fmt, "") == 0) {
		
			return;
		}		
	}
	
	switch ( sType ) {
			
		case SUNSET_STAT_ENERGY_TX: 
		{
			va_list vl;
			va_start(vl, fmt);
			
			double watt=va_arg(vl, double);
			double txPower=va_arg(vl, double);
			double sec=va_arg(vl, double);
			
			setTxDuration(node, watt, txPower, sec);
			
			return;
		}
			
		case SUNSET_STAT_ENERGY_RX: 
		{
			va_list vl;
			va_start(vl, fmt);
			
			double rxPower=va_arg(vl, double);
			double sec=va_arg(vl, double);
			
			setRxDuration(node, rxPower, sec);
			
			return;
		}
			
		case SUNSET_STAT_ENERGY_IDLE: 
		{
			va_list vl;
			va_start(vl, fmt);
			
			double idlePower=va_arg(vl, double);
			double sec=va_arg(vl, double);
			
			setIdleDuration(node, idlePower, sec);
			
			return;
		}
			
		default:
			break;
	}
	
	if (p != 0 ) {
		
		spktType = pktType(p);
		agt_src = HDR_SUNSET_AGT(p)->srcId();
		agt_dst = HDR_SUNSET_AGT(p)->dstId();
		agt_pktId = HDR_SUNSET_AGT(p)->pktId();
		src = HDR_SUNSET_MAC(p)->src;
		dst = HDR_SUNSET_MAC(p)->dst;
		pkt_size = HDR_CMN(p)->size();
		num_hop = HDR_CMN(p)->num_forwards();   
		lq = SUNSET_HDR_CMN(p)->link_quality;
		rssi = SUNSET_HDR_CMN(p)->rssi;
	}
	
	realTime = Sunset_Utilities::get_epoch();
	
	st.sType = sType;
	st.agt_src = agt_src;
	st.agt_dst = agt_dst;
	st.agt_pktId = agt_pktId;
	st.spktType = spktType;
	st.realTime = realTime;
	st.time = time;
	st.node = node;
	st.src = src;
	st.dst = dst;
	st.pkt_size = pkt_size;
	st.num_hop = num_hop;
	st.link_quality = lq;
	st.rssi = rssi;
		
	if (sType == SUNSET_STAT_AGENT_TX) {
		
		src = 0;
		dst = 0;
	} 
	
	if (outFile.is_open()) {
		
		va_list ap;
		va_start(ap, fmt);
		char* stPtr;
		int size = 0;
		
		if (binaryOutput) {
			
			int len = 0;
			
			vsprintf(command, fmt, ap);
			
			len = strlen(command);
			size = (int)(sizeof(statInfo) + sizeof(int));
			stPtr = (char*)malloc(size);
			
			if ( stPtr == NULL ) {
				
				Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::logStatInfo MALLOC ERROR");
				
				return;
			}
			
			memset(stPtr, '\0', size);
			memcpy(stPtr, (&size), sizeof(int)); 
			memcpy(&(stPtr[sizeof(int)]), (char*)(&st), sizeof(statInfo)); 
		}
		else {
			sprintf(command, "%d %d %d %d %d %6.4f %d %d %d %d %d %6.4f %6.4f %6.4f", 
				(int)sType, agt_src, agt_dst, agt_pktId, spktType, realTime, node, src, dst, pkt_size, num_hop, time, rssi, lq);
			
			vsprintf(command + strlen(command), fmt, ap);
			sprintf(command + strlen(command), "\n");
		}
		
		if (Sunset_Utilities::isEmulation()) {
			
			pthread_mutex_lock(&mutex_stat);
		}
		
		outFile.write(stPtr, size);
		outFile.write(command, (int)(strlen(command)));
		
		if (Sunset_Utilities::isEmulation()) {
			
			pthread_mutex_unlock(&mutex_stat);	
		}
		
		outFile.flush();
	}
	
	update_data(st);
}


/*!
 * 	@brief The update_data function updates the different data structures according to the statistics data 
 *		structure in input.
 *	@param st The data structure conatining all the statistics information for a given action.
 */
void Sunset_Protocol_Statistics::update_data(statInfo st) 
{

	if (st.spktType == SUNSET_STAT_NONE) {
		
		return;
	}
	
	switch(st.sType) {
		
		case SUNSET_STAT_AGENT_TX:
			processCreateData(st);
			break;
			
		case SUNSET_STAT_AGENT_RX:
			processRecvData(st);
			break;
		
		case SUNSET_STAT_ENQUE:
			processQueueLength(st, 1);
			break;
		
		case SUNSET_STAT_DEQUE:
			processQueueLength(st, -1);
			break;
		
		case SUNSET_STAT_QUEUE_DISCARD:
			processQueueLength(st, -1);
			break;
		
		case SUNSET_STAT_MAC_TX:
			processMacSent(st);
			break;
		
		case SUNSET_STAT_MAC_RX:
			processMacRecv(st);
			break;
		
		case SUNSET_STAT_MAC_RX_ERROR:
			processMacRecvFailed(st);
			break;
		
		case SUNSET_STAT_MAC_DISCARD:
			processMacDiscard(st);
			break;
		
		case SUNSET_STAT_MAC_TX_DONE:
			processTxDone(st);
			break;
		
		case SUNSET_STAT_MAC_TX_ABORTED:
			processTxAborted(st);
			break;
		
		case SUNSET_STAT_MAC_NEW:
			processNewMacData(st);
			break;
		
		default:
			return;
	}
}

/*!
 * 	@brief The processCreateData function updates the different data structures according to the creation of a
 *		new data packet at the application layer.
 *	@param st The data structure conatining all the statistics information for a given action.
 */
void Sunset_Protocol_Statistics::processCreateData(statInfo st) 
{
	double time = st.realTime;
	int node = st.agt_src;
	int dest = st.agt_dst;
	int size = st.pkt_size;
	int pkt_id = st.agt_pktId;
	
	((pkt_size_info[st.spktType])[node])[pkt_id] = size;
	((pkt_dest_info[st.spktType])[node])[pkt_id] = dest;
	(((pkt_sent[st.spktType])[node])[pkt_id]).push_back(time);
	
	Sunset_Trace::print_info("stat - (%f) Node:%d - AGT_TX node %d  destination %d size %d id %d len %d\n", time, node, node, dest, size, pkt_id, (((pkt_sent[st.spktType])[node])[pkt_id]).size());

}

/*!
 * 	@brief The processRecvData function updates the different data structures according to the reception of a
 *		new data packet at the application layer.
 *	@param st The data structure conatining all the statistics information for a given action.
 */
void Sunset_Protocol_Statistics::processRecvData(statInfo st) 
{
	double time = st.realTime;
	int src = st.agt_src;
	int node = st.agt_dst;
	int size = st.pkt_size;
	int pkt_id = st.agt_pktId;
	int num_hop = st.num_hop;
	
	if (pkt_recv.find(st.spktType) != pkt_recv.end() && 
	    pkt_recv[st.spktType].find(node) != pkt_recv[st.spktType].end() &&
	    (pkt_recv[st.spktType])[node].find(src) != (pkt_recv[st.spktType])[node].end() &&	
	    ((pkt_recv[st.spktType])[node])[src].find(pkt_id) != ((pkt_recv[st.spktType])[node])[src].end()) {
		
		(((avg_num_hop_dup[st.spktType])[node])[src])[pkt_id].push_back(num_hop);        
	}
	else {
		
		(((avg_num_hop[st.spktType])[node])[src])[pkt_id].push_back(num_hop);
	}
	
	((((pkt_recv[st.spktType])[node])[src])[pkt_id]).push_back(time);
	Sunset_Trace::print_info("stat - (%f) Node:%d - AGT_RX source %d destination %d size %d id %d len %d time %f\n", time, node, src, node, size, pkt_id, ((((pkt_recv[st.spktType])[node])[src])[pkt_id]).size(), st.realTime);
}


/*!
 * 	@brief The processNewMacData function updates the different data structures according to the reception of a
 *		new data packet coming from the upper layer.
 *	@param st The data structure conatining all the statistics information for a given action.
 */
void Sunset_Protocol_Statistics::processNewMacData(statInfo st) 
{
	
	double time = st.realTime;
	double simTime = st.time;
	int src = st.src;
	int dst = st.dst;
	int node = st.node;
	int size = st.pkt_size - preamble_size; //remove preamble size since it is due to modem coding/decoding and training time are not bytes transmitted in water

	((((mac_new_pkt[st.spktType])[node])[dst])[size]).push_back(time);
}

/*!
 * 	@brief The processMacSent function updates the different data structures according to the transmission of a
 *		data packet at the MAC layer.
 *	@param st The data structure conatining all the statistics information for a given action.
 */
void Sunset_Protocol_Statistics::processMacSent(statInfo st) 
{
	double time = st.realTime;
	double simTime = st.time;
	int src = st.src;
	int dst = st.dst;
	int node = st.node;
	int size = st.pkt_size - preamble_size; // remove preamble size since it is due to modem coding/decoding and training time are not bytes transmitted in water 
	
	((((mac_pkt_sent[st.spktType])[node])[dst])[size]).push_back(time);
}

/*!
 * 	@brief The processMacDiscard function updates the different data structures when a packet is discarded at the MAC layer.
 *	@param st The data structure conatining all the statistics information for a given action.
 */
void Sunset_Protocol_Statistics::processMacDiscard(statInfo st) 
{
	double time = st.realTime;
	double simTime = st.time;
	int src = st.src;
	int dst = st.dst;
	int node = st.node; 
	int size = st.pkt_size - preamble_size; // remove preamble size since it is due to modem coding/decoding and training time are not bytes transmitted in water
	
	((((mac_pkt_discard[st.spktType])[node])[dst])[size]).push_back(time);
}

/*!
 * 	@brief The processTxDone function updates the different data structures when the trasnmission of
 *		a packet is correctly completed at the MAC layer.
 *	@param st The data structure conatining all the statistics information for a given action.
 */
void Sunset_Protocol_Statistics::processTxDone(statInfo st) 
{
	double time = st.realTime;
	double simTime = st.time;
	int node = st.src;
	int dst = st.dst;
	int size = st.pkt_size - preamble_size; // remove preamble size since it is due to modem coding/decoding and training time are not bytes transmitted in water
	
	((((tx_done[st.spktType])[node])[dst])[size]).push_back(time);
}


/*!
 * 	@brief The processTxAborted function updates the different data structures when an error occurs during 
 * 		the trasnmission of a packet at the MAC layer.
 *	@param st The data structure conatining all the statistics information for a given action.
 */
void Sunset_Protocol_Statistics::processTxAborted(statInfo st) 
{
	double time = st.realTime;
	double simTime = st.time;
	int node = st.src;
	int dst = st.dst;
	int size = st.pkt_size - preamble_size; //data_size; //getSizeStr(s);
	//remove preamble size since it is due to modem coding/decoding and training time are not bytes transmitted in water
	
	((((tx_aborted[st.spktType])[node])[dst])[size]).push_back(time);
}


/*!
 * 	@brief The processMacRecv function updates the different data structures when a packet is received at the 
 * 		MAC layer.
 *	@param st The data structure conatining all the statistics information for a given action.
 */
void Sunset_Protocol_Statistics::processMacRecv(statInfo st) 
{
	double time = st.realTime;
	double simTime = st.time;
	int node = st.node;
	int src = st.src;
	int dst = st.dst;
	int size = st.pkt_size - preamble_size; //remove preamble size since it is due to modem coding/decoding and training time are not bytes transmitted in water
	double lq = st.link_quality;	
	pair<double, double> p_info;
	p_info.first = time;
	p_info.second = lq;
	
	(((((mac_pkt_recv[st.spktType])[node])[src])[dst])[size]).push_back(p_info);
}

/*!
 * 	@brief The processMacRecvFailed function updates the different data structures when an error occurs during 
 *		the reception of a packet at the MAC layer.
 *	@param st The data structure conatining all the statistics information for a given action.
 */
void Sunset_Protocol_Statistics::processMacRecvFailed(statInfo st) 
{
	double time = st.realTime;
	double simTime = st.time;
	int node = st.node;
	
	((mac_pkt_recv_failed[st.spktType])[node]).push_back(time);
}


/*!
 * 	@brief The processQueueLength function updates the different data structures when an action occur on the
 * 		packet queue: a packet is added or removed.
 *	@param st The data structure conatining all the statistics information for a given action.
 *	@param x The number of packet added (if it is a positive value) or removed (if it is a negative value).
 */
void Sunset_Protocol_Statistics::processQueueLength(statInfo st, int x) 
{
	double time = st.realTime;
	double simTime = st.time;
	int node = st.node;
	
	(queue_length[node]).push_back(x);
}

/*!
 * 	@brief The getGeneratedPacket() function returns the number of packet generated by the src node to the dst node. 
 *	@param src The source node generating data.
 *	@param dst The destination node. 
 *	@retval generated_pkt The number of packet generated by the src node to the dst node.
 */

int Sunset_Protocol_Statistics::getGeneratedPacket(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	int tot_pkt_sent = 0;
	int count = -1;
	
	map < sunset_statisticPktType, map <int, map <int, list <double> > > >::iterator it;
	map <int, map <int, list <double> > >::iterator it1;
	map <int, list <double> >::iterator it2;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it3;
	map <int, map <int, map <int, list <double> > > >::iterator it4;
	
	it = pkt_sent.begin();
	
	for (; it != pkt_sent.end(); it++) {
		
		spktType = it->first;
		
		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}
		
		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
			
			node_src = it1->first;
			
			if (node_src != src) {

				continue;
			
			}
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				pkt_id = it2->first;

				if (pkt_dest_info.find(spktType) == pkt_dest_info.end() ||
				    (pkt_dest_info[spktType]).find(node_src) == (pkt_dest_info[spktType]).end() ||
				    (pkt_dest_info[spktType])[node_src].find(pkt_id) == (pkt_dest_info[spktType])[node_src].end()) {
				
					Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::getGeneratedPacket dest node %d pkt_id %d possible ERROR", node_src, pkt_id);
					
					continue;
				}

				node_dst = ((pkt_dest_info[spktType])[node_src])[pkt_id];

				if (node_dst != dst) {

					continue;
				
				}
				
				count = (it2->second).size();
				tot_pkt_sent += count;
				
				if (count > 1) {
					
					Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::getGeneratedPacket node %d pkt_id %d count %d possible ERROR", node_src, pkt_id, count);
					
					continue;
				}
			}
		}
	}
	

	return tot_pkt_sent;
}

/*!
 * 	@brief The getDeliveredPacket() function returns the number of packet delivered by the src node to the dst node. 
 *	@param src The source node generating data.
 *	@param dst The destination node receiving the data. 
 *	@retval delivered_pkt The number of packet delivered by the src node to the dst node.
 */

int Sunset_Protocol_Statistics::getDeliveredPacket(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	int tot_pkt_received = 0;
	int count = -1;
	
	map <int, map <int, list <double> > >::iterator it1;
	map <int, list <double> >::iterator it2;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it3;
	map <int, map <int, map <int, list <double> > > >::iterator it4;
	
	it3 = pkt_recv.begin();
	
	for (; it3 != pkt_recv.end(); it3++) {
		
		spktType = it3->first;
		
		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it4 = (it3->second).begin();
		
		for (; it4 != (it3->second).end(); it4++) {
			
			node_dst = it4->first;
			
			if (node_dst != dst) {
			
				continue;
				
			}
			
			it1 = (it4->second).begin();
			
			for (; it1 != (it4->second).end(); it1++) {
			
				node_src = it1->first;
				
				if (node_src != src) {
				
					continue;
				
				}
				
				it2 = (it1->second).begin();
				
				for (; it2 != (it1->second).end(); it2++) {
					
					pkt_id = it2->first;
					count = (it2->second).size();
					
					tot_pkt_received++;
										
				}
			}
		}
	}
	
	return tot_pkt_received;

}

/*!
 * 	@brief The getDeliveredDuplicatedPacket() function returns the number of duplicated packet delivered by the src node to the dst node. 
 *	@param src The source node generating data.
 *	@param dst The destination node receiving the data. 
 *	@retval delivered_pkt The number of duplicated packet delivered by the src node to the dst node.
 */

int Sunset_Protocol_Statistics::getDeliveredDuplicatedPacket(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	int pkt_received_dup = 0;
	int count = -1;
	
	map <int, map <int, list <double> > >::iterator it1;
	map <int, list <double> >::iterator it2;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it3;
	map <int, map <int, map <int, list <double> > > >::iterator it4;
	
	it3 = pkt_recv.begin();
	
	for (; it3 != pkt_recv.end(); it3++) {
		
		spktType = it3->first;
		
		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it4 = (it3->second).begin();
		
		for (; it4 != (it3->second).end(); it4++) {
			
			node_dst = it4->first;
			
			if (node_dst != dst) {
			
				continue;
			
			}
			
			it1 = (it4->second).begin();
			
			for (; it1 != (it4->second).end(); it1++) {
			
				node_src = it1->first;

				if (node_src != src) {
				
					continue;
				
				}
				
				it2 = (it1->second).begin();
				
				for (; it2 != (it1->second).end(); it2++) {
					
					pkt_id = it2->first;
					
					count = (it2->second).size();
										
					if (count > 1) {
					
						pkt_received_dup += count - 1;
					}
					
				}
			}
		}
	}
	
	return pkt_received_dup;
	
}

/*!
 * 	@brief The getGeneratedBytes() function returns the number of bytes generated by the src node to the dst node. 
 *	@param src The source node generating data.
 *	@param dst The destination node. 
 *	@retval generated_bytes The number of bytes generated by the src node to the dst node.
 */

int Sunset_Protocol_Statistics::getGeneratedBytes(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	int tot_bytes_sent = 0;
	int count = -1;
	
	map < sunset_statisticPktType, map <int, map <int, list <double> > > >::iterator it;
	map <int, map <int, list <double> > >::iterator it1;
	map <int, list <double> >::iterator it2;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it3;
	map <int, map <int, map <int, list <double> > > >::iterator it4;
	
	it = pkt_sent.begin();
	
	for (; it != pkt_sent.end(); it++) {
		
		spktType = it->first;
		
		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
			
			node_src = it1->first;
			
			if (node_src != src) {

				continue;
			
			}
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				pkt_id = it2->first;

				if (pkt_dest_info.find(spktType) == pkt_dest_info.end() ||
				    (pkt_dest_info[spktType]).find(node_src) == (pkt_dest_info[spktType]).end() ||
				    (pkt_dest_info[spktType])[node_src].find(pkt_id) == (pkt_dest_info[spktType])[node_src].end()) {
				
					Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::getGeneratedPacket dest node %d pkt_id %d possible ERROR", node_src, pkt_id);
					
					continue;
				}

				node_dst = ((pkt_dest_info[spktType])[node_src])[pkt_id];

				if (node_dst != dst) {

					continue;
				
				}
				
				count = (it2->second).size();
				
				if (count > 1) {
					
					Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::getGeneratedPacket node %d pkt_id %d count %d possible ERROR", node_src, pkt_id, count);
					
					continue;
				}
				
				if (pkt_size_info.find(spktType) == pkt_size_info.end() ||
				    (pkt_size_info[spktType]).find(node_src) == (pkt_size_info[spktType]).end() ||
				    (pkt_size_info[spktType])[node_src].find(pkt_id) == (pkt_size_info[spktType])[node_src].end()) {
				
					Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::getGeneratedPacket size node_src %d pkt_id %d possible ERROR", node_src, pkt_id);
					
					continue;
				}
				
				tot_bytes_sent += count * ((pkt_size_info[spktType])[node_src])[pkt_id];
			}
		}
	}
	
	return tot_bytes_sent;
	
}

/*!
 * 	@brief The getDeliveredBytes() function returns the number of bytes delivered by the src node to the dst node. 
 *	@param src The source node generating data.
 *	@param dst The destination node receiving the data. 
 *	@retval result The number of bytes delivered by the src node to the dst node.
 */

int Sunset_Protocol_Statistics::getDeliveredBytes(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	int tot_bytes_delivered = 0;
	int count = -1;
	
	map <int, map <int, list <double> > >::iterator it1;
	map <int, list <double> >::iterator it2;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it3;
	map <int, map <int, map <int, list <double> > > >::iterator it4;
	
	it3 = pkt_recv.begin();
	
	for (; it3 != pkt_recv.end(); it3++) {
		
		spktType = it3->first;
		
		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it4 = (it3->second).begin();
		
		for (; it4 != (it3->second).end(); it4++) {
			
			node_dst = it4->first;

			if (node_dst != dst) {
			
				continue;
			
			}

			it1 = (it4->second).begin();
			
			for (; it1 != (it4->second).end(); it1++) {
			
				node_src = it1->first;

				if (node_src != src) {
				
					continue;
				
				}

				it2 = (it1->second).begin();
				
				for (; it2 != (it1->second).end(); it2++) {
					
					pkt_id = it2->first;

					count = (it2->second).size();
					
					if (pkt_size_info.find(spktType) == pkt_size_info.end() ||
					    (pkt_size_info[spktType]).find(src) == (pkt_size_info[spktType]).end() 
					    || (pkt_size_info[spktType])[src].find(pkt_id) == (pkt_size_info[spktType])[src].end()) {
						
						Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::getDeliveredBytes size node %d pkt_id %d possible ERROR", src, pkt_id);
						
						continue;
					}
					
					tot_bytes_delivered += ((pkt_size_info[spktType])[src])[pkt_id];
					
				}
			}
		}
	}
	
	return tot_bytes_delivered;
}

/*!
 * 	@brief The getDeliveredDuplicatedBytes() function returns the number of duplicated bytes delivered by the src node to the dst node. 
 *	@param src The source node generating data.
 *	@param dst The destination node receiving the data. 
 *	@retval result The number of deplicated bytes delivered by the src node to the dst node.
 */
int Sunset_Protocol_Statistics::getDeliveredDuplicatedBytes(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	int tot_bytes_delivered_dup = 0;
	int count = -1;
	
	map <int, map <int, list <double> > >::iterator it1;
	map <int, list <double> >::iterator it2;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it3;
	map <int, map <int, map <int, list <double> > > >::iterator it4;
	
	it3 = pkt_recv.begin();
	
	for (; it3 != pkt_recv.end(); it3++) {
		
		spktType = it3->first;
		
		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it4 = (it3->second).begin();
		
		for (; it4 != (it3->second).end(); it4++) {
			
			node_dst = it4->first;

			if (node_dst != dst) {
			
				continue;
			
			}

			it1 = (it4->second).begin();
			
			for (; it1 != (it4->second).end(); it1++) {
			
				node_src = it1->first;

				if (node_src != src) {
				
					continue;
				
				}

				it2 = (it1->second).begin();
				
				for (; it2 != (it1->second).end(); it2++) {
					
					pkt_id = it2->first;

					count = (it2->second).size();
					
					if (count > 1) {
					
						if (pkt_size_info.find(spktType) == pkt_size_info.end() ||
						(pkt_size_info[spktType]).find(src) == (pkt_size_info[spktType]).end() 
						|| (pkt_size_info[spktType])[src].find(pkt_id) == (pkt_size_info[spktType])[src].end()) {
							
							Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::getDeliveredBytes size node %d pkt_id %d possible ERROR", node_dst, pkt_id);
							
							continue;
						}

						tot_bytes_delivered_dup += (count - 1) * ((pkt_size_info[spktType])[src])[pkt_id];
					}
					
				}
			}
		}
	}
	
	return tot_bytes_delivered_dup;
}

/*!
 * 	@brief The getRouteLength() function returns the average route length traversed by packets delivered by the src node to the dst node. 
 *	@param src The source node generating data.
 *	@param dst The destination node receiving the data. 
 *	@retval result The average route length traversed by packets delivered by the src node to the dst node.
 */
double Sunset_Protocol_Statistics::getRouteLength(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	double num_hop = 0.0;
	int count = 0;
	
	map < sunset_statisticPktType, map <int, map <int, map <int, list <int> > > > >::iterator it;
	map <int, map <int, map <int, list <int> > > >::iterator it1;
	map <int, map <int, list <int> > >::iterator it2;
	map <int, list <int> >::iterator it3;
	list <int>::iterator it4;
		
	it = avg_num_hop.begin();
	
	for (; it != avg_num_hop.end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_dst = it1->first;
			
			if (node_dst != dst) {
			
				continue;
				
			}
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_src = it2->first;
				
				if (node_src != src) {
				
					continue;
					
				}
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					pkt_id = it3->first;
					it4 = (it3->second).begin();
					
					if (it4 != (it3->second).end()) {
					
						num_hop += *it4;
						count++;
						
					}
				}
				
				if (count > 0) {
					
					num_hop = num_hop / (double)count;
				}
				else {
					num_hop = 0.0;
				}
				
			}
		}
	}    
	
	return num_hop;
}

/*!
 * 	@brief The getDuplicatedRouteLength() function returns the average route length traversed by duplicated packets delivered by the src node to the dst node. 
 *	@param src The source node generating data.
 *	@param dst The destination node receiving the data. 
 *	@retval result The average route length traversed by duplicated packets delivered by the src node to the dst node.
 */

double Sunset_Protocol_Statistics::getDuplicatedRouteLength(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	double num_hop = 0.0;
	int count = 0;
	
	map < sunset_statisticPktType, map <int, map <int, map <int, list <int> > > > >::iterator it;
	map <int, map <int, map <int, list <int> > > >::iterator it1;
	map <int, map <int, list <int> > >::iterator it2;
	map <int, list <int> >::iterator it3;
	list <int>::iterator it4;
		
	it = avg_num_hop_dup.begin();
	
	for (; it != avg_num_hop_dup.end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_dst = it1->first;
			
			if (node_dst != dst) {
			
				continue;
				
			}
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_src = it2->first;
				
				if (node_src != src) {
				
					continue;
					
				}
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					pkt_id = it3->first;
					it4 = (it3->second).begin();
					
					if (it4 != (it3->second).end()) {
					
						num_hop += *it4;
						count++;
						
					}
				}
				
				if (count > 0) {
					
					num_hop = num_hop / (double)count;
				}
				else {
					num_hop = 0.0;
				}
				
			}
		}
	}    
	
	return num_hop;
}

/*!
 * 	@brief The getMaxRouteLength() function returns the maximal route length traversed by packets delivered by the src node to the dst node. 
 *	@param src The source node generating data.
 *	@param dst The destination node receiving the data. 
 *	@retval result The maximal route length traversed by packets delivered by the src node to the dst node.
 */

int Sunset_Protocol_Statistics::getMaxRouteLength(int src, int dst)
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	int max_num_hop = -1;
	
	map < sunset_statisticPktType, map <int, map <int, map <int, list <int> > > > >::iterator it;
	map <int, map <int, map <int, list <int> > > >::iterator it1;
	map <int, map <int, list <int> > >::iterator it2;
	map <int, list <int> >::iterator it3;
	list <int>::iterator it4;
		
	it = avg_num_hop.begin();
	
	for (; it != avg_num_hop.end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_dst = it1->first;
			
			if (node_dst != dst) {
			
				continue;
				
			}
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_src = it2->first;
				
				if (node_src != src) {
				
					continue;
					
				}
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					pkt_id = it3->first;
					it4 = (it3->second).begin();
					
					if (it4 != (it3->second).end()) {
					
						if (max_num_hop < *it4 ) {
							
							max_num_hop = *it4;
							
						}
					}
				}
				
			}
		}
	}    
	
	return max_num_hop;
}

/*!
 * 	@brief The getMaxDuplicatedRouteLength() function returns the maximal duplicated route length traversed by packets delivered by the src node to the dst node. 
 *	@param src The source node generating data.
 *	@param dst The destination node receiving the data. 
 *	@retval result The maximal duplicated route length traversed by packets delivered by the src node to the dst node.
 */
int Sunset_Protocol_Statistics::getMaxDuplicatedRouteLength(int src, int dst)
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	int max_num_hop = -1;
	
	map < sunset_statisticPktType, map <int, map <int, map <int, list <int> > > > >::iterator it;
	map <int, map <int, map <int, list <int> > > >::iterator it1;
	map <int, map <int, list <int> > >::iterator it2;
	map <int, list <int> >::iterator it3;
	list <int>::iterator it4;
		
	it = avg_num_hop_dup.begin();
	
	for (; it != avg_num_hop_dup.end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_dst = it1->first;
			
			if (node_dst != dst) {
			
				continue;
				
			}
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_src = it2->first;
				
				if (node_src != src) {
				
					continue;
					
				}
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					pkt_id = it3->first;
					it4 = (it3->second).begin();
					
					if (it4 != (it3->second).end()) {
					
						if (max_num_hop < *it4 ) {
							
							max_num_hop = *it4;
							
						}
					}
				}
				
			}
		}
	}    
	
	return max_num_hop;
}

/*!
 * 	@brief The getNumRoutes() function returns the number of different routes traversed by packets delivered by the src node to the dst node. 
 *	@param src The source node generating data.
 *	@param dst The destination node receiving the data. 
 *	@retval result The number of different routes traversed by packets delivered by the src node to the dst node.
 */

int Sunset_Protocol_Statistics::getNumRoutes(int src, int dst)
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	set<int> num_path;
	
	map < sunset_statisticPktType, map <int, map <int, map <int, list <int> > > > >::iterator it;
	map <int, map <int, map <int, list <int> > > >::iterator it1;
	map <int, map <int, list <int> > >::iterator it2;
	map <int, list <int> >::iterator it3;
	list <int>::iterator it4;
		
	it = avg_num_hop.begin();
	
	for (; it != avg_num_hop.end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_dst = it1->first;
			
			if (node_dst != dst) {
			
				continue;
				
			}
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_src = it2->first;
				
				if (node_src != src) {
				
					continue;
					
				}
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					pkt_id = it3->first;
					it4 = (it3->second).begin();
					
					if (it4 != (it3->second).end()) {
					
						num_path.insert(*it4);

					}
				}
				
			}
		}
	}    
	
	if (num_path.empty()) {
		
		return 0;
	}
	
	return (int)(num_path.size());
}

/*!
 * 	@brief The getDuplicatedNumRoutes() function returns the number of different routes traversed by duplicated packets delivered by the src node to the dst node. 
 *	@param src The source node generating data.
 *	@param dst The destination node receiving the data. 
 *	@retval result The number of different routes traversed by duplicated packets delivered by the src node to the dst node.
 */
int Sunset_Protocol_Statistics::getDuplicatedNumRoutes (int src, int dst)
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	set<int> num_path;
	
	map < sunset_statisticPktType, map <int, map <int, map <int, list <int> > > > >::iterator it;
	map <int, map <int, map <int, list <int> > > >::iterator it1;
	map <int, map <int, list <int> > >::iterator it2;
	map <int, list <int> >::iterator it3;
	list <int>::iterator it4;
		
	it = avg_num_hop_dup.begin();
	
	for (; it != avg_num_hop_dup.end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_dst = it1->first;
			
			if (node_dst != dst) {
			
				continue;
				
			}
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_src = it2->first;
				
				if (node_src != src) {
				
					continue;
					
				}
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					pkt_id = it3->first;
					it4 = (it3->second).begin();
					
					if (it4 != (it3->second).end()) {
					
						num_path.insert(*it4);

					}
				}
				
			}
		}
	}    
	
	if (num_path.empty()) {
		
		return 0;
	}
	
	return (int)(num_path.size());
}

/*!
 * 	@brief The getPacketLatency() function returns the average end-to-end for packets delivered by the src node to the dst node. 
 *	@param src The source node generating data.
 *	@param dst The destination node receiving the data. 
 *	@retval result The average end-to-end for packets delivered by the src node to the dst node.
 */

double Sunset_Protocol_Statistics::getPacketLatency(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	int num_pkt = 0;
	double tot_delay = 0.0;
	
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it;
	map <int, map <int, map <int, list <double> > > >::iterator it1;
	map <int, map <int, list <double> > >::iterator it2;
	map <int, list <double> >::iterator it3;
	
	it = pkt_recv.begin();
	
	for (; it != pkt_recv.end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_dst = it1->first;
			
			if (node_dst != dst) {
			
				continue;
			
			}
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_src = it2->first;
				
				if (node_src != src) {
				
					continue;
				
				}
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					pkt_id = it3->first;
					
					if (pkt_sent.find(spktType) != pkt_sent.end() &&
					    (pkt_sent[spktType]).find(src) != (pkt_sent[spktType]).end() &&
					    (pkt_sent[spktType])[src].find(pkt_id) != (pkt_sent[spktType])[src].end()) {
					
						double rx_time = *((it3->second).begin());
						double tx_time = *((((pkt_sent[spktType])[src])[pkt_id]).begin());
						
						if (rx_time - tx_time < 0.0) {
						
							Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::getPacketLatency latency node %d src %d pkt_id %d tx_time %f rx_time %f possible ERROR", dst, src, pkt_id, tx_time, rx_time);
							
							continue;
						} 
						
						tot_delay += rx_time - tx_time;
						
						num_pkt++;
					}
				}
			}
		}
	}
	
	if (num_pkt > 0) {
		
		tot_delay = tot_delay/num_pkt;
	}
	else {
		
		tot_delay = 0.0;
	}
	
	return tot_delay;
	
}

/*!
 * 	@brief The getDuplicatedPacketLatency() function returns the average end-to-end for duplicated packets delivered by the src node to the dst node. 
 *	@param src The source node generating data.
 *	@param dst The destination node receiving the data. 
 *	@retval result The average end-to-end for duplicated packets delivered by the src node to the dst node.
 */

double Sunset_Protocol_Statistics::getDuplicatedPacketLatency(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	int num_pkt = 0;
	double tot_delay = 0.0;
	
	int count = 0;
	
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it;
	map <int, map <int, map <int, list <double> > > >::iterator it1;
	map <int, map <int, list <double> > >::iterator it2;
	map <int, list <double> >::iterator it3;
	list <double>::iterator it4;
	
	
	it = pkt_recv.begin();
	
	for (; it != pkt_recv.end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_dst = it1->first;
			
			if (node_dst != dst) {
			
				continue;
			
			}
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_src = it2->first;
				
				if (node_src != src) {
				
					continue;
				
				}
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					pkt_id = it3->first;

					count = (it3->second).size();
					
					if (count > 1) {
						
						if (pkt_sent.find(spktType) == pkt_sent.end() ||
							(pkt_sent[spktType]).find(src) == (pkt_sent[spktType]).end() ||
							(pkt_sent[spktType])[src].find(pkt_id) == (pkt_sent[spktType])[src].end()) {
						
							Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::getDuplicatedPacketLatency latency node %d src %d pkt_id %d tx_time possible ERROR", dst, src, pkt_id);

							continue;
						}

						double tx_time = *((((pkt_sent[spktType])[src])[pkt_id]).begin());
						
						it4 = (it3->second).begin();
						
						it4++; //skip first packet which is not a duplicated
				
						for (; it4 != (it3->second).end(); it4++) {
						
							double rx_time = *it4;
							
							if (rx_time - tx_time < 0.0) {
							
								Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::getDuplicatedPacketLatency latency node %d src %d pkt_id %d tx_time %f rx_time %f possible ERROR", dst, src, pkt_id, tx_time, rx_time);
								
								continue;
							} 
							
							tot_delay += rx_time - tx_time;
							
							num_pkt++;
						}
					}
				}
			}
		}
	}
	
	if (num_pkt > 0) {
		
		tot_delay = tot_delay/num_pkt;
	}
	else {
		
		tot_delay = 0.0;
	}
	
	return tot_delay;
	
}

/*!
 * 	@brief The getExperimentTime() function returns the experiment duration in seconds. 
 *	@retval result The the experiment duration in seconds.
 */

double Sunset_Protocol_Statistics::getExperimentTime() 
{
	if (stop_time == 0.0 && start_time == 0.0 && start_traffic == 0.0) {
		return -1;
	}
	
	if (stop_time == 0) {
	
		return Sunset_Utilities::get_now() - start_traffic;
	}
	
	return stop_time - start_traffic;
}

/*!
 * 	@brief The getCreatedMacDataPacket() function returns the number of data packets received at the MAC layer, generated at the upper layer to be transmitted on the link from the src node to the dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The number of data packets received at the MAC layer for transmission from the src node to the destination node.
 */
int Sunset_Protocol_Statistics::getCreatedMacDataPacket(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int mac_data_new = 0;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it;
	map <int, map <int, map <int, list <double> > > >::iterator it1;
	map <int, map <int, list <double> > >::iterator it2;
	map <int, list <double> >::iterator it3;
	
	it = (mac_new_pkt).begin();
	
	for (; it != (mac_new_pkt).end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}
		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_src = it1->first;
			
		        if (node_src != src) {
			
				continue;
			
			}         
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_dst = it2->first;

				if (node_dst != dst) {
				
					continue;
				
				}         
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					if (spktType == SUNSET_STAT_DATA) {
					
						mac_data_new += (it3->second).size();
					}
				}
			}
		}
	}
	
	return mac_data_new;
}

/*!
 * 	@brief The getMacDataPacketTransmissions() function returns the number of data packets transmitted at the MAC layer, from the src node to the dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The number of data packets transmitted by the src node to the destination node.
 */
int Sunset_Protocol_Statistics::getMacDataPacketTransmissions(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int mac_data_tx = 0;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it;
	map <int, map <int, map <int, list <double> > > >::iterator it1;
	map <int, map <int, list <double> > >::iterator it2;
	map <int, list <double> >::iterator it3;
	
	it = (tx_done).begin();
	
	for (; it != (tx_done).end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_src = it1->first;
			
		        if (node_src != src) {
			
				continue;
			
			}         
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_dst = it2->first;

				if (node_dst != dst) {
				
					continue;
				
				}         
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					if (spktType == SUNSET_STAT_DATA) {
					
						mac_data_tx += (it3->second).size();
					}
				}
			}
		}
	}
	
	return mac_data_tx;
}

/*!
 * 	@brief The getMacDataPacketReceptions() function returns the number of data packets received at the MAC layer, transmitted on the link from the src node to the dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The number of data packets received at the MAC layer on the link from the src node to the destination node.
 */
int Sunset_Protocol_Statistics::getMacDataPacketReceptions(int src, int dst) 
{
	int node = -1;
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int pkt_received = 0;
	
	map < sunset_statisticPktType, map <int, map <int, map <int, map <int, list < pair<double, double> > > > > > > ::iterator it;
	map <int, map <int, map <int, map <int, list < pair<double, double> > > > > >::iterator it1;
	map <int, map <int, map <int, list < pair<double, double> > > > >::iterator it2;
	map <int, map <int, list < pair<double, double> > > >::iterator it3;
	map <int, list < pair<double, double> > >::iterator it4;
	
	it = mac_pkt_recv.begin();
	
	for (; it != mac_pkt_recv.end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}
		
		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node = it1->first;
			
			if (node != dst && dst != Sunset_Address::getBroadcastAddress()) { //TODO check if broadcast has to be added
			
				continue;
				
			}
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_src = it2->first;

				if (node_src != src) {
				
					continue;
					
				}
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					node_dst = it3->first;

					if (node_dst != dst) { //TODO check if broadcast has to be added
					
						continue;
						
					}
					
					it4 = (it3->second).begin();
					
					for (; it4 != (it3->second).end(); it4++) {
						
						pkt_received += (it4->second).size(); 
						
					}
				}
			}
		}
	}

	return pkt_received;
}

/*!
 * 	@brief The getCreatedMacDataBytes() function returns the number of data bytes received at the MAC layer, generated at the upper layer to be transmitted on the link from the src node to the dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The number of data bytes received at the MAC layer for transmission from the src node to the destination node.
 */

int Sunset_Protocol_Statistics::getCreatedMacDataBytes(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int mac_data_new = 0;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it;
	map <int, map <int, map <int, list <double> > > >::iterator it1;
	map <int, map <int, list <double> > >::iterator it2;
	map <int, list <double> >::iterator it3;
	
	it = (mac_new_pkt).begin();
	
	for (; it != (mac_new_pkt).end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}
		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_src = it1->first;
			
		        if (node_src != src) {
			
				continue;
			
			}         
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_dst = it2->first;

				if (node_dst != dst) {
				
					continue;
				
				}         
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					if (spktType == SUNSET_STAT_DATA) {
					
						mac_data_new += (it3->first) * (it3->second).size();
					}
				}
			}
		}
	}
	
	return mac_data_new;
}

/*!
 * 	@brief The getMacDataBytesTransmissions() function returns the number of data bytes transmitted at the MAC layer, from the src node to the dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The number of data bytes transmitted by the src node to the destination node.
 */

int Sunset_Protocol_Statistics::getMacDataBytesTransmissions(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int mac_data_tx = 0;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it;
	map <int, map <int, map <int, list <double> > > >::iterator it1;
	map <int, map <int, list <double> > >::iterator it2;
	map <int, list <double> >::iterator it3;
	
	it = (tx_done).begin();
	
	for (; it != (tx_done).end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_src = it1->first;
			
		        if (node_src != src) {
			
				continue;
			
			}         
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_dst = it2->first;

				if (node_dst != dst) {
				
					continue;
				
				}         
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					if (spktType == SUNSET_STAT_DATA) {
					
						mac_data_tx += it3->first * (it3->second).size();
					}
				}
			}
		}
	}
	
	return mac_data_tx;
}

/*!
 * 	@brief The getMacDataBytesReceptions() function returns the number of data bytes received at the MAC layer, transmitted on the link from the src node to the dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The number of data bytes received at the MAC layer on the link from the src node to the destination node.
 */

int Sunset_Protocol_Statistics::getMacDataBytesReceptions(int src, int dst) 
{
	int node = -1;
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int pkt_received = 0;
	
	map < sunset_statisticPktType, map <int, map <int, map <int, map <int, list < pair<double, double> > > > > > > ::iterator it;
	map <int, map <int, map <int, map <int, list < pair<double, double> > > > > >::iterator it1;
	map <int, map <int, map <int, list < pair<double, double> > > > >::iterator it2;
	map <int, map <int, list < pair<double, double> > > >::iterator it3;
	map <int, list < pair<double, double> > >::iterator it4;
	
	it = mac_pkt_recv.begin();
	
	for (; it != mac_pkt_recv.end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}
		
		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node = it1->first;
			
			if (node != dst  && dst != Sunset_Address::getBroadcastAddress()) {
			
				continue;
				
			}
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_src = it2->first;

				if (node_src != src) {
				
					continue;
					
				}
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					node_dst = it3->first;

					if (node_dst != dst) {
					
						continue;
						
					}
					
					it4 = (it3->second).begin();
					
					for (; it4 != (it3->second).end(); it4++) {
					
						pkt_received += it4->first * (it4->second).size(); 
						
					}
				}
			}
		}
	}
	
	return pkt_received;
}

/*!
 * 	@brief The getMacDataPacketDiscarded() function returns the number of data packets discarded at the MAC layer, by the src node addressed to dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The number of data packets discarded at the MAC layer by the src node addressed to dst node..
 */

int Sunset_Protocol_Statistics::getMacDataPacketDiscarded(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int mac_data_discarded = 0;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it;
	map <int, map <int, map <int, list <double> > > >::iterator it1;
	map <int, map <int, list <double> > >::iterator it2;
	map <int, list <double> >::iterator it3;
	
	it = (mac_pkt_discard).begin();
	
	for (; it != (mac_pkt_discard).end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_src = it1->first;
			
		        if (node_src != src) {
			
				continue;
			
			}         
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_dst = it2->first;

				if (node_dst != dst) {
				
					continue;
				
				}         
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					if (spktType == SUNSET_STAT_DATA) {
					
						mac_data_discarded += (it3->second).size();
					}
				}
			}
		}
	}
	
	return mac_data_discarded;
}

/*!
 * 	@brief The getMacDataBytesDiscarded() function returns the number of data bytes discarded at the MAC layer, by the src node addressed to dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The number of data pabytescket discarded at the MAC layer by the src node addressed to dst node..
 */

int Sunset_Protocol_Statistics::getMacDataBytesDiscarded(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int mac_data_discarded = 0;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it;
	map <int, map <int, map <int, list <double> > > >::iterator it1;
	map <int, map <int, list <double> > >::iterator it2;
	map <int, list <double> >::iterator it3;
	
	it = (mac_pkt_discard).begin();
	
	for (; it != (mac_pkt_discard).end(); it++) {
	
		spktType = it->first;

		if (spktType != SUNSET_STAT_DATA) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_src = it1->first;
			
		        if (node_src != src) {
			
				continue;
			
			}         
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_dst = it2->first;

				if (node_dst != dst) {
				
					continue;
				
				}         
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					if (spktType == SUNSET_STAT_DATA) {
					
						mac_data_discarded += it3->first * (it3->second).size();
					}
				}
			}
		}
	}
	
	return mac_data_discarded;
}



/*!
 * 	@brief The getMacCtrlPacketTransmissions() function returns the number of control packets transmitted at the MAC layer, by the src node to the dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The number of control packets transmitted at the MAC layer by the src node to the destination node.
 */

int Sunset_Protocol_Statistics::getMacCtrlPacketTransmissions(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int mac_ctrl_tx = 0;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it;
	map <int, map <int, map <int, list <double> > > >::iterator it1;
	map <int, map <int, list <double> > >::iterator it2;
	map <int, list <double> >::iterator it3;
	
	it = (tx_done).begin();
	
	for (; it != (tx_done).end(); it++) {
	
		spktType = it->first;

		if (spktType == SUNSET_STAT_DATA || spktType == SUNSET_STAT_NONE) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_src = it1->first;
			
		        if (node_src != src) {
			
				continue;
			
			}         
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_dst = it2->first;

				if (node_dst != dst) { //TODO check if broadcast has to be added
				
					continue;
				
				}         
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
// 					if (spktType == SUNSET_STAT_DATA) {
					
						mac_ctrl_tx += (it3->second).size();
// 					}
				}
			}
		}
	}
	
	return mac_ctrl_tx;
}

/*!
 * 	@brief The getMacCtrlPacketReceptions() function returns the number of control packets received at the MAC layer, transmitted on the link from the src node to the dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The number of control packets received at the MAC layer on the link from the src node to the destination node.
 */

int Sunset_Protocol_Statistics::getMacCtrlPacketReceptions(int src, int dst) 
{
	int node = -1;
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int pkt_received = 0;
	
	map < sunset_statisticPktType, map <int, map <int, map <int, map <int, list < pair<double, double> > > > > > > ::iterator it;
	map <int, map <int, map <int, map <int, list < pair<double, double> > > > > >::iterator it1;
	map <int, map <int, map <int, list < pair<double, double> > > > >::iterator it2;
	map <int, map <int, list < pair<double, double> > > >::iterator it3;
	map <int, list < pair<double, double> > >::iterator it4;
	
	it = mac_pkt_recv.begin();
	
	for (; it != mac_pkt_recv.end(); it++) {
	
		spktType = it->first;

		if (spktType == SUNSET_STAT_DATA || spktType == SUNSET_STAT_NONE) {
		
			continue;
			
		}
		
		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node = it1->first;
			
			if (node != dst && dst != Sunset_Address::getBroadcastAddress()) { //TODO check if broadcast has to be added
			
				continue;
				
			}
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_src = it2->first;

				if (node_src != src) {
				
					continue;
					
				}
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					node_dst = it3->first;

					if (node_dst != dst) {
					
						continue;
						
					}
					
					it4 = (it3->second).begin();
					
					for (; it4 != (it3->second).end(); it4++) {
					
						pkt_received += (it4->second).size(); 
						
					}
				}
			}
		}
	}
	
	return pkt_received;
}



/*!
 * 	@brief The getMacCtrlBytesTransmissions() function returns the number of control bytes transmitted at the MAC layer, by the src node to the dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The number of control bytes transmitted at the MAC layer by the src node to the destination node.
 */

int Sunset_Protocol_Statistics::getMacCtrlBytesTransmissions(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int mac_ctrl_tx = 0;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it;
	map <int, map <int, map <int, list <double> > > >::iterator it1;
	map <int, map <int, list <double> > >::iterator it2;
	map <int, list <double> >::iterator it3;
	
	it = (tx_done).begin();
	
	for (; it != (tx_done).end(); it++) {
	
		spktType = it->first;

		if (spktType == SUNSET_STAT_DATA || spktType == SUNSET_STAT_NONE) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_src = it1->first;
			
		        if (node_src != src) {
			
				continue;
			
			}         
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_dst = it2->first;

				if (node_dst != dst) {
				
					continue;
				
				}         
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					if (spktType == SUNSET_STAT_DATA) {
					
						mac_ctrl_tx += it3->first * (it3->second).size();
					}
				}
			}
		}
	}
	
	return mac_ctrl_tx;
}

/*!
 * 	@brief The getMacCtrlBytesReceptions() function returns the number of control byes received at the MAC layer, transmitted on the link from the src node to the dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The number of control bytes received at the MAC layer on the link from the src node to the destination node.
 */

int Sunset_Protocol_Statistics::getMacCtrlBytesReceptions(int src, int dst) 
{
	int node = -1;
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int pkt_received = 0;
	
	map < sunset_statisticPktType, map <int, map <int, map <int, map <int, list < pair<double, double> > > > > > > ::iterator it;
	map <int, map <int, map <int, map <int, list < pair<double, double> > > > > >::iterator it1;
	map <int, map <int, map <int, list < pair<double, double> > > > >::iterator it2;
	map <int, map <int, list < pair<double, double> > > >::iterator it3;
	map <int, list < pair<double, double> > >::iterator it4;
	
	it = mac_pkt_recv.begin();
	
	for (; it != mac_pkt_recv.end(); it++) {
	
		spktType = it->first;

		if (spktType == SUNSET_STAT_DATA || spktType == SUNSET_STAT_NONE) {
		
			continue;
			
		}
		
		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node = it1->first;
			
			if (node != dst && dst != Sunset_Address::getBroadcastAddress()) {
			
				continue;
				
			}
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_src = it2->first;

				if (node_src != src) {
				
					continue;
					
				}
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					node_dst = it3->first;

					if (node_dst != dst) {
					
						continue;
						
					}
					
					it4 = (it3->second).begin();
					
					for (; it4 != (it3->second).end(); it4++) {
					
						pkt_received += it4->first * (it4->second).size(); 
						
					}
				}
			}
		}
	}
	
	return pkt_received;
}

/*!
 * 	@brief The getMacCtrlPacketDiscarded() function returns the number of control packets discarded at the MAC layer, by the src node addressed to dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The number of control packets discarded at the MAC layer by the src node addressed to dst node..
 */

int Sunset_Protocol_Statistics::getMacCtrlPacketDiscarded(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int mac_ctrl_discarded = 0;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it;
	map <int, map <int, map <int, list <double> > > >::iterator it1;
	map <int, map <int, list <double> > >::iterator it2;
	map <int, list <double> >::iterator it3;
	
	it = (mac_pkt_discard).begin();
	
	for (; it != (mac_pkt_discard).end(); it++) {
	
		spktType = it->first;

		if (spktType == SUNSET_STAT_DATA || spktType == SUNSET_STAT_NONE) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_src = it1->first;
			
		        if (node_src != src) {
			
				continue;
			
			}         
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_dst = it2->first;

				if (node_dst != dst) {
				
					continue;
				
				}         
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					if (spktType == SUNSET_STAT_DATA) {
					
						mac_ctrl_discarded += (it3->second).size();
					}
				}
			}
		}
	}
	
	return mac_ctrl_discarded;
}


/*!
 * 	@brief The getMacCtrlPacketDiscarded() function returns the number of control bytes discarded at the MAC layer, by the src node addressed to dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The number of control buyes discarded at the MAC layer by the src node addressed to dst node..
 */

int Sunset_Protocol_Statistics::getMacCtrlBytesDiscarded(int src, int dst) 
{
	int node_src = -1;
	int node_dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int mac_ctrl_discarded = 0;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it;
	map <int, map <int, map <int, list <double> > > >::iterator it1;
	map <int, map <int, list <double> > >::iterator it2;
	map <int, list <double> >::iterator it3;
	
	it = (mac_pkt_discard).begin();
	
	for (; it != (mac_pkt_discard).end(); it++) {
	
		spktType = it->first;

		if (spktType == SUNSET_STAT_DATA || spktType == SUNSET_STAT_NONE) {
		
			continue;
			
		}

		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node_src = it1->first;
			
		        if (node_src != src) {
			
				continue;
			
			}         
			
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				node_dst = it2->first;

				if (node_dst != dst) {
				
					continue;
				
				}         
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					if (spktType == SUNSET_STAT_DATA) {
					
						mac_ctrl_discarded += it3->first * (it3->second).size();
					}
				}
			}
		}
	}
	
	return mac_ctrl_discarded;
}

/*!
 * 	@brief The getPDR() function returns the packet delivery ratio between src node and dst node. 
 *	@param src The source node of the packet.
 *	@param dst The destination of the packet. 
 *	@retval result The packet delivery ratio between src node and dst node.
 */

double Sunset_Protocol_Statistics::getPDR(int src, int dst)
{
	int pkt_tx = getGeneratedPacket(src, dst);
	
	int pkt_rx = getDeliveredPacket(src, dst);
	
	if (pkt_tx > 0) {
		
		return pkt_rx / pkt_tx;
	
	}
	
	return 0.0;
}

/*!
 * 	@brief The getPDR() function returns the packet delivery ratio in the network. 
 *	@retval result The packet delivery ratio in the network.
 */

double Sunset_Protocol_Statistics::getPDR()
{
	int pkt_tx = getGeneratedPacket();

	int pkt_rx = getDeliveredPacket();

	if (pkt_tx > 0) {
		
		return (double)pkt_rx / (double)pkt_tx;
	}
	
	return 0.0;
}

/*!
 * 	@brief The getGeneratedPacket() function returns the number of data packets generated at the application layer in the network.
 *	@retval result The number of data packets generated at the application layer in the network.
 */

double Sunset_Protocol_Statistics::getGeneratedPacket()
{
	int pkt_tx = 0;

	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_tx += getGeneratedPacket(src, dst);
				
			}
		}

		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt_tx += getGeneratedPacket(src, Sunset_Address::getBroadcastAddress());			
			
		}
	}
	
		
	return pkt_tx;
}

/*!
 * 	@brief The getMacDataPacketTransmissions() function returns the number of data packets transmitted at the MAC layer.
 *	@retval result The number of data packets generated at the MAC layer.
 */

double Sunset_Protocol_Statistics::getMacDataPacketTransmissions()
{
	int pkt_tx = 0;

	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_tx += getMacDataPacketTransmissions(src, dst);
				
			}
		}

		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt_tx += getMacDataPacketTransmissions(src, Sunset_Address::getBroadcastAddress());			
			
		}
	}
	
		
	return pkt_tx;
}

/*!
 * 	@brief The getMacCtrlPacketTransmissions() function returns the number of ctrl packets transmitted at the MAC layer.
 *	@retval result The number of ctrl packets generated at the MAC layer.
 */

double Sunset_Protocol_Statistics::getMacCtrlPacketTransmissions()
{
	int pkt_tx = 0;

	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_tx += getMacCtrlPacketTransmissions(src, dst);
				
			}
		}

		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt_tx += getMacCtrlPacketTransmissions(src, Sunset_Address::getBroadcastAddress());			
			
		}
	}
	
		
	return pkt_tx;
}

/*!
 * 	@brief The getMacDataPacketReceptions() function returns the number of data packets received at the MAC layer.
 *	@retval result The number of data packets received at the MAC layer.
 */

double Sunset_Protocol_Statistics::getMacDataPacketReceptions()
{
	int pkt_rx = 0;

	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_rx += getMacDataPacketReceptions(src, dst);
				
			}
		}

		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt_rx += getMacDataPacketReceptions(src, Sunset_Address::getBroadcastAddress());			
			
		}
	}
	
		
	return pkt_rx;
}

/*!
 * 	@brief The getMacCtrlPacketReceptions() function returns the number of ctrl packets received at the MAC layer.
 *	@retval result The number of ctrl packets received at the MAC layer.
 */

double Sunset_Protocol_Statistics::getMacCtrlPacketReceptions()
{
	int pkt_rx = 0;

	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_rx += getMacCtrlPacketReceptions(src, dst);
				
			}
		}

		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt_rx += getMacCtrlPacketReceptions(src, Sunset_Address::getBroadcastAddress());			
			
		}
	}
	
		
	return pkt_rx;
}

/*!
 * 	@brief The getDeliveredPacket() function returns the number of data packets correctly delivered at the application layer with no repetitions.
 *	@retval result The number of data packets correctly delivered at the application layer with no repetitions.
 */

double Sunset_Protocol_Statistics::getDeliveredPacket()
{
	int pkt_rx = 0;

	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_rx += getDeliveredPacket(src, dst);
				
			}
		}
	}
		
	return pkt_rx;
}

/*!
 * 	@brief The getGeneratedPacket() function returns the number of data packets generated at the application layer by the src node.
 *	@param src The source node of the packet.
 *	@retval result The number of data packets generated at the application layer by the src node.
 */

double Sunset_Protocol_Statistics::getGeneratedPacket(int src)
{
	int pkt_tx = 0;

	for (int dst = 0; dst <= max_node_id; dst++) {
	
		if (src != dst) {
			
			pkt_tx += getGeneratedPacket(src, dst);
			
		}
	}

	if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
		
			pkt_tx += getGeneratedPacket(src, Sunset_Address::getBroadcastAddress());			
		
	}

	return pkt_tx;
}

/*!
 * 	@brief The getDeliveredPacket() function returns the number of data packets correctly delivered at the application layer with no repetitions generated by the src node.
 *	@param src The source node of the packet.
 *	@retval result The number of data packets correctly delivered at the application layer with no repetitions generated by the src node.
 */

double Sunset_Protocol_Statistics::getDeliveredPacket(int src)
{
	int pkt_rx = 0;

	for (int dst = 0; dst <= max_node_id; dst++) {
	
		if (src != dst) {
			
			pkt_rx += getDeliveredPacket(src, dst);
			
		}
	}
		
	return pkt_rx;
}

/*!
 * 	@brief The getApplicationThroughput() function returns the throughput at the application layer between the src node to the dst node. 
 *	@param src The source node of the packet.
 *	@param dst The destination of the packet. 
 *	@retval result The throughput at the application layer between the src node to the dst node.
 */

double Sunset_Protocol_Statistics::getApplicationThroughput(int src, int dst)
{

	double time = getExperimentTime();
	
	if (time <= 0) {
		
		return 0.0;
	}
	
	int pkt_rx = getDeliveredBytes(src, dst) * 8.0;
	
	return pkt_rx / time;

}

/*!
 * 	@brief The getApplicationThroughput() function returns the throughput at the application layer in the network. 
 *	@retval result The  throughput at the application layer in the network.
 */

double Sunset_Protocol_Statistics::getApplicationThroughput()
{
	double time = getExperimentTime();
	
	if (time <= 0) {
		
		return 0.0;
	}
	
	int pkt_rx = 0;
	
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_rx += getDeliveredBytes(src, dst) * 8.0;
			
			}
		}
	}
	
	return pkt_rx / time;
}

/*!
 * 	@brief The getPacketLatency() function returns the average end-to-end packet latency in the network.
 *	@retval result The average end-to-end packet latency in the network.
 */

double Sunset_Protocol_Statistics::getPacketLatency()
{
	double aux_val = 0.0;
	double pkt_latency = 0;
	int count = 0;
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				aux_val = getPacketLatency(src, dst);
				
				if (aux_val > 0.0) {
					
					pkt_latency += aux_val;
					
					count++;
				
				}
			
			}
		}
	}
	
	if (count > 0) {
		
		return pkt_latency / count;
	}
	
	return 0.0;
}

/*!
 * 	@brief The getDuplicatedPacketLatency() function returns the average end-to-end packet latency in the network for the reception of duplicated packets.
 *	@retval result The average end-to-end packet latency in the network for the reception of duplicated packets.
 */

double Sunset_Protocol_Statistics::getDuplicatedPacketLatency()
{
	double aux_val = 0.0;
	double pkt_latency = 0;
	int count = 0;
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				aux_val = getDuplicatedPacketLatency(src, dst);
				
				if (aux_val > 0.0) {
					
					pkt_latency += aux_val;
					
					count++;
				
				}
			
			}
		}
	}
	
	if (count > 0) {
		
		return pkt_latency / count;
	}
	
	return 0.0;
}

/*!
 * 	@brief The getRouteLength() function returns the average route length for packets delivered in the network, with no repetitions.
 *	@retval result The average route length for packets delivered in the network.
 */

double Sunset_Protocol_Statistics::getRouteLength()
{
	double aux_val = 0.0;
	double route_len = 0;
	int count = 0;
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				aux_val = getRouteLength(src, dst);
				
				if (aux_val > 0.0) {
					
					route_len += aux_val;
					
					count++;
				
				}
			
			}
		}
	}
	
	if (count > 0) {
		
		return route_len / count;
	}
	
	return 0.0;
}

/*!
 * 	@brief The getNumRoutes() function returns the number of different routes used in the network when delivering data packets to the destination node. 
 *	@retval result The number of differente routes used in the network.
 */

double Sunset_Protocol_Statistics::getNumRoutes()
{
	double aux_val = 0.0;
	double routes = 0;
	int count = 0;
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				aux_val = getNumRoutes(src, dst);
				
				if (aux_val > 0.0) {
					
					routes += aux_val;
					
					count++;
				
				}
			
			}
		}
	}
	
	if (count > 0) {
		
		return routes / count;
	}
	
	return 0.0;
}

/*!
 * 	@brief The getMaxRouteLength() function returns the maximal route length in the network.
 *	@retval result The  maximal route length in the network.
 */

double Sunset_Protocol_Statistics::getMaxRouteLength()
{
	double aux_val = 0.0;
	
	double max_route_len = 0;
	
	int count = 0;

	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				aux_val = (double) getMaxRouteLength(src, dst);
				
				if (aux_val > 0.0) {
					
					max_route_len += aux_val;
					
					count++;
					
				}
			
			}
		}
	}
	
	if (count > 0) {
	
		return max_route_len / count;
		
	}
	
	return 0;
}

/*!
 * 	@brief The getDuplicatedRouteLength() function returns the average route length for duplicated packets delivered in the network.
 *	@retval result The average route length for duplicated packets delivered in the network.
 */

double Sunset_Protocol_Statistics::getDuplicatedRouteLength()
{
	double aux_val = 0.0;
	double route_len = 0;
	int count = 0;
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				aux_val = getDuplicatedRouteLength(src, dst);
				
				if (aux_val > 0.0) {
					
					route_len += aux_val;
					
					count++;
				
				}
			
			}
		}
	}
	
	if (count > 0) {
		
		return route_len / count;
	}
	
	return 0.0;
}

/*!
 * 	@brief The getDuplicatedNumRoutes() function returns the number of different routes used in the network when delivering duplicated data packets to the destination node. 
 *	@retval result The number of different routes used in the network.
 */

double Sunset_Protocol_Statistics::getDuplicatedNumRoutes()
{
	double aux_val = 0.0;
	double routes = 0;
	int count = 0;
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				aux_val = getDuplicatedNumRoutes(src, dst);
				
				if (aux_val > 0.0) {
					
					routes += aux_val;
					
					count++;
				
				}
			
			}
		}
	}
	
	if (count > 0) {
		
		return routes / count;
	}
	
	return 0.0;
}

/*!
 * 	@brief The getMaxDuplicatedRouteLength() function returns the maximal routes length in the network when delivering duplicated data packets to the destination node. 
 *	@retval result The maximal route length for duplicated packets.
 */

double Sunset_Protocol_Statistics::getMaxDuplicatedRouteLength()
{
	double aux_val = 0.0;
	
	double max_route_len = 0;
	
	int count = 0;

	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				aux_val = getDuplicatedRouteLength(src, dst);
				
				if (aux_val > 0.0) {
					
					max_route_len += aux_val;
					
					count++;
					
				}
			
			}
		}
	}
	
	if (count > 0) {
	
		return max_route_len / count;
	
	}
	
	return 0.0;
}

/*!
 * 	@brief The getMacThroughput() function returns the throughput at the MAC layer on the link src to dst. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The throughput at the MAC layer.
 */

double Sunset_Protocol_Statistics::getMacThroughput(int src, int dst)
{

	double time = getExperimentTime();
	
	if (time <= 0) {
		
		return 0.0;
	}
	
	int pkt_rx = (getMacDataBytesReceptions(src, dst) + getMacCtrlBytesReceptions(src, dst)) * 8.0;
	
	return pkt_rx / time;

}

/*!
 * 	@brief The getMacThroughput() function returns the throughput at the MAC layer in the network. 
 *	@retval result The throughput at the MAC layer.
 */

double Sunset_Protocol_Statistics::getMacThroughput()
{
	double time = getExperimentTime();
	
	if (time <= 0) {
		
		return 0.0;
	}
	
	int pkt_rx = 0;
	
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_rx += (getMacDataBytesReceptions(src, dst) + getMacCtrlBytesReceptions(src, dst)) * 8.0;

			}
		}
	}

	return pkt_rx / time;
}

/*!
 * 	@brief The getMacLoad() function returns the number of MAC packets transmitted in the network.
 *	@retval result The number of  MAC packets transmitted in the network.
 */

double Sunset_Protocol_Statistics::getMacLoad()
{
	int pkt_tx = 0;
	
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_tx += (getMacDataPacketTransmissions(src, dst) + getMacCtrlPacketTransmissions(src, dst));
			
			}
		}

		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt_tx += (getMacDataPacketTransmissions(src, Sunset_Address::getBroadcastAddress()) + 
					getMacCtrlPacketTransmissions(src, Sunset_Address::getBroadcastAddress()));
			
		}
	}
	
	return pkt_tx;
}

/*!
 * 	@brief The getMacLoadBytes() function returns the number of MAC bytes transmitted in the network.
 *	@retval result The number of  MAC bytes transmitted in the network.
 */

double Sunset_Protocol_Statistics::getMacLoadBytes()
{
	int pkt_tx = 0;
	
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_tx += (getMacDataBytesTransmissions(src, dst) + getMacCtrlBytesTransmissions(src, dst));
			
			}
		}
		
		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt_tx += (getMacDataBytesTransmissions(src, Sunset_Address::getBroadcastAddress()) + 
					getMacCtrlBytesTransmissions(src, Sunset_Address::getBroadcastAddress()));
			
		}

	}
	
	return pkt_tx;
}

/*!
 * 	@brief The getMacDataLoad() function returns the number of MAC data packets transmitted in the network.
 *	@retval result The number of  MAC data packets transmitted in the network.
 */

double Sunset_Protocol_Statistics::getMacDataLoad()
{
	int pkt_tx = 0;
	
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_tx += (getMacDataPacketTransmissions(src, dst));
			
			}
		}

		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt_tx += (getMacDataPacketTransmissions(src, Sunset_Address::getBroadcastAddress()));
			
		}
	
	}
	
	return pkt_tx;
}

/*!
 * 	@brief The getMacDataLoadBytes() function returns the number of MAC data bytes transmitted in the network.
 *	@retval result The number of  MAC data bytes transmitted in the network.
 */

double Sunset_Protocol_Statistics::getMacDataLoadBytes()
{
	int pkt_tx = 0;
	
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_tx += (getMacDataBytesTransmissions(src, dst));
			
			}
		}

		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt_tx += (getMacDataBytesTransmissions(src, Sunset_Address::getBroadcastAddress()));
			
		}
	
	}
	
	return pkt_tx;
}

/*!
 * 	@brief The getMacCtrlLoad() function returns the number of MAC control packets transmitted in the network.
 *	@retval result The number of  MAC control packets transmitted in the network.
 */

double Sunset_Protocol_Statistics::getMacCtrlLoad()
{
	int pkt_tx = 0;
	
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_tx += (getMacCtrlPacketTransmissions(src, dst));
			
			}
		}

		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt_tx += (getMacCtrlPacketTransmissions(src, Sunset_Address::getBroadcastAddress()));
			
		}
	
	}
	
	return pkt_tx;
}

/*!
 * 	@brief The getMacCtrlLoadBytes() function returns the number of MAC control bytes transmitted in the network.
 *	@retval result The number of  MAC control bytes transmitted in the network.
 */

double Sunset_Protocol_Statistics::getMacCtrlLoadBytes()
{
	int pkt_tx = 0;
	
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_tx += (getMacCtrlBytesTransmissions(src, dst));
			
			}
		}

		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt_tx += (getMacCtrlBytesTransmissions(src, Sunset_Address::getBroadcastAddress()));
			
		}	
	
	}
	
	return pkt_tx;
}

/*!
 * 	@brief The getMacDataRetransmissions() function returns the average number of data packets retransmissions on the link from the src node to the dst node. 
 *	@param src The source node of the link.
 *	@param dst The destination of the link. 
 *	@retval result The average number of data packets retransmissions over the link.
 */

double Sunset_Protocol_Statistics::getMacDataRetransmissions(int src, int dst)
{
	int pkt = getCreatedMacDataPacket(src, dst);
	
	int pkt_tx = getMacDataPacketTransmissions(src, dst);
	
	if (pkt > 0 && pkt < pkt_tx) {
		
		return (pkt_tx - pkt) / pkt;
	
	}
	
	return 0.0;
}

/*!
 * 	@brief The getMacDataRetransmissions() function returns the average number of data packets retransmissions in the network. 
 *	@retval result The average number of data packets retransmissions in the network.
 */

double Sunset_Protocol_Statistics::getMacDataRetransmissions()
{
	int pkt_tx = 0;
	int pkt = 0;
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt_tx += getMacDataPacketTransmissions(src, dst);
				
				pkt += getCreatedMacDataPacket(src, dst);
			
			}
		}

		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt_tx += (getMacDataPacketTransmissions(src, Sunset_Address::getBroadcastAddress()));
			
				pkt += getCreatedMacDataPacket(src, Sunset_Address::getBroadcastAddress());
			
		}
	
	}
	
	if (pkt > 0 && pkt < pkt_tx) {
		
		return (pkt_tx - pkt) / pkt;
	
	}
	
	return 0.0;
}

/*!
 * 	@brief The getMacDataPacketDiscarded() function returns the number of data packets discarded at the MAC layer. 
 *	@retval result The number of data packets discarded at the MAC layer.
 */

int Sunset_Protocol_Statistics::getMacDataPacketDiscarded()
{
	double pkt = 0;
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt += getMacDataPacketDiscarded(src, dst);
			
			}
		}

		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt += getMacDataPacketDiscarded(src, Sunset_Address::getBroadcastAddress());
			
		}
	
	}
		
	return pkt;
}

/*!
 * 	@brief The getMacCtrlPacketDiscarded() function returns the number of control packets discarded at the MAC layer. 
 *	@retval result The number of control packets discarded at the MAC layer.
 */

int Sunset_Protocol_Statistics::getMacCtrlPacketDiscarded()
{
	int pkt = 0;
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
				
				pkt += getMacCtrlPacketDiscarded(src, dst);
			
			}
		}

		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt += getMacCtrlPacketDiscarded(src, Sunset_Address::getBroadcastAddress());
			
		}
	
	}
		
	return pkt;
}


/*!
 * 	@brief The getOverheadPerBit() function returns the amount of overhead information transmitted to deliver one bit of information from the  src node to the dst node. 
 *	@param src The source node of the information.
 *	@param dst The destination of the information. 
 *	@retval result The amount of overhead information transmitted to deliver one bit of information from the  src node to the dst node.
 */

double Sunset_Protocol_Statistics::getOverheadPerBit(int src, int dst)
{
	
	int pkt_tx = (getMacDataBytesTransmissions(src, dst) + getMacCtrlBytesTransmissions(src, dst)) * 8.0;

	int pkt_rx = getDeliveredBytes(src, dst) * 8.0;

	if (pkt_rx > 0 && pkt_rx < pkt_tx) {
		
		return (pkt_tx - pkt_rx) / pkt_rx;
	
	}
	
	return 0.0;

}

/*!
 * 	@brief The getOverheadPerBit() function returns the amount of overhead information transmitted to deliver one bit of information in the network. 
 *	@retval result The amount of overhead information transmitted to deliver one bit of information in the network.
 */

double Sunset_Protocol_Statistics::getOverheadPerBit()
{

	int pkt_tx = 0;
	
	int pkt_rx = 0;
	
	for (int src = 0; src <= max_node_id; src++) {

		for (int dst = 0; dst <= max_node_id; dst++) {
		
			if (src != dst) {
			
				pkt_tx += (getMacDataBytesTransmissions(src, dst) + getMacCtrlBytesTransmissions(src, dst)) * 8;

				pkt_rx += getDeliveredBytes(src, dst) * 8;
			}
		}

		if (Sunset_Address::getBroadcastAddress() > max_node_id || Sunset_Address::getBroadcastAddress() < 0) {
			
				pkt_tx += (getMacDataBytesTransmissions(src, Sunset_Address::getBroadcastAddress()) +
					getMacCtrlBytesTransmissions(src, Sunset_Address::getBroadcastAddress())) * 8;
			
		}
	
	}

	if (pkt_rx > 0 && pkt_rx < pkt_tx) {
		
		return (double)(pkt_tx - pkt_rx) / (double)pkt_rx;
	
	}
	
	return 0.0;

}

/*!
 * 	@brief The show_data() function is used to collect the processed informaiton and store in the file selected
 *	by the user.
 */

void Sunset_Protocol_Statistics::show_data() 
{
		
	char command[STAT_MAX_BUF];
	memset(command, 0x0, STAT_MAX_BUF);
	
	outFile2.open(fileOut2, ios::out | ios::app );
	outFile2.setf( ios::left, ios::adjustfield );    
	
	if (!outFile2.is_open()) {
		
		Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::show_data outputFile2 %s cannot be opened ERROR", fileOut2);
		
		return;
	}

// 	printf("STAMPO %d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n", run_id, duration_traffic_, duration_, pkt_sent_, pkt_received_, pdr_, th_, latency_, num_hop_, max_num_hop_, num_path_, num_hop_dup_, max_num_hop_dup_, num_path_dup_, mac_load_, mac_data_load_, mac_ctrl_load_, overhead_, data_tx_per_pkt_);
	
 	sprintf(command, "%d %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n", 
		run_id, stop_time - start_traffic, stop_time - start_time, getGeneratedPacket(), getDeliveredPacket(), 
		getPDR(), getApplicationThroughput(), getPacketLatency(), getRouteLength(), getMaxRouteLength(), 
		getNumRoutes(), getDuplicatedRouteLength(), getMaxDuplicatedRouteLength(), getDuplicatedNumRoutes(),
		getMacLoad(), getMacDataLoad(), getMacCtrlLoad(), getOverheadPerBit(), getMacThroughput(),
		getMacDataRetransmissions());
	
	outFile2.write(command, (int)(strlen(command)));
	outFile2.close();
}


void Sunset_Protocol_Statistics::setTxDuration(int id, double watt, double pow, double sec) 
{
	map<int, map <double, double> >::iterator it;
	
	it = txTime.find(id);
	
	if ( it == txTime.end() ) {
		
		txTime[id][pow] = 0.0;
	}
	else if ( (*it).second.find(pow) == (*it).second.end() ) {
		
		txTime[id][pow] = 0.0;
	}
	
	if ( txPower.find(pow) == txPower.end() ) {
		
		txPower[pow] = watt;
	}
	
	txTime[id][pow] += sec; 
		
	if ( residualEnergy.find(id) == residualEnergy.end() ) {
		
		residualEnergy[id] = totalEnergy;
	}
	
	residualEnergy[id] = residualEnergy[id] - (watt * sec);
	
	return;
}

void Sunset_Protocol_Statistics::setRxDuration(int id, double pow, double sec) 
{ 
	map<int, map <double, double> >::iterator it;
	
	it = rxTime.find(id);
	
	if ( it == rxTime.end() ) {
		
		rxTime[id][pow] = 0.0;
	}
	else if ( (*it).second.find(pow) == (*it).second.end() ) {
		
		rxTime[id][pow] = 0.0;
	}
	
	rxTime[id][pow] += sec; 
	
	if ( residualEnergy.find(id) == residualEnergy.end() ) {
		
		residualEnergy[id] = totalEnergy;
	}
	
	residualEnergy[id] = residualEnergy[id] - (pow * sec);
	
	return; 
}

void Sunset_Protocol_Statistics::setIdleDuration(int id, double pow, double sec) 
{ 
	map<int, map <double, double> >::iterator it;
	
	it = idleTime.find(id);
	
	if ( it == idleTime.end() ) {
		
		idleTime[id][pow] = 0.0;
	}
	else if ( (*it).second.find(pow) == (*it).second.end() ) {
		
		idleTime[id][pow] = 0.0;
	}
	
	idleTime[id][pow] += sec; 
		
	if ( residualEnergy.find(id) == residualEnergy.end() ) {
		
		residualEnergy[id] = totalEnergy;
	}
	
	residualEnergy[id] = residualEnergy[id] - (pow * sec);
	
	return; 
}

	/* Functions used to collect a subste of statistics information from other modules which can be 
	 * transmitted acoustically. */

/*!
 * 	@brief The get_agt_tx computes the information related to the packet transmitted at the application layer
 * 		and updates the information structure provided  in input.
 *	@param node_id The ID of the node we are interested in collecting the information about.
 *	@param info The data structure which contains the computed information.
 */
void Sunset_Protocol_Statistics::get_agt_tx(int node_id, map<int, agt_tx_info>& info) 
{
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int tot_pkt_sent = 0;
	int node = -1;
	int pkt_id =-1;
	
	map < sunset_statisticPktType, map <int, map <int, list <double> > > >::iterator it;
	map <int, map <int, list <double> > >::iterator it1;
	map <int, list <double> >::iterator it2;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it3;
	map <int, map <int, map <int, list <double> > > >::iterator it4;
	it = pkt_sent.begin();
	
	for (; it != pkt_sent.end(); it++) {
		
		spktType = it->first;
		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node = it1->first;
			
			if (node_id == node) {
			
				it2 = (it1->second).begin();
				
				for (; it2 != (it1->second).end(); it2++) {
					
					int dst = 0;
				
					pkt_id = it2->first;
					
					if ( pkt_dest_info.find(spktType) == pkt_dest_info.end() ||
					    (pkt_dest_info[spktType]).find(node) == (pkt_dest_info[spktType]).end() ||
					    (pkt_dest_info[spktType])[node].find(pkt_id) == (pkt_dest_info[spktType])[node].end()) {
						
						Sunset_Debug::debugInfo(-1, -1, "Sunset_Protocol_Statistics::get_agt_tx ERROR size node %d pkt_id %d", node, pkt_id);
						
						continue;
					}
					
					dst = ((pkt_dest_info[spktType])[node])[pkt_id];
					
					if (info.find(dst) == info.end()) {
					
						agt_tx_info ati;
						ati.pkt_ = 0;
						info[dst] = ati;
					}
					
					(info[dst]).pkt_++;
				}
			}
		}
	}
}

/*!
 * 	@brief The get_agt_rx computes the information related to the packet received by the application layer
 * 		and updates the information structure provided in input.
 *	@param node_id The ID of the node we are interested in collecting the information about.
 *	@param info The data structure which contains the computed information.
 */
void Sunset_Protocol_Statistics::get_agt_rx(int node_id, map<int, agt_rx_info>& info) 
{
	int node = -1;
	int src = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int pkt_id = -1;
	
	int count = -1;
	
	map < sunset_statisticPktType, map <int, map <int, list <double> > > >::iterator it;
	map <int, map <int, list <double> > >::iterator it1;
	map <int, list <double> >::iterator it2;
	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it3;
	map <int, map <int, map <int, list <double> > > >::iterator it4;
	
	map < sunset_statisticPktType, map <int, map <int, map <int, list <int> > > > >::iterator it_hop;
	map <int, map <int, map <int, list <int> > > >::iterator it1_hop;
	map <int, map <int, list <int> > >::iterator it2_hop;
	map <int, list <int> >::iterator it3_hop;
	list <int>::iterator it4_hop;
	
	int count2 = 0;
	count = 0;
	double num_hop = 0.0;
	double num_hop_dup = 0.0;
	int count_dup = 0;
	
	it3 = pkt_recv.begin();
	
	for (; it3 != pkt_recv.end(); it3++) {
	
		spktType = it3->first;
		it4 = (it3->second).begin();
		
		for (; it4 != (it3->second).end(); it4++) {
		
			node = it4->first;
			
			if (node_id == node) {
			
				it1 = (it4->second).begin();
				
				for (; it1 != (it4->second).end(); it1++) {
				
					src = it1->first;
					
					if ((it1->second).size() == 0) {
					
						continue;
					}
					
					if (info.find(src) == info.end()) {
					
						agt_rx_info ari;
						ari.pkt_ = 0;
						ari.pkt_duplicated_ = 0;
						ari.pkt_hops_ = 0.0;
						ari.pkt_duplicated_hops_ = 0.0;
						info[src] = ari;
					}
					
					it2 = (it1->second).begin();
					
					for (; it2 != (it1->second).end(); it2++) {
					
						pkt_id = it2->first;
						count = (it2->second).size();
						(info[src]).pkt_++;
						
						if (count > 1) {
						
							(info[src]).pkt_duplicated_ += count - 1;
						}
					}
				}
			}
		}
	}
	
	it_hop = avg_num_hop.begin();
	
	for (; it_hop != avg_num_hop.end(); it_hop++) {
	
		spktType = it_hop->first;
		it1_hop = (it_hop->second).begin();
		
		for (; it1_hop != (it_hop->second).end(); it1_hop++) {
		
			node = it1_hop->first;
			
			if (node_id == node) {
			
				it2_hop = (it1_hop->second).begin();
				
				for (; it2_hop != (it1_hop->second).end(); it2_hop++) {
				
					src = it2_hop->first;
					
					if (info.find(src) == info.end()) {
					
						agt_rx_info ari;
						ari.pkt_ = 0;
						ari.pkt_duplicated_ = 0;
						ari.pkt_hops_ = 0.0;
						ari.pkt_duplicated_hops_ = 0.0;
						info[src] = ari;
					}
					
					it3_hop = (it2_hop->second).begin();
					
					for (; it3_hop != (it2_hop->second).end(); it3_hop++) {
					
						pkt_id = it3_hop->first;
						it4_hop = (it3_hop->second).begin();
						
						if (it4_hop != (it3_hop->second).end()) {
						
							num_hop += *it4_hop;
							count++;
						}
					}
					
					if (count > 0) {
					
						num_hop = num_hop / (double)count;
					}
					else {
						num_hop = 0.0;
					}
					
					(info[src]).pkt_hops_ = num_hop;
					count = 0;
				}
			}
		}
	}    
	
	it_hop = avg_num_hop_dup.begin();
	
	for (; it_hop != avg_num_hop_dup.end(); it_hop++) {
	
		spktType = it_hop->first;
		it1_hop = (it_hop->second).begin();
		
		for (; it1_hop != (it_hop->second).end(); it1_hop++) {
		
			node = it1_hop->first;
			
			if (node_id == node) {
			
				it2_hop = (it1_hop->second).begin();
				
				for (; it2_hop != (it1_hop->second).end(); it2_hop++) {
				
					src = it2_hop->first;
					
					if (info.find(src) == info.end()) {
					
						agt_rx_info ari;
						ari.pkt_ = 0;
						ari.pkt_duplicated_ = 0;
						ari.pkt_hops_ = 0.0;
						ari.pkt_duplicated_hops_ = 0.0;
						info[src] = ari;
					}
					
					it3_hop = (it2_hop->second).begin();
					
					for (; it3_hop != (it2_hop->second).end(); it3_hop++) {
					
						pkt_id = it3_hop->first;
						it4_hop = (it3_hop->second).begin();
						
						for (; it4_hop != (it3_hop->second).end(); it4_hop++) {
						
							num_hop_dup += *it4_hop;
							count_dup++;
						}
					}
					
					if (count_dup > 0) {
					
						num_hop_dup = num_hop_dup / (double)count_dup;
					}
					else {
						num_hop_dup = 0.0;
					}
					
					(info[src]).pkt_duplicated_hops_ = num_hop_dup;
					count_dup = 0;
				}
			}
		}
	}    
}

/*!
 * 	@brief The get_mac_tx computes the information related to the packet transmitted at the MAC layer
 * 		and updates the information structure provided in input.
 *	@param node_id The ID of the node we are interested in collecting the information about.
 *	@param info The data structure which contains the computed information.
 */
void Sunset_Protocol_Statistics::get_mac_tx(int node_id, map<int, mac_tx_info>& info) 
{
	int node = -1;
	int src = -1;
	int dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	map<int, double> mac_data_new;
	map<int, double> mac_ctrl_new;
	map<int, double> mac_data_sent;
	map<int, double> mac_ctrl_sent;

	map < sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > >::iterator it;
	map <int, map <int, map <int, list <double> > > >::iterator it1;
	map <int, map <int, list <double> > >::iterator it2;
	map <int, list <double> >::iterator it3;
	map < sunset_statisticPktType, map <int, map <int, list <double> > > > ::iterator it5;
	
	map<int, double>::iterator it_aux;
	
	it = (tx_done).begin();
	
	for (; it != (tx_done).end(); it++) {
	
		spktType = it->first;
		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			src = it1->first;
			
			if (node_id == src) {
			
				it2 = (it1->second).begin();
				
				for (; it2 != (it1->second).end(); it2++) {
				
					dst = it2->first;
					it3 = (it2->second).begin();
					
					if (info.find(dst) == info.end()) {
					
						mac_tx_info mti;
						mti.pkt_ = 0;
						mti.pkt_retry_ = 0;
						mti.pkt_discarded_ = 0;
						info[dst] = mti;
					}
					
					for (; it3 != (it2->second).end(); it3++) {
						
						(info[dst]).pkt_ += (it3->second).size();
					}
				}
			}
		}
	}
	
	it = (mac_pkt_discard).begin();
	
	for (; it != (mac_pkt_discard).end(); it++) {
	
		spktType = it->first;
		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			src = it1->first;
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				dst = it2->first;
				
				if (info.find(dst) == info.end()) {
				
					mac_tx_info mti;
					mti.pkt_ = 0;
					mti.pkt_retry_ = 0;
					mti.pkt_discarded_ = 0;
					info[dst] = mti;
				}
				
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					(info[dst]).pkt_discarded_ += (it3->second).size();
				}
			}
		}
	}

	it = (mac_pkt_sent).begin();
	
	for (; it != (mac_pkt_sent).end(); it++) {
	
		spktType = it->first;
		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			src = it1->first;
			
			if (node_id == src) {
			
				it2 = (it1->second).begin();
				
				for (; it2 != (it1->second).end(); it2++) {
				
					dst = it2->first;
					it3 = (it2->second).begin();
					
					for (; it3 != (it2->second).end(); it3++) {
					
						if (spktType == SUNSET_STAT_DATA) {
						
							if (mac_data_sent.find(dst) == mac_data_sent.end()) {
							
								mac_data_sent[dst] = 0.0;
							}
							
							mac_data_sent[dst] += (int)((it3->second).size());
						}
						else {
							if (mac_ctrl_sent.find(dst) == mac_ctrl_sent.end()) {
							
								mac_ctrl_sent[dst] = 0.0;
							}
							
							mac_ctrl_sent[dst] += (int)((it3->second).size());                
						}
					}
				}
			}
		}
	}
	
	it = (mac_new_pkt).begin();
	
	for (; it != (mac_new_pkt).end(); it++) {
	
		spktType = it->first;
		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			src = it1->first;
			it2 = (it1->second).begin();
			
			for (; it2 != (it1->second).end(); it2++) {
			
				dst = it2->first;
				it3 = (it2->second).begin();
				
				for (; it3 != (it2->second).end(); it3++) {
				
					if (spktType == SUNSET_STAT_DATA) {
					
						if (mac_data_new.find(dst) == mac_data_new.end()) {
						
							mac_data_new[dst] = 0.0;
						}
						
						mac_data_new[dst] += (int)((it3->second).size());
					}
					else {
						
						if (mac_ctrl_new.find(dst) == mac_ctrl_new.end()) {
						
							mac_ctrl_new[dst] = 0.0;
						}
						
						mac_ctrl_new[dst] += (int)((it3->second).size());                
					}
				}
			}
		}
	}
	
	it_aux = mac_data_sent.begin();
	
	for (; it_aux != mac_data_sent.end(); it_aux++) {
	
		int aux_retry = 0;
		
		if (mac_data_new.find(it_aux->first) != mac_data_new.end() && mac_data_new[it_aux->first] > 0) {
		
			if (info.find(it_aux->first) == info.end()) {
			
				mac_tx_info mti;
				mti.pkt_ = 0;
				mti.pkt_retry_ = 0;
				mti.pkt_discarded_ = 0;
				info[it_aux->first] = mti;
			}
			
			aux_retry = (int)((it_aux->second) - mac_data_new[it_aux->first]);
			
			if (aux_retry < 0) {
			
				aux_retry = 0;
			}
			
			(info[it_aux->first]).pkt_retry_ +=  aux_retry / mac_data_new[it_aux->first];
		}
	}
}

/*!
 * 	@brief The get_mac_rx computes the information related to the packet received by the MAC layer
 * 		and updates the information structure provided in input.
 *	@param node_id The ID of the node we are interested in collecting the information about.
 *	@param info The data structure which contains the computed information.
 */
void Sunset_Protocol_Statistics::get_mac_rx(int node_id, map<int, mac_rx_info>& info) 
{
	int node = -1;
	int src = -1;
	int dst = -1;
	sunset_statisticPktType spktType = SUNSET_STAT_NONE;
	int size = -1;
	int count = 0;
	
	map < sunset_statisticPktType, map <int, map <int, map <int, map <int, list < pair<double, double> > > > > > > ::iterator it;
	map <int, map <int, map <int, map <int, list < pair<double, double> > > > > >::iterator it1;
	map <int, map <int, map <int, list < pair<double, double> > > > >::iterator it2;
	map <int, map <int, list < pair<double, double> > > >::iterator it3;
	map <int, list < pair<double, double> > >::iterator it4;
	list < pair<double, double> >::iterator it5;
	
	it = mac_pkt_recv.begin();
	
	for (; it != mac_pkt_recv.end(); it++) {
		
		spktType = it->first;
		it1 = (it->second).begin();
		
		for (; it1 != (it->second).end(); it1++) {
		
			node = it1->first;
			
			if (node_id == node) {
			
				it2 = (it1->second).begin();
				
				for (; it2 != (it1->second).end(); it2++) {
				
					src = it2->first;
					it3 = (it2->second).begin();
					
					if (info.find(src) == info.end()) {
					
						mac_rx_info mri;
						mri.pkt_ = 0;
						mri.link_quality_ = 0.0;
						info[src] = mri;
					}

					for (; it3 != (it2->second).end(); it3++) {
						
						dst = it3->first;
						it4 = (it3->second).begin();
						
						for (; it4 != (it3->second).end(); it4++) {
						
							info[src].pkt_ += (it4->second).size();
							it5 = (it4->second).begin();
							
							for (; it5 != (it4->second).end(); it5++) {
							
								info[src].link_quality_ += (it5->second);
								count++;
							}
						}
					}
					
					if (count > 0) {
					
						info[src].link_quality_ /= count;
						count = 0;
					}
				}
			}
		}
	}
}

/*! 	@brief The getResidualEnergy() function returns the residual energy in the network.
 *	@retval energy The residual energy. 
 */

float Sunset_Protocol_Statistics::getResidualEnergy()
{
	float energy = 0.0;

	for (int id = 0; id <= max_node_id; id++) {

		energy += getResidualEnergy(id);
	}
		
	return energy;
}

/*! 	@brief The getResidualEnergy() function returns the residual energy of a given node.
 * 	@param id The node id.
 *	@retval energy The residual energy of the node. 
 */

float Sunset_Protocol_Statistics::getResidualEnergy(int id)
{
	return residualEnergy[id];
}


/*! 	@brief The getIdleTime() function returns the amount of time spent by the network in idle.
 *	@retval Idle time expressed in seconds. 
 */

float Sunset_Protocol_Statistics::getIdleTime() 
{
	float time = 0.0;

	for (int id = 0; id <= max_node_id; id++) {

		time += getIdleTime(id);
	}
		
	return time;
}

/*! 	@brief The getIdleTime() function returns the amount of time spent by the node in idle.
 * 	@param id The node id
 *	@retval Idle time expressed in seconds. 
 */

float Sunset_Protocol_Statistics::getIdleTime(int id) 
{
	map <double, double>::iterator it;
	float tmp = 0.0;
	
	for ( it = idleTime[id].begin(); it != idleTime[id].end(); ++it ) {
		
		tmp += (*it).second;
	}
	
	return tmp;
}


/*! 	@brief The getRxTime(int) function returns the amount of time spent by the network while receiving.
 *	@retval Receiving time expressed in seconds. 
 */

float Sunset_Protocol_Statistics::getRxTime() 
{
	float time = 0.0;

	for (int id = 0; id <= max_node_id; id++) {

		time += getRxTime(id);
	}
		
	return time;
}

/*! 	@brief The getRxTime() function returns the amount of time spent by the node while receiving.
 * 	@param id The node id
 *	@retval Receiving time expressed in seconds. 
 */

float Sunset_Protocol_Statistics::getRxTime(int id) 
{
	map <double, double>::iterator it;
	float tmp = 0.0;
	
	for ( it = rxTime[id].begin(); it != rxTime[id].end(); ++it ) {
		
		tmp += (*it).second;
	}
	
	return tmp; 
}

/*! 	@brief The getTotTxTime() function returns the amount of time spent by the network while transmitting.
 *	@retval Transmitting time expressed in seconds. 
 */

float Sunset_Protocol_Statistics::getTotTxTime() 
{
	float time = 0.0;

	for (int id = 0; id <= max_node_id; id++) {

		time += getTotTxTime(id);
	}
		
	return time;
}

/*! 	@brief The getTotTxTime() function returns the amount of time spent by the node while transmitting.
 * 	@param id The node id
 *	@retval Transmitting time expressed in seconds. 
 */

float Sunset_Protocol_Statistics::getTotTxTime(int id) 
{
	map <double, double>::iterator it;
	double tmp = 0.0;
	
	for ( it = txTime[id].begin(); it != txTime[id].end(); ++it ) {
		
		tmp += (*it).second;
	}
	
	return tmp; 
}

/*! 	@brief The getTxTime() function returns the amount of time spent by the network while transmitting using a given transmission power.
 * 	@param pow The transmission power we are interested in.
 *	@retval Transmitting time using the given transmission power expressed in seconds. 
 */

float Sunset_Protocol_Statistics::getTxTime(double pow) 
{
	float time = 0.0;

	for (int id = 0; id <= max_node_id; id++) {

		time += getTxTime(id, pow);
	}
		
	return time;
}

/*! 	@brief The getTxTime() function returns the amount of time spent by the node while transmitting using a given transmission power.
 * 	@param id The node id
 * 	@param pow The transmission power we are interested in.
 *	@retval Transmitting time using the given transmission power expressed in seconds. 
 */

float Sunset_Protocol_Statistics::getTxTime(int id, double pow) 
{
	map <double, double>::iterator it;
	float tmp = 0.0;
	
	for ( it = txTime[id].begin(); it != txTime[id].end(); ++it ) {
		
		if ( (*it).first == pow ) {
			
			tmp += (*it).second;
		}
	}
	
	return tmp; 
}

/*! 	@brief The getIdleConsumption() function returns the energy consumed by the network in idle mode.
 *	@retval Energy consumed expressed in Joule. 
 */

float Sunset_Protocol_Statistics::getIdleConsumption()
{
	float energy = 0.0;

	for (int id = 0; id <= max_node_id; id++) {

		energy += getIdleConsumption(id);
	}
		
	return energy;
}

/*! 	@brief The getIdleConsumption() function returns the energy consumed by the node in idle mode.
 * 	@param id The node id
 *	@retval Energy consumed expressed in Joule. 
 */

float Sunset_Protocol_Statistics::getIdleConsumption(int id)
{
	map <double, double>::iterator it;
	float tmp = 0.0;
	
	for ( it = idleTime[id].begin(); it != idleTime[id].end(); ++it ) {
		
		tmp += (*it).second * (*it).first;
	}
	
	return tmp; 
}
	 

/*! 	@brief The getRxConsumption() function returns the energy consumed by the network in receiving mode.
 *	@retval Energy consumed expressed in Joule. 
 */

float Sunset_Protocol_Statistics::getRxConsumption()
{
	float energy = 0.0;

	for (int id = 0; id <= max_node_id; id++) {

		energy += getRxConsumption(id);
	}
		
	return energy;
}

/*! 	@brief The getRxConsumption() function returns the energy consumed by the node in receiving mode.
 * 	@param id The node id
 *	@retval Energy consumed expressed in Joule. 
 */

float Sunset_Protocol_Statistics::getRxConsumption(int id)
{
	map <double, double>::iterator it;
	float tmp = 0.0;
	
	for ( it = rxTime[id].begin(); it != rxTime[id].end(); ++it ) {
		
		tmp += (*it).second * (*it).first;
	}
	
	return tmp; 
}

/*! 	@brief The getTotTxConsumption() function returns the total energy consumed by the node while transmitting.
 *	@retval Energy consumed expressed in Joule. 
 */

float Sunset_Protocol_Statistics::getTotTxConsumption()
{
	float energy = 0.0;

	for (int id = 0; id <= max_node_id; id++) {

		energy += getTotTxConsumption(id);
	}
		
	return energy;
}

/*! 	@brief The getTotTxConsumption() function returns the total energy consumed by the node while transmitting.
 * 	@param id The node id
 *	@retval Energy consumed expressed in Joule. 
 */

float Sunset_Protocol_Statistics::getTotTxConsumption(int id)
{
	map <double, double>::iterator it;
	float tmp = 0.0;
	
	for ( it = txTime[id].begin(); it != txTime[id].end(); ++it ) {
		
		tmp += (*it).second * txPower[(*it).first];
	}
	
	return tmp; 
}

/*! 	@brief The getTxTime() function returns the energy consumed by the network while transmitting using a given transmission power.
 * 	@param pow The transmission power we are interested in.
 *	@retval Energy consumed using the given transmission power expressed in Joule. 
 */

float Sunset_Protocol_Statistics::getTxConsumption(double pow)
{
	float energy = 0.0;

	for (int id = 0; id <= max_node_id; id++) {

		energy += getTxConsumption(id, pow);
	}
		
	return energy;
}

/*! 	@brief The getTxTime() function returns the energy consumed by the node while transmitting using a given transmission power.
 *  	@param id The node id
 * 	@param pow The transmission power we are interested in.
 *	@retval Energy consumed using the given transmission power expressed in Joule. 
 */

float Sunset_Protocol_Statistics::getTxConsumption(int id, double pow)
{
	map <double, double>::iterator it;
	float tmp = 0.0;
	
	for ( it = txTime[id].begin(); it != txTime[id].end(); ++it ) {
		
		if ( (*it).first == pow ) {
			
			tmp += (*it).second * txPower[(*it).first];
		}
	}
	
	return tmp; 
}
