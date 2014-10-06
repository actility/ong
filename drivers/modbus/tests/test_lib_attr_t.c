#include <assert.h>
#include "modbus-acy.h"


const char attribute_wago_acy_custom[] =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
"<modbus:description xmlns:modbus=\"http://uri.actility.com/m2m/adaptor-modbus\">"
" <modbus:interfaces>"
"  <modbus:interface>"
"   <modbus:attributes>"
"    <modbus:attr modbus:name=\"m2mCustomPowerMeasure\" modbus:cnt=\"0x9999.0x9999.0.m2m\" modbus:type=\"array\" modbus:display=\"Actility power measurement\" modbus:reg=\"/holding/12288/bytes/42/\" modbus:readable=\"true\" modbus:writable=\"false\">"
"     <modbus:subattributes>"
"      <modbus:subattr modbus:name=\"DATE_TEMP\" modbus:type=\"datetime\" modbus:display=\"\" modbus:offset=\"/beginbyte/0/endbyte/3/\"/>"
"      <modbus:subattr modbus:name=\"FREQ\" modbus:type=\"float\" modbus:display=\"\" modbus:offset=\"/beginbyte/4/endbyte/7/\"/>"
"      <modbus:subattr modbus:name=\"Pw_1\" modbus:type=\"float\" modbus:display=\"\" modbus:offset=\"/beginbyte/8/endbyte/11/\"/>"
"      <modbus:subattr modbus:name=\"Pw_2\" modbus:type=\"float\" modbus:display=\"\" modbus:offset=\"/beginbyte/12/endbyte/15/\"/>"
"      <modbus:subattr modbus:name=\"Pw_3\" modbus:type=\"float\" modbus:display=\"\" modbus:offset=\"/beginbyte/16/endbyte/19/\"/>"
"      <modbus:subattr modbus:name=\"Pw_4\" modbus:type=\"float\" modbus:display=\"\" modbus:offset=\"/beginbyte/20/endbyte/23/\"/>"
"      <modbus:subattr modbus:name=\"Pw_5\" modbus:type=\"float\" modbus:display=\"\" modbus:offset=\"/beginbyte/24/endbyte/27/\"/>"
"      <modbus:subattr modbus:name=\"P_soutirage\" modbus:type=\"float\" modbus:display=\"\" modbus:offset=\"/beginbyte/28/endbyte/31/\"/>"
"      <modbus:subattr modbus:name=\"CD1\" modbus:type=\"bool\" modbus:display=\"\" modbus:offset=\"/beginbyte/32/endbyte/32/\"/>"
"      <modbus:subattr modbus:name=\"CD2\" modbus:type=\"bool\" modbus:display=\"\" modbus:offset=\"/beginbyte/33/endbyte/33/\"/>"
"      <modbus:subattr modbus:name=\"CD3\" modbus:type=\"bool\" modbus:display=\"\" modbus:offset=\"/beginbyte/34/endbyte/34/\"/>"
"      <modbus:subattr modbus:name=\"CD4\" modbus:type=\"bool\" modbus:display=\"\" modbus:offset=\"/beginbyte/35/endbyte/35/\"/>"
"      <modbus:subattr modbus:name=\"CD5\" modbus:type=\"bool\" modbus:display=\"\" modbus:offset=\"/beginbyte/36/endbyte/36/\"/>"
"      <modbus:subattr modbus:name=\"Status1\" modbus:type=\"bool\" modbus:display=\"\" modbus:offset=\"/beginbyte/37/endbyte/37/\"/>"
"      <modbus:subattr modbus:name=\"Status2\" modbus:type=\"bool\" modbus:display=\"\" modbus:offset=\"/beginbyte/38/endbyte/38/\"/>"
"      <modbus:subattr modbus:name=\"Status3\" modbus:type=\"bool\" modbus:display=\"\" modbus:offset=\"/beginbyte/39/endbyte/39/\"/>"
"      <modbus:subattr modbus:name=\"Status4\" modbus:type=\"bool\" modbus:display=\"\" modbus:offset=\"/beginbyte/40/endbyte/40/\"/>"
"      <modbus:subattr modbus:name=\"Status5\" modbus:type=\"bool\" modbus:display=\"\" modbus:offset=\"/beginbyte/41/endbyte/41/\"/>"
"     </modbus:subattributes>"
"    </modbus:attr>"
"   </modbus:attributes>"
"  </modbus:interface>"
" </modbus:interfaces>"
"</modbus:description>";

char *xoSerializeAndFree(void *xo)
{
  void *xoWriter;
  char *subsXml;

  xoWriter = XoWritXmlMem(xo, 0, &subsXml, NULL);
  if  (!xoWriter)
  {
    fprintf(stderr, "ERROR cannot serialize (XML) xo '%s'\n",
      XoNmType(xo));
    XoFree(xo, 1);
    return  NULL;
  }
  XoWritXmlFreeCtxt(xoWriter);

  XoFree(xo, 1);
  return subsXml;
}


