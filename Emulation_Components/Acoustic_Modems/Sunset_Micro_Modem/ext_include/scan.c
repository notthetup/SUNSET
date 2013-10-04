/*!
 * \file scan.c
 * \author jon shusta <jshusta@whoi.edu>
 * \date 2009.09
 * \brief functions wrapping nmeasscanf for specific modem sentences.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libumutil.h"


/*!
 */
int um_scan_caerr(char *buf, caerr_t *caerr)
{
  int ret, cs, hh, mm, ss;
  memset(caerr->mod, '\0', sizeof(char) * UMMAXSZSZ);
  memset(caerr->msg, '\0', sizeof(char) * UMMAXSZSZ);
  ret = nmeasscanf(&cs, buf, CAERR, &hh, &mm, &ss, caerr->mod, &(caerr->errno), caerr->msg);
  um_tstotm(&(caerr->ts), hh, mm, ss);
  return ret;
}


/*!
 */
int um_scan_camsg(char *buf, camsg_t *camsg)
{
  int ret, cs;
  memset(camsg->type, '\0', sizeof(char) * UMMAXSZSZ);
  ret = nmeasscanf(&cs, buf, CAMSG, camsg->type, &(camsg->val));
  return ret;
}


/*!
 */
int um_scan_cadbg(char *buf, cadbg_t *cadbg)
{
  int ret, cs;
  memset(cadbg->msg, '\0', sizeof(char) * UMMAXSZSZ);
  ret = nmeasscanf(&cs, buf, CADBG, cadbg->msg);
  return ret;
}


/*!
 */
int um_scan_carev(char *buf, carev_t *carev)
{
  int ret, cs, hh, mm, ss;
  memset(carev->ident, '\0', sizeof(char) * UMMAXIDSZ);
  memset(carev->vers,  '\0', sizeof(char) * UMMAXVRSZ);
  ret = nmeasscanf(&cs, buf, CAREV, &hh, &mm, &ss, carev->ident, carev->vers);
  um_tstotm(&(carev->ts), hh, mm, ss);
  return ret;
}


/*!
 */
int um_scan_cacfg(char *buf, cacfg_t *cacfg)
{
  int ret, cs;
  memset(cacfg->key, '\0', sizeof(char) * UMMAXCFSZ);
  ret = nmeasscanf(&cs, buf, CACFG, cacfg->key, &(cacfg->val));
  return ret;
}


/*!
 */
int um_scan_caclk(char *buf, caclk_t *caclk)
{
  int ret, cs, yyyy, mm, dd, hh, ii, ss;
  ret = nmeasscanf(&cs, buf, CACLK, &yyyy, &mm, &dd, &hh, &ii, &ss);
  um_dttotm(&(caclk->ts), yyyy, mm, dd, hh, ii, ss);
  return ret;
}


/*!
 */
int um_scan_cacyc(char *buf, cacyc_t *cacyc)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CACYC, &(cacyc->cmd),
             &(cacyc->src),
             &(cacyc->dest),
             &(cacyc->rate),
             &(cacyc->flag),
             &(cacyc->skct));
  return ret;
}


/*!
 */
int um_scan_cadrq(char *buf, cadrq_t *cadrq)
{
  int ret, cs, hh, mm, ss;
  ret = nmeasscanf(&cs, buf, CADRQ,
            &hh, &mm, &ss,
            &(cadrq->src),
            &(cadrq->dest),
            &(cadrq->flag),
            &(cadrq->bytes),
            &(cadrq->idx));
  um_tstotm(&(cadrq->ts), hh, mm, ss);
  return ret;
}


/*!
 */
int um_scan_catxp(char *buf, catxp_t *catxp)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CATXP, &(catxp->bytes));
  return ret;
}


/*!
 */
int um_scan_catxf(char *buf, catxf_t *catxf)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CATXF, &(catxf->bytes));
  return ret;
}


/*!
 */
int um_scan_catxd(char *buf, catxd_t *catxd)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CATXD, &(catxd->src), &(catxd->dest), &(catxd->ack), &(catxd->bytes));
  return ret;
}


/*!
 */
int um_scan_catxa(char *buf, catxa_t *catxa)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CATXA, &(catxa->src), &(catxa->dest), &(catxa->ack), &(catxa->bytes));
  return ret;
}


/*!
 */
int um_scan_caxst(char *buf, caxst_t *caxst)
{
  int ret, cs, yy, mm, dd, hh, ii, clkst;
  float ss;

  ret = nmeasscanf(&cs, buf, CAXST,
    &yy, &mm, &dd,
    &hh, &ii, &ss,
    &clkst,
    &(caxst->result),
    &(caxst->probelen),
    &(caxst->bandw),
    &(caxst->carrier),
    &(caxst->rate),
    &(caxst->src),
    &(caxst->dest),
    &(caxst->ack),
    &(caxst->nfr_try),
    &(caxst->nfr_out),
    &(caxst->pktype),
    &(caxst->txbytes));

  um_dttotm(&(caxst->tot), yy, mm, dd, hh, ii, (int)ss);
  return ret;
}


