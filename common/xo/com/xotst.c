
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "xoapipr.h"

char	*Xml = "\
<m2m:container xmlns:m2m='http://uri.etsi.org/m2m'  m2m:id='containerId'>\
  <m2m:expirationTime>2012-05-20T20:00:00Z</m2m:expirationTime>\
  <m2m:accessRightID>/toto</m2m:accessRightID>\
  <m2m:searchStrings>\
  <m2m:searchString>searchString1</m2m:searchString>\
  <m2m:searchString>searchString2</m2m:searchString>\
  </m2m:searchStrings>\
  <m2m:creationTime>2012-04-20T20:00:00Z</m2m:creationTime>\
  <m2m:lastModifiedTime>2012-04-25T20:00:00Z</m2m:lastModifiedTime>\
  <m2m:announceTo>\
  <m2m:activated>true</m2m:activated>\
  <m2m:sclList>\
  <reference>/ref1</reference>\
  <reference>/ref2</reference>\
  </m2m:sclList>\
  <m2m:global>false</m2m:global>\
  </m2m:announceTo>\
  <m2m:maxNrOfInstances>10</m2m:maxNrOfInstances>\
  <m2m:maxByteSize>100</m2m:maxByteSize>\
  <m2m:maxInstanceAge>PT2S</m2m:maxInstanceAge>\
  <m2m:contentInstancesReference>/contentInstancesReference</m2m:contentInstancesReference>\
  <m2m:subscriptionsReference>/subscriptionsReference</m2m:subscriptionsReference>\
</m2m:container>\
";
int	XmlLg;
char	*Binary;
char	*Bis;
int	BinaryLg;
void	*Xo;

int	Flags	= XO_SHARED_DICO | XO_COMPRESS_0;
//int	Flags	= XO_SHARED_DICO;
int	Ret;

int	main(int argc,char *argv[])
{
	XoInit(0);
	XoLoadNameSpaceDir(".");
	XoLoadSharedDicoDir(".");
	XoExtLoadRefDir(".");

	XmlLg	= strlen(Xml);

	Xo	= XoReadXmlMem(Xml,XmlLg,NULL,0,&Ret);
	if	(!Xo)
	{
		printf	("ReadXmlMem() error=%d\n",Ret);
		exit(1);
	}

//	XoSaveAscii(Xo,stdout);
	printf	("Ok ReadXmlMem()\n");

	Ret	= XoSaveMemAllocFlg(Xo,&Binary,&BinaryLg,Flags);
	if	(Ret <= 0)
	{
		printf	("SaveMemAllocFlg() error=%d\n",Ret);
		exit(1);
	}

	printf	("Ok SaveMemAllocFlg() lg=%d\n",BinaryLg);
	XoFree	(Xo,1);

//	Xo	= XoLoadMem(Binary,Ret);
	Xo	= XoLoadMem(Binary,BinaryLg);
	if	(!Xo)
	{
		printf	("XoLoadMem() error\n");
		exit(1);
	}

	XoSaveAscii(Xo,stdout);
	printf	("Ok XoLoadMem()\n");

	exit(0);
}
