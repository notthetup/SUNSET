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


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <poll.h>
#include <getopt.h>

#include <sunset_debug.h>
#include <sunset_micro_modem_messages.h>

/*!
 * 	@brief The calculateCheckSum function calculate the chacksum for a given buffer and add the chakesum at the end of the buffer.
 *	@param buf The buffer used to calculate the checksum.
 */

void MicroModem_Messages::calculateCheckSum(char *buf) 
{
	
	char checksum [3] = {'\0'};
	long chksum = 0;
	
	Sunset_Debug::debugInfo(10, -1, "MicroModem_Messages::calculateCheckSum");
	
	buf[strlen(buf)-2] = '*';
	buf[strlen(buf)-1] = '\0';
	
	nmea_cksum(buf, &chksum);
	
	sprintf(checksum, "%ld", chksum);
	strcat(buf, checksum);
	
	buf[strlen(buf)] = '\r';
	buf[strlen(buf)+1] = '\n';
	
	return;
}

/*!
 * 	@brief The creaPktCCCFG function is use to create a message to set a given parameter of the modem 'par' with a specified value 'val'
 *	@param buf The buffer containing the created message.
 *	@param maxlen The maximal length of the buffer.
 *	@param par The parameter to be set
 *	@param val The value assigned to par
 *	@param cksum If it is set to 1 the checksum for this message is calculated and added to the created packet. 
 */

void MicroModem_Messages::creaPktCCCFG(char *buf, int maxlen, char *par, int val, int cksum)
{
	cccfg_t cfg;
	
	strncpy(cfg.key, par, 4);
	
	cfg.val = val;
	
	um_print_cccfg(buf, maxlen, &cfg);
	
	if (cksum) {
		
		calculateCheckSum(buf);
	}
	
	return;
}

/*!
 * 	@brief The creaPktCCCFQ function is use to create a message to return the value of a given parameter of the modem 'par'
 *	@param buf The buffer containing the created message.
 *	@param maxlen The maximal length of the buffer.
 *	@param par The parameter to be investigated
 *	@param cksum If it is set to 1 the checksum for this message is calculated and added to the created packet. 
 */

void MicroModem_Messages::creaPktCCCFQ(char *buf, int maxlen, char *par, int cksum)
{
	cccfq_t cfq;
	
	strncpy(cfq.key, par, 4);
	
	um_print_cccfq(buf, maxlen, &cfq);
	
	if (cksum) {
		
		calculateCheckSum(buf);
	}
	
	return;
}

/**
 * cksum Set to 1 means that checksum is calculated and added to the sending packet.
 */

void MicroModem_Messages::creaPktCCCLQ(char *buf, int maxlen, int cksum)
{
	ccclq_t clq;
	
	um_print_ccclq(buf, maxlen, &clq);
	
	if (cksum) {
		
		calculateCheckSum(buf);
	}
	
	return;
}

/*!
 * 	@brief The creaPktCCCLK function is use to create a message to set the internal clock of the modem, providing timing information.
 *	@param buf The buffer containing the created message.
 *	@param maxlen The maximal length of the buffer.
 *	@param year The year to be set.
 *	@param month The month to be set.
 *	@param mday The day to be set.
 *	@param hour The hour to be set.
 *	@param min The minute to be set.
 *	@param sec The second to be set.
 *	@param cksum If it is set to 1 the checksum for this message is calculated and added to the created packet. 
 */

void MicroModem_Messages::creaPktCCCLK(char *buf, int maxlen, int year, int month, int mday, int hour, int min, int sec, int cksum)
{
	ccclk_t clk;
	
	clk.ts.tm_year  = year; /* 1900 */
	clk.ts.tm_mon   = month; /*    1 */
	clk.ts.tm_mday  = mday;
	clk.ts.tm_hour  = hour;
	clk.ts.tm_min   = min;
	clk.ts.tm_sec   = sec;
	
	um_print_ccclk(buf, maxlen, &clk);
	
	if (cksum) {
		
		calculateCheckSum(buf);
	}
	
	return;
}

