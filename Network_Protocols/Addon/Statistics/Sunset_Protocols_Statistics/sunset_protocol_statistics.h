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


#ifndef __Sunset_Protocol_Statistics_h__
#define __Sunset_Protocol_Statistics_h__

#include "sunset_statistics.h"
#include "sunset_utilities.h"
#include "sunset_address.h"
#include "sunset_trace.h"
#include "sunset_information_dispatcher.h"

#include "sunset_agent_pkt.h"
#include "sunset_routing_pkt.h"
#include "sunset_mac_pkt.h"
#include "sunset_common_pkt.h"

#define FILE_NAME 	500
#define TIME2INT 	10000
#define STAT_MAX_BUF	3000

/*!
 @brief This data structure defines the different packet types we want to capture in our statistic analysis.
 */
typedef enum  {
	
	SUNSET_STAT_NONE = 0,
	SUNSET_STAT_DATA = 1,
	SUNSET_STAT_CTRL = 2,
	SUNSET_STAT_RTS = 3,
	SUNSET_STAT_CTS = 4,
	SUNSET_STAT_ACK = 5,
	SUNSET_STAT_WARNING = 6,
	SUNSET_STAT_TONE = 7,
	SUNSET_STAT_HELLO = 8
	
} sunset_statisticPktType;


/*!
 @brief This data structure defines the information to store for on-line or postprocessing analysis.
 */
typedef struct statInfo {
	
	int8_t sType;
	u_int16_t agt_src;
	u_int16_t agt_dst;
	u_int16_t agt_pktId;
	sunset_statisticPktType spktType;
	double realTime;
	u_int16_t node; 
	u_int16_t src;
	u_int16_t dst; 
	u_int16_t pkt_size;
	u_int16_t num_hop;
	double time;
	double link_quality;
	double rssi;
	
} statInfo;

/************************ _ START OF DATA STRUCTURES _ **********************************************************/
/* The following structures define the information provided to external modules to estimate the protocol performance. */

typedef struct agt_tx_info {
	
	int pkt_;
	
} agt_tx_info;

typedef struct agt_rx_info {
	
	int pkt_;
	int pkt_duplicated_;
	double pkt_hops_;
	double pkt_duplicated_hops_;
	
} agt_rx_info;

typedef struct mac_tx_info {
	
	int pkt_;
	double pkt_retry_;
	int pkt_discarded_;
	
} mac_tx_info;

typedef struct mac_rx_info {
	
	int pkt_;
	double link_quality_;
	
} mac_rx_info;

/************************ _ END OF DATA STRUCTURES _ **********************************************************/

/*! @brief This class implements all the functionalities to evaluate the performance of the protocol solutions
 * currently supported by the SUNSET framework. Key metrics are considered: Packet delivery ratio; Throughput;
 * Goodput; end-to-end latency; number of packet discarded and retransmitted at the MAC layer; protocol overhead; 
 * route lenght; queue size; etc.
 * When running in simulation mode the collected information are locally stored and processed and they are also
 * stored in a file for later use. When runnin in emulation mode the collected data are stored in a file to allow
 * for later post processing when all the files from the different nodes will be available
 * @see class Sunset_Statistics
 */

class Sunset_Protocol_Statistics: public Sunset_Statistics {
	
public:
	
 	Sunset_Protocol_Statistics();
	~Sunset_Protocol_Statistics()
	{
		pthread_mutex_destroy(&mutex_stat);
	}
	
	virtual int command( int argc, const char*const* argv );
	
	/*!< @brief return the packet type corresponding to the packet in input, according to the definition 
	in the sunset_statisticPktType strucute above. */
	virtual sunset_statisticPktType pktType(Packet *p); 
	
	/*!< @brief This is the function collecting the statisitcal information from the data and packet in input. */
	virtual void logStatInfo(sunset_statisticType sType, u_int16_t node, Packet* p, double time, const char *fmt, ...);
	
	virtual void start(); 	// start the staistics module
	virtual void stop();  	// stop the staistics module
	
	/* Functions used to collect a subste of statistics information from other modules which can be 
	 * transmitted acoustically. */
	void get_agt_tx(int node_id, map<int, agt_tx_info>& info);
	void get_agt_rx(int node_id, map<int, agt_rx_info>& info);
	void get_mac_tx(int node_id, map<int, mac_tx_info>& info);
	void get_mac_rx(int node_id, map<int, mac_rx_info>& info);
	/*******************************************************/
		
protected:
	
	pthread_mutex_t mutex_stat;
	
	static char fileOut[FILE_NAME]; //output file to log all the collected data
	static ofstream outFile;

	char fileOut2[FILE_NAME]; //output file to log the processed data.
	ofstream outFile2;
	
	map< sunset_statisticPktType, map <int, map <int, list <double> > > > pkt_sent;     // src - id - <tx time>
	map< sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > > pkt_recv;     // node - src - id - <rx time>
	map< sunset_statisticPktType, map <int, map <int, map <int, list <int> > > > > avg_num_hop;
	map< sunset_statisticPktType, map <int, map <int, map <int, list <int> > > > > avg_num_hop_dup;
	
	map <int, double> node_time;
	map <int, double> node_duration;
	
	map< sunset_statisticPktType, map <int, map <int, int> > > pkt_size_info; //node - pkt_id - pkt_size
	map< sunset_statisticPktType, map <int, map <int, int> > > pkt_dest_info; //node - pkt_id - dest_id
	
	//end-2-end latency, PDR, throughput, num_hop, duplicated pkt, hops per duplicated pkt
	
