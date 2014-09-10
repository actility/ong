#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>

#include "xo.h"
#include "knx.h"

/*
 *  Create symbols need by Actility shared libs
 */
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
/* ---------- */

/*
 *  Create template variables need by KNX parser & XML renderer
 */
#define VAR_INIT_EMPTY(x) {.v_name = #x, .v_default = "", .v_fct = NULL, .v_value = NULL}
t_var TbVar[256] = {
    VAR_INIT_EMPTY(w_template),
    VAR_INIT_EMPTY(raw_a),
    VAR_INIT_EMPTY(raw_Ack_Sup),
    VAR_INIT_EMPTY(raw_AlarmUnAck),
    VAR_INIT_EMPTY(raw_AlarmPriority),
    VAR_INIT_EMPTY(raw_AlarmText_Sup),
    VAR_INIT_EMPTY(raw_AlarmUnAck_Sup),
    VAR_INIT_EMPTY(raw_ApplicationArea),
    VAR_INIT_EMPTY(raw_b),
	VAR_INIT_EMPTY(raw_c),
	VAR_INIT_EMPTY(raw_character),
	VAR_INIT_EMPTY(raw_CLQ),
	VAR_INIT_EMPTY(raw_d),
	VAR_INIT_EMPTY(raw_Day),
	VAR_INIT_EMPTY(raw_DayOfMonth),
	VAR_INIT_EMPTY(raw_DayOfWeek),
	VAR_INIT_EMPTY(raw_DeltaTime),
	VAR_INIT_EMPTY(raw_e),
	VAR_INIT_EMPTY(raw_ErrorCode_Sup),
	VAR_INIT_EMPTY(raw_ErrorClass),
	VAR_INIT_EMPTY(raw_field1),
	VAR_INIT_EMPTY(raw_F),
	VAR_INIT_EMPTY(raw_Fault),
	VAR_INIT_EMPTY(raw_FloatValue),
	VAR_INIT_EMPTY(raw_Hour),
	VAR_INIT_EMPTY(raw_HourOfDay),
	VAR_INIT_EMPTY(raw_incrementedCode),
	VAR_INIT_EMPTY(raw_InAlarm),
	VAR_INIT_EMPTY(raw_InAlarm_Sup),
	VAR_INIT_EMPTY(raw_m0),
	VAR_INIT_EMPTY(raw_m1),
	VAR_INIT_EMPTY(raw_m2),
	VAR_INIT_EMPTY(raw_m3),
	VAR_INIT_EMPTY(raw_m4),
	VAR_INIT_EMPTY(raw_m5),
	VAR_INIT_EMPTY(raw_m6),
	VAR_INIT_EMPTY(raw_m7),
	VAR_INIT_EMPTY(raw_m8),
	VAR_INIT_EMPTY(raw_m9),
	VAR_INIT_EMPTY(raw_m10),
	VAR_INIT_EMPTY(raw_m11),
	VAR_INIT_EMPTY(raw_m12),
	VAR_INIT_EMPTY(raw_m13),
	VAR_INIT_EMPTY(raw_m14),
	VAR_INIT_EMPTY(raw_m15),
	VAR_INIT_EMPTY(raw_magicNumber),
	VAR_INIT_EMPTY(raw_manufacturerCode),
	VAR_INIT_EMPTY(raw_Minutes),
	VAR_INIT_EMPTY(raw_mode),
	VAR_INIT_EMPTY(raw_Month),
	VAR_INIT_EMPTY(raw_NT),
	VAR_INIT_EMPTY(raw_ND),
	VAR_INIT_EMPTY(raw_NDOW),
	VAR_INIT_EMPTY(raw_NWD),
	VAR_INIT_EMPTY(raw_NY),
	VAR_INIT_EMPTY(raw_OutOfService),
	VAR_INIT_EMPTY(raw_Overridden),
	VAR_INIT_EMPTY(raw_OwnIA),
	VAR_INIT_EMPTY(raw_Locked),
	VAR_INIT_EMPTY(raw_Locked_Sup),
	VAR_INIT_EMPTY(raw_LogNumber),
	VAR_INIT_EMPTY(raw_v),
	VAR_INIT_EMPTY(raw_UnsignedValue),
	VAR_INIT_EMPTY(raw_UserStopped),
	VAR_INIT_EMPTY(raw_revisionNumber),
	VAR_INIT_EMPTY(raw_s),
	VAR_INIT_EMPTY(raw_s0),
	VAR_INIT_EMPTY(raw_s1),
	VAR_INIT_EMPTY(raw_s2),
	VAR_INIT_EMPTY(raw_s3),
	VAR_INIT_EMPTY(raw_s4),
	VAR_INIT_EMPTY(raw_s5),
	VAR_INIT_EMPTY(raw_s6),
	VAR_INIT_EMPTY(raw_s7),
	VAR_INIT_EMPTY(raw_s8),
	VAR_INIT_EMPTY(raw_s9),
	VAR_INIT_EMPTY(raw_s10),
	VAR_INIT_EMPTY(raw_s11),
	VAR_INIT_EMPTY(raw_s12),
	VAR_INIT_EMPTY(raw_s13),
	VAR_INIT_EMPTY(raw_s14),
	VAR_INIT_EMPTY(raw_s15),
	VAR_INIT_EMPTY(raw_SceneNumber),
	VAR_INIT_EMPTY(raw_Seconds),
	VAR_INIT_EMPTY(raw_StepCode),
	VAR_INIT_EMPTY(raw_SignedValue),
	VAR_INIT_EMPTY(raw_RelSignedValue),
	VAR_INIT_EMPTY(raw_TimePeriod),
	VAR_INIT_EMPTY(raw_TS_Sup),
	VAR_INIT_EMPTY(raw_VerifyMode),
	VAR_INIT_EMPTY(raw_versionNumber),
	VAR_INIT_EMPTY(raw_Year),
	VAR_INIT_EMPTY(raw_WD),
    VAR_INIT_EMPTY(raw_SUTI),
    VAR_INIT_EMPTY(dpt_raw),
	VAR_INIT_EMPTY(dpt_value)
};

