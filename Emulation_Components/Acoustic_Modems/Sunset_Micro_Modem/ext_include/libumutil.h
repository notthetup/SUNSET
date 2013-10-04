/*!
 * \file libumutil.h
 * \author jon shusta <jshusta@whoi.edu>
 * \date 2009.09
 * \brief a library of utilities for supporting the WHOI micro-modem.
 */

#ifndef LIBUMUTIL_H
#define LIBUMUTIL_H

#include <stdlib.h>
#include <string.h>
#include <time.h>

/* hash table sizes
 */
#define NMEAMPHSZ    (70)   /*!< number of nmea sentence keys */
#define UMCFMPHSZ    (56)   /*!< number of config setting keys */
#define RVIDMPHSZ     (4)   /*!< number of carev ident keys */


/* buffer sizes
 */
#define UMMAXCFSZ   (0x0004)  /*!< max config key size, also other very short strings */
#define UMMAXSKCT   (0x0008)  /*!< max number of data fragments */
#define UMMAXMKSZ   (0x0010)  /*!< max mini frame size (encoded), plus null */
#define UMMAXSKSZ   (0x0201)  /*!< max data fragment size (encoded), plus null */
#define UMMAXFRSZ   (0x1601)  /*!< max data frame size (encoded), plus null */
#define UMMAXMSSZ   (0x1620)  /*!< max nmea sentence size */
#define UMMAXBBSZ  (0x80000)  /*!< max baseband data string size (big) */
#define UMMAXVRSZ   (0x0010)  /*!< max CAREV version size */
#define UMMAXIDSZ   (0x0010)  /*!< max CAREV ident size */
#define UMMAXSZSZ   (0x0040)  /*!< max string data size */


/* modem cycle rates
 */
#define TXR_80_FSK       (0)  /*!< 80bps   FH-FSK */
#define TXR_250_SPRD     (1)  /*!< 250bps  PSK: spreading */
#define TXR_500_SPRD     (2)  /*!< 500bps  */
#define TXR_1200_SPRD    (3)  /*!< 1200bps */
#define TXR_1300_RBC     (4)  /*!< 1300bps PSK: rate block code */
#define TXR_5300_RBC     (5)  /*!< 5300bps */
#define TXR_NO_NAME_YET  (6)  /*!< super secret */
#define TXR_MAXIMUM      (6)  /*!< max rate */


/* RXP type
 */
#define RXP_PKT_FSK    (0)
#define RXP_PKT_PSK    (1)


/* CACST field values
 */
#define CST_MODE_GOOD      (0)  /*!< cacst::mode good packet */
#define CST_MODE_BAD_CRC   (1)  /*!< cacst::mode bad crc */
#define CST_MODE_TIMEOUT   (2)  /*!< cacst::mode timeout */
#define CST_PSK_NONE       (0)  /*!< cacst::pskerr successful PSK packet */
#define CST_PSK_EMOD       (1)  /*!< cacst::pskerr bad modulation header */
#define CST_PSK_ECRCH      (2)  /*!< cacst::pskerr bad crc in data header */
#define CST_PSK_ECRCF      (3)  /*!< cacst::pskerr bad crc in at least one frame */
#define CST_PSK_ECOPROC    (4)  /*!< cacst::pskerr lost connection to coproc */
#define CST_PSK_EEQLZR     (5)  /*!< cacst::pskerr equalizer timeout */
#define CST_PSK_EMISS      (6)  /*!< cacst::pskerr missed start of PSK packet */


/* XST/CST packet type ID
 */
#define XST_PKT_UNKNOWN   (-1)  /*!< unknown packet header */
#define XST_PKT_FSK        (1)  /*!< FSK data */
#define XST_PKT_FSK_MINI   (2)  /*!< FSK mini-packet */
#define XST_PKT_PSK        (3)  /*!< PSK data */
#define XST_PKT_PSK_MINI   (4)  /*!< PSK mini-packet */


/* CCMEC options
 */
