
/*
* Copyright (C) Actility, SA. All Rights Reserved.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License version
* 2 only, as published by the Free Software Foundation.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License version 2 for more details (a copy is
* included at /legal/license.txt).
*
* You should have received a copy of the GNU General Public License
* version 2 along with this work; if not, write to the Free Software
* Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
* 02110-1301 USA
*
* Please contact Actility, SA.,  4, rue Ampere 22300 LANNION FRANCE
* or visit www.actility.com if you need additional
* information or have any questions.
*/

/*! @file diadat.c
 *
 */
#if 0
#include <stdio.h>      /************************************/
#include <stdlib.h>      /*   run in main thread       */
#include <string.h>      /************************************/
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <poll.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#endif

#include "modbus.h"
#include "diadef.h"

extern  dia_context_t  *DiaCtxt;

void DIA_LEAK WDiaUCBRequestOk_DiaProductsDatOk(t_dia_req *preq, t_cmn_sensor *cmn) {}

int DIA_LEAK WDiaUCBPreLoadTemplate_DiaProductsDatContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void **xo,char *templ)
{
  return 0;
}

int DIA_LEAK WDiaUCBPostLoadTemplate_DiaProductsDatContCreate(t_dia_req *preq, char *target, t_cmn_sensor *cmn, void *xo)
{
  return 0;
}

static  void DiaProductsDatOk(t_dia_req *preq, t_dia_rspparam *par);
static  void DiaProductsDatError(t_dia_req *preq, t_dia_rspparam *par);
static  void DiaProductsDatContCreate(t_dia_req *preq, t_dia_rspparam *par, ...);
static  void DiaProductsDatContRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);

static  void  DiaProductsDatOk(t_dia_req *preq,t_dia_rspparam *par)
{
  RTL_TRDBG(2,"OK DIA DAT 'IPU' num=%04x attr=%04x member=%d\n",
    preq->rq_cluster,preq->rq_attribut,preq->rq_member);

  WDiaUCBRequestOk(DIA_FUNC,preq,NULL);
  WDiaUCBRequestOk_DiaProductsDatOk(preq,NULL);
}

static  void  DiaProductsDatError(t_dia_req *preq,t_dia_rspparam *par)
{
  RTL_TRDBG(0,"ERROR DIA DAT FAILURE 'IPU' req='%s' tid=%d code='%s' num=%04x\n",
  preq->rq_name,preq->rq_tid,preq->rq_scode,preq->rq_cluster);

  if  (par && par->pr_timeout)  // TIMEOUT => RETRY NO
  {
    WDiaUCBRequestTimeout(DIA_FUNC,preq,NULL);
    return;
  }
  RTL_TRDBG(0,"ERROR DIA DAT DEFINTIVE FAILURE\n");

  WDiaUCBRequestError(DIA_FUNC,preq,NULL);
}

// Create the Products container
static
void
DiaProductsDatContCreate(t_dia_req *preq, t_dia_rspparam *par, ...)
{
  DIA_LOCAL_VAR();

if  (PseudoOng)
  sprintf(target,"%s%s/%s/containers/products", GetHostTarget(), SCL_ROOT_APP, GetVar("w_ipuid"));
else
  sprintf(target,"%s%s/%s/containers", GetHostTarget(), SCL_ROOT_APP, GetVar("w_ipuid"));

  if  (par == NULL)
  {
  // REQUEST
retry  :
  if  (preq->rq_waitRsp == NUSE_REQUEST_DIA) {
    xo  = NULL;
    strcpy (tmp, "dat_ipu_cont.xml");
    
    WDiaUCBPreLoadTemplate(reqname,preq,target,NULL,&xo,tmp);
    WDiaUCBPreLoadTemplate_DiaProductsDatContCreate(preq,target,NULL,&xo,tmp);
    
    if (!xo) {
      xo  = WXoNewTemplate("dat_ipu_cont.xml",0);
      if (!xo) {
        return;
      }
    }
      
    WDiaUCBPostLoadTemplate(reqname,preq,target,NULL,xo);
    WDiaUCBPostLoadTemplate_DiaProductsDatContCreate(preq,target,NULL,xo);
  }

  DIA_CREATE_REQUEST(DiaProductsDatContCreate);
return;
  }

  // RESPONSE
  DIA_CREATE_RESPONSE(DiaProductsDatError,DiaProductsDatError,DiaProductsDatOk);
}

// Ensure Products container exists
static
void
DiaProductsDatContRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...)
{
  DIA_LOCAL_VAR();

  sprintf(target,"%s%s/%s/containers/products", GetHostTarget(), SCL_ROOT_APP, GetVar("w_ipuid"));

  if  (par == NULL)
  {
  // REQUEST
retry  :
  DIA_RETRIEVE_REQUEST(DiaProductsDatContRetrieve);
return;
  }
  
  // RESPONSE
  DIA_RETRIEVE_RESPONSE(DiaProductsDatError,DiaProductsDatContCreate,DiaProductsDatOk);
}

void
DiaProductsDatStart(void)
{
  t_dia_req  req;

  memset  (&req,0,sizeof(t_dia_req));
  req.rq_cli  = NULL;
  req.rq_name  = (char *)__func__;
  DiaProductsDatContRetrieve(&req, NULL);
}

/* -------------------------------------------- */

static  void DiaProductsOk(t_dia_req *preq, t_dia_rspparam *par);
static  void DiaProductsError(t_dia_req *preq, t_dia_rspparam *par);
static  void DiaProductsRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...);

static
void
DiaProductsOk(t_dia_req *preq, t_dia_rspparam *par)
{

  int parseflags = 0;
  void *xo = NULL;
     
  xo = WDiaXoLoadResponse(preq, parseflags);
  ProductsSetDescription(preq->rq_dev, xo);
}

static
void
DiaProductsError(t_dia_req *preq,t_dia_rspparam *par)
{
  if (par && par->pr_timeout) {
    return;
  }
  RTL_TRDBG(0,"ERROR DIA DAT DEFINTIVE FAILURE\n");
  ProductsSetDescription(preq->rq_dev, NULL);
}

static
void
DiaProductsRetrieve(t_dia_req *preq, t_dia_rspparam *par, ...)
{
  DIA_LOCAL_VAR();
  
  sprintf(target,"%s%s/%s/containers/products/contentInstances/%s/content", 
      GetHostTarget(),
      SCL_ROOT_APP,
      GetVar("w_ipuid"),
      ProductsGetRefFromId(preq->rq_dev));

  if (par == NULL) {
    // REQUEST
retry:
    DIA_RETRIEVE_REQUEST(DiaProductsRetrieve);
    return;
  }
  
  // RESPONSE
  DIA_RETRIEVE_RESPONSE(DiaProductsError,DiaProductsError,DiaProductsOk);
}


int
DiaProductsFetch(int dev)
{
  t_dia_req  req;

  memset(&req, 0, sizeof(t_dia_req));
  req.rq_dev  = dev;
  req.rq_name  = (char *)__func__;

  DiaProductsRetrieve(&req, NULL);
  
  return 0;
}
