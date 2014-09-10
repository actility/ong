
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

/*! @file model.c
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

/*ML-20121001-retargeting-response+*/
#include "dIa.h"
/*ML-20121001-retargeting-response-*/

#include "cmndefine.h"

#include "cmnstruct.h"
#include "cmnproto.h"
#include "speccproto.h"


#define	DEFAULT_PREF	"zigbee"


extern	int	Operating;

char	*GetVar(char *name);
char	*SetVar(char *name,char *value);


static	void	*ModelLoad(char *fcfg)
{
	void	*o;
	int	parseerr;
	int	parseflags	= 0;

	o	= XoReadXmlEx(fcfg,NULL,parseflags,&parseerr);
	if	(parseerr > 0)
	{
		RTL_TRDBG(0,"ERROR config file '%s' parsing error=%d \n",
					fcfg,parseerr);
		return	NULL;
	}
	if	(parseerr < 0)
	{
		RTL_TRDBG(0,"ERROR config file '%s' loading error=%d \n",
					fcfg,parseerr);
		return	NULL;
	}

	return	o;
}



/*!
 *
 * @brief load model configuration file
 * @param hot cold or hot configuration phase
 * @return xo obj or NULL
 * 
 */
void	*MdlLoad(int hot)
{
	char	*awd;
	char	file[1024];
	void	*ret	= NULL;


	if	(!Operating)
	{
		awd	= getenv(GetEnvVarName());
		if	(awd)
		{
			sprintf	(file,"%s/config/modelconfig.xml",awd);
			if	(!access(file,R_OK))
			{
				ret	= ModelLoad(file);
				return	ret;
			}
		}
		awd	= getenv("ROOTACT");
		if	(awd)
		{
			sprintf	(file,"%s/%s/config/modelconfig.xml",
							awd,GetAdaptorName());
			ret	= ModelLoad(file);
			return	ret;
		}

		if	(!ret)
		{
			RTL_TRDBG(0,"ERROR modelconfig.xml not found\n");
		}
		
		return	ret;
	}

	/*ML-20121001-retargeting-response+*/
	awd = getenv("ROOTACT");
	if (!awd)
	{
		RTL_TRDBG(0,"ERROR ROOTACT not set\n");
		return NULL;
	}

	snprintf(file, 1024, "%s/usr/etc/%s/modelconfig.xml", awd,
							GetAdaptorName());
	if (!access(file, R_OK))
	{
		return ModelLoad(file);
	}

	snprintf(file, 1024, "%s/etc/%s/modelconfig.xml",
							awd, GetAdaptorName());
	if (!access(file,R_OK))
	{
		return ModelLoad(file);
	}

	RTL_TRDBG(0,"ERROR modelconfig.xml not found\n");
	/*ML-20121001-retargeting-response-*/

	return	ret;
}

/*!
 *
 * @brief find an interface in the model configuration
 * @param mdl xo model
 * @param pref prefixe to add to the filter
 * @param type cluster type (server,client)
 * @param id cluster id (0xXXXX)
 * @return xo obj or NULL
 * 
 */
void	*MdlGetInterface(void *mdl,char *pref,char *type,char *id)
{
	char	filter[256];
	char	*pt;
	void	*o;
	int	i;
	int	nb;

	if	(!mdl || !type || !id)
	{
RTL_TRDBG(0,"ERROR search interface filter\n");
		return	NULL;
	}
	if	(!pref || !*pref)
		pref	= DEFAULT_PREF;

	sprintf	(filter,"%s:/%s/%s",pref,type,id);


	nb	= XoNmNbInAttr(mdl,"ong:interfaces");
	for	(i = 0 ; i < nb ; i++)
	{
		o	= XoNmGetAttr(mdl,"ong:interfaces[%d]",NULL,i);
		if	(!o)	continue;

		pt	= XoNmGetAttr(o,"ong:filter",NULL,0);
		if	(!pt || !*pt)	continue;
		if	(strcasecmp(filter,pt) == 0)
		{
RTL_TRDBG(3,"interface filter '%s' found in model config\n",filter);
			return	o;
		}
	}

	RTL_TRDBG(1,"interface filter '%s' not found in model config\n",filter);

	return	NULL;
}

/*!
 *
 * @brief find a descriptor in an interface model configuration
 * @param itf xo interface
 * @param name name of descriptor
 * @return xo obj or NULL
 * 
 */
