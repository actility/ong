
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

/*! @file covconfig.c
 *
 */
#include <stdio.h>			/************************************/
#include <stdlib.h>			/*     run in main thread           */
#include <string.h>			/************************************/
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include "rtlbase.h"
#include "rtlimsg.h"
#include "xoapipr.h"

#include "adaptor.h"

#include "define.h"
#include "state.h"
#include "struct.h"

#include "cproto.h"
#include "cmnproto.h"
#include "extern.h"


t_cov_to_zcl	*TbCz[CZ_MAX];
int		NbCz;

// also used by cli => not static
void	DoCovDataTranslation(t_cov_to_zcl *cz)
{
	t_zcl_msg		zmsg;
	t_zcl_simple_config	*simple;
	t_zcl_metering_config	*meter;
	t_zcl_tic_config	*tic;
	t_zcl_power_config	*power;
	char			*pt	= cz->cz_mincov;

	int			zlg;
	char			*zdata;

	zdata			= (char *)&zmsg.u_pack.m_data;
	memset	(&zmsg,0,sizeof(t_zcl_msg));

	zmsg.m_cmd		= W_CMD_REPO_CONF_RQST;
	zmsg.m_cluster_id	= cz->cz_cluster;
	zmsg.m_attr_id		= cz->cz_attribut;	

	switch	(cz->cz_cluster)
	{
	case	W_CLU_CONFIGURATION :
	{
		int	mode,source;

		mode = source = 0;
		power	= &zmsg.u_param.m_power_config;
		power->m_min_time = cz->cz_mint;
		power->m_max_time = cz->cz_maxt;

		sscanf	(pt,"%u:%u",&mode,&source);
		power->m_mode	= (unsigned int)mode;
		power->m_source	= (unsigned int)source;
	}
	break;
	case	W_CLU_SIMPLEMETER :
		meter	= &zmsg.u_param.m_metering_config;
		meter->m_min_time = cz->cz_mint;
		meter->m_max_time = cz->cz_maxt;

		if	(strchr(pt,':') == NULL) // only one elem
		{
			meter->m_sumWh	= atoi(pt);
			meter->m_sumVarh= atoi(pt);
			meter->m_count	= 0;
			meter->m_curWh	= 0;
			meter->m_curVarh= 0;
		}
		else
		{
			int sw,sv,c,cw,cv;

			sw=sv=c=cw=cv=0;
			sscanf	(pt,"%u:%u:%u:%u:%u",&sw,&sv,&c,&cw,&cv);
			meter->m_sumWh	= (unsigned int)sw;
			meter->m_sumVarh= (unsigned int)sv;
			meter->m_count	= (unsigned short)c;
			meter->m_curWh	= (unsigned short)cw;
			meter->m_curVarh= (unsigned short)cv;
		}
	break;
	case	W_CLU_TICICE :
	case	W_CLU_TICCBE :
	case	W_CLU_TICCJE :
	{
		tic	= &zmsg.u_param.m_tic_config;
		tic->m_min_time = cz->cz_mint;
		tic->m_max_time = cz->cz_maxt;
		tic->m_sz	= 0;
		if	(strlen(pt))
		{
			zlg		= sizeof(tic->m_data);
			AwStrToBin(pt,tic->m_data,&zlg);
			tic->m_sz	= (unsigned char)zlg;
		}
	}
	break;
	case	W_CLU_ANALOGINPUT :
	{
		float	fv	= atof(cz->cz_mincov);
//		unsigned int v	= *(unsigned int *)&fv; => NO

		simple	= &zmsg.u_param.m_simple_config;
		simple->m_min_time = cz->cz_mint;
		simple->m_max_time = cz->cz_maxt;
		simple->u.ufm_min_change = fv;
//printf	("%s %f %x %f\n",cz->cz_mincov,fv,simple->u.uim_min_change,simple->u.ufm_min_change);
	}
	break;
	default :
		simple	= &zmsg.u_param.m_simple_config;
		simple->m_min_time = cz->cz_mint;
		simple->m_max_time = cz->cz_maxt;
		simple->m_min_change = atoi(cz->cz_mincov);
	break;
	}

	zlg	= AwZclEncode(&zmsg,NULL,0);
	if	(zlg < 0 || zmsg.m_xxcode == 0)
	{
		RTL_TRDBG(0,"ERROR cannot encode zcl message from model\n");
		cz->cz_error	= 1;
		return;
	}
	cz->cz_sz	= zlg;
	cz->cz_data	= malloc(zlg);
	if	(!cz->cz_data)
	{
		cz->cz_error	= 2;
		return;
	}
	memcpy	(cz->cz_data,zdata,zlg);

#if	0
	{
		char	tmp[2048];

		AwBinToStr((unsigned char*)zdata,zlg,tmp,sizeof(tmp));
printf	("[%s]\n",tmp);
	}
#endif

}

