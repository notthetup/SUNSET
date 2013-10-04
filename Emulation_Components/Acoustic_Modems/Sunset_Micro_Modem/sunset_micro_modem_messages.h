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


#ifndef __MicroModem_Messages_h__
#define __MicroModem_Messages_h__  

extern "C" {
	
#include "ext_include/libumutil.h"
#include "ext_include/extern.h"
#include "ext_include/nmeakeys.h"
#include "ext_include/libnmea.h"
#include "ext_include/libnmea.h"
};


/* umodem states */

#define MM_S_DISABLED  		(0x00)   /* disabled */
#define MM_S_WAIT_CMD  		(0x01)   /* idle */
#define MM_S_WAIT_CFQ  		(0x02)   /* wrote CCCFQ wait CACFQ */
#define MM_S_WAIT_CFG  		(0x03)   /* wrote CCCFG wait CACFG */
#define MM_S_WAIT_CYC  		(0x04)   /* wrote CCCYC, want CACYC */
#define MM_S_WAIT_RXP  		(0x05)   /* heard CARXP, want CACYC (named funny, i know) or CARXD */
#define MM_S_WAIT_RXD  		(0x06)   /* heard CACYC, src is not us, want CARXD */
#define MM_S_WAIT_DRQ  		(0x07)   /* heard CACYC, src is us, want CADRQ */
#define MM_S_WAIT_MUC  		(0x08)   /* sent CCMUC, want CAMUC */
#define MM_S_WAIT_MUA  		(0x09)   /* heard CAMUC, dest not BCAST, want CAMUR */
#define MM_S_WAIT_MPC  		(0x0a)   /* sent CCMPC, want CAMPC */
#define MM_S_WAIT_MPR  		(0x0b)   /* heard CAMPC, want CAMPR */
#define MM_S_WAIT_TXD  		(0x0c)   /* wrote CCTXD, want CATXD */
#define MM_S_WAIT_TXP  		(0x0d)   /* heard CATXD/CAMUC/CAMPC, want CATXP */
#define MM_S_WAIT_TXF  		(0x0e)   /* heard CATXP want CATXF */
#define MM_S_WAIT_XST  		(0x0f)   /* heard CATXF, want CAXST */
#define MM_S_WAIT_CST  		(0x10)   /* heard CARXD, want CACST */
#define MM_S_WAIT_ACK  		(0x11)   /* heard CACST, ACK=1, want all CAACKs */
#define MM_S_WAIT_POL  		(0x12)   /* polled send state between CACYC and CADRQ */
#define MM_S_HAVE_ERR  		(0x13)   /* heard a CAERR, ERROR is initialized with the error code */
#define MM_S_HAVE_MSG  		(0x14)   /* heard a CAMSG, MSG is initialized with the error code */
#define MM_S_MANAGE_MSG_ERR 	(0x15)   /* heard a CAMSG or a CAERR, MSG is initialized with the error code or ERR is initialized with the code error */
#define MM_S_SERIAL_ERR  	(0x16)   /* we had a problem transmitting over serial */

/*! @brief This class is responsable for the creation of messages directed to the Micro-Modem*/

class MicroModem_Messages 
{
	
public:
	
	MicroModem_Messages() {};
	~MicroModem_Messages() {};
	
public:
	
	void calculateCheckSum(char *buf);
	void creaPktCCCFG(char *buf, int maxlen, char *par, int val, int cksum);
	void creaPktCCCFQ(char *buf, int maxlen, char *par, int cksum);
	void creaPktCCCLQ(char *buf, int maxlen, int cksum);
	void creaPktCCCLK(char *buf, int maxlen, int year, int month, int mday, int hour, int min, int sec, int cksum);
	void creaPktCCCYC(char *buf, int maxlen, int cmd, int src, int dest, int rate, int flag, int skct, int cksum);
	void creaPktCCTXD(char *buf, int maxlen, int src, int dest, int ack, int len, char *msg, int cksum);
	void creaPktCCTXA(char *buf, int maxlen, int src, int dest, int ack, int len, char *msg, int cksum);
	void creaPktCCMUC(char *buf, int maxlen, int src, int dest, char *msg, int cksum);
	void creaPktCCMPC(char *buf, int maxlen, int src, int dest, int cksum);
	void creaPktCCMEC(char *buf, int maxlen, int src, int dest, int line, int mode, int arg, int cksum);
	void creaPktCCMSC(char *buf, int maxlen, int src, int dest, int arg, int cksum);
	//	void creaPktCCEXL(char *buf, int maxlen, char *mode, int cksum);
	void creaPktCCRSP(char *buf, int maxlen, int txsig, int cksum);
	void creaPktCCCFR(char *buf, int maxlen, int sd_msec, int cksum);
	void creaPktCCPNT(char *buf, int maxlen, int txfreq, int rxl_msec, int txl_msec, 
			  int timeo, int freqa, int freqb, int freqc, int freqd, int extsync, int cksum);
	void creaPktCCPDT(char *buf, int maxlen, int group, int channel, int extsync, int pps_timeo, 
			  int timeo, int af, int bf, int cf, int df, int cksum);
	int NMEAPktType(char* buf);
	int sscannmea(char *buf, int nmeai, void *pkt);
	
};

#endif