#define MEC_LINE_EXTSEL1  (1)  /*!< */
#define MEC_LINE_EXTSEL2  (2)  /*!< */
#define MEC_LINE_GPIO1    (3)  /*!< daughter board required */
#define MEC_LINE_GPIO2    (4)  /*!< */
#define MEC_LINE_GPIO3    (5)  /*!< */
#define MEC_LINE_GPIO4    (6)  /*!< */
#define MEC_LINE_COM1     (7)  /*!< read CTS, write RTS */
#define MEC_LINE_COM2     (8)  /*!< read CTS, write RTS */
#define MEC_MODE_READ     (0)  /*!< read. arg=0 */
#define MEC_MODE_SET      (1)  /*!< set. arg=0 for set, arg=1 for assert */
#define MEC_MODE_HI       (2)  /*!< toggle high, arg=timeo */
#define MEC_MODE_LO       (3)  /*!< toggle low, arg=timeo */
#define MEC_TIMEO_1MS     (0)  /*!< toggle 1 msec */
#define MEC_TIMEO_500MS   (1)  /*!< toggle 500 msec */
#define MEC_TIMEO_1S      (2)  /*!< toggle 1 second */
#define MEC_TIMEO_2S      (3)  /*!< toggle 2 seconds */
#define MEC_TIMEO_6S      (4)  /*!< */
#define MEC_TIMEO_10S     (5)  /*!< */
#define MEC_TIMEO_20S     (6)  /*!< */
#define MEC_TIMEO_30S     (7)  /*!< */


/* CCMSC options
 */
#define MSC_SD_WAKE    (-1)  /*!< ccmsc::sd_min sleep until hardware wake */
#define MSC_SD_REBOOT   (0)  /*!< ccmsc::sd_min force reboot now */


/* CCEXL mode bitflags
 */
#define EXL_MODE_EXTSEL1  (0x01)  /*!< 0 low, 1 high */
#define EXL_MODE_EXTSEL2  (0x02)  /*!< 0 low, 1 high */
#define EXL_MODE_NOOP_3   (0x04)  /*!< */
#define EXL_MODE_NOOP_4   (0x08)  /*!< */
#define EXL_MODE_NOOP_5   (0x10)  /*!< */
#define EXL_MODE_GPO      (0x20)  /*!< 0 low, 1 high */
#define EXL_MODE_NOOP_7   (0x40)  /*!< */
#define EXL_MODE_NOOP_8   (0x80)  /*!< */


/* CCRSP options
 */
#define RSP_TX_FSK     (1)    /*!< ccrsp::txsig use FSK */
#define RSP_TX_PSK     (2)    /*!< ccrsp::txsig use PSK */


/* GPRMC figures
 */
#define RMC_NRW_OK     ('A')    /*!< nav recv warning: OK */
#define RMC_NRW_BAD    ('V')    /!*< nav recv warning: warning! */


/* sprintf strings *************************************************************
 */
#define MSG_CCCFG  "$CCCFG,%s,%d"              /*!< setting, value */
#define MSG_CCCFQ  "$CCCFQ,%s"                 /*!< setting */
#define MSG_CCCLQ  "$CCCLQ,0"                  /*!< */
#define MSG_CCCLK  "$CCCLK,%d,%d,%d,%d,%d,%d"  /*!< yyyy, mm, dd, hh, ii, ss */
#define MSG_CCCYC  "$CCCYC,%d,%d,%d,%d,%d,%d"  /*!< cmd, src, dest, rate, flag, npkt */
#define MSG_CCTXD  "$CCTXD,%d,%d,%d,%s"        /*!< src, dest, ack, hexdata */
#define MSG_CCTXA  "$CCTXA,%d,%d,%d,%s"        /*!< src, dest, ack, ascdata */
#define MSG_CCMUC  "$CCMUC,%d,%d,%s"           /*!< src, dest, 13-bit hexdata */
#define MSG_CCMPC  "$CCMPC,%d,%d"              /*!< src, dest */
#define MSG_CCMEC  "$CCMEC,%d,%d,%d,%d,%d"     /*!< src, dest, line, mode, arg */
#define MSG_CCMSC  "$CCMSC,%d,%d,%d"           /*!< src, dest, sleep-duration (min) */
#define MSG_CCEXL  "$CCEXL,%s"                 /*!< 8 mode bits hexdata */
#define MSG_CCRSP  "$CCRSP,0,%d,0"             /*!< sweep type: 1 for FSK, 2 for PSK. */
#define MSG_CCCFR  "$CCCFR,%d"                 /*!< sample duration (msec) */
/*!
 * tx-freq, tx-len-msec, rx-len-msec, timeo, freq-a, freq-b, freq-c, freq-d, now-flag
 */
#define MSG_CCPNT  "$CCPNT,%d,%d,%d,%d,%d,%d,%d,%d,%d"
/*!
 * group, channel, sync-flag, sync-timeo, timeo, af, bf, cf, df
 */