static	void	AddCovCz(int cluster,int attribut,void *cov)
{
	t_cov_to_zcl	*cz;

	char		*pt;
	int		mint;
	int		maxt;
	char		*mincov;

	if	(NbCz >= CZ_MAX)
	{
		RTL_TRDBG(0,"ERROR max cov translators %d...\n",NbCz);
		return;
	}

	cz	= (t_cov_to_zcl *)malloc(sizeof(t_cov_to_zcl));
	if	(!cz)	return;
	memset	(cz,0,sizeof(t_cov_to_zcl));

	cz->cz_index	= NbCz;
	cz->cz_cluster	= cluster;
	cz->cz_attribut	= attribut;

	pt	= XoNmGetAttr(cov,"ong:minInterval",NULL);
	if	(!pt || !*pt)
		mint	= 5*60;
	else
		mint	= parseISO8601Duration(pt);
	cz->cz_mint	= mint;

	pt	= XoNmGetAttr(cov,"ong:maxInterval",NULL);
	if	(!pt || !*pt)
		maxt	= 3600;
	else
		maxt	= parseISO8601Duration(pt);
	cz->cz_maxt	= maxt;

	mincov	= XoNmGetAttr(cov,"ong:minCOV",NULL);
	if	(!mincov || !*mincov)
		mincov	= "1";
	cz->cz_mincov	= mincov;

//printf	("cluster=%04x attribut=%04x mint=%d maxt=%d mincov='%s'\n",
//			cluster,attribut,mint,maxt,mincov);

	DoCovDataTranslation(cz);

	TbCz[NbCz]	= cz;
	NbCz++;
}

void	CovToZclLoadTranslator(void *mdl)
{
	int	nbitf;
	int	i;
	void	*itf;
	int	nbcov;
	int	c;
	void	*cov;

	char	*fitf;
	char	*fatt;

	char	*pt;
	int	cluster;
	int	attribut;

	nbitf	= XoNmNbInAttr(mdl,"ong:interfaces");
	for	(i = 0 ; i < nbitf ; i++)
	{
		itf	= XoNmGetAttr(mdl,"ong:interfaces[%d]",NULL,i);
		if	(!itf)	continue;

		fitf	= XoNmGetAttr(itf,"ong:filter",NULL,0);
		if	(!fitf || !*fitf)	continue;

		pt	= strrchr(fitf,'/');
		if	(pt)
			pt++;
		else
			pt	= fitf;
		sscanf	(pt,"%x",&cluster);
		nbcov	= XoNmNbInAttr(itf, "ong:covConfigurations");
		for	(c = 0 ; c < nbcov ; c++)
		{
			cov	= XoNmGetAttr(itf, "ong:covConfigurations[%d]", NULL, c);
			if	(!cov)	continue;

			fatt	= XoNmGetAttr(cov,"ong:filter",NULL,0);
			if	(!fatt || !*fatt)	continue;

			pt	= strrchr(fatt,'/');
			if	(pt)
				pt++;
			else
				pt	= fatt;
			sscanf	(pt,"%x",&attribut);
//printf	("cluster=%04x attribut=%04x\n",cluster,attribut);
			AddCovCz(cluster,attribut,cov);
		}
	}
}