void	*MdlGetDescriptorByName(void *itf,char *name)
{
	char	*pt;
	void	*o;
	int	i;
	int	nb;

	if	(!itf || !name || !*name)
	{
RTL_TRDBG(0,"ERROR search descriptor\n");
		return	NULL;
	}

	nb	= XoNmNbInAttr(itf,"ong:descriptor");
	for	(i = 0 ; i < nb ; i++)
	{
		o	= XoNmGetAttr(itf,"ong:descriptor[%d]",NULL,i);
		if	(!o)	continue;

		pt	= XoNmGetAttr(o,"ong:name",NULL,0);
		if	(!pt || !*pt)	continue;
		if	(strcasecmp(name,pt) == 0)
		{
RTL_TRDBG(3,"descriptor name '%s' found in model config\n",name);
			return	o;
		}
	}

RTL_TRDBG(3,"descriptor name '%s' not found in model config\n",name);

	return	NULL;
}

/*!
 *
 * @brief configure an interface template with the model configuration
 * @param mdlitf xo interface model
 * @param itf xo/obix interface
 * @return void
 * 
 */
void	MdlConfigureInterfaceTemplate(void *mdlitf,void *itf)
{
	void	*o;
	int	pos;
	int	nb;
	void	*mdldesc;

	char	*name;
	char	*href;
	char	*status;

	if	(!mdlitf || !itf || XoIsObix(itf) <= 0)
		return;

	nb	= XoNmNbInAttr(itf,"[]");
	RTL_TRDBG(3,"number of objects in interface template %d\n",nb);
	pos	= XoNmFirstAttr(itf,"[]");
	while	(pos > 0)
	{
		o	= XoNmGetAttr(itf,"[]",NULL);
		if	(!o)	goto next;
/*ML-20121001-retargeting-response+*/
		name	= XoNmGetAttr(o,"name",0,0);
/*ML-20121001-retargeting-response-*/
		status	= XoNmGetAttr(o,"status",0,0);
		if	(status && strcmp(status,"cfg-mandatory") == 0)
		{
			RTL_TRDBG(1,"attr/point/cmd '%s' mandatory => keep\n",
				name);
			goto	next;
		}
#if	0
		href	= XoNmGetAttr(o,"href",0,0);
		if	(!href)	goto next;	// not a cmd,attr,point ... ???
#endif
/*ML-20121001-retargeting-response+*/
//		name	= XoNmGetAttr(o,"name",0,0);
//		RTL_TRDBG(5,"name='%s' href='%s'\n",name,href);
		RTL_TRDBG(5,"name='%s'\n",name);
/*ML-20121001-retargeting-response-*/

		mdldesc	= MdlGetDescriptorByName(mdlitf,name);
		if	(!mdldesc)
		{
			RTL_TRDBG(1,"attr/point/cmd '%s' not found => dropped\n",
				name);
			XoNmSupInAttr(itf,"[]");
		}
next :
		pos	= XoNmNextAttr(itf,"[]");
	}
}

/*!
 *
 * @brief for an interface template clear status attribut, must be called even
 * if modeling configuration is not used
 * @param itf xo/obix interface
 * @return void
 * 
 */
void	MdlUnsetStatusInterfaceTemplate(void *itf)
{
	void	*o;
	int	pos;
	int	nb;

	if	(!itf || XoIsObix(itf) <= 0)
		return;

	nb	= XoNmNbInAttr(itf,"[]");
	pos	= XoNmFirstAttr(itf,"[]");
	while	(pos > 0)
	{
		o	= XoNmGetAttr(itf,"[]",NULL);
		if	(!o)	goto next;
		XoNmUnSetAttr(o,"status",0,0);
next :
		pos	= XoNmNextAttr(itf,"[]");
	}
}

/*!
 *
 * @brief configure a container template with the model configuration
 * @param mdlitf xo interface model
 * @param cnt xo container template
 * @param m2mvar variable containing the m2m mapping identifier 
 * @return void
 * 
 */
