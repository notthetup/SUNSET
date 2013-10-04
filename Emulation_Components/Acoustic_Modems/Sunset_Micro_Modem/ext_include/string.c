/*!
 */

#include <stdio.h>
#include <string.h>
#include "libumutil.h"


/*!
 */
int um_print_cccfg(char *buf, const size_t maxlen, const cccfg_t *cccfg)
{
  return nmeasprintf(buf, maxlen, CCCFG, cccfg->key, cccfg->val);
}


/*!
 */
int um_print_cccfq(char *buf, const size_t maxlen, const cccfq_t *cccfq)
{
  return nmeasprintf(buf, maxlen, CCCFQ, cccfq->key);
}


/*!
 */
int um_print_ccclq(char *buf, const size_t maxlen, const ccclq_t *ccclq)
{
  return nmeasprintf(buf, maxlen, CCCLQ);
}


/*!
 */
int um_print_ccclk(char *buf, const size_t maxlen, const ccclk_t *ccclk)
{
  return nmeasprintf(buf, maxlen, CCCLK,
    ccclk->ts.tm_year + 1900,
    ccclk->ts.tm_mon  + 1,
    ccclk->ts.tm_mday,
    ccclk->ts.tm_hour,
    ccclk->ts.tm_min,
    ccclk->ts.tm_sec);
}


/*!
 */
int um_print_cccyc(char *buf, const size_t maxlen, const cccyc_t *cccyc)
{
  return nmeasprintf(buf, maxlen, CCCYC,
    cccyc->cmd, cccyc->src, cccyc->dest,
    cccyc->rate, cccyc->flag, cccyc->skct);
}


/*!
 */
int um_print_cctxd(char *buf, const size_t maxlen, const cctxd_t *cctxd)
{
  return nmeasprintf(buf, maxlen, CCTXD, cctxd->src, cctxd->dest, cctxd->ack, cctxd->buf);
}


/*!
 */
int um_print_cctxa(char *buf, const size_t maxlen, const cctxa_t *cctxa)
{
  return nmeasprintf(buf, maxlen, CCTXA, cctxa->src, cctxa->dest, cctxa->ack, cctxa->buf);
}


/*!
 */
int um_print_ccmuc(char *buf, const size_t maxlen, const ccmuc_t *ccmuc)
{
  return nmeasprintf(buf, maxlen, CCMUC, ccmuc->src, ccmuc->dest, ccmuc->buf);
}


/*!
 */
int um_print_ccmpc(char *buf, const size_t maxlen, const ccmpc_t *ccmpc)
{
  return nmeasprintf(buf, maxlen, CCMPC, ccmpc->src, ccmpc->dest);
}


/*!
 */
int um_print_ccmec(char *buf, const size_t maxlen, const ccmec_t *ccmec)
{
  return nmeasprintf(buf, maxlen, CCMEC, ccmec->src, ccmec->dest, ccmec->line, ccmec->mode, ccmec->arg);
}


/*!
 */
int um_print_ccmsc(char *buf, const size_t maxlen, const ccmsc_t *ccmsc)
{
  return nmeasprintf(buf, maxlen, CCMSC, ccmsc->src, ccmsc->dest, ccmsc->sd_min);
}


/*!
 */
int um_print_ccexl(char *buf, const size_t maxlen, const ccexl_t *ccexl)
{
  return nmeasprintf(buf, maxlen, CCEXL, ccexl->mode);
}


/*!
 */
int um_print_ccrsp(char *buf, const size_t maxlen, const ccrsp_t *ccrsp)
{
  return nmeasprintf(buf, maxlen, CCRSP, ccrsp->txsig);
}


/*!
 */
int um_print_cccfr(char *buf, const size_t maxlen, const cccfr_t *cccfr)
{
  return nmeasprintf(buf, maxlen, CCCFR, cccfr->sd_msec);
}


/*!
 */
int um_print_ccpnt(char *buf, const size_t maxlen, const ccpnt_t *ccpnt)
{
  return nmeasprintf(buf, maxlen, CCPNT,
    ccpnt->txfreq,
    ccpnt->txl_msec,
    ccpnt->rxl_msec,
    ccpnt->timeo,
    ccpnt->freqa,
    ccpnt->freqb,
    ccpnt->freqc,
    ccpnt->freqd,
    ccpnt->extsync);
}


/*!
 */
int um_print_ccpdt(char *buf, const size_t maxlen, const ccpdt_t *ccpdt)
{
  return nmeasprintf(buf, maxlen, CCPDT,
    ccpdt->group,
    ccpdt->channel,
    ccpdt->extsync,
    ccpdt->pps_timeo,
    ccpdt->timeo,
    ccpdt->af,
    ccpdt->bf,
    ccpdt->cf,
    ccpdt->df);
}


/*!
 */
int um_print_ccffl(char *buf, const size_t maxlen, const ccffl_t *ccffl)
{
  return -1;
}


/*!
 */
int um_print_ccfst(char *buf, const size_t maxlen, const ccfst_t *ccfst)
{
  return -1;
}


/*!
 */
int um_print_ccbbd(char *buf, const size_t maxlen, const ccbbd_t *ccbbd)
{
  return -1;
}
