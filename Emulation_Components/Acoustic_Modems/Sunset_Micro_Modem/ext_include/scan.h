#include <ctype.h>

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


/*!
 */
int um_tstotm(struct tm *ts, int hh, int mm, int ss)
{
  time_t t = time(NULL);
  gmtime_r(&t, ts);
  ts->tm_sec  = ss;
  ts->tm_min  = mm;
  ts->tm_hour = hh;
  return mktime(ts);
}


/*!
 */
int um_dttotm(struct tm *ts, int yy, int mm, int dd, int hh, int ii, int ss)
{
  ts->tm_sec  = ss;
  ts->tm_min  = mm;
  ts->tm_hour = hh;
  ts->tm_mday = dd;
  ts->tm_mon  = mm - 1;
  ts->tm_year = yy - 1900;
  return mktime(ts);
}


/*!
 */
int um_cdtosign(int *deg, char cd[2])
{
  int sign = 1;

  switch(toupper(cd[0]))
  {
    case 'E':
    case 'N':
      break;
    case 'W':
    case 'S':
      sign = -1;
      break;
    default:
      return -1;
  }

  if (*deg > 0) *deg *= sign;
  return 0;
}



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


/*!
 */
static void nmeabuf_shift(nmeabuf_t *buffer)
{
  size_t  off;
  switch(buffer->state)
  {
    case NMEA_S_CLEAR: off = buffer->index; break;
    case NMEA_S_MARK:  off = buffer->start; break;
    default: return;
  }
  buffer->start   = 0;
  buffer->index  -= off;
  buffer->length -= off;
  if (buffer->length) memmove(buffer->buffer, buffer->buffer + off, sizeof(char) * buffer->length);
}


/*!
 */
void nmea_ctor(nmeabuf_t *buffer, const char *source, const size_t maxlen)
{
  buffer->buffer = (char *)source;
  buffer->alloc  = maxlen;
  nmea_reset(buffer);
}


/*!
 */
void nmea_reset(nmeabuf_t *buffer)
{
  memset(buffer->buffer, '\0', sizeof(char) * buffer->alloc);
  buffer->length = 0;
  buffer->index  = 0;
  buffer->start  = 0;
  buffer->state  = NMEA_S_CLEAR;
}


/*!
 */
int nmea_concat(nmeabuf_t *buffer, const char *rbuf, const size_t rlen)
{
  nmeabuf_shift(buffer);

  if (!rlen) return 0;

  if (rlen > (buffer->alloc - buffer->length)) return 0;

  memcpy(buffer->buffer + buffer->length, rbuf, sizeof(char) * rlen);

  buffer->length += rlen;

  return 1;
}


/*!
 */
int nmea_scan(nmeabuf_t *buffer, nmeamsg_t *message)
{
  size_t  i = buffer->index;
  int     ret = 0;

  while ((i < buffer->length) && (0 == ret))
  {
    switch(buffer->state)
    {
      case NMEA_S_CLEAR:
        if ('$' == buffer->buffer[i])
        {
          buffer->start = i;
          buffer->state = NMEA_S_MARK;
        }
        break;

      case NMEA_S_MARK:
        if ('\n' == buffer->buffer[i])
        {
          if (message)
          {
            message->nmeabuf = buffer;
            message->start   = buffer->start;
            message->length  = i - buffer->start + 1;
          }
          buffer->state = NMEA_S_CLEAR;
          ret = 1;
        }
        break;
    }
    i++;
  }
  buffer->index = i;
  return ret;
}


/*!
 */
size_t nmea_peek(char *wbuf, const size_t maxlen, nmeamsg_t *message)
{
  if (message->length > maxlen) return 0;
  memcpy(wbuf, message->nmeabuf->buffer + message->start, sizeof(char) * message->length);
  return message->length;
}


/*!
 */
size_t nmea_parse(char *wbuf, const size_t maxlen, nmeamsg_t *message)
{
  size_t len = nmea_peek(wbuf, maxlen, message);
  nmeabuf_shift(message->nmeabuf);
  return len;
}


/*!
 */
int nmea_cksum(char *nmeasz, long *cksum)
{
  char *  end      = NULL,
       *  start    = NULL;
  long    calcsum  = 0,
          checksum = 0;

  /* get start/end of sentence
   * verify they're in order
   */
  start = strchr(nmeasz, '$');
  if (NULL == start) return -1;
  end = strchr(start, '*');
  if (NULL == end) return -1;

  checksum = strtol(end+1, (char **)NULL, 16);
  if (0 > checksum)
  {
    perror("strtol");
    return -1;
  }

  start++;                                    /* begin after '$' */
  while (start < end) calcsum ^= *(start++);  /* calculate */
  if (NULL != cksum) *cksum = calcsum;        /* store calculated checksum */
  return (calcsum == checksum) ? 0 : -1;      /* test, return */
}


/*!
 */
int nmea_cksum_msg(nmeamsg_t *message, long *cksum)
{
  char buf[message->length+1];
  memset(buf, '\0', sizeof(char) * message->length + 1);
  nmea_parse(buf, message->length, message);
  return nmea_cksum(buf, cksum);
}


/*!
 */
void nmea_debug(FILE *stream, const nmeabuf_t *buffer)
{
  size_t i;

  for (i = 0; i < buffer->length; i++)
    fprintf(stream, "%c", isprint(buffer->buffer[i]) ? buffer->buffer[i] : '.');
  fprintf(stream, "\n");

  for (i = 0; i <= buffer->length; i++)
    if (i == buffer->start)
    {
      if (i == buffer->index)
        fprintf(stream, "X");
      else
        fprintf(stream, "S");
    }
    else
    {
      if (i == buffer->index)
        fprintf(stream, "I");
      else
        fprintf(stream, " ");
    }
  fprintf(stream, "\n");
}