#define MSG_CCPDT  "$CCPDT,%d,%d,%d,%d,%d,%d,%d,%d,%d"
#define MSG_CCFFL
#define MSG_CCFST
#define MSG_CCBBD


/* sscanf strings **************************************************************
 */
#define MSG_CAERR  "$CAERR,%2d%2d%2d,%[^,],%d,%[^*]"  /*!< hhmmss, module, number, message */
#define MSG_CAMSG  "$CAMSG,%[^,],%d"               /*!< type, value */
#define MSG_CADBG  "$CADBG,%[^*]"                  /*!< debug message */
#define MSG_CAREV  "$CAREV,%2d%2d%2d,%[^,],%[^*]"  /*!< hhmmss ident, rev */
#define MSG_CACFG  "$CACFG,%3s,%d"                 /*!< setting, value */
#define MSG_CACLK  "$CACLK,%d,%d,%d,%d,%d,%d"      /*!< yyyy, mm, dd, hh, ii, ss */

#define MSG_CACYC  "$CACYC,%d,%d,%d,%d,%d,%d"         /*!< cmd, src, dest, rate, ack, npkt */
#define MSG_CADRQ  "$CADRQ,%2d%2d%2d,%d,%d,%d,%d,%d"  /*!< hhmmss, src, dest, ack, bytes, fc */
#define MSG_CATXP  "$CATXP,%d"               /*!< bytes */
#define MSG_CATXF  "$CATXF,%d"               /*!< bytes */
#define MSG_CATXD  "$CATXD,%d,%d,%d,%d"      /*!< src, dest, ack, bytes */
#define MSG_CATXA  "$CATXA,%d,%d,%d,%d"      /*!< src, dest, ack, bytes */
/*! CAXST message.
 * check the software interface guide for a description.
 */
#define MSG_CAXST  "$CAXST,%4d%2d%2d,%2d%2d%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"

#define MSG_CARXP  "$CARXP,%d"                 /*!< packet type */
#define MSG_CAACK  "$CAACK,%d,%d,%d,%d"        /*!< src, dest, idx, ack(1) */
#define MSG_CARXD  "$CARXD,%d,%d,%d,%d,%[^*]"  /*!< src, dest, ack, fc, hexdata */
#define MSG_CARXA  "$CARXA,%d,%d,%d,%d,%[^*]"  /*!< src, dest, ack, fc, ascdata */
#define MSG_CADQF  "$CADQF,%d,%d"              /*!< dqf, type */
#define MSG_CAMFD  "$CAMFD,%d,%d,%d,%d"        /*!< peak, power, rssi, spl */
#define MSG_CAMSE
#define MSG_CASHF  "$CASHF,%d,%d,%d,%d,%d"     /*!< agn, ain-pshift, ain-shift, mfd-shift, p2b-shift */
#define MSG_CASNR  "$CASNR,%d,%d,%d,%d,%d"     /*!< rss, snr in, snr out, symbol snr, noise level */
#define MSG_CADOP  "$CADOP,%f"                 /*!< speed (m/s) */
#define MSG_CATOA
#define MSG_CAIRE
/*!
 * check the software interface guide for a description.
 * example: $CACST,1,153026.0000,0,4105,30,0318,0193,10,00,00,01,01,2,004,004,3,3,3,1,154,09,08,10,-6,000,0.0,4125000,5000*5B
 */
#define MSG_CACST  "$CACST,%d,%2d%2d%f,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f,%d,%d"

#define MSG_CAMUC  "$CAMUC,%d,%d,%[^*]"     /*!< src, dest, 13-bit hexdata */
#define MSG_CAMUA  "$CAMUA,%d,%d,%[^*]"     /*!< src, dest, 13-bit hexdata */
#define MSG_CAMUR  "$CAMUR,%d,%d"           /*!< src, dest */
#define MSG_CAMPC  "$CAMPC,%d,%d"           /*!< src, dest */
#define MSG_CAMPA  "$CAMPA,%d,%d"           /*!< src, dest */
#define MSG_CAMPR  "$CAMPR,%d,%d,%f"        /*!< src, dest, traveltime (sec) */
#define MSG_CAMEC  "$CAMEC,%d,%d,%d,%d,%d"  /*!< src, dest, line, mode */
#define MSG_CAMEA  "$CAMEA,%d,%d,%d,%d,%d"  /*!< src, dest, line, mode */
#define MSG_CAMER  "$CAMER,%d,%d,%d,%d,%d"  /*!< src, dest, line, mode */
#define MSG_CAMSC  "$CAMSC,%d,%d,%d"        /*!< src, dest, arg */
#define MSG_CAMSA  "$CAMSA,%d,%d,%d"        /*!< src, dest, arg */
#define MSG_CAMSR  "$CAMSR,%d,%d,%d"        /*!< src, dest, arg */
#define MSG_CARSP  "$CARSP,%d,%d,%d"        /*!< ignored, sweep type, ignored */
/*!
 * hhmmss, tx-freq, tx-len-msec, rx-len-msec, timeo, freq-a, freq-b, freq-c, freq-d, unknown, unknown
 */