void	MdlConfigureDataContainerTemplate(void *mdlitf,void *xo,char *m2mvar)
{
	char	*filteritf;
	char	*m2mid;
	void	*desc;
	void	*store;
	char	*v;

	if	(!mdlitf || !xo || !m2mvar || !*m2mvar)
	{
RTL_TRDBG(0,"ERROR modeling configure data container template\n");
		return;
	}
	filteritf	= XoNmGetAttr(mdlitf,"ong:filter",NULL,0);
	m2mid	= GetVar(m2mvar);
	if	(!m2mid || !*m2mid)
	{
RTL_TRDBG(0,"ERROR modeling no mapping for '%s'\n",m2mvar);
		return;
	}
	desc	= MdlGetDescriptorByName(mdlitf,m2mid);
	if	(!desc)
	{
RTL_TRDBG(0,"ERROR modeling no descriptor for '%s' in '%s'\n",m2mid,filteritf);
		return;
	}

/* TODO
	v	= XoNmGetAttr(desc,"ong:config",NULL,0);
	if	(v && *v)
	{
		XoNmSetAttr(xo,"m2m:XXXXXXXXX",v,0);
RTL_TRDBG(1,"modeling container '%s' config '%s'\n",m2mid,v);
	}
*/
	v	= XoNmGetAttr(desc,"ong:maxNrOfInstances",NULL,0);
	if	(v && *v)
	{
		XoNmSetAttr(xo,"m2m:maxNrOfInstances",v,0);
RTL_TRDBG(1,"modeling container '%s' maxnofi '%s'\n",m2mid,v);
	}

	v	= XoNmGetAttr(desc,"ong:maxByteSize",NULL,0);
	if	(v && *v)
	{
		XoNmSetAttr(xo,"m2m:maxByteSize",v,0);
RTL_TRDBG(1,"modeling container '%s' maxage '%s'\n",m2mid,v);
	}

	v	= XoNmGetAttr(desc,"ong:maxInstanceAge",NULL,0);
	if	(v && *v)
	{
		XoNmSetAttr(xo,"m2m:maxInstanceAge",v,0);
RTL_TRDBG(1,"modeling container '%s' maxage '%s'\n",m2mid,v);
	}

	store	= XoNmNew("acy:storageConfiguration");
	if	(!store)
		return;

	// new+set acy:storageConfiguration
	XoNmSetAttr(store,"name$","acy:storageConfiguration",0);

#if	0
	if	(TraceDebug)	// TODO test only
	{
		void	*p;

		p	= XoNmNew("acy:param");
		XoNmSetAttr(p,"name$","acy:param",0);
		XoNmSetAttr(p,"name","debug:filteritf",0);
		XoNmSetAttr(p,"val",filteritf,0);
		XoNmAddInAttr(store,"acy:param",p,0,0);

		p	= XoNmNew("acy:param");
		XoNmSetAttr(p,"name$","acy:param",0);
		XoNmSetAttr(p,"name","debug:m2mvar",0);
		XoNmSetAttr(p,"val",m2mvar,0);
		XoNmAddInAttr(store,"acy:param",p,0,0);

		p	= XoNmNew("acy:param");
		XoNmSetAttr(p,"name$","acy:param",0);
		XoNmSetAttr(p,"name","debug:m2mid",0);
		XoNmSetAttr(p,"val",m2mid,0);
		XoNmAddInAttr(store,"acy:param",p,0,0);
	}
#endif

	v	= XoNmGetAttr(desc,"ong:storageDriver",NULL,0);
	if	(v && *v)
	{
		void	*p;

		p	= XoNmNew("acy:param");

		XoNmSetAttr(p,"name$","acy:param",0);
		XoNmSetAttr(p,"name","store:driver",0);
		XoNmSetAttr(p,"val",v,0);

		XoNmAddInAttr(store,"acy:param",p,0,0);
		
RTL_TRDBG(1,"modeling container '%s' storageDriver '%s'\n",m2mid,v);
	}

	if	(XoNmNbInAttr(store,"acy:param") >= 1)
	{
		XoSetNameSpace(xo,"acy");
		XoNmSetAttr(xo,"acy:storageConfiguration",store,0);
	}
	else
		XoFree(store,1);

}


/*!
 *
 * @brief get the number of "covConfiguration" entries in an interface modeling
 * configuration.
 * @param itf xo interface
 * @return the number of "covConfiguration" elements in the interface.
 */
int MdlGetNbCovConfigurationEntries(void *itf)
{
	int nb;

	if  (!itf)
	{
	RTL_TRDBG(0,"ERROR search covConfiguration\n");
	return  0;
	}

	nb = XoNmNbInAttr(itf, "ong:covConfigurations");
	return nb;
}

/*!
 *
 * @brief get the nth covConfiguration element in an interface model configuration
 * @param itf xo interface
 * @param nth the nth covConfiguration entry
 * @return xo obj or NULL
 */
void *MdlGetCovConfigurationByNumEntry(void *itf, int n)
{
	void * o = NULL;
	int nb;

	if  (!itf)
	{
RTL_TRDBG(0,"ERROR search covConfiguration (itf:0x%.8x) (n:%d)\n", itf, n);
	return  NULL;
	}

	nb = XoNmNbInAttr(itf, "ong:covConfigurations");
	if (n < 0 || n >= nb)
	{
RTL_TRDBG(3,"no more covConfiguration for this interface (nb:%d) (n:%d)\n", nb, n);
	return NULL;
	}

	o = XoNmGetAttr(itf, "ong:covConfigurations[%d]", NULL, n);
	return  o;
}
