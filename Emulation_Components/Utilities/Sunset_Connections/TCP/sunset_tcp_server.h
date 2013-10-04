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


#ifndef __Sunset_Tcp_Server_h__
#define __Sunset_Tcp_Server_h__

#include <sunset_tcp.h>
#include <sunset_debug.h>

#define SERVER_ALREADY_RUNNING		-2

/*! \brief TCP Server connections class */

class Sunset_Tcp_Server_Connection : public Sunset_Tcp_Connection {
	
public:
	
	Sunset_Tcp_Server_Connection(const char *, int);
	~Sunset_Tcp_Server_Connection();
	
	int accept_connection();
	
	int fd_client;
	
	virtual bool open_connection();
 	virtual bool close_connection() { Sunset_Tcp_Connection::close_connection(); } 
	virtual bool close_connection(int );
	
	int get_fd_client() { return fd_client; } 	/*!< @brief This function returns the client socket id. */
	
};

#endif