/*!
 * 	@brief The creaPktCCCYC function is use to create a Network Cycle Initialization Command message to start a communication with another node.
 *	@param buf The buffer containing the created message.
 *	@param maxlen The maximal length of the buffer.
 *	@param cmd It is deprecated. Use any number from 0 - 7 in this field
 *	@param src Packet source ID
 *	@param dest Packet destination ID
 *	@param rate Packet type:\n
 *	- 0 80 bps FH-FSK \n
 *	- 1 250 bps 1/31 spreading (unimplemented) \n
 *	- 2 500 bps 1/15 spreading \n
 *	- 3 1200 bps 1/7 spreading \n
 *	- 4 1300 bps 1/6 rate block code (unimplemented)\n
 *	- 5 5300 bps 9/14 rate block code. \n
 *	@param flag Deprecated. Use either 0 or 1 in this field. To send acknowledgment mini-packets, 
 *							set the ACK bit in the $CCTXD or $CCTXA messages.
 *	@param skct  Frame count
 *	@param cksum Set to 1 means that checksum is calculated and added to the sending packet.
 */

void MicroModem_Messages::creaPktCCCYC(char *buf, int maxlen, int cmd, int src, int dest, int rate, int flag, int skct, int cksum)
{
	cccyc_t cyc;
	
	cyc.cmd  = cmd;
	cyc.src  = src;
	cyc.dest = dest;
	cyc.rate = rate;
	cyc.flag = flag;
	cyc.skct = skct;
	
	um_print_cccyc(buf, maxlen, &cyc);
	
	if (cksum) {
		
		calculateCheckSum(buf);
	}
	
	return;
}

/*!
 * 	@brief The creaPktCCTXD function is use to create a binary data message, host to modem.
 *	@param buf The buffer containing the created message.
 *	@param maxlen The maximal length of the buffer.
 *	@param src Packet source ID
 *	@param dest Packet destination ID
 *	@param ack The ack bit set by transmitter, 0 or 1. 1 means that an ack from the modem (mini-packet) is desired for this frame
 *	@param msg The hex coded message payload
 *	@param len The data length
 *	@param cksum Set to 1 means that checksum is calculated and added to the sending packet.
 */

void MicroModem_Messages::creaPktCCTXD(char *buf, int maxlen, int src, int dest, int ack, int len, char *msg, int cksum)
{
	cctxd_t txd;
	
	txd.src  = src;
	txd.dest = dest;
	txd.ack  = ack;
	txd.len  = len;
	
	memset(txd.buf,'\0', len+1);
	
	strncpy(txd.buf, msg, txd.len);
	
	um_print_cctxd(buf, maxlen, &txd);
	
	if (cksum) {
		
		calculateCheckSum(buf);
	}
	
	return;
}

/*!
 * 	@brief The creaPktCCTXA function is use to create a ASCII data message, host to modem.
 *	@param buf The buffer containing the created message.
 *	@param maxlen The maximal length of the buffer.
 *	@param src Packet source ID
 *	@param dest Packet destination ID
 *	@param ack The ack bit set by transmitter, 0 or 1. 1 means that an ack from the modem (mini-packet) is desired for this frame
 *	@param msg The ASCII coded message payload
 *	@param len The data length
 *	@param cksum Set to 1 means that checksum is calculated and added to the sending packet.
 */

void MicroModem_Messages::creaPktCCTXA(char *buf, int maxlen, int src, int dest, int ack, int len, char *msg, int cksum)
{
	cctxa_t txa;
	
	txa.src  = src;
	txa.dest = dest;
	txa.ack  = ack;
	txa.len  = len;
	
	strncpy(txa.buf, msg, txa.len);
	
	um_print_cctxa(buf, maxlen, &txa);
	
	if (cksum) {
		
		calculateCheckSum(buf);
	}
	
	return;
}

/*!
 * 	@brief The creaPktCCMUC function is use to create a mini-packet, host to modem.
 *	@param buf The buffer containing the created message.
 *	@param maxlen The maximal length of the buffer.
 *	@param src Packet source ID
 *	@param dest Packet destination ID
 *	@param msg ASCII-coded hex data (2 Hex values). Values in the range of 0 to 1FFF are legal.
 *	@param cksum Set to 1 means that checksum is calculated and added to the sending packet.
 */

void MicroModem_Messages::creaPktCCMUC(char *buf, int maxlen, int src, int dest, char *msg, int cksum)
{
	ccmuc_t muc;
	
	muc.src  = src;
	muc.dest = dest;
	
	strncpy(muc.buf, msg, strlen(msg)+1);
	
	um_print_ccmuc(buf, maxlen, &muc);
	
	if (cksum) {
		
		calculateCheckSum(buf);
	}
	
	return;
}

