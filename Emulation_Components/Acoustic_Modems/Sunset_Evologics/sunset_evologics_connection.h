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


#ifndef __Sunset_Evologics_Conn_h__
#define __Sunset_Evologics_Conn_h__  

#include <sunset_tcp_client.h>

#define EV_READ_NO_CLIENT	-1
#define EV_READ_ERR		-2
#define EV_READ_BUF_MAX		-3

#define EV_CONN_NO_SOCK 	-1
#define EV_CONN_NO_ADDR 	-2
#define EV_CONN_NO_CONN 	-3
#define EV_CONN_NOFD 		-4
#define EV_CONN_NO_TCGET 	-5
#define EV_CONN_NO_TCSET 	-6

#define EV_IM_BUF	0
#define EV_BURST_BUF	1
#define EV_IMS_BUF	2

/*! @brief This class is used for the connection to the Evologics Modem. A TCP connection is used.
 */

class Sunset_Evologics_Conn : public Sunset_Tcp_Client_Connection {
public:
	
	Sunset_Evologics_Conn(const char* ip, int port);	
	~Sunset_Evologics_Conn();
	
public:
	virtual int read_data(char *recvb, int max_len);
	virtual bool open_connection();
	virtual bool close_connection();
	
protected:
	virtual int checkData(char *, int);
	int countComma(char *);
};

#endif