void	CovToCzDump(void *cl,char *buf)
{
	t_cov_to_zcl	*cz;
	int		i;
	char	tmp[1024];

	AdmPrint(cl,"current cov configuration loaded :\n");

	for	(i = 0 ; i < NbCz ; i++)
	{
		cz	= TbCz[i];
		if	(!cz)	continue;

		AdmPrint(cl,"[%03d] cluster=0x%04x attr=0x%04x self=%x\n",
			cz->cz_index,cz->cz_cluster,cz->cz_attribut,cz);

		AdmPrint(cl,"[%03d] szdata=%d error=%d used=%d\n",
			cz->cz_index,cz->cz_sz,cz->cz_error,cz->cz_used);

		AdmPrint(cl,"[%03d] mint=%d maxt=%d cov='%s'\n",
			cz->cz_index,cz->cz_mint,cz->cz_maxt,cz->cz_mincov);

		AwBinToStr((unsigned char*)cz->cz_data,cz->cz_sz,
							tmp,sizeof(tmp));

		AdmPrint(cl,"[%03d] data='%s'\n",cz->cz_index,tmp);
		AdmPrint(cl,"[%03d] itfname='%s'\n",
		cz->cz_index,SensorClusterToInterfaceName(cz->cz_cluster));

		AdmPrint(cl,"------------------------------\n");
	}
}

static	int	IndexWithCluster(int epn,int cluster,int nbset,
					t_cov_to_zcl *tab[],int eptab[],int max)
{
	int	n;
	int	nb = 0;
	t_cov_to_zcl	*cz;

	for	(n = 0 ; n < NbCz && nbset < max; n++)
	{
		cz	= TbCz[n];
		if	(!cz)	continue;

		if	(cz->cz_error)	continue;

		if	(cz->cz_cluster == cluster)
		{
			tab[nbset]	= cz;
			eptab[nbset]	= epn;
			nbset++;
			nb++;
		}
	}
	return	nb;
}

int	CovToZclIndexWithDesc(t_zcl_simple_desc *desc,t_cov_to_zcl *tab[],
					int eptab[],int max)
{
	int	n;
	int	nbset	= 0;

	for	(n = 0 ; n < desc->m_nbclin && nbset < max ; n++)
	{
		nbset += IndexWithCluster(desc->m_epin[n],desc->m_clin[n],
						nbset,tab,eptab,max);
	}
	for	(n = 0 ; n < desc->m_nbclout && nbset < max ; n++)
	{
		nbset += IndexWithCluster(desc->m_epin[n],desc->m_clout[n],
						nbset,tab,eptab,max);
	}

	return	nbset;
}


int	CovToZclMaxReportTime(t_cov_to_zcl *tab[],int nb)
{
	int	def	= 24*3600;
	int	report	= INT_MAX;
	int	i;
	t_cov_to_zcl	*cz;

	if	(nb <= 0)
		return	def;

	for	(i = 0 ; i < nb && i < CZ_MAX_PER_SS ; i++)
	{
		cz	= tab[i];
		if	(cz->cz_maxt < report)
		{
			report	= cz->cz_maxt;
		}
	}
	if	(report == INT_MAX)
		return	def;

	report	= report * 3;
	return	report;
}

int	CovToZclMinReportTime(t_cov_to_zcl *tab[],int nb)
{
	int	def	= 5*60;
	int	report	= INT_MIN;
	int	i;
	t_cov_to_zcl	*cz;

	if	(nb <= 0)
		return	def;

	for	(i = 0 ; i < nb && i < CZ_MAX_PER_SS ; i++)
	{
		cz	= tab[i];
		if	(cz->cz_mint > report)
		{
			report	= cz->cz_mint;
		}
	}
	if	(report == INT_MIN)
		return	def;

	report	= report * 2;
	return	report;
}