#define MSG_SNPNT  "$SNPNT,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"
#define MSG_SNMFD  "$SNMFD,%d,%d,%d,%d"                    /*!< channel, peak, power, rssi */
#define MSG_SNTTA  "$SNTTA,%f,%f,%f,%f,%2d%2d%f"           /*!< tta, ttb, ttc, ttd, hhmmss.ss */
#define MSG_SNCFR  "$SNCFR,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d"  /*!< 14 voltage estimates */
#define MSG_CAFFL
#define MSG_CAFST
#define MSG_CABBD  "$CABBD,%d,%s"    /*!< nwords, hexdata */


/* forwarded GPS messages ******************************************************
 */
#define MSG_PGRME
#define MSG_PGRMM
#define MSG_PGRMT
#define MSG_PGRMZ
#define MSG_GPRMB
/*!
 * example: $GPRMC,150853,A,4132.1365,N,07040.3828,W,000.0,077.3,090909,015.5*0B
 */
#define MSG_GPRMC  "$GPRMC,%2d%2d%2d,%c,%2d%f,%[NS],%3d%f,%[WE],%f,%f,%2d%2d%2d,%f"
#define MSG_GPGSV
#define MSG_GPGSA
/*!
 * hhmmsss, lat degree, min frac, cardinal, lon degree, min frac, cardinal
 * example: $GPGGA,150853,4132.1365,N,07040.3828,W,2,11,0.8,6.7,M,-33.6,M,*50
 */
#define MSG_GPGGA  "$GPGGA,%2d%2d%2d,%2d%f,%[NS],%3d%f,%[WE]"


/* miscellaneous ***************************************************************
 */
#define UMCFRVEST (14)  /*!< number of voltage estimates in SNCFR */


/* macro wrappers for sprintf/sscanf with message strings **********************
 */
#define nmeasprintf(buf, len, msg, ...)  (snprintf(buf, len, MSG_##msg "\r\n", ## __VA_ARGS__))
#define nmeasscanf(cs, buf, msg, ...)    (sscanf(buf, MSG_##msg "*%x\r\n", ## __VA_ARGS__, cs))


/** typedefs ******************************************************************/


/*! macro for defining forward declarations of all storage types
 * not all of these types are complete and/or still in use
 */
#define UM_TYPEDEF(nmea) typedef struct nmea nmea##_t

UM_TYPEDEF(cccfg);
UM_TYPEDEF(cccfq);
UM_TYPEDEF(ccclq);
UM_TYPEDEF(ccclk);
UM_TYPEDEF(cccyc);
UM_TYPEDEF(cctxd);
UM_TYPEDEF(cctxa);
UM_TYPEDEF(ccmuc);
UM_TYPEDEF(ccmpc);
UM_TYPEDEF(ccmec);
UM_TYPEDEF(ccmsc);
UM_TYPEDEF(ccexl);
UM_TYPEDEF(ccrsp);
UM_TYPEDEF(cccfr);
UM_TYPEDEF(ccbbd);
UM_TYPEDEF(ccpnt);
UM_TYPEDEF(ccpdt);
UM_TYPEDEF(ccffl);
UM_TYPEDEF(ccfst);

/*!
 */
struct cccfg
{
  char  key[UMMAXCFSZ];  /*!< config setting */
  int   val;             /*!< config value */
};

/*!
 */
struct cccfq
{
  char  key[UMMAXCFSZ];  /*!< config setting */
};

/*!
 */
struct ccclq
{
  int x;  /*!< nothing */
};

/*!
 */
struct ccclk
{
  struct tm ts; /*!< timestamp */
};

/*!
 */
struct cccyc
{
  int cmd,   /*!< */
      src,   /*!< */
      dest,  /*!< */
      rate,  /*!< see TXR_* constants */
      flag,  /*!< drq flag */
      skct;  /*!< frame count */
};