	map< sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > > mac_new_pkt; // node - dst - size - <time>
	map< sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > > mac_pkt_sent; // src - dst - size - <time>
	map< sunset_statisticPktType, map <int, map <int, map <int, map <int, list < pair <double, double> > > > > > > mac_pkt_recv; // node - src - dst - size - <time, quality>
	map< sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > > mac_pkt_discard; // src - dst - size - <time>
	
	map< sunset_statisticPktType, map <int, list<double> > > mac_pkt_recv_failed;
	
	map <int, list<int> > queue_length;
	map< sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > > tx_done;
	map< sunset_statisticPktType, map <int, map <int, map <int, list <double> > > > > tx_aborted;
	map< sunset_statisticPktType, map <int, map <int, map <int, double> > > > pkt_discarded;
	double start_time;
	double stop_time;
	

	void processCreateData(statInfo st);
	void processRecvData(statInfo st);
	void processNewMacData(statInfo st);
	
	void processMacSent(statInfo st);
	void processMacDiscard(statInfo st);
	void processTxAborted(statInfo st);
	void processTxDone(statInfo st);
	
	void processMacRecv(statInfo st);
	void processMacRecvFailed(statInfo st);
	
	void processQueueLength(statInfo st, int x);
	
	void update_data(statInfo st);
	void show_data();
	void compute_data();

	int ctrl_size;
	int preamble_size;
	double start_traffic;
	
	int max_node_id;
	
	
	int run_id;
	
	/* if 1 all the collected information are lgged in bunary format to reduce the file size, 
	 *  otherwise the ASCII format is used.*/
	int binaryOutput; 
	
	void setTxDuration(int, double, double, double );
	void setRxDuration(int id, double, double sec);
	void setIdleDuration(int id, double, double sec);
	
	map<int, double> residualEnergy;
	
	map<int, map <double, double> > txTime;		//<nodeId, tx_power, tx_time>
	map<int, map <double, double> > rxTime;		//<nodeId, rx_power, tx_time>
	map<int, map <double, double> > idleTime;	//<nodeId, idle_power, tx_time>
	
	double totalEnergy;

	
	int getGeneratedPacket(int src, int dst);

	int getDeliveredPacket(int src, int dst);

	int getDeliveredDuplicatedPacket(int src, int dst);

	int getGeneratedBytes(int src, int dst);

	int getDeliveredBytes(int src, int dst);

	int getDeliveredDuplicatedBytes(int src, int dst);

	double getRouteLength(int src, int dst);

	double getDuplicatedRouteLength(int src, int dst);
	
	int getMaxRouteLength(int src, int dst);

	int getMaxDuplicatedRouteLength(int src, int dst);

	int getNumRoutes(int src, int dst);
	
	int getDuplicatedNumRoutes (int src, int dst);

	double getPacketLatency(int src, int dst);

	double getDuplicatedPacketLatency(int src, int dst);

	double getExperimentTime();

	int getCreatedMacDataPacket(int src, int dst);

	int getMacDataPacketTransmissions(int src, int dst);

	int getMacDataPacketReceptions(int src, int dst);

	int getCreatedMacDataBytes(int src, int dst);

	int getMacDataBytesTransmissions(int src, int dst);

	int getMacDataBytesReceptions(int src, int dst);

	int getMacDataPacketDiscarded(int src, int dst);

	int getMacDataBytesDiscarded(int src, int dst);

	int getMacCtrlPacketTransmissions(int src, int dst);

	int getMacCtrlPacketReceptions(int src, int dst);

	int getMacCtrlBytesTransmissions(int src, int dst);

	int getMacCtrlBytesReceptions(int src, int dst);

	int getMacCtrlPacketDiscarded(int src, int dst);

	int getMacCtrlBytesDiscarded(int src, int dst);

	int getMacDataPacketDiscarded();

	int getMacCtrlPacketDiscarded();

	double getPDR(int src, int dst);

	double getPDR();

	double getGeneratedPacket();

	double getDeliveredPacket();

	double getGeneratedPacket(int src);

	double getDeliveredPacket(int src);

	double getApplicationThroughput(int src, int dst);

	double getApplicationThroughput();

	double getPacketLatency();

	double getDuplicatedPacketLatency();

	double getRouteLength();

	double getNumRoutes();

	double getMaxRouteLength();
	
	double getDuplicatedRouteLength();

	double getDuplicatedNumRoutes();

	double getMaxDuplicatedRouteLength();

	double getMacThroughput(int src, int dst);

	double getMacThroughput();

	double getMacDataPacketTransmissions();
	
	double getMacDataPacketReceptions();
	
	double getMacCtrlPacketTransmissions();

	double getMacCtrlPacketReceptions();

	double getMacLoad();

	double getMacLoadBytes();

	double getMacDataLoad();

	double getMacDataLoadBytes();

	double getMacCtrlLoad();

	double getMacCtrlLoadBytes();

	double getMacDataRetransmissions(int src, int dst);

	double getMacDataRetransmissions();
	
	double getOverheadPerBit(int src, int dst);
	
	double getOverheadPerBit();
	
	float getResidualEnergy();
	
	float getResidualEnergy(int id);
	
	float getIdleTime();
	
	float getIdleTime(int );
	
	float getRxTime();
	
	float getRxTime(int );
	
	float getTxTime(double );
	
	float getTxTime(int, double);
	
	float getTotTxTime();
	
	float getTotTxTime(int);
	
	float getIdleConsumption();
	
	float getIdleConsumption(int );
	
	float getRxConsumption();
	
	float getRxConsumption(int );
	
	float getTxConsumption(double );
	
	float getTxConsumption(int, double );
	
	float getTotTxConsumption();
	
	float getTotTxConsumption(int );
	
	map<float, float> txPower;	//dbrem pascal, watt
	
};

#endif