/*!
 * 	@brief The creaPktCCMPC function is use to create a mini-packet Ping command, host to modem.
 *	@param buf The buffer containing the created message.
 *	@param maxlen The maximal length of the buffer.
 *	@param src Packet source ID. Node pinging
 *	@param dest Packet destination ID. Node to be pinged.
 *	@param cksum Set to 1 means that checksum is calculated and added to the sending packet.
 */

void MicroModem_Messages::creaPktCCMPC(char *buf, int maxlen, int src, int dest, int cksum)
{
	ccmpc_t mpc;
	
	mpc.src  = src;
	mpc.dest = dest;
	
	um_print_ccmpc(buf, maxlen, &mpc);
	
	return;
}

/**
 * External hardware control command, host to modem
 * src = source
 * dest = destination
 __
 * line   \_
 * mode ---_|--> see reference manual
 * arg  __/
 * cksum Set to 1 means that checksum is calculated and added to the sending packet.
 */
void MicroModem_Messages::creaPktCCMEC(char *buf, int maxlen, int src, int dest, int line, int mode, int arg, int cksum)
{
	ccmec_t mec;
	
	mec.src  = src;
	mec.dest = dest;
	mec.line = line;
	mec.mode = mode;
	mec.arg  = arg;
	
	um_print_ccmec(buf, maxlen, &mec);
 	
	return;
}

/*!
 * 	@brief The creaPktCCMSC function is use to create a sleep command, host to modem.
 *	@param buf The buffer containing the created message.
 *	@param maxlen The maximal length of the buffer.
 *	@param src Packet source ID.
 *	@param dest Packet destination ID.
 *	@param arg: -1: sleep until hardware wakeup; 0: force reboot; 1-1524: sleep time in minutes. 
 *	@param cksum Set to 1 means that checksum is calculated and added to the sending packet.
 */

void MicroModem_Messages::creaPktCCMSC(char *buf, int maxlen, int src, int dest, int arg, int cksum)
{
	ccmsc_t msc;
	
	msc.src    = src;
	msc.dest   = dest;
	msc.sd_min = arg;
	
	um_print_ccmsc(buf, maxlen, &msc);
	
	if (cksum) {
		
		calculateCheckSum(buf);
	}
	
	return;
}

/**
 * External hardware control command, local modem only
 * rsig, timeo: obsolete, set to 0
 * txsig: Signal to send
 * cksum Set to 1 means that checksum is calculated and added to the sending packet.
 */

void MicroModem_Messages::creaPktCCRSP(char *buf, int maxlen, int txsig, int cksum)
{
	ccrsp_t rsp;
	
	rsp.rxsig = 0;
	rsp.txsig = txsig;
	rsp.timeo = 0;
	
	um_print_ccrsp(buf, maxlen, &rsp);
	
	if (cksum) {
		
		calculateCheckSum(buf);
	}
	
	return;
}

/**
 * Measure noise level at receiver, host to modem
 * sd_msec: Time to average in milliseconds
 * cksum Set to 1 means that checksum is calculated and added to the sending packet.
 */

void MicroModem_Messages::creaPktCCCFR(char *buf, int maxlen, int sd_msec, int cksum)
{
	cccfr_t cfr;
	
	cfr.sd_msec = sd_msec;
	
	um_print_cccfr(buf, maxlen, &cfr);
	
	if (cksum) {
		
		calculateCheckSum(buf);
	}
	
	return;
}

/**
 * Ping narrowband transponder, host to modem
 * txfreq: transmit freq.
 txl_msec tx ping length (msec)
 rxl_msec: rx ping length (msec)
 timeo: listen timeout (msec)
 freqa: list of four rx freqs. (Hz)
 freqb
 freqc
 freqd
 extsync: set 1 to sync tx to PPS
 cksum Set to 1 means that checksum is calculated and added to the sending packet.
 */

void MicroModem_Messages::creaPktCCPNT(char *buf, int maxlen, int txfreq, int rxl_msec, int txl_msec, int timeo, int freqa, int freqb, int freqc, int freqd, int extsync, int cksum)
{
	ccpnt_t pnt;
	
	pnt.txfreq   = txfreq;
	pnt.rxl_msec = rxl_msec;
	pnt.txl_msec = txl_msec;
	pnt.timeo    = timeo;
	pnt.freqa    = freqa;
	pnt.freqb    = freqb;
	pnt.freqc    = freqc;
	pnt.freqd    = freqd;
	pnt.extsync  = extsync;
	
	um_print_ccpnt(buf, maxlen, &pnt);
	
	if (cksum) {
		
		calculateCheckSum(buf);
	}
	
	return;
}