/*!
 */
struct cctxd
{
  int     src,   /*!< */
          dest,  /*!< */
          ack;   /*!< ack flag */
  char    buf[UMMAXSKSZ]; /*!< hexdata */
  size_t  len;   /*!< len */
};

/*!
 */
struct cctxa
{
  int     src,   /*!< */
          dest,  /*!< */
          ack;   /*!< ack flag */
  char    buf[UMMAXSKSZ]; /*!< ascdata */
  size_t  len;   /*!< len */
};

/*!
 */
struct ccmuc
{
  int   src,  /*!< */
        dest; /*!< */
  char  buf[UMMAXSZSZ]; /*!< 13 bit hexdata (0x0 to 0x1fff) */
};

/*!
 */
struct ccmpc
{
  int  src,  /*!< */
       dest; /*!< */
};

/*!
 */
struct ccmec
{
  int     src,   /*!< */
          dest,  /*!< */
          line,  /*!< hardware line (MEC_LINE_*) */
          mode,  /*!< op mode (MEC_MODE_*)  */
          arg;   /*!< set/assert, timeout for toggle (MEC_TIMEO_*) */
};

/*!
 */
struct ccmsc
{
  int  src,     /*!< */
       dest,    /*!< */
       sd_min;  /*!< sleep duration in minutes (MSC_ARG_*) */
};

/*!
 */
struct ccexl
{
  char mode[UMMAXCFSZ];  /*!< 8 mode bits in hexdata */
};

/*!
 */
struct ccrsp
{
  int  rxsig,  /*!< obsolete */
       txsig,  /*!< transmit signal */
       timeo;  /*!< obsolete */
};

/*!
 */
struct cccfr
{
  int sd_msec;  /*!< sample duration (msec) */
};


struct ccbbd
{
  char unimplemented;
};


/*!
 */
struct ccpnt
{
  int  txfreq,    /*!< transmit freq.*/
       txl_msec,  /*!< tx ping length (msec)*/
       rxl_msec,  /*!< rx ping length (msec) */
       timeo,     /*!< listen timeout (msec) */
       freqa,     /*!< list of four rx freqs. (Hz) */
       freqb,     /*!< */
       freqc,     /*!< */
       freqd,     /*!< */
       extsync;   /*!< set 1 to sync tx to PPS */
};

/*!
 */
struct ccpdt
{
  int  group,      /*!< group ID */
       channel,    /*!< interrogation channel (1-4) */
       extsync,    /*!< set 1 to sync tx to PPS */
       pps_timeo,  /*!< custom PPS timeout */
       timeo,      /*!< ping timeout */
       af,         /*!< set 1 to ID first transponder */
       bf,         /*!< */
       cf,         /*!< */
       df;         /*!< */
};


struct ccffl
{
  char unimplemented;
};


struct ccfst
{
  char unimplemented;
};


UM_TYPEDEF(caerr);
UM_TYPEDEF(camsg);
UM_TYPEDEF(cadbg);
UM_TYPEDEF(carev);
UM_TYPEDEF(cacfg);
UM_TYPEDEF(caclk);

/*!
 */
struct caerr
{
  char       mod[UMMAXSZSZ],  /*!< name of module error came from */
             msg[UMMAXSZSZ];  /*!< error message */
  int        errno;           /*!< error number */
  struct tm  ts;              /*!< timestamp: hhmmss */
};


/*!
 */
struct camsg
{
  char  type[UMMAXSZSZ];  /*!< message type */
  int   val;              /*!< message value (context-specific) */
};


/*!
 */
struct cadbg
{
  char msg[UMMAXSZSZ];  /*!< message */
};


/*!
 */
struct carev
{
  char ident[UMMAXIDSZ],  /*!< */
       vers[UMMAXVRSZ];   /*!< */
  struct tm ts;           /*!< */
};


/*!
 */
struct cacfg
{
  char  key[UMMAXCFSZ];  /*!< config setting */
  int   val;             /*!< config value */
};


/*!
 */
struct caclk
{
  struct tm ts;
};


UM_TYPEDEF(cacyc);
UM_TYPEDEF(cadrq);
UM_TYPEDEF(catxd);
UM_TYPEDEF(catxa);
UM_TYPEDEF(catxp);
UM_TYPEDEF(catxf);
UM_TYPEDEF(caxst);

/*!
 */
struct cacyc
{
  int  cmd,
       src,
       dest,
       rate,
       flag,
       skct;
};


/*!
 */