/*!
 */
int um_scan_carxp(char *buf, carxp_t *carxp)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CARXP, &(carxp->pktype));
  return ret;
}


/*!
 */
int um_scan_caack(char *buf, caack_t *caack)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CAACK,
             &(caack->src),
             &(caack->dest),
             &(caack->idx),
             &(caack->ack));
  return ret;
}


/*!
 */
int um_scan_carxd(char *buf, carxd_t *carxd)
{
  int ret, cs;
  memset(carxd->buf, '\0', sizeof(char) * UMMAXSKSZ);

  ret = nmeasscanf(&cs, buf, CARXD,
             &(carxd->src),
             &(carxd->dest),
             &(carxd->ack),
             &(carxd->fc),
             carxd->buf);

  carxd->len = strlen(carxd->buf);
  return ret;
}


/*!
 */
int um_scan_carxa(char *buf, carxa_t *carxa)
{
  int ret, cs;
  memset(carxa->buf, '\0', sizeof(char) * UMMAXSKSZ);

  ret = nmeasscanf(&cs, buf, CARXA,
             &(carxa->src),
             &(carxa->dest),
             &(carxa->ack),
             &(carxa->fc),
             carxa->buf);

  carxa->len = strlen(carxa->buf);
  return ret;
}


/*!
 */
int um_scan_cadqf(char *buf, cadqf_t *cadqf)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CADQF, &(cadqf->quality), &(cadqf->pktype));
  return ret;
}


/*!
 */
int um_scan_camfd(char *buf, camfd_t *camfd)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CAMFD, &(camfd->peak), &(camfd->power), &(camfd->rssi), &(camfd->spl));
  return ret;
}


/*!
 */
int um_scan_camse(char *buf, camse_t *camse)
{ return -1; }


/*!
 */
int um_scan_cashf(char *buf, cashf_t *cashf)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CASHF, &(cashf->gain), &(cashf->prv), &(cashf->cur), &(cashf->mfd), &(cashf->p2b));
  return ret;
}


/*!
 */
int um_scan_casnr(char *buf, casnr_t *casnr)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CASNR, &(casnr->rss), &(casnr->in), &(casnr->out), &(casnr->sym), &(casnr->level));
  return ret;
}


/*!
 */
int um_scan_cadop(char *buf, cadop_t *cadop)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CADOP, &(cadop->speed));
  return ret;
}


/*!
 */
int um_scan_catoa(char *buf, catoa_t *catoa)
{ return -1; }


/*!
 */
int um_scan_caire(char *buf, caire_t *caire)
{ return -1; }


/*!
 */
int um_scan_cacst(char *buf, cacst_t *cacst)
{
  int ret, cs, hh, mm;
  float ss;

  ret = nmeasscanf(&cs, buf, CACST,
          &(cacst->mode),
          &hh, &mm, &ss,
          &(cacst->clk_stat),
          &(cacst->camfd.peak),
          &(cacst->camfd.power),
          &(cacst->camfd.rssi),
          &(cacst->camfd.spl),
          &(cacst->cashf.gain),
          &(cacst->cashf.prv),
          &(cacst->cashf.cur),
          &(cacst->cashf.mfd),
          &(cacst->cashf.p2b),
          &(cacst->rate),
          &(cacst->src),
          &(cacst->dest),
          &(cacst->result),
          &(cacst->pktype),
          &(cacst->nframes),
          &(cacst->nfailed),
          &(cacst->casnr.rss),
          &(cacst->casnr.in),
          &(cacst->casnr.out),
          &(cacst->casnr.sym),
          &(cacst->mse),
          &(cacst->dqf),
          &(cacst->dop));

  um_tstotm(&(cacst->toa), hh, mm, (int)ss);
  return ret;
}


/*!
 */
int um_scan_camuc(char *buf, camuc_t *camuc)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CAMUC, &(camuc->src), &(camuc->dest), camuc->buf);
  camuc->len = strlen(camuc->buf);
  return ret;
}


/*!
 */
int um_scan_camua(char *buf, camua_t *camua)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CAMUA, &(camua->src), &(camua->dest), camua->buf);
  camua->len = strlen(camua->buf);
  return ret;
}


/*!
 */
int um_scan_camur(char *buf, camur_t *camur)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CAMUR, &(camur->src), &(camur->dest));
  return ret;
}


/*!
 */
int um_scan_campc(char *buf, campc_t *campc)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CAMPC, &(campc->src), &(campc->dest));
  return ret;
}


/*!
 */
int um_scan_campa(char *buf, campa_t *campa)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CAMPA, &(campa->src), &(campa->dest));
  return ret;
}


/*!
 */
int um_scan_campr(char *buf, campr_t *campr)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CAMPR, &(campr->src), &(campr->dest), &(campr->htt));
  return ret;
}


/*!
 */
int um_scan_camec(char *buf, camec_t *camec)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CAMEC, &(camec->src), &(camec->dest), &(camec->line), &(camec->mode), &(camec->arg));
  return ret;
}