/**
 * Ping REMUS digital transponder, host to modem
 * group: group ID
 channel: interrogation channel (1-4)
 extsync: set 1 to sync tx to PPS
 pps_timeo :custom PPS timeout
 timeo: ping timeout
 af: set 1 to ID first transponder
 bf
 cf
 df
 cksum Set to 1 means that checksum is calculated and added to the sending packet.
 */

void MicroModem_Messages::creaPktCCPDT(char *buf, int maxlen, int group, int channel, int extsync, int pps_timeo, int timeo, int af, int bf, int cf, int df, int cksum)
{
	ccpdt_t pdt;
	
	pdt.group     = group;
	pdt.channel   = channel;
	pdt.extsync   = extsync;
	pdt.pps_timeo = pps_timeo;
	pdt.timeo     = timeo;
	pdt.af        = af;
	pdt.bf        = bf;
	pdt.cf        = cf;
	pdt.df        = df;
	
	um_print_ccpdt(buf, maxlen, &pdt);
	
	if (cksum) {
		
		calculateCheckSum(buf);
	}
	
	return;
}

/*!
 * 	@brief The NMEAPktType function reads a given buffer and returns type of the NMEA information in it.
 * 	@retval The NMEA type of the information inside the buffer.
 */

int MicroModem_Messages::NMEAPktType(char* buf)
{
	int aux = 0;
	
	Sunset_Debug::debugInfo(5, -1, "MicroModem_Messages::NMEAPktType %s %s", buf, buf+1);
	
	aux = um_hash_nmea(buf+1);
	
	Sunset_Debug::debugInfo(5, -1, "MicroModem_Messages::NMEAPktType aux %d", aux);
	
	return aux;	    
}

/*!
 * 	@brief The sscannmea function reads a given buffer and the type of the NMEA information in it and extracts the useful information.
 *	@param buf The buffer to be read.
 *	@param nmeai The NMEA type information.
 *	@param[out] pkt The information extracted.
 */