struct cadrq
{
  int  src,      /*!< source */
       dest,     /*!< destination */
       flag,     /*!< cycle flag */
       bytes,    /*!< bytes requested */
       idx;      /*!< frame number */
  struct tm ts;  /*!< request time HHMMSS */
};


/*!
 */
struct catxp
{
  int  bytes;
};


/*!
 */
struct catxf
{
  int  bytes;
};


/*!
 */
struct catxd
{
  int  src,
       dest,
       ack,
       bytes;
};


/*!
 */
struct catxa
{
  int  src,
       dest,
       ack,
       bytes;
};


/*! packet transmit statistics.
 */
struct caxst
{
  int  result,    /*!< field 4 */
       probelen,  /*!< */
       bandw,     /*!< */
       carrier,   /*!< */
       rate,      /*!< */
       src,       /*!< */
       dest,      /*!< */
       ack,       /*!< */
       nfr_try,   /*!< */
       nfr_out,   /*!< */
       pktype,    /*!< packet type and size (CST_PKT_*) */
       txbytes;   /*!< */
  struct tm tot;  /*!< time of transmit */
};


UM_TYPEDEF(carxp);
UM_TYPEDEF(caack);
UM_TYPEDEF(carxd);
UM_TYPEDEF(carxa);
UM_TYPEDEF(cadqf);
UM_TYPEDEF(camfd);
UM_TYPEDEF(camse);
UM_TYPEDEF(cashf);
UM_TYPEDEF(casnr);
UM_TYPEDEF(cadop);
UM_TYPEDEF(catoa);
UM_TYPEDEF(caire);
UM_TYPEDEF(cacst);


/*! \brief packet detection. see CARXP in manual.
 */
struct carxp
{
  int  pktype;  /*!< 0 for FSK, 1 for PSK */
};


struct caack
{
  int  src,
       dest,
       idx,  /*!< index of frame being acked */
       ack;  /*!< ack bit (always 1) */
};


/*! \brief hex data frame received.
 */
struct carxd
{
  char  buf[UMMAXSKSZ];  /*!< data buffer */
  int   src,   /*!< */
        dest,  /*!< */
        ack,   /*!< ack request bit */
        len,   /*!< buffer length */
        fc;    /*!< frame number */
};


/*! \brief ascii data frame received.
 */
struct carxa
{
  char  buf[UMMAXSKSZ];  /*!< data buffer */
  int   src,   /*!< */
        dest,  /*!< */
        ack,   /*!< ack request bit */
        len,   /*!< buffer length */
        fc;    /*!< frame number */
};


/*! \brief data quality for FSK packets.
 */
struct cadqf
{
  int  quality,  /*!< quality factor (0-255) */
       pktype;   /*!< FSK data or mini-packet (DQF_PKT_*) */
};


/*! \brief comms-matched filter information.
 */
struct camfd
{
  int  peak,   /*!< peak value */
       power,  /*!< broadband power at peak */
       rssi,   /*!< peak/power ratio */
       spl;    /*!< sound pressure level */
};


struct camse
{
  char deprecated;
};


/*! \brief shift statistics.
 */
struct cashf
{
  int  gain, /*!< value of AGN setting */
       prv,  /*!< previous input buffer */
       cur,  /*!< current input buffer */
       mfd,  /*!< basebanded buffer (w/detect) */
       p2b;  /*!< measured during basebanding incoming data */
};


/*! \brief signal to noise ratio.
 */
struct casnr
{
  int  rss,    /*!< received signal strength */
       in,     /*!< input SNR */
       out,    /*!< equalizer output SNR */
       sym,    /*!< symbol SNR for spread-spectrum packets */
       level;  /*!< noise level from channel 1 */
};


/*!
 */
struct cadop
{
  float speed;  /*!< relative speed in m/s */
};


/*!
 */
struct catoa
{
  char deprecated;
};


/*!
 */
struct caire
{
  char deprecated;
};


/*! \brief packet receive statistics.
 */
struct cacst
{
  int      mode,      /*!< field 1 */
           clk_stat,  /*!< clock status */
           rate,      /*!< */
           src,       /*!< */
           dest,      /*!< */
           result,    /*!< field 16 */
           pktype,    /*!< packet type and size (CST_PKT_*) */
           nframes,   /*!< number of frames */
           nfailed;   /*!< number of bad_crc's */
  camfd_t  camfd;     /*!< CAMFD */
  cashf_t  cashf;     /*!< CASHF */
  casnr_t  casnr;     /*!< CASNR */
  float    stddev,    /*!< noise stddev */
           mse;       /*!< mean square error from equalizer */
  int      dqf,       /*!< FSK data quality factor */
           dop;       /*!< doppler shift (meters/sec) */
  struct tm  toa;     /*!< time of arrival */
};


