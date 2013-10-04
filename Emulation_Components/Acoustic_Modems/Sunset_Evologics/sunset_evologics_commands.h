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


#define ATLn		"AT!L"		/* Signal transmission source level during data exchange.
					   ”0” - maximum transmission power.
					   ”1” - max. level -6dB;
					   ”2” - max. level -12dB;
					   ”3” - max. level -20dB; */
#define ATCn		"AT!C"		/* Carrier Waveform ID of the local device [0-3]*/
#define ATALn 		"AT!AL"		/* Local address of the device [1-14]*/

#define ATGn 		"AT!G"		/* Input amplifier gain. Value ”0” corresponds
					   to high sensitivity, value ”1” corresponds to
					   reduced sensitivity and is recommended for
					   short distances or for testing purposes [0-1] */

#define ATRCn 		"AT!RC"		/* Number of connection establishment retries [1-50]*/

#define ATRTn 		"AT!RT"		/* Device’s response timeout (ms) [500-12000]*/

#define ATZPn 		"AT!ZP"		/* Maximum data packet duration (ms). Must
					   have the same value on all devices. [200-5000]*/
#define ATZLn 		"AT!ZL"		/* Maximum number of bytes in outgoing buffer[1-1048576] */
#define ATZIn 		"AT!ZI"		/* Idle Timeout (s). [0-3600]*/
#define ATZHn 		"AT!ZH"		/* Hold Timeout [0-3600]*/
#define ATZD 		"AT!ZD"		/* Reset Drop Counter to zero. Dropped bytes
					   count due to Idle Timeout expiration */
#define ATZO 		"AT!ZO"		/* Reset Overflow Counter to zero. Lost bytes
					   count due to buffer overflow */
#define ATRI 		"AT!RI"		/* Number of instant message delivery retries [1-255]*/
#define ATRP 		"AT!RP"		/* Promiscuous mode ON/OFF switch for foreign
					   instant messages overhearing [0-1]*/

// SEND DEFINES

#define AT_SEND 	"AT*SEND"		/* Send point to point data */
#define AT_SENDIM	"AT*SENDIM"		/* Send instant message */
#define AT_SENDIMS	"AT*SENDIMS"		/* Send synchronous instant message */

// MISC

#define ATH0		"ATH0"			/* ATH0 command closes an established acoustic connection between
									    devices by sending a special message to the remote device. */
#define ATH1		"ATH1" 			/* command aborts a connection without remote device notification. */

// NET

#define ATA		"ATA"			/* listen */
#define ATDn		"ATD"			/* establishes an acoustic connection with a remote modem with a
									    specified address (1..14) n */
									   
#define ATDI		"AT?DI"			/* Instant message delivery status */
									   
#define ATT		"AT?T"			/* Signal propagation delay (microseconds) */

#define AT_MULTI_PATH		"AT?P"			/* Collect multipath information */

#define ATS		"ATS"		/* enable the DEAF mode */



