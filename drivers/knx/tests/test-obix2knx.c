#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>

#include "ok.h"
#include "xo.h"
#include "knx.h"

/*
 *  Create symbols need by Actility shared libs
 */
#include "cmnstruct.h"
int TraceLevel = 9;
int TraceDebug = 9;
int TraceProto = 9;
int Operating = 0;
int	NbVar;
int	InternalVar;
void *MainTbPoll;
char *rootactPath;
char *GetAdaptorVersion() {return "knx-0.0.1-unit-tests";}
char *GetAdaptorName() {return "knx";}
char *GetEnvVarName(void) {if (!Operating){return "ROOTACT";} return NULL;}
t_var TbVar[256] = {
	VAR_INIT_EMPTY(foobar)
};
/* ---------- */

#define INIT_XO(str) \
    xo = (void *) XoReadXmlMem(str, strlen(str), NULL, XOML_PARSE_OBIX, &err);      \
    if (xo == NULL) { printf("Error on XoReadXmlMem\n"); exit(1); }

int
main (int argc, char **argv)
{
    uint8_t knxOut[1024];
    int knxOutLen;
    void *xo;
    int	err;
    Dpt_t dpt;

    // Init actility stuff
    char tmp[512];
    rootactPath = getenv("ROOTACT");
    if ((!rootactPath || !*rootactPath)) {
        printf("ROOTACT unset, abort\n");
        exit(1);
    }
    VarInit();
    XoInit(0);
    if (!Operating) {
        sprintf(tmp, "%s/%s/xo/.", rootactPath, GetAdaptorName());
        XoLoadNameSpaceDir(tmp);
        XoLoadNameSpaceDir(".");
    } else {
        sprintf(tmp, "%s/etc/xo/.", rootactPath);
        XoLoadNameSpaceDir(tmp);
        sprintf(tmp, "%s/etc/%s/.", rootactPath, GetAdaptorName());
        XoLoadNameSpaceDir(tmp);
    }

    if (LoadM2MXoRef() < 0)
        exit(1);

    if (LoadDrvXoRef(GetAdaptorName()) < 0)
        exit(1);

    // Init KNX Stuff
    RangeLoadDatabase();


    /****************************************************************************************

                           d888888o. 8888888 8888888888 8 888888888o.
                         .`8888:' `88.     8 8888       8 8888    `^888.
                         8.`8888.   Y8     8 8888       8 8888        `88.
                         `8.`8888.         8 8888       8 8888         `88
                          `8.`8888.        8 8888       8 8888          88
                           `8.`8888.       8 8888       8 8888          88
                            `8.`8888.      8 8888       8 8888         ,88
                        8b   `8.`8888.     8 8888       8 8888        ,88'
                        `8b.  ;8.`8888     8 8888       8 8888    ,o88P'
                         `Y8888P ,88P'     8 8888       8 888888888P'

     ****************************************************************************************/

    /*
     *  DPT 1
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 1, 1);
    INIT_XO("<bool xmlns=\"http://obix.org/ns/schema/1.1\" val=\"true\">"
                "<bool name=\"b\" val=\"true\"/>"
            "</bool>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 1);
    assert((knxOut[0] & 0x01) == 0x01);
    XoFree(xo);

    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 1, 1);
    INIT_XO("<bool xmlns=\"http://obix.org/ns/schema/1.1\" val=\"true\">"
                "<bool name=\"b\" val=\"false\"/>"
            "</bool>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 1);
    assert((knxOut[0] & 0x01) == 0x00);
    XoFree(xo);

    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 1, 1);
    INIT_XO("<bool xmlns=\"http://obix.org/ns/schema/1.1\" val=\"false\">"
                "<bool name=\"b\" val=\"true\"/>"
            "</bool>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 1);
    assert((knxOut[0] & 0x01) == 0x01);
    XoFree(xo);

    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 1, 1);
    INIT_XO("<bool xmlns=\"http://obix.org/ns/schema/1.1\" val=\"false\">"
                "<bool name=\"b\" val=\"false\"/>"
            "</bool>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 1);
    assert((knxOut[0] & 0x01) == 0x00);
    XoFree(xo);

    /*
     *  DPT 2
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 2, 1);
    INIT_XO("<enum xmlns=\"http://obix.org/ns/schema/1.1\" val=\"noControl\">"
                "<bool name=\"v\" val=\"false\"/>"
                "<bool name=\"c\" val=\"false\"/>"
            "</enum>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 2);
    assert((knxOut[0] & 0x03) == 0x00);
    XoFree(xo);

    /*
     *  DPT 3
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 3, 1);
    INIT_XO("<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"25.000000\">"
                "<bool name=\"b\" val=\"true\"/>"
                "<int name=\"StepCode\" val=\"2\"/>"
            "</real>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 4);
    assert(knxOut[0] == 0x0A);
    XoFree(xo);

    /*
     *  DPT 4
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 4, 1);
    INIT_XO("<str xmlns=\"http://obix.org/ns/schema/1.1\" val=\"Hello\"/>\n");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 48);
    assert(!strncmp(knxOut, "Hello", 6));
    XoFree(xo);

    /*
     *  DPT 5
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 5, 1);
    INIT_XO("<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"50.000000\">"
                "<int name=\"UnsignedValue\" val=\"128\"/>"
            "</real>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 8);
    assert(knxOut[0] == 0x80);
    XoFree(xo);


    /*
     *  DPT 6
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 6, 1);
    INIT_XO("<int xmlns=\"http://obix.org/ns/schema/1.1\" val=\"18\">"
                "<int name=\"RelSignedValue\" val=\"18\"/>"
            "</int>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 8);
    assert(knxOut[0] == 18);
    XoFree(xo);

    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 6, 2);
    INIT_XO("<obj xmlns=\"http://obix.org/ns/schema/1.1\">"
                "<bool name=\"a\" val=\"false\"/>"
                "<bool name=\"b\" val=\"true\"/>"
                "<bool name=\"c\" val=\"false\"/>"
                "<bool name=\"d\" val=\"true\"/>"
                "<bool name=\"e\" val=\"false\"/>"
                "<int name=\"mode\" val=\"5\"/>"
            "</obj>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 8);
    assert(knxOut[0] == 0x55);
    XoFree(xo);

    /*
     *  DPT 7
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 7, 1);
    INIT_XO("<int xmlns=\"http://obix.org/ns/schema/1.1\" val=\"527\">"
                "<int name=\"UnsignedValue\" val=\"527\"/>"
            "</int>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 16);
    assert(knxOut[0] == 0x02);
    assert(knxOut[1] == 0x0F);
    XoFree(xo);

    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 7, 5);
    INIT_XO("<reltime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"P2H3M4S\"/>\n");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 16);
    assert(knxOut[0] == 0x1C);
    assert(knxOut[1] == 0xD8);
    XoFree(xo);

    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 7, 2);
    INIT_XO("<reltime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"P8S,123\"/>\n");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 16);
    assert(knxOut[0] == 0x1F);
    assert(knxOut[1] == 0xBB);
    XoFree(xo);
    
    /*
     *  DPT 8
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 7, 1);
    INIT_XO("<int xmlns=\"http://obix.org/ns/schema/1.1\" val=\"256\">"
                "<int name=\"SignedValue\" val=\"256\"/>"
            "</int>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 16);
    assert(knxOut[0] == 0x01);
    assert(knxOut[1] == 0x00);
    XoFree(xo);

    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 8, 5);
    INIT_XO("<reltime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"P2H3M4S\"/>\n");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 16);
    assert(knxOut[0] == 0x1C);
    assert(knxOut[1] == 0xD8);
    XoFree(xo);

    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 8, 2);
    INIT_XO("<reltime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"-P0S,255\" />");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 16);
    assert(knxOut[0] == 0xFF);
    assert(knxOut[1] == 0x01);
    XoFree(xo);
    
    /*
     *  DPT 9
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 9, 2);
    INIT_XO("<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"-2.0\" />");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 16);
    printf("%hhu %hhu\n", knxOut[0], knxOut[1]);
    assert(knxOut[0] == 0xC8);
    assert(knxOut[1] == 0x80);
    XoFree(xo);

    /*
     *  DPT 10
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 10, 1);
    INIT_XO("<reltime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"T2:3:4\"/>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 24);
    assert(knxOut[0] == 0x02);
    assert(knxOut[1] == 0x03);
    assert(knxOut[2] == 0x04);
    XoFree(xo);

    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 10, 1);
    INIT_XO("<reltime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"T2:3:4\">"
                "<int name=\"Day\" val=\"1\"/>"
            "</reltime>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 24);
    assert(knxOut[0] == 0x22);
    assert(knxOut[1] == 0x03);
    assert(knxOut[2] == 0x04);
    XoFree(xo);

    /*
     *  DPT 11
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 11, 1);
    INIT_XO("<date xmlns=\"http://obix.org/ns/schema/1.1\" val=\"1999-09-10\"/>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 24);
    assert(knxOut[0] == 0x0A);
    assert(knxOut[1] == 0x09);
    assert(knxOut[2] == 0x63);
    XoFree(xo);

    /*
     *  DPT 12
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 12, 1);
    INIT_XO("<int xmlns=\"http://obix.org/ns/schema/1.1\" val=\"16909060\">"
                "<int name=\"UnsignedValue\" val=\"16909060\"/>"
            "</int>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 32);
    assert(knxOut[0] == 0x01);
    assert(knxOut[1] == 0x02);
    assert(knxOut[2] == 0x03);
    assert(knxOut[3] == 0x04);
    XoFree(xo);

    /*
     *  DPT 13
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 13, 1);
    INIT_XO("<int xmlns=\"http://obix.org/ns/schema/1.1\" val=\"16909060\">"
                "<int name=\"SignedValue\" val=\"16909060\"/>"
            "</int>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 32);
    assert(knxOut[0] == 0x01);
    assert(knxOut[1] == 0x02);
    assert(knxOut[2] == 0x03);
    assert(knxOut[3] == 0x04);
    XoFree(xo);
    
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 13, 100);
    INIT_XO("<reltime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"P1000D\"/>\n");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 32);
    assert(knxOut[0] == 0x05);
    assert(knxOut[1] == 0x26);
    assert(knxOut[2] == 0x5C);
    assert(knxOut[3] == 0x00);
    XoFree(xo);

    /*
     *  DPT 14
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 14, 2);
    INIT_XO("<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"-2.0\" />");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 32);
    assert(knxOut[0] == 0x00);
    assert(knxOut[1] == 0x00);
    assert(knxOut[2] == 0x00);
    assert(knxOut[3] == 0xC0);
    XoFree(xo);

    /*
     *  DPT 15
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 15, 1);
    INIT_XO("<obj xmlns=\"http://obix.org/ns/schema/1.1\">"
                "<int name=\"d1\" val=\"1\"/>"
                "<int name=\"d2\" val=\"2\"/>"
                "<int name=\"d3\" val=\"3\"/>"
                "<int name=\"d4\" val=\"4\"/>"
                "<int name=\"d5\" val=\"5\"/>"
                "<int name=\"d6\" val=\"6\"/>"
                "<int name=\"index\" val=\"5\"/>"
                "<bool name=\"e\" val=\"true\"/>"
                "<bool name=\"p\" val=\"false\"/>"
                "<bool name=\"d\" val=\"true\"/>"
                "<bool name=\"c\" val=\"false\"/>"
            "</obj>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 32);
    assert(knxOut[0] == 0x65);
    assert(knxOut[1] == 0x43);
    assert(knxOut[2] == 0x21);
    assert(knxOut[3] == 0xA5);
    XoFree(xo);

    /*
     *  DPT 16 (ASCII)
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 16, 0);
    INIT_XO("<str xmlns=\"http://obix.org/ns/schema/1.1\" val=\"Hello\"/>\n");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 48);
    assert(!strncmp(knxOut, "Hello", 6));
    XoFree(xo);

    /*
     *  DPT 16 (Latin 1)
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 16, 1);
    INIT_XO("<str xmlns=\"http://obix.org/ns/schema/1.1\" val=\"hé110\"/>\n");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 48);
    assert(!strncmp(knxOut, "h" "\xE9" "110", 6));
    XoFree(xo);
    
    /*
     *  DPT 17
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 17, 1);
    INIT_XO("<int xmlns=\"http://obix.org/ns/schema/1.1\" val=\"5\">"
                "<int name=\"SceneNumber\" val=\"5\"/>"
            "</int>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 8);
    assert(knxOut[0] == 0x05);
    XoFree(xo);

    /*
     *  DPT 18
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 18, 1);
    INIT_XO("<obj xmlns=\"http://obix.org/ns/schema/1.1\">"
                "<bool name=\"c\" val=\"true\"/>"
                "<int name=\"SceneNumber\" val=\"5\"/>"
            "</obj>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 8);
    assert(knxOut[0] == 0x85);
    XoFree(xo);

    /*
     *  DPT 19 : Abstime
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 19, 1);
    INIT_XO("<abstime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"2013-02-03T05:06:07Z\"/>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 64);
    assert(knxOut[0] == 0x71);
    assert(knxOut[1] == 0x02);
    assert(knxOut[2] == 0x03);
    assert(knxOut[3] == 0x05);
    assert(knxOut[4] == 0x06);
    assert(knxOut[5] == 0x07);
    assert(knxOut[6] == 0x24);
    assert(knxOut[7] == 0x00);
    XoFree(xo);

    /*
     *  DPT 20
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 20, 1);
    INIT_XO("<enum xmlns=\"http://obix.org/ns/schema/1.1\" val=\"master\">"
                "<int name=\"field1\" val=\"2\"/>"
            "</enum>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 8);
    assert((knxOut[0] & 0x3F) == 0x02);
    XoFree(xo);

    /*
     *  DPT 21
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 21, 1);
    INIT_XO("<bool xmlns=\"http://obix.org/ns/schema/1.1\" val=\"true\">"
                "<bool name=\"OutOfService\" val=\"true\"/>"
                "<bool name=\"Fault\" val=\"false\"/>"
                "<bool name=\"Overridden\" val=\"true\"/>"
                "<bool name=\"InAlarm\" val=\"false\"/>"
                "<bool name=\"AlarmUnAck\" val=\"true\"/>"
            "</bool>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 8);
    assert(knxOut[0] == 0x15);
    XoFree(xo);

    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 21, 2);
    INIT_XO("<bool xmlns=\"http://obix.org/ns/schema/1.1\" val=\"true\">"
                "<bool name=\"UserStopped\" val=\"true\"/>"
                "<bool name=\"OwnIA\" val=\"false\"/>"
                "<bool name=\"VerifyMode\" val=\"true\"/>"
            "</bool>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 8);
    assert(knxOut[0] == 0x05);
    XoFree(xo);

    /*
     *  DPT 23
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 23, 2);
    INIT_XO("<enum xmlns=\"http://obix.org/ns/schema/1.1\" val=\"alarmPositionIsDown\">"
                "<int name=\"s\" val=\"2\"/>"
            "</enum>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 2);
    assert(knxOut[0] == 0x02);
    XoFree(xo);

    /*
     *  DPT 24
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 24, 1);
    INIT_XO("<str xmlns=\"http://obix.org/ns/schema/1.1\" val=\"Hello\"/>\n");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 48);
    assert(!strncmp(knxOut, "Hello", 6));
    XoFree(xo);

    // FFS : Add other test with multibyte character

    /*
     *  DPT 26
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 26, 1);
    INIT_XO("<obj xmlns=\"http://obix.org/ns/schema/1.1\">"
                "<bool name=\"b\" val=\"true\"/>"
                "<int name=\"SceneNumber\" val=\"5\"/>"
            "</obj>");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 8);
    assert(knxOut[0] == 0x45);
    XoFree(xo);

    /*
     *  DPT 27
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 27, 1);
    INIT_XO("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<obj xmlns=\"http://obix.org/ns/schema/1.1\">"
                "<bool name=\"s0\" val=\"true\"/>"
                "<bool name=\"s1\" val=\"false\"/>"
                "<bool name=\"s2\" val=\"true\"/>"
                "<bool name=\"s3\" val=\"false\"/>"
                "<bool name=\"s4\" val=\"true\"/>"
                "<bool name=\"s5\" val=\"false\"/>"
                "<bool name=\"s6\" val=\"true\"/>"
                "<bool name=\"s7\" val=\"false\"/>"
                "<bool name=\"s8\" val=\"true\"/>"
                "<bool name=\"s9\" val=\"true\"/>"
                "<bool name=\"s10\" val=\"true\"/>"
                "<bool name=\"s11\" val=\"true\"/>"
                "<bool name=\"s12\" val=\"true\"/>"
                "<bool name=\"s13\" val=\"true\"/>"
                "<bool name=\"s14\" val=\"true\"/>"
                "<bool name=\"s15\" val=\"true\"/>"
                "<bool name=\"m0\" val=\"true\"/>"
                "<bool name=\"m1\" val=\"false\"/>"
                "<bool name=\"m2\" val=\"true\"/>"
                "<bool name=\"m3\" val=\"false\"/>"
                "<bool name=\"m4\" val=\"true\"/>"
                "<bool name=\"m5\" val=\"false\"/>"
                "<bool name=\"m6\" val=\"true\"/>"
                "<bool name=\"m7\" val=\"false\"/>"
                "<bool name=\"m8\" val=\"false\"/>"
                "<bool name=\"m9\" val=\"false\"/>"
                "<bool name=\"m10\" val=\"false\"/>"
                "<bool name=\"m11\" val=\"false\"/>"
                "<bool name=\"m12\" val=\"false\"/>"
                "<bool name=\"m13\" val=\"false\"/>"
                "<bool name=\"m14\" val=\"false\"/>"
                "<bool name=\"m15\" val=\"false\"/>"
                "<str name=\"raw\" val=\"0x0055FF55\"/>"
            "</obj>\n");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 32);
    assert(knxOut[0] == 0x00);
    assert(knxOut[1] == 0x55);
    assert(knxOut[2] == 0xFF);
    assert(knxOut[3] == 0x55);
    XoFree(xo);

    /*
     *  DPT 28
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 24, 1);
    INIT_XO("<str xmlns=\"http://obix.org/ns/schema/1.1\" val=\"Hello\"/>\n");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 48);
    assert(!strncmp(knxOut, "Hello", 6));
    XoFree(xo);
    
    /*
     *  DPT 217
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 217, 1);
    INIT_XO("<str xmlns=\"http://obix.org/ns/schema/1.1\" val=\"1.2.3\">"
                "<int name=\"magicNumber\" val=\"1\"/>"
                "<int name=\"versionNumber\" val=\"2\"/>"
                "<int name=\"revisionNumber\" val=\"3\"/>"
            "</str>\n");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 16);
    assert(knxOut[0] == 0x08);
    assert(knxOut[1] == 0x83);
    XoFree(xo);

    /*
     *  DPT 219
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 219, 1);
    INIT_XO("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<obj xmlns=\"http://obix.org/ns/schema/1.1\">"
                "<bool name=\"Ack_Sup\" val=\"true\"/>"
                "<bool name=\"TS_Sup\" val=\"false\"/>"
                "<bool name=\"AlarmText_Sup\" val=\"true\"/>"
                "<bool name=\"ErrorCode_Sup\" val=\"false\"/>"
                "<bool name=\"InAlarm\" val=\"false\"/>"
                "<bool name=\"AlarmUnAck\" val=\"true\"/>"
                "<bool name=\"Locked\" val=\"false\"/>"
                "<int name=\"LogNumber\" val=\"1\"/>"
                "<int name=\"AlarmPriority\" val=\"2\"/>"
                "<int name=\"ApplicationArea\" val=\"3\"/>"
                "<int name=\"ErrorClass\" val=\"4\"/>"
            "</obj>\n");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 48);
    assert(knxOut[0] == 0x01);
    assert(knxOut[1] == 0x02);
    assert(knxOut[2] == 0x03);
    assert(knxOut[3] == 0x04);
    assert(knxOut[4] == 0x05);
    assert(knxOut[5] == 0x02);
    XoFree(xo);

    /*
     *  DPT 221
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 221, 1);
    INIT_XO("<str xmlns=\"http://obix.org/ns/schema/1.1\" val=\"123.456789\">"
                "<int name=\"manufacturerCode\" val=\"123\"/>"
                "<int name=\"incrementedCode\" val=\"456789\"/>"
            "</str>\n");
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 48);
    assert(knxOut[0] == 0x00);
    assert(knxOut[1] == 0x06);
    assert(knxOut[2] == 0xF8);
    assert(knxOut[3] == 0x55);
    assert(knxOut[4] == 0x00);
    assert(knxOut[5] == 0x7B);
    XoFree(xo);

    /****************************************************************************************

                8 8888        8 `8.`888b           ,8' .8.           ,o888888o.
                8 8888        8  `8.`888b         ,8' .888.         8888     `88.
                8 8888        8   `8.`888b       ,8' :88888.     ,8 8888       `8.
                8 8888        8    `8.`888b     ,8' . `88888.    88 8888
                8 8888        8     `8.`888b   ,8' .8. `88888.   88 8888
                8 8888        8      `8.`888b ,8' .8`8. `88888.  88 8888
                8 8888888888888       `8.`888b8' .8' `8. `88888. 88 8888
                8 8888        8        `8.`888' .8'   `8. `88888.`8 8888       .8'
                8 8888        8         `8.`8' .888888888. `88888.  8888     ,88'
                8 8888        8          `8.` .8'       `8. `88888.  `8888888P'

     ****************************************************************************************/

    /*
     *  DPT 202
     */
    memset(knxOut, 0, 1024);
    DPT_SET(dpt, 202, 1);
    INIT_XO("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"64.000000\">"
                "<int name=\"UnsignedValue\" val=\"64\"/>"
                "<bool name=\"OutOfService\" val=\"true\"/>"
                "<bool name=\"Fault\" val=\"false\"/>"
                "<bool name=\"Overridden\" val=\"true\"/>"
                "<bool name=\"InAlarm\" val=\"false\"/>"
                "<bool name=\"AlarmUnAck\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x4055\"/>"
            "</real>\n");;
    obix2knx(dpt, xo, knxOut, &knxOutLen);
    assert(knxOutLen == 8 + 8);
    assert(knxOut[0] == 0x15);
    assert(knxOut[1] == 0x40);
    XoFree(xo);


    PRINT_OK();
    return 0;
}

