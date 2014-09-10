#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>
#include	<unistd.h>
#include	<fcntl.h>
#include	<time.h>

#include	"xoapipr.h"


int	main(int argc,char *argv[])
{
	char	*buffer;
	void*	xo;
	void	*ret;

	XoInit(0);
	XoExtLoadRef("../xoref/m2m.xor");
	XoLoadNameSpace("../xoref/namespace.xns");


	xo = XoNmNew("m2m:Application");
	XoNmSetAttr(xo,XOML_BALISE_ATTR,"<dont_save>",0);
//	XoNmSetAttr(xo,XOML_BALISE_ATTR,"m2m:Application",0);
	XoNmSetAttr(xo,"m2m:expirationTime","2012-05-20T20:00:00",0);

//	XoSetNameSpace(xo,NULL);
	XoSetNameSpace(xo,"m2m");

//	prochaine version
//	XoWritXmlMem(xo, XOML_PARSE_TARGET, &buffer,"");
	ret	= XoWritXmlMem(xo, 0, &buffer,"");
	if	(ret)
		printf	("%s\n",buffer);


	return	0;
}