UM_TYPEDEF(camuc);
UM_TYPEDEF(camua);
UM_TYPEDEF(camur);
UM_TYPEDEF(campc);
UM_TYPEDEF(campa);
UM_TYPEDEF(campr);
UM_TYPEDEF(camec);
UM_TYPEDEF(camea);
UM_TYPEDEF(camer);
UM_TYPEDEF(camsc);
UM_TYPEDEF(camsa);
UM_TYPEDEF(camsr);
UM_TYPEDEF(carsp);


struct camuc
{
  int     src,
          dest;
  char    buf[UMMAXMKSZ];
  size_t  len;
};


struct camua
{
  int     src,
          dest;
  char    buf[UMMAXMKSZ];
  size_t  len;
};


struct camur
{
  int  src,
       dest;
};


struct campc
{
  int  src,
       dest;
};


/*! \brief ping received.
 */
struct campa
{
  int  src,
       dest;
};


/*! \brief ping reply received.
 */
struct campr
{
  int    src,
         dest;
  float  htt;  /*!< one-way (half) travel time measured by modem (sec) */
};


struct camec
{
  int     src,   /*!< */
          dest,  /*!< */
          line,  /*!< hardware line (MEC_LINE_*) */
          mode,  /*!< op mode (MEC_MODE_*)  */
          arg;   /*!< set/assert, timeout for toggle (MEC_TIMEO_*) */
};


struct camea
{
  int     src,   /*!< */
          dest,  /*!< */
          line,  /*!< hardware line (MEC_LINE_*) */
          mode,  /*!< op mode (MEC_MODE_*)  */
          arg;   /*!< set/assert, timeout for toggle (MEC_TIMEO_*) */
};


struct camer
{
  int     src,   /*!< */
          dest,  /*!< */
          line,  /*!< hardware line (MEC_LINE_*) */
          mode,  /*!< op mode (MEC_MODE_*)  */
          arg;   /*!< set/assert, timeout for toggle (MEC_TIMEO_*) */
};


struct camsc
{
  int  src,     /*!< */
       dest,    /*!< */
       sd_min;  /*!< sleep duration in minutes (MSC_ARG_*) */
};


struct camsa
{
  int  src,     /*!< */
       dest,    /*!< */
       sd_min;  /*!< sleep duration in minutes (MSC_ARG_*) */
};


struct camsr
{
  int  src,     /*!< */
       dest,    /*!< */
       sd_min;  /*!< sleep duration in minutes (MSC_ARG_*) */
};


struct carsp
{
  int  rxsig,  /*!< obsolete */
       txsig,  /*!< transmit signal */
       timeo;  /*!< obsolete */
};


UM_TYPEDEF(snpnt);
UM_TYPEDEF(snmfd);
UM_TYPEDEF(sntta);
UM_TYPEDEF(sncfr);
UM_TYPEDEF(caffl);
UM_TYPEDEF(cafst);
UM_TYPEDEF(cabbd);


struct snpnt
{
  char undocumented;
};


struct snmfd
{
  int  channel,
       peak,
       power,
       rssi;
};


struct sntta
{
  float  tta,
         ttb,
         ttc,
         ttd;
  struct tm ts;
};


/*! \brief noise estimate.
 */ 
struct sncfr
{
  int v[UMCFRVEST];  /*!< voltage estimates */
};

struct caffl
{
};

struct cafst
{};

struct cabbd
{
  char    buf[UMMAXBBSZ];
  int     nwords;
  size_t  len;
};

UM_TYPEDEF(pgrme);
UM_TYPEDEF(pgrmm);
UM_TYPEDEF(pgrmt);
UM_TYPEDEF(pgrmz);


UM_TYPEDEF(gprmb);
UM_TYPEDEF(gprmc);
UM_TYPEDEF(gpgga);
UM_TYPEDEF(gpgsv);
UM_TYPEDEF(gpgsa);


/*! \brief gps coordinate with datetime.
 */
struct gprmc
{
  int   latd,  /*!< lat degree (+N, -S) */
        lond;  /*!< lon degree (+E, -W) */
  float latm,  /*!< latitude minute */
        lonm;  /*!< longitude minute */
  struct tm ts;
};