void
assertTemplate(uint8_t *tpl, uint8_t *valid)
{
    int parseflags = XOML_PARSE_OBIX;

    void *xo = WXoNewTemplate(tpl, parseflags);
    if (xo != NULL) {
		char *buffer;
		void *w = XoWritXmlMem(xo, XO_FMT_STD_XML, &buffer, "");
		if (!w) {
			printf("XoWritXmlMem FAILED\n");
			XoFree(xo, 1);
		} else {
		    printf("Valid = %s\nParsed = %s-----------------\n", valid, buffer);
			assert(!strcmp(buffer, valid));
			XoWritXmlFreeCtxt(w);
		}
    }
}

int
main (int argc, char **argv)
{
    Dpt_t dpt;
    void *xo;
    uint8_t *buffer;
    uint8_t *valid;
	
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
    DPT_SET(dpt, 1, 001);
    buffer = "\x00";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<bool xmlns=\"http://obix.org/ns/schema/1.1\" val=\"false\">"
                "<bool name=\"b\" val=\"false\"/>"
                "<str name=\"raw\" val=\"0x00\"/>"
            "</bool>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 1, 001);
    buffer = "\x01";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<bool xmlns=\"http://obix.org/ns/schema/1.1\" val=\"true\">"
                "<bool name=\"b\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x01\"/>"
            "</bool>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 2
     */
    DPT_SET(dpt, 2, 001);
    buffer = "\x00";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<enum xmlns=\"http://obix.org/ns/schema/1.1\" val=\"noControl\">"
                "<bool name=\"v\" val=\"false\"/>"
                "<bool name=\"c\" val=\"false\"/>"
                "<str name=\"raw\" val=\"0x00\"/>"
            "</enum>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 2, 001);
    buffer = "\x01";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<enum xmlns=\"http://obix.org/ns/schema/1.1\" val=\"noControl\">"
                "<bool name=\"v\" val=\"true\"/>"
                "<bool name=\"c\" val=\"false\"/>"
                "<str name=\"raw\" val=\"0x01\"/>"
            "</enum>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 2, 001);
    buffer = "\x02";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<enum xmlns=\"http://obix.org/ns/schema/1.1\" val=\"controlOff\">"
                "<bool name=\"v\" val=\"false\"/>"
                "<bool name=\"c\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x02\"/>"
            "</enum>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 2, 001);
    buffer = "\x03";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<enum xmlns=\"http://obix.org/ns/schema/1.1\" val=\"controlOn\">"
                "<bool name=\"v\" val=\"true\"/>"
                "<bool name=\"c\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x03\"/>"
            "</enum>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 3
     */
    DPT_SET(dpt, 3, 001);
    buffer = "\x08";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"0.000000\">"
                "<bool name=\"b\" val=\"true\"/>"
                "<int name=\"StepCode\" val=\"0\"/>"
                "<str name=\"raw\" val=\"0x08\"/>"
            "</real>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 3, 001);
    buffer = "\x0A";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"25.000000\">"
                "<bool name=\"b\" val=\"true\"/>"
                "<int name=\"StepCode\" val=\"2\"/>"
                "<str name=\"raw\" val=\"0x0A\"/>"
            "</real>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 4
     */
    DPT_SET(dpt, 4, 2);
    buffer = "Hello, world !";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<str xmlns=\"http://obix.org/ns/schema/1.1\" val=\"Hello, world !\">"
                "<str name=\"raw\" val=\"0x48656C6C6F2C20776F726C642021\"/>"
            "</str>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 4, 2);
    buffer = "\xe4\xeb\xe9\xe8\xee\xb2\xac";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<str xmlns=\"http://obix.org/ns/schema/1.1\" val=\"äëéèî²¬\">"
                "<str name=\"raw\" val=\"0xE4EBE9E8EEB2AC\"/>"
            "</str>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 5
     */
    DPT_SET(dpt, 5, 001);
    buffer = "\x40";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"25.000000\">"
                "<int name=\"UnsignedValue\" val=\"64\"/>"
                "<str name=\"raw\" val=\"0x40\"/>"
            "</real>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 5, 001);
    buffer = "\x80";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"50.000000\">"
                "<int name=\"UnsignedValue\" val=\"128\"/>"
                "<str name=\"raw\" val=\"0x80\"/>"
            "</real>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 6
     */
    DPT_SET(dpt, 6, 001);
    buffer = "\x12";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<int xmlns=\"http://obix.org/ns/schema/1.1\" val=\"18\">"
                "<int name=\"RelSignedValue\" val=\"18\"/>"
                "<str name=\"raw\" val=\"0x12\"/>"
            "</int>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 6, 002);
    buffer = "\x55";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<obj xmlns=\"http://obix.org/ns/schema/1.1\">"
                "<bool name=\"a\" val=\"false\"/>"
                "<bool name=\"b\" val=\"true\"/>"
                "<bool name=\"c\" val=\"false\"/>"
                "<bool name=\"d\" val=\"true\"/>"
                "<bool name=\"e\" val=\"false\"/>"
                "<int name=\"mode\" val=\"5\"/>"
                "<str name=\"raw\" val=\"0x55\"/>"
            "</obj>\n";
    assertTemplate(xo, valid);


    /*
     *  DPT 7
     */
    DPT_SET(dpt, 7, 001);
    buffer = "\x02\x0F";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<int xmlns=\"http://obix.org/ns/schema/1.1\" val=\"527\">"
                "<int name=\"UnsignedValue\" val=\"527\"/>"
                "<str name=\"raw\" val=\"0x020F\"/>"
            "</int>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 7, 002);
    buffer = "\x40\x0F";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<reltime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"P16S,399\">"
                "<int name=\"TimePeriod\" val=\"16399\"/>"
                "<str name=\"raw\" val=\"0x400F\"/>"
            "</reltime>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 8
     */
    DPT_SET(dpt, 8, 001);
    buffer = "\x01\x00";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<int xmlns=\"http://obix.org/ns/schema/1.1\" val=\"256.000000\">"
                "<int name=\"SignedValue\" val=\"256\"/>"
                "<str name=\"raw\" val=\"0x0100\"/>"
            "</int>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 8, 002);
    buffer = "\x80\xFF";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<reltime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"-P0S,255\">"
                "<int name=\"DeltaTime\" val=\"-255\"/>"
                "<str name=\"raw\" val=\"0x80FF\"/>"
            "</reltime>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 9
     */
    // FFS : half float

    /*
     *  DPT 10
     */
    DPT_SET(dpt, 10, 001);
    buffer = "\x22\x03\x04";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<reltime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"T2:3:4\">"
                "<int name=\"Day\" val=\"1\"/>"
                "<int name=\"Hour\" val=\"2\"/>"
                "<int name=\"Minutes\" val=\"3\"/>"
                "<int name=\"Seconds\" val=\"4\"/>"
                "<str name=\"raw\" val=\"0x220304\"/>"
            "</reltime>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 11
     */
    DPT_SET(dpt, 11, 001);
    buffer = "\x01\x02\x03";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<date xmlns=\"http://obix.org/ns/schema/1.1\" val=\"2003-2-1\">"
                "<int name=\"Year\" val=\"3\"/>"
                "<int name=\"Month\" val=\"2\"/>"
                "<int name=\"Day\" val=\"1\"/>"
                "<str name=\"raw\" val=\"0x010203\"/>"
            "</date>\n";
    assertTemplate(xo, valid);


    /*
     *  DPT 12
     */
    DPT_SET(dpt, 12, 001);
    buffer = "\x01\x02\x03\x04";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<int xmlns=\"http://obix.org/ns/schema/1.1\" val=\"16909060\">"
                "<int name=\"UnsignedValue\" val=\"16909060\"/>"
                "<str name=\"raw\" val=\"0x01020304\"/>"
            "</int>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 13
     */
    DPT_SET(dpt, 13, 001);
    buffer = "\x01\x02\x03\x04";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<int xmlns=\"http://obix.org/ns/schema/1.1\" val=\"16909060\">"
                "<int name=\"SignedValue\" val=\"16909060\"/>"
                "<str name=\"raw\" val=\"0x01020304\"/>"
            "</int>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 13, 100);
    buffer = "\x80\x00\x00\xF0";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<reltime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"-P04M\">"
                "<int name=\"SignedValue\" val=\"-240\"/>"
                "<str name=\"raw\" val=\"0x800000F0\"/>"
            "</reltime>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 14
     */
    DPT_SET(dpt, 14, 1);
    buffer = "\x67\x7e\xf6\x42";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"123.246880\">"
                "<real name=\"FloatValue\" val=\"123.246880\"/>"
                "<str name=\"raw\" val=\"0x677EF642\"/>"
            "</real>\n";
    assertTemplate(xo, valid);

    // FFS DPT 15 !!
    // FFS DPT 16 !!

    /*
     *  DPT 17
     */
    DPT_SET(dpt, 17, 1);
    buffer = "\x05";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<int xmlns=\"http://obix.org/ns/schema/1.1\" val=\"5\">"
                "<int name=\"SceneNumber\" val=\"5\"/>"
                "<str name=\"raw\" val=\"0x05\"/>"
            "</int>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 18
     */
    DPT_SET(dpt, 18, 1);
    buffer = "\x85";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<obj xmlns=\"http://obix.org/ns/schema/1.1\">"
                "<bool name=\"c\" val=\"true\"/>"
                "<int name=\"SceneNumber\" val=\"5\"/>"
                "<str name=\"raw\" val=\"0x85\"/>"
            "</obj>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 19
     */
    DPT_SET(dpt, 19, 1);
    buffer = "\x01\x02\x03\x85\x06\x07\x55\x00";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<abstime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"00010203T050607Z\">"
                "<int name=\"Year\" val=\"1\"/>"
                "<int name=\"Month\" val=\"2\"/>"
                "<int name=\"DayOfMonth\" val=\"3\"/>"
                "<int name=\"DayOfWeek\" val=\"4\"/>"
                "<int name=\"HourOfDay\" val=\"5\"/>"
                "<int name=\"Minutes\" val=\"6\"/>"
                "<int name=\"Seconds\" val=\"7\"/>"
                "<bool name=\"F\" val=\"false\"/>"
                "<bool name=\"WD\" val=\"true\"/>"
                "<bool name=\"NWD\" val=\"false\"/>"
                "<bool name=\"NY\" val=\"true\"/>"
                "<bool name=\"ND\" val=\"false\"/>"
                "<bool name=\"NDOW\" val=\"true\"/>"
                "<bool name=\"NT\" val=\"false\"/>"
                "<bool name=\"SUTI\" val=\"true\"/>"
                "<bool name=\"CLQ\" val=\"false\"/>"
                "<str name=\"raw\" val=\"0x0102038506075500\"/>"
            "</abstime>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 20
     */
    DPT_SET(dpt, 20, 1);
    buffer = "\x02";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<enum xmlns=\"http://obix.org/ns/schema/1.1\" val=\"master\">"
                "<int name=\"field1\" val=\"2\"/>"
                "<str name=\"raw\" val=\"0x02\"/>"
            "</enum>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 21
     */
    DPT_SET(dpt, 21, 1);
    buffer = "\x55";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<bool xmlns=\"http://obix.org/ns/schema/1.1\" val=\"true\">"
                "<bool name=\"OutOfService\" val=\"true\"/>"
                "<bool name=\"Fault\" val=\"false\"/>"
                "<bool name=\"Overridden\" val=\"true\"/>"
                "<bool name=\"InAlarm\" val=\"false\"/>"
                "<bool name=\"AlarmUnAck\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x55\"/>"
            "</bool>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 21, 2);
    buffer = "\x55";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<bool xmlns=\"http://obix.org/ns/schema/1.1\" val=\"true\">"
                "<bool name=\"UserStopped\" val=\"true\"/>"
                "<bool name=\"OwnIA\" val=\"false\"/>"
                "<bool name=\"VerifyMode\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x55\"/>"
            "</bool>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 23
     */
    DPT_SET(dpt, 23, 2);
    buffer = "\x02";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<enum xmlns=\"http://obix.org/ns/schema/1.1\" val=\"alarmPositionIsDown\">"
                "<int name=\"s\" val=\"2\"/>"
                "<str name=\"raw\" val=\"0x02\"/>"
            "</enum>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 24
     */
    // FFS

    /*
     *  DPT 26
     */
    DPT_SET(dpt, 26, 2);
    buffer = "\x55";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<obj xmlns=\"http://obix.org/ns/schema/1.1\">"
                "<bool name=\"B\" val=\"true\"/>"
                "<int name=\"Scene-Number\" val=\"21\"/>"
                "<str name=\"raw\" val=\"0x55\"/>"
            "</obj>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 27
     */
    DPT_SET(dpt, 27, 1);
    buffer = "\x00\x55\xFF\x55";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
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
            "</obj>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 28
     */
    DPT_SET(dpt, 28, 1);
    buffer = "ⰹⰺⰻⰼⰽⰱⰰⰡⰔⰓⰆ";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<str xmlns=\"http://obix.org/ns/schema/1.1\" val=\"ⰹⰺⰻⰼⰽⰱⰰⰡⰔⰓⰆ\">"
                "<str name=\"raw\" val=\"0xE2B0B9E2B0BAE2B0BBE2B0BCE2B0BDE2B0B1E2B0B0E2B0A1E2B094E2B093E2B086\"/>"
            "</str>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 217
     */
    DPT_SET(dpt, 217, 1);
    buffer = "\x08\x83";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<str xmlns=\"http://obix.org/ns/schema/1.1\" val=\"1.2.3\">"
                "<int name=\"magicNumber\" val=\"1\"/>"
                "<int name=\"versionNumber\" val=\"2\"/>"
                "<int name=\"revisionNumber\" val=\"3\"/>"
                "<str name=\"raw\" val=\"0x0883\"/>"
            "</str>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 219
     */
    DPT_SET(dpt, 219, 1);
    buffer = "\x01\x02\x03\x04\x55\xAA";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<obj xmlns=\"http://obix.org/ns/schema/1.1\">"
                "<bool name=\"Ack_Sup\" val=\"true\"/>"
                "<bool name=\"TS_Sup\" val=\"false\"/>"
                "<bool name=\"AlarmText_Sup\" val=\"true\"/>"
                "<bool name=\"ErrorCode_Sup\" val=\"false\"/>"
                "<bool name=\"InAlarm_Sup\" val=\"false\"/>"
                "<bool name=\"AlarmUnAck_Sup\" val=\"true\"/>"
                "<bool name=\"Locked_Sup\" val=\"false\"/>"
                "<int name=\"LogNumber\" val=\"1\"/>"
                "<int name=\"AlarmPriority\" val=\"2\"/>"
                "<int name=\"ApplicationArea\" val=\"3\"/>"
                "<int name=\"ErrorClass\" val=\"4\"/>"
                "<str name=\"raw\" val=\"0x0102030455AA\"/>"
            "</obj>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 221
     */
    DPT_SET(dpt, 221, 1);
    buffer = "\x00\x06\xF8\x55\x00\x7B";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<str xmlns=\"http://obix.org/ns/schema/1.1\" val=\"123.456789\">"
                "<int name=\"manufacturerCode\" val=\"123\"/>"
                "<int name=\"incrementedCode\" val=\"456789\"/>"
                "<str name=\"raw\" val=\"0x0006F855007B\"/>"
            "</str>\n";
    assertTemplate(xo, valid);


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
    DPT_SET(dpt, 202, 1);
    buffer = "\x55\x40";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"64.000000\">"
                "<int name=\"UnsignedValue\" val=\"64\"/>"
                "<bool name=\"OutOfService\" val=\"true\"/>"
                "<bool name=\"Fault\" val=\"false\"/>"
                "<bool name=\"Overridden\" val=\"true\"/>"
                "<bool name=\"InAlarm\" val=\"false\"/>"
                "<bool name=\"AlarmUnAck\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x5540\"/>"
            "</real>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 203
     */
    DPT_SET(dpt, 203, 002);
    buffer = "\x55\x40\x0F";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<reltime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"P16S,399\">"
                "<int name=\"TimePeriod\" val=\"16399\"/>"
                "<bool name=\"OutOfService\" val=\"true\"/>"
                "<bool name=\"Fault\" val=\"false\"/>"
                "<bool name=\"Overridden\" val=\"true\"/>"
                "<bool name=\"InAlarm\" val=\"false\"/>"
                "<bool name=\"AlarmUnAck\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x55400F\"/>"
            "</reltime>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 203, 11);
    buffer = "\x55\x67\x7e\xf6\x42";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"123.246880\">"
                "<real name=\"FloatValue\" val=\"123.246880\"/>"
                "<bool name=\"OutOfService\" val=\"true\"/>"
                "<bool name=\"Fault\" val=\"false\"/>"
                "<bool name=\"Overridden\" val=\"true\"/>"
                "<bool name=\"InAlarm\" val=\"false\"/>"
                "<bool name=\"AlarmUnAck\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x55677EF642\"/>"
            "</real>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 203, 12);
    buffer = "\x55\x02\x0F";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<int xmlns=\"http://obix.org/ns/schema/1.1\" val=\"527\">"
                "<int name=\"UnsignedValue\" val=\"527\"/>"
                "<bool name=\"OutOfService\" val=\"true\"/>"
                "<bool name=\"Fault\" val=\"false\"/>"
                "<bool name=\"Overridden\" val=\"true\"/>"
                "<bool name=\"InAlarm\" val=\"false\"/>"
                "<bool name=\"AlarmUnAck\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x55020F\"/>"
            "</int>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 203, 14);
    buffer = "\x55\x80\xC8";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"-2.000000\">"
                "<real name=\"FloatValue\" val=\"-2.000000\"/>"
                "<bool name=\"OutOfService\" val=\"true\"/>"
                "<bool name=\"Fault\" val=\"false\"/>"
                "<bool name=\"Overridden\" val=\"true\"/>"
                "<bool name=\"InAlarm\" val=\"false\"/>"
                "<bool name=\"AlarmUnAck\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x5580C8\"/>"
            "</real>\n";
    assertTemplate(xo, valid);

    DPT_SET(dpt, 203, 17);
    buffer = "\x55\x40";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"25.000000\">"
                "<int name=\"UnsignedValue\" val=\"64\"/>"
                "<bool name=\"OutOfService\" val=\"true\"/>"
                "<bool name=\"Fault\" val=\"false\"/>"
                "<bool name=\"Overridden\" val=\"true\"/>"
                "<bool name=\"InAlarm\" val=\"false\"/>"
                "<bool name=\"AlarmUnAck\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x5540\"/>"
            "</real>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 204
     */
    DPT_SET(dpt, 204, 1);
    buffer = "\x55\x12";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<int xmlns=\"http://obix.org/ns/schema/1.1\" val=\"18\">"
                "<int name=\"RelSignedValue\" val=\"18\"/>"
                "<bool name=\"OutOfService\" val=\"true\"/>"
                "<bool name=\"Fault\" val=\"false\"/>"
                "<bool name=\"Overridden\" val=\"true\"/>"
                "<bool name=\"InAlarm\" val=\"false\"/>"
                "<bool name=\"AlarmUnAck\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x5512\"/>"
            "</int>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 205
     */
    DPT_SET(dpt, 205, 2);
    buffer = "\x55\x80\xFF";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<reltime xmlns=\"http://obix.org/ns/schema/1.1\" val=\"-P0S,255\">"
                "<int name=\"DeltaTime\" val=\"-255\"/>"
                "<bool name=\"OutOfService\" val=\"true\"/>"
                "<bool name=\"Fault\" val=\"false\"/>"
                "<bool name=\"Overridden\" val=\"true\"/>"
                "<bool name=\"InAlarm\" val=\"false\"/>"
                "<bool name=\"AlarmUnAck\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x5580FF\"/>"
            "</reltime>\n";
    assertTemplate(xo, valid);

    /*
     *  DPT 218
     */
    DPT_SET(dpt, 218, 1);
    buffer = "\x55\x67\x7e\xf6\x42";
    knx2obix(dpt, buffer, &xo);
    valid = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
            "<real xmlns=\"http://obix.org/ns/schema/1.1\" val=\"123.246880\">"
                "<real name=\"FloatValue\" val=\"123.246880\"/>"
                "<bool name=\"OutOfService\" val=\"true\"/>"
                "<bool name=\"Fault\" val=\"false\"/>"
                "<bool name=\"Overridden\" val=\"true\"/>"
                "<bool name=\"InAlarm\" val=\"false\"/>"
                "<bool name=\"AlarmUnAck\" val=\"true\"/>"
                "<str name=\"raw\" val=\"0x55677EF642\"/>"
            "</real>\n";
    assertTemplate(xo, valid);

printf("\
        ,o888888o.     8 8888     ,88' \n\
     . 8888     `88.   8 8888    ,88'  \n\
    ,8 8888       `8b  8 8888   ,88'   \n\
    88 8888        `8b 8 8888  ,88'    \n\
    88 8888         88 8 8888 ,88'     \n\
    88 8888         88 8 8888 88'      \n\
    88 8888        ,8P 8 888888<       \n\
    `8 8888       ,8P  8 8888 `Y8.     \n\
     ` 8888     ,88'   8 8888   `Y8.   \n\
        `8888888P'     8 8888     `Y8. \n");

    return 0;
}