/*!
 */
int um_scan_camea(char *buf, camea_t *camea)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CAMEA, &(camea->src), &(camea->dest), &(camea->line), &(camea->mode), &(camea->arg));
  return ret;
}


/*!
 */
int um_scan_camer(char *buf, camer_t *camer)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CAMER, &(camer->src), &(camer->dest), &(camer->line), &(camer->mode), &(camer->arg));
  return ret;
}


/*!
 */
int um_scan_camsc(char *buf, camsc_t *camsc)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CAMSC, &(camsc->src), &(camsc->dest), &(camsc->sd_min));
  return ret;
}


/*!
 */
int um_scan_camsa(char *buf, camsa_t *camsa)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CAMSA, &(camsa->src), &(camsa->dest), &(camsa->sd_min));
  return ret;
}


/*!
 */
int um_scan_camsr(char *buf, camsr_t *camsr)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CAMSR, &(camsr->src), &(camsr->dest), &(camsr->sd_min));
  return ret;
}


/*!
 */
int um_scan_carsp(char *buf, carsp_t *carsp)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CARSP, &(carsp->rxsig), &(carsp->txsig), &(carsp->timeo));
  return ret;
}


/*!
 */
int um_scan_snpnt(char *buf, snpnt_t *snpnt)
{
  return -1;
/*
  int ret, cs;
  ret = nmeasscanf(&cs, buf, SNPNT,
          &(snpnt->), &(snpnt->),
          &(snpnt->), &(snpnt->),
          &(snpnt->), &(snpnt->),
          &(snpnt->), &(snpnt->),
          &(snpnt->), &(snpnt->));
  return ret;
*/
}


/*!
 */
int um_scan_snmfd(char *buf, snmfd_t *snmfd)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, SNMFD, &(snmfd->channel), &(snmfd->peak), &(snmfd->power), &(snmfd->rssi));
  return ret;
}


/*!
 */
int um_scan_sntta(char *buf, sntta_t *sntta)
{
  int ret, cs, hh, mm;
  float ss;
  ret = nmeasscanf(&cs, buf, SNTTA, &(sntta->tta), &(sntta->ttb), &(sntta->ttc), &(sntta->ttd), &hh, &mm, &ss);
  um_tstotm(&(sntta->ts), hh, mm, (int)ss);
  return ret;
}


/*!
 */
int um_scan_sncfr(char *buf, sncfr_t *sncfr)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, SNCFR,
          sncfr->v + 0,
          sncfr->v + 1,
          sncfr->v + 2,
          sncfr->v + 3,
          sncfr->v + 4,
          sncfr->v + 5,
          sncfr->v + 6,
          sncfr->v + 7,
          sncfr->v + 8,
          sncfr->v + 9,
          sncfr->v + 10,
          sncfr->v + 11,
          sncfr->v + 12,
          sncfr->v + 13);
  return ret;
}


/*!
 */
int um_scan_caffl(char *buf, caffl_t *caffl)
{ return -1; }

/*!
 */
int um_scan_cafst(char *buf, cafst_t *cafst)
{ return -1; }

/*!
 */
int um_scan_cabbd(char *buf, cabbd_t *cabbd)
{
  int ret, cs;
  ret = nmeasscanf(&cs, buf, CABBD, &(cabbd->nwords), cabbd->buf);
  cabbd->len = strlen(cabbd->buf);
  return ret;
}


/*!
 */
int um_scan_pgrme(char *buf, pgrme_t *pgrme)
{ return -1; }

/*!
 */
int um_scan_pgrmm(char *buf, pgrmm_t *pgrmm)
{ return -1; }

/*!
 */
int um_scan_pgrmt(char *buf, pgrmt_t *pgrmt)
{ return -1; }

/*!
 */
int um_scan_pgrmz(char *buf, pgrmz_t *pgrmz)
{ return -1; }

/*!
 */
int um_scan_gprmb(char *buf, gprmb_t *gprmb)
{ return -1; }

/*!
 */
int um_scan_gprmc(char *buf, gprmc_t *gprmc)
{ return -1; }

/*!
 */
int um_scan_gpgsv(char *buf, gpgsv_t *gpgsv)
{ return -1; }

/*!
 */
int um_scan_gpgsa(char *buf, gpgsa_t *gpgsa)
{ return -1; }

/*!
 */
int um_scan_gpgga(char *buf, gpgga_t *gpgga)
{
  int ret, cs, hh, mm, ss;
  char  lat_cd[2] = {'\0'},
        lon_cd[2] = {'\0'};

  ret = nmeasscanf(&cs, buf, GPGGA,
              &hh, &mm, &ss,
              &(gpgga->latd),
              &(gpgga->latm),
              lat_cd,
              &(gpgga->lond),
              &(gpgga->lonm),
              lon_cd);

  if (um_cdtosign(&(gpgga->latd), lat_cd)) return -1;
  if (um_cdtosign(&(gpgga->lond), lon_cd)) return -1;

  um_tstotm(&(gpgga->ts), hh, mm, ss);
  return ret;
}