/*! \brief gps coordinate, time of day, geoid stuff.
 */
struct gpgga
{
  int   latd,  /*!< lat degree (+N, -S) */
        lond;  /*!< lon degree (+E, -W) */
  float latm,  /*!< latitude minute */
        lonm;  /*!< longitude minute */
  struct tm  ts;   /*!< sample time HHMMSS */
};


struct gpgsv
{};

struct gpgsa
{};


/** hex.c *********************************************************************/

size_t um_hexencode(char *        ret,
                    const size_t  maxlen,
                    const char *  buf,
                    const size_t  len);

size_t um_hexdecode(char *        ret,
                    const size_t  maxlen,
                    const char *  buf,
                    const size_t  len);


/** hash.c ********************************************************************/

int um_hash_nmea(const char *buf);
int um_hash_conf(const char *buf);
int um_hash_rvid(const char *buf, const size_t len);


/** conv.c ********************************************************************/

int um_tstotm(struct tm *ts, int hh, int mm, int ss);
int um_dttotm(struct tm *ts, int yy, int mm, int dd, int hh, int ii, int ss);
int um_cdtosign(int *deg, char cd[2]);


/** string.c ******************************************************************/

#define UM_DECL_PRINT(nmea) int um_print_##nmea (char *buf, const size_t maxlen, const nmea##_t *nmea)

UM_DECL_PRINT(cccfg);
UM_DECL_PRINT(cccfq);
UM_DECL_PRINT(ccclq);
UM_DECL_PRINT(ccclk);
UM_DECL_PRINT(cccyc);
UM_DECL_PRINT(cctxd);
UM_DECL_PRINT(cctxa);
UM_DECL_PRINT(ccmuc);
UM_DECL_PRINT(ccmpc);
UM_DECL_PRINT(ccmec);
UM_DECL_PRINT(ccmsc);
UM_DECL_PRINT(ccexl);
UM_DECL_PRINT(ccrsp);
UM_DECL_PRINT(cccfr);
UM_DECL_PRINT(ccbbd);
UM_DECL_PRINT(ccpnt);
UM_DECL_PRINT(ccpdt);
UM_DECL_PRINT(ccffl);
UM_DECL_PRINT(ccfst);


/** scan.c ******************************************************************/

#define UM_DECL_SCAN(nmea) int um_scan_##nmea (char *buf, nmea##_t *nmea)

UM_DECL_SCAN(caerr);
UM_DECL_SCAN(camsg);
UM_DECL_SCAN(cadbg);
UM_DECL_SCAN(carev);
UM_DECL_SCAN(cacfg);
UM_DECL_SCAN(caclk);

UM_DECL_SCAN(cacyc);
UM_DECL_SCAN(cadrq);
UM_DECL_SCAN(catxd);
UM_DECL_SCAN(catxa);
UM_DECL_SCAN(catxp);
UM_DECL_SCAN(catxf);
UM_DECL_SCAN(caxst);

UM_DECL_SCAN(carxp);
UM_DECL_SCAN(caack);
UM_DECL_SCAN(carxd);
UM_DECL_SCAN(carxa);
UM_DECL_SCAN(cadqf);
UM_DECL_SCAN(camfd);
UM_DECL_SCAN(cashf);
UM_DECL_SCAN(casnr);
UM_DECL_SCAN(cadop);
UM_DECL_SCAN(catoa);
UM_DECL_SCAN(caire);
UM_DECL_SCAN(cacst);

UM_DECL_SCAN(camuc);
UM_DECL_SCAN(camua);
UM_DECL_SCAN(camur);
UM_DECL_SCAN(campc);
UM_DECL_SCAN(campa);
UM_DECL_SCAN(campr);

UM_DECL_SCAN(camec);
UM_DECL_SCAN(camea);
UM_DECL_SCAN(camer);
UM_DECL_SCAN(camsc);
UM_DECL_SCAN(camsa);
UM_DECL_SCAN(camsr);

UM_DECL_SCAN(snmfd);
UM_DECL_SCAN(snpnt);
UM_DECL_SCAN(sntta);
UM_DECL_SCAN(sncfr);
UM_DECL_SCAN(caffl);
UM_DECL_SCAN(cafst);
UM_DECL_SCAN(cabbd);

UM_DECL_SCAN(gprmc);
UM_DECL_SCAN(gpgga);
UM_DECL_SCAN(gpgsv);
UM_DECL_SCAN(gpgsa);

#endif