void test_buildXoFromProductDescriptor()
{
  int parseErr;
  int nbElt;
  void *xoProductDesc;
  void *xoAttr;
  void *xoObixRes;
  char *param;
  char *res;
  uint16_t array[] = {
      0x6772, 0x5425, 0xe77e, 0x4247, 
      0x0000, 0x4120, 0x0000, 0x41a0,
      0x0000, 0x41f0, 0x0000, 0x4220,
      0x0000, 0x4248, 0x0000, 0x4158,
      0x0000, 0x0001, 0x0100, 0x0000,
      0x0001
    };

//  char expectedRes[] = 
//"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n"
//"<obj xmlns=\"http://obix.org/ns/schema/1.1\">"
//"<abstime name=\"DATE_TEMP\" val=\"2014-09-26T15:17:38.0+02:00\"/>"
//"<real name=\"FREQ\" val=\"49.9761\"/>"
//"<real name=\"Pw_1\" val=\"10\"/>"
//"<real name=\"Pw_2\" val=\"20\"/>"
//"<real name=\"Pw_3\" val=\"30\"/>"
//"<real name=\"Pw_4\" val=\"40\"/>"
//"<real name=\"Pw_5\" val=\"50\"/>"
//"<real name=\"P_soutirage\" val=\"13.5\"/>"
//"<bool name=\"CD1\" val=\"false\"/>"
//"<bool name=\"CD2\" val=\"false\"/>"
//"<bool name=\"CD3\" val=\"true\"/>"
//"<bool name=\"CD4\" val=\"false\"/>"
//"<bool name=\"CD5\" val=\"false\"/>"
//"<bool name=\"Status1\" val=\"true\"/>"
//"<bool name=\"Status2\" val=\"false\"/>"
//"<bool name=\"Status3\" val=\"false\"/>"
//"<bool name=\"Status4\" val=\"true\"/>"
//"<bool name=\"Status5\" val=\"false\"/>"
//"</obj>";
//
  Attr_t *a;

  printf("test_buildXoFromProductDescriptor\n");

  xoProductDesc = XoReadXmlMem(attribute_wago_acy_custom, strlen(attribute_wago_acy_custom), 
    "modbus:description", 0, &parseErr);
  printf("parseErr = %d\n", parseErr);
  assert(0 == parseErr);

  xoAttr = XoNmGetAttr(xoProductDesc, "modbus:interfaces[0].modbus:attributes[%d]", 0, 0);
  assert(NULL != xoAttr);

  a = new_Attr_t();
  a->xoModelAttr = xoAttr;
  a->modbusType = (char *) XoNmGetAttr(xoAttr, "modbus:type", 0);
  assert(NULL != a->modbusType);
  obixTypeFromModbus(a->modbusType, &(a->modbusTypeID)); 
  a->modbusAccess = XoNmGetAttr(xoAttr, "modbus:reg", 0);
  assert(NULL != a->modbusAccess);

  decodeModbusAccess(a, a->modbusAccess);
  assert( a->reg == 12288 );
  assert( a->bytes == 42 );

  xoObixRes = a->buildXoFromProductDescriptor(a, array, 21);
  assert(NULL != xoObixRes);

  param = XoNmGetAttr(xoObixRes, "[name=DATE_TEMP].val", &parseErr); 
  assert( NULL != param);
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "2014-09-26T15:17:38.0+02:00")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=FREQ].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "49.9761")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=Pw_1].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "10")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=Pw_2].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "20")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=Pw_3].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "30")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=Pw_4].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "40")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=Pw_5].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "50")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=P_soutirage].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "13.5")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=CD1].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "false")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=CD2].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "false")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=CD3].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "true")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=CD4].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "false")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=CD5].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "false")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=Status1].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "true")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=Status2].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "false")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=Status3].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "false")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=Status4].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "true")); 
 
  param = XoNmGetAttr(xoObixRes, "[name=Status5].val", &parseErr); 
  assert( NULL != param); 
  printf("param=%s\n", param);
  assert( 0 == strcmp(param, "false")); 
 
  res = xoSerializeAndFree(xoObixRes);
  printf("resulting XML:\n%s\n", res);

  printf("OK\n");
}

int main(int argc, char *argv[])
{
  char path[256];
  int rc;
  char *rootactPath = getenv("ROOTACT");
  if ((!rootactPath || !*rootactPath)) {
    printf("ROOTACT unset, abort\n");
    exit(1);
  }

  XoInit(0);
  XoVersion();
  sprintf(path, "%s/modbus/xoref/", rootactPath);
  rc = XoLoadNameSpaceDir(path);
  assert(rc > 1);
  sprintf(path, "%s/modbus/xoref/modbus.xor", rootactPath);
  rc = XoExtLoadRef(path);
  assert(1 == rc);

  sprintf(path, "%s/m2mxoref/xoref/", rootactPath);
  rc = XoLoadNameSpaceDir(path);
  assert(rc > 1);
  sprintf(path, "%s/m2mxoref/xoref/xobix.xor", rootactPath);
  rc = XoExtLoadRef(path);
  assert(1 == rc);

  test_buildXoFromProductDescriptor();
  return 0;
}


