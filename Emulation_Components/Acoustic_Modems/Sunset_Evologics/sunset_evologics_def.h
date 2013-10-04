/* SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
 *
 * Copyright (C) 2012 Regents of UWSN Group of SENSES Lab <http://reti.dsi.uniroma1.it/SENSES_lab/>
 *
 * Authors: Roberto Petroccia - petroccia@di.uniroma1.it
 *          Daniele Spaccini - spaccini@di.uniroma1.it
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


// GLOBAL DEFINES

#ifndef _Sunset_Evologics_Def_
#define _Sunset_Evologics_Def_

// MODEM STATE

#define     EV_SETTING          1 
#define     EV_IDLE             2 
#define     EV_DATA_IM          3 
#define     EV_DATA_BURST       4 
#define     EV_WAIT_DATA_STATUS 5 
#define     EV_WAIT_BURST_RESP 	6 
#define     EV_GET_DELAY 	7 
#define     EV_INITIAL_STATE 	8 
#define     EV_RANGING          9

// MODEM SETTING ACTION

#define     EV_SET_ADDR         1
#define     EV_SET_POWER        2
#define     EV_SET_PROMISCOUS   3

//---------------

#define EV_TIMEOUT_CMD          3.0
#define EV_TIMEOUT_DLV          0.2
#define EV_TIMEOUT_BURSTRESP	10.0

#define EV_TX_BASIC_TIME 		0.2

#define EV_TIMEOUT_DLV_MAX_RET	10
#define EVO_TIMEOUT_CONNECTION 	5

#define EV_RANGE_SIZE           1


#define EV_MAX_IP_LEN 		16		/*!< @brief max length of an ip address */
#define EV_BUFSIZE 		1054		/*!< @brief max buffer size, including AT command */
#define EV_PACKET_LENGHT_DATA	1024
#define EV_PACKET_LENGHT_IM	64
#define EV_CODING_FACTOR	2.0		/*!< @brief standard modem coding factor for acousti transmission. 
X data bytes will result in X * EV_CODING_FACTOR bytes transmitted in water */


#define EV_RANGING_SIZE 1

#define MODEM_HEADER		21		// MODEM ADDITIONAL OVERHEAD TO THE PAYLOAD ALREADY CODED 	
#define SYNC_TIME		0.072		// SYNC TRAINING TIME
#define PROCESSING_DELAY	0.020		// MODEM PROCESSING DELAY


#define EV_OK			"OK"
#define EV_OK2 			"[*]OK"
#define EV_ERROR		"ERROR"		/*!< @brief syntax error or buf size exceeded */
#define EV_EOC			"\n"

#define EV_BUSY				"BUSY"
#define EV_ERROR_WRONG_FORMAT		"ERROR WRONG FORMAT"
#define EV_EXPIREDIMS			"EXPIREDIMS"
#define EV_DELIVERED 			"DELIVERED"
#define EV_FAILED			"FAILED"
#define EV_DELIVEREDIM 			"DELIVEREDIM"
#define EV_FAILEDIM			"FAILEDIM"
#define EV_INITIATION_LISTEN		"INITIATION LISTEN"
#define EV_INITIATION_ESTABLISH		"INITIATION ESTABLISH"
#define EV_INITIATION_DEAF		"INITIATION DEAF"
#define EV_RECVIMS			"RECVIMS"
#define EV_RECVIMS_COMMA		"RECVIMS,"
#define EV_SENTIMS			"SENTIMS"
#define EV_RECVIM			"RECVIM"
#define EV_RECVIM_COMMA			"RECVIM,"
#define EV_RECV				"RECV"
#define EV_RECV_COMMA			"RECV,"
#define EV_RECVFAILED			"RECVFAILED"
#define EV_DELIVERING			"DELIVERING"
#define EV_EMPTY			"EMPTY"
#define EV_CANCELEDIM			"CANCELEDIM"

#define EV_ERROR_OUT_OF_CONTEXT				"ERROR OUT OF CONTEXT"
#define EV_ERROR_NOT_ALLOWED_IN_AUTOMATIC_REGIME	"ERROR NOT ALLOWED IN AUTOMATIC REGIME"
#define EV_ERROR_BUFFERS_NOT_EMPTY			"ERROR BUFFERS NOT EMPTY"

#define EV_AT_SENDIMS		"AT*SENDIMS"		/*!< @brief Send synchronous instant message */
#define EV_AT_SENDIM		"AT*SENDIM"		/*!< @brief Send instant message */
#define EV_AT_SEND		"AT*SEND"		/*!< @brief Send burst message */


#define EV_ACK			"ack"
#define EV_NO_ACK		"noack"
#define EV_FLAG_STR_SIZE	5

#define EV_ENC_TOKEN	'$'

/*! @brief This structure contains the instant message fields.  */

struct im_struct {
	
	char data[EV_BUFSIZE];
	int length;
	int source, destination;
	char flag[EV_FLAG_STR_SIZE+1];
	double bitrate, rms, integrity, prop_time, velocity, duration;
	
};

/*! @brief This structure contains the synchronous instant message fields.  */

struct ims_struct {
	
	char data[EV_BUFSIZE];
	int length;
	int source, destination;
	double sync_usec, delta;
	double bitrate, rms, integrity, prop_time, velocity, duration;
	
};

/*! @brief This structure contains the burst message fields.  */

struct burst_struct {
	
	char data[EV_BUFSIZE];
	int length;
	int source, destination;
	double bitrate, rms, integrity, prop_time, velocity;
	
};

/*! @brief This structure contains the modem configuration settings.  */

struct evo_setting {
	
	int type;
	int value;
};

#endif
