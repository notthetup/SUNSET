/*/* SUNSET - Sapienza University Networking framework for underwater Simulation, Emulation and real-life Testing
 *
 * Copyright (C) 2012 Regents of UWSN Group of SENSES Lab <http://reti.dsi.uniroma1.it/SENSES_lab/>
 *
 * Author: Roberto Petroccia - petroccia@di.uniroma1.it
 *         Daniele Spaccini - spaccini@di.uniroma1.it
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

#ifndef __Sunset_Udp_Connection_h__
#define __Sunset_Udp_Connection_h__

#include <sunset_connection.h>
#include <sunset_debug.h>

#define SUNSET_UDP_MAX_IP_LEN		16
#define SUNSET_UDP_MAX_BUF_SIZE		32768

/*! \brief The generic UDP connections class */

class Sunset_Udp_Connection : public Sunset_Connection {
	
public:
	
	Sunset_Udp_Connection(const char *, int);
	Sunset_Udp_Connection(const char *, int, int);
	~Sunset_Udp_Connection();
	
	virtual bool open_connection();	
	virtual bool close_connection();
	virtual bool write_data(char * data, int len_data);
	virtual bool write_data(int fd, char * data, int len_data);
	virtual int read_data(char *, int);
	virtual int read_data(int fd, char *, int);
	
	char *get_ip();		//return used IP
	
	int get_rx_port();
	int get_tx_port();
	
protected:
	
	char ip_addr[SUNSET_UDP_MAX_IP_LEN];
	int portTx, portRx;
	
	struct sockaddr_in serv_add; 
};

#endif