int MicroModem_Messages::sscannmea(char *buf, int nmeai, void *pkt)
{
	switch(nmeai)
	{
		case NMEA_CAERR:
		{
			//caerr_t err;
			return um_scan_caerr(buf, (caerr_t*)pkt);
		}
			
		case NMEA_CAMSG:
		{
			//camsg_t msg;
			return um_scan_camsg(buf, (camsg_t*)pkt);
		}
			
		case NMEA_CADBG:
		{
			//cadbg_t dbg;
			return um_scan_cadbg(buf, (cadbg_t*)pkt);
		}
			
		case NMEA_CAREV:
		{
			//carev_t rev;
			return um_scan_carev(buf, (carev_t*)pkt);
		}
			
		case NMEA_CACFG:
		{
			//cacfg_t cfg;
			return um_scan_cacfg(buf, (cacfg_t*)pkt);
		}
			
		case NMEA_CACLK:
		{
			//caclk_t clk;
			return um_scan_caclk(buf, (caclk_t*)pkt);
		}
			
		case NMEA_CACYC:
		{
			//cacyc_t cyc;
			return um_scan_cacyc(buf, (cacyc_t*)pkt);
		}
			
		case NMEA_CADRQ:
		{
			//cadrq_t drq;
			return um_scan_cadrq(buf, (cadrq_t*)pkt);
		}
			
		case NMEA_CATXP:
		{
			//catxp_t txp;
			return um_scan_catxp(buf, (catxp_t*)pkt);
		}
			
		case NMEA_CATXF:
		{
			//catxf_t txf;
			return um_scan_catxf(buf, (catxf_t*)pkt);
		}
			
		case NMEA_CATXD:
		{
			//catxd_t txd;
			return um_scan_catxd(buf, (catxd_t*)pkt);
		}
			
		case NMEA_CATXA:
		{
			//catxa_t txa;
			return um_scan_catxa(buf, (catxa_t*)pkt);
		}
			
		case NMEA_CAXST:
		{
			//caxst_t xst;
			return um_scan_caxst(buf, (caxst_t*)pkt);
		}
			
		case NMEA_CARXP:
		{
			//carxp_t rxp;
			return um_scan_carxp(buf, (carxp_t*)pkt);
		}
			
		case NMEA_CAACK:
		{
			//caack_t ack;
			return um_scan_caack(buf, (caack_t*)pkt);
		}
			
		case NMEA_CARXD:
		{
			//carxd_t rxd;
			return um_scan_carxd(buf, (carxd_t*)pkt);
		}
			
		case NMEA_CARXA:
		{
			//carxa_t rxa;
			return um_scan_carxa(buf, (carxa_t*)pkt);
		}
			
		case NMEA_CADQF:
		{
			//cadqf_t dqf;
			return um_scan_cadqf(buf, (cadqf_t*)pkt);
		}
			
		case NMEA_CAMFD:
		{
			//camfd_t mfd;
			return um_scan_camfd(buf, (camfd_t*)pkt);
		}
			
		case NMEA_CASHF:
		{
			//cashf_t shf;
			return um_scan_cashf(buf, (cashf_t*)pkt);
		}
			
		case NMEA_CASNR:
		{
			//casnr_t snr;
			return um_scan_casnr(buf, (casnr_t*)pkt);
		}
			
		case NMEA_CADOP:
		{
			//cadop_t dop;
			return um_scan_cadop(buf, (cadop_t*)pkt);
		}
			
		case NMEA_CACST:
		{
			//cacst_t cst;
			return um_scan_cacst(buf, (cacst_t*)pkt);
		}
			
		case NMEA_CAMUC:
		{
			//camuc_t muc;
			return um_scan_camuc(buf, (camuc_t*)pkt);
		}
			
		case NMEA_CAMUA:
		{
			//camua_t mua;
			return um_scan_camua(buf, (camua_t*)pkt);
		}
			
		case NMEA_CAMUR:
		{
			//camur_t mur;
			return um_scan_camur(buf, (camur_t*)pkt);
		}
			
		case NMEA_CAMPC:
		{
			//campc_t mpc;
			return um_scan_campc(buf, (campc_t*)pkt);
		}
			
		case NMEA_CAMPA:
		{
			//campa_t mpa;
			return um_scan_campa(buf, (campa_t*)pkt);
		}
			
		case NMEA_CAMPR:
		{
			//campr_t mpr;
			return um_scan_campr(buf, (campr_t*)pkt);
		}
			
		case NMEA_CAMEC:
		{
			//camec_t mec;
			return um_scan_camec(buf, (camec_t*)pkt);
		}
			
		case NMEA_CAMEA:
		{
			//camea_t mea;
			return um_scan_camea(buf, (camea_t*)pkt);
		}
			
		case NMEA_CAMER:
		{
			//camer_t mer;
			return um_scan_camer(buf, (camer_t*)pkt);
		}
			
		case NMEA_CAMSC:
		{
			//camsc_t msc;
			return um_scan_camsc(buf, (camsc_t*)pkt);
		}
			
		case NMEA_CAMSA:
		{
			//camsa_t msa;
			return um_scan_camsa(buf, (camsa_t*)pkt);
		}
			
		case NMEA_CAMSR:
		{
			//camsr_t msr;
			return um_scan_camsr(buf, (camsr_t*)pkt);
		}
			
		case NMEA_SNPNT:
		{
			//snpnt_t snpnt;
			return um_scan_snpnt(buf, (snpnt_t*)pkt);
		}
			
		case NMEA_SNMFD:
		{
			//snmfd_t snmfd;
			return um_scan_snmfd(buf, (snmfd_t*)pkt);
		}
			
		case NMEA_SNTTA:
		{
			//sntta_t sntta;
			return um_scan_sntta(buf, (sntta_t*)pkt);
		}
			
		case NMEA_SNCFR:
		{
			//sncfr_t sncfr;
			return um_scan_sncfr(buf, (sncfr_t*)pkt);
		}
			
		case NMEA_CABBD:
		{
			//cabbd_t cabbd;
			return um_scan_cabbd(buf, (cabbd_t*)pkt);
		}
			
		case NMEA_GPGGA:
		{
			//gpgga_t gpgga;
			return um_scan_gpgga(buf, (gpgga_t*)pkt);
		}
	}
	
	return 0;
}

