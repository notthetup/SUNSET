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

#include "sunset_evologics_connection.h"
#include "sunset_evologics_def.h"


Sunset_Evologics_Conn::Sunset_Evologics_Conn(const char *ip,  int port) : Sunset_Tcp_Client_Connection(ip, port)
{
	
}

Sunset_Evologics_Conn::~Sunset_Evologics_Conn() {
}

/*! @brief The open_connection() function opens TCP connetion.  */

bool Sunset_Evologics_Conn::open_connection() {
	
	Sunset_Debug::debugInfo(4, -1, "DRIVER_EVO connecting to %s port %d", get_ip(), get_port());
	
	return Sunset_Tcp_Client_Connection::open_connection();
}

/*! @brief The close_connection() function closes the TCP connetion.  */

bool Sunset_Evologics_Conn::close_connection() {
	
	Sunset_Debug::debugInfo(4, -1, "DRIVER_EVO close_connection to %s port %ld", get_ip(), get_port());
	
	return Sunset_Tcp_Client_Connection::close_connection();
}


/*!	@brief The read_data() function reads from the TCP socket looking for packet delimiter to extract the packet received by the modem which has to be provided to the upper layers.
 *	@param buf The buffer to be read.
 *	@param maxlen The maximal length of the buffer.
 * 	@retval nbytes The number of bytes correctly read.
 */

int Sunset_Evologics_Conn::read_data(char *buf, int max_len) {
	
	int  nbytes = 0;
	int i = 0;
	
	memset(buf, 0x0, max_len);
	
	if ( fd <= 0 ) {
		
		return -1;
		
	}
	
	while ((i = read(fd, buf+nbytes, sizeof(char))) >= 0)
	{
		
		Sunset_Debug::debugInfo(8, -1, " Sunset_Evologics_Conn::read_data %s - len %d(%d)", buf, nbytes, i);
		
		if ( i == 0 ) {
			
			return EV_READ_NO_CLIENT;
			
		}
		
		if ( i < 0 ){
			
			return EV_READ_ERR;
			
		}
		
		nbytes = nbytes + i;
		
		if ( nbytes >= max_len ) {
			
			return EV_READ_BUF_MAX;
			
		}
		
		/* If I received a EV_RECVIMS I invoke the checkData to read all the data */
		
		if ( strncmp(buf, EV_RECVIMS_COMMA, strlen(EV_RECVIMS_COMMA)) == 0  && countComma(buf) == 11  ) {
			
			int l = checkData(buf, EV_IMS_BUF);
			Sunset_Debug::debugInfo(5, -1, " Sunset_Evologics_Conn::read_data %s - len %d(%d) l %d", buf, nbytes, i, l);
			return l;
			
		}
		
		/* If I received a EV_RECVIM I invoke the checkData to read all the data */
		
		if ( strncmp(buf, EV_RECVIM_COMMA, strlen(EV_RECVIM_COMMA)) == 0  && countComma(buf) == 10  ) {
			
			int l = checkData(buf, EV_IM_BUF);
			Sunset_Debug::debugInfo(5, -1, " Sunset_Evologics_Conn::read_data___ %s - len %d(%d) l %d", buf, nbytes, i, l);
			return l;
			
		}
		
		/* If I received a EV_RECV I invoke the checkData to read all the data */
		
		if ( strncmp(buf, EV_RECV_COMMA, strlen(EV_RECV_COMMA)) == 0  && countComma(buf) == 9  ) {
			
			return checkData(buf, EV_BURST_BUF);
			
		}
		
		/* If I received a EV_RECVFAILED I invoke the checkData to read all the data */
		
		if ( strcmp(buf, EV_RECVFAILED) == 0  && strstr(buf, "\r\n") != NULL  ) {
			
			int buflen = strlen(buf);
			
			buf[buflen-2] = 0x0;
			buf[buflen-1] = 0x0;
			
			return strlen(buf);
			
		}
		
		/* If I have read the "\r\n" string */
		
		if ( strstr(buf, "\r\n") != NULL ) {
			
			/* If the data starts with EV_RECVIM I'll invoke the checkData to check the 
			 payload. Else I'll simply return the string read. */
			
       			if ( strncmp(buf, EV_RECVIM, strlen(EV_RECVIM)) == 0) {
				
				return checkData(buf, EV_IM_BUF);
				
			} else {
				
				int buflen = strlen(buf);
				
				buf[buflen-2] = 0x0;
				buf[buflen-1] = 0x0;
				
				return strlen(buf);
			}
		}
	}
	
	return -1;
}

int Sunset_Evologics_Conn::countComma(char *buf) {
	
	int i = 0;
	int count = 0;
	
	for ( i = 0; i < strlen(buf); i++ ) {
		
		if ( buf[i] == ',' ) {
		
			count++;
		}
	}
	
	return count;
	
}

/*!	@brief The checkData() function extracts the payload from the Evologics packet.
 *	@param buf The buffer containing the payload.
 *	@param type The type of packet (EV_RECVIM or EV_RECV).
 * 	@retval buflen The payload length.
 */

int Sunset_Evologics_Conn::checkData(char *recvb, int type) {
	
	char temp[EV_BUFSIZE];
	char *tok;
	int len = 0;
	int i = 0;
	int buflen = 0;
	char* aux_mem; 
	
	memset(temp, 0x0, EV_BUFSIZE);
	
	buflen = strlen(recvb);
	
	strncpy(temp, recvb, strlen(recvb));
	
	Sunset_Debug::debugInfo(8, -1, " Sunset_BaseConnection::checkdata entering");
	
	tok = strtok_r(temp, ",", &aux_mem);
	
	int count = 8;
	
	if ( tok == NULL ) {
		
		buflen = strlen(recvb);
		
		return strlen(recvb);
		
	}
	
	if ( type == EV_IM_BUF ) {
		
		tok = strtok_r(NULL, ",", &aux_mem);
		
		if ( tok == NULL ) {
			
			buflen = strlen(recvb);
			
			return strlen(recvb);
			
		}
		
	}
	
	if ( type == EV_IMS_BUF ) {
		
		tok = strtok_r(NULL, ",", &aux_mem);
		
		if ( tok == NULL ) {
			
			buflen = strlen(recvb);
			
			return strlen(recvb);
			
		}
		
		count = 9;
		
	}

	/* I store the length of the payload extracted from the buffer */
	
	len = atoi(tok);
	
	while( i++ < count ){
		
		tok = strtok_r(NULL, ",", &aux_mem);
		
		if ( tok == NULL ) {
			
			Sunset_Debug::debugInfo(8, -1, " Sunset_BaseConnection::checkdata exit");
 			
			return strlen(recvb);
			
		}
	}
	
	i = 0;
	
	/* I read all the "len" data + "\r\n" */
	
	i = read(get_fd(), recvb+buflen, len+2);
	
	if ( i < 0 ) {

		return i;
	}
	
	
	return buflen+len;
}

