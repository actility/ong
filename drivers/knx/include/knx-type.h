#ifndef __KNX_TYPE__
#define __KNX_TYPE__

#include <stdint.h>
#include <stdbool.h>
#include <rtllist.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <signal.h>

#include "cmnstruct.h"
#include "eibclient.h"

/*
 *  DPT Type for Parser
 *
 *  Dpt_t
 *  Dpt_xxx_t
 *  Dpt_Ctx_t
 */
typedef struct _Dpt {
    uint16_t primary;
    uint16_t sub;
} Dpt_t;
#define DPT_IS_EQUAL(x,y) (x.primary == y.primary && x.sub == y.sub)
#define DPT_INIT(x, y) {.primary = x, .sub = y}
#define DPT_SET(dpt, x, y) do{dpt.primary = x; dpt.sub = y;}while(0)

typedef struct _Dpt_1 {
    bool b;
} Dpt_1_t;

typedef struct _Dpt_2 {
    bool v, c;
} Dpt_2_t;

typedef struct _Dpt_3 {
    bool b;
    uint32_t StepCode;
} Dpt_3_t;

typedef struct _Dpt_4 {
    char *character;
} Dpt_4_t;

typedef struct _Dpt_5 {
    uint32_t UnsignedValue;
} Dpt_5_t;

typedef struct _Dpt_6 {
    int32_t RelSignedValue, mode;
    bool a, b, c, d, e;
} Dpt_6_t;

typedef struct _Dpt_7 {
    uint32_t UnsignedValue, TimePeriod;
} Dpt_7_t;

typedef struct _Dpt_8 {
    int32_t SignedValue, DeltaTime;
} Dpt_8_t;

typedef struct _Dpt_9 {
    float FloatValue;
} Dpt_9_t;

typedef struct _Dpt_10 {
    uint32_t Day, Hour, Minutes, Seconds;
} Dpt_10_t;

typedef struct _Dpt_11 {
    uint32_t Year, Month, Day;
} Dpt_11_t;

typedef struct _Dpt_12 {
    uint32_t UnsignedValue;
} Dpt_12_t;

typedef struct _Dpt_13 {
    int32_t SignedValue;
} Dpt_13_t;

typedef struct _Dpt_14 {
    float FloatValue;
} Dpt_14_t;

typedef struct _Dpt_15 {
    bool c, e, d, p;
    uint32_t d1, d2, d3, d4, d5, d6, index;
} Dpt_15_t;

typedef struct _Dpt_16 {
    char *character;
} Dpt_16_t;

typedef struct _Dpt_17 {
   uint32_t SceneNumber;
} Dpt_17_t;

typedef struct _Dpt_18 {
    bool c;
    uint32_t SceneNumber;
} Dpt_18_t;

typedef struct _Dpt_19 {
    bool F, WD, NWD, NY, ND, NDOW, NT, SUTI, CLQ;
    uint32_t Year, Month, DayOfMonth, DayOfWeek, HourOfDay, Minutes, Seconds;
} Dpt_19_t;

typedef struct _Dpt_20 {
    uint32_t field1;
} Dpt_20_t;

typedef struct _Dpt_21 {
    bool OutOfService, Fault, Overridden, InAlarm, AlarmUnAck;
    bool UserStopped, OwnIA, VerifyMode;
} Dpt_21_t;

typedef struct _Dpt_23 {
    uint32_t s;
} Dpt_23_t;

typedef struct _Dpt_24 {
    char *character;
} Dpt_24_t;

typedef struct _Dpt_26 {
    bool b;
    uint32_t SceneNumber;
} Dpt_26_t;

typedef struct _Dpt_27 {
    bool s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11, s12, s13, s14, s15;
    bool m0, m1, m2, m3, m4, m5, m6, m7, m8, m9, m10, m11, m12, m13, m14, m15;
} Dpt_27_t;

typedef struct _Dpt_28 {
    char *character;
} Dpt_28_t;

typedef struct _Dpt_217 {
    uint32_t magicNumber, versionNumber, revisionNumber;
} Dpt_217_t;

typedef struct _Dpt_219 {
    bool Ack_Sup, TS_Sup, AlarmText_Sup, ErrorCode_Sup, InAlarm, AlarmUnAck, Locked;
    uint32_t LogNumber, AlarmPriority, ApplicationArea, ErrorClass;
} Dpt_219_t;

typedef struct _Dpt_221 {
    uint32_t manufacturerCode, incrementedCode;
} Dpt_221_t;

typedef struct _Dpt_229 {
    int32_t CountVal;
    uint8_t ValInfField;
} Dpt_229_t;

typedef union {
    Dpt_1_t dpt_1;
    Dpt_2_t dpt_2;
    Dpt_3_t dpt_3;
    Dpt_4_t dpt_4;
    Dpt_5_t dpt_5;
    Dpt_6_t dpt_6;
    Dpt_7_t dpt_7;
    Dpt_8_t dpt_8;
    Dpt_9_t dpt_9;
    Dpt_10_t dpt_10;
    Dpt_11_t dpt_11;
    Dpt_12_t dpt_12;
    Dpt_13_t dpt_13;
    Dpt_14_t dpt_14;
    Dpt_15_t dpt_15;
    Dpt_16_t dpt_16;
    Dpt_17_t dpt_17;
    Dpt_18_t dpt_18;
    Dpt_19_t dpt_19;
    Dpt_20_t dpt_20;
    Dpt_21_t dpt_21;
    Dpt_23_t dpt_23;
    Dpt_24_t dpt_24;
    Dpt_26_t dpt_26;
    Dpt_27_t dpt_27;
    Dpt_28_t dpt_28;
    Dpt_217_t dpt_217;
    Dpt_219_t dpt_219;
    Dpt_221_t dpt_221;
    Dpt_229_t dpt_229;
} Dpt_Ctx_t;

/*
 *  KNX types
 */
typedef uint16_t fp16_t;
typedef struct _DptInfos DptInfos_t;
typedef struct _NetworkList_t NetworkList_t;
typedef struct _ManufacturerInfos ManufacturerInfos_t;
typedef	struct _t_sensor t_sensor;
typedef struct _sensorList_t sensorList_t;
typedef struct _knxReport_t knxReport_t;
typedef struct _knxCov knxCov_t;

/*
 *  dpt.xml
 */
struct _DptInfos {
    char *name;
    char *unit;
    Dpt_t dpt;
    short bits;
};

/*
 *  manufacturer.xml
 */
struct _ManufacturerInfos {
    int id;
    char *name;
};

/*
 *  networks context
 */
struct _NetworkList_t {
    struct list_head list;
    
    bool isInit;            // Create on ONG ?
    bool toDelete;            // Delete pending on ONG ?
    
    t_sensor *device;
    struct list_head deviceApp;     // List of t_sensor for real device M2M  application
    struct list_head groupApp;      // List of t_sensor for groupAddr application
    
    char name[NAME_MAX];    // Name of the XML file without suffix
    void *xo;               // ETS XML Content
    
    char *eibdURI;                       // Address of the TCP interface of the KNX/IP Gateway device
    char *eibdGatewayURI;                // Address of the TCP interface of EIB Daemon
    int eibdPort;
    EIBConnection *eibdGroupReadCon;     // EIB handle to the group addr sniffer
    pid_t eibdPid;
    pid_t stdinLoggerPid;
};

/*
 *  CoV
 */
struct _knxCov {
    struct list_head list;
    Dpt_t dpt;
    bool primaryConfig; // Ignore the sub code, the cov cover all the primary dpt code
    int lastReportTime;
    int minInterval;
    int maxInterval;
};



/*
 *  From Sensor header
 */
#define 	cp_self         cp_cmn.cmn_self
#define 	cp_num          cp_cmn.cmn_num
#define 	cp_serial       cp_cmn.cmn_serial
#define 	cp_ieee         cp_cmn.cmn_ieee
#define 	cp_dia_devok    cp_cmn.cmn_dia_devok
#define	    cp_dia_appok    cp_cmn.cmn_dia_appok
#define	    cp_dia_datok    cp_cmn.cmn_dia_datok
#define 	cp_dia_metok    cp_cmn.cmn_dia_metok
#define 	cp_dia_accok    cp_cmn.cmn_dia_accok
#define 	cp_dia_repok    cp_cmn.cmn_dia_repok
#define	    cp_dia_reperr   cp_cmn.cmn_dia_reperr

struct _t_sensor
{
	t_cmn_sensor	cp_cmn;		

    /*
     *  Network handle for this device
     */
    NetworkList_t *network;
    
    // State of the device
    bool isInit;            // Created on ONG ?
    bool isAppInit;         // Always one application by device, created on ONG ?
    bool isCntInit;         // Always one Cnt in the APP
    bool isARInit;          // AR created ?
    bool isAR2Init;         // AR2 created ?
    bool isAR3Init;         // AR3 created ?
    bool toDelete;            // Delete pending on ONG ?
    
    // Allow the object to be linked in NetworkList_t
    struct list_head list;

    /*
     *  Xo Context into the network configuration file
     */
    void *xo;
    
    /*
     *  Device Addr or Group Addr depends of the deviceType
     */
    uint32_t knxId;

    char *groupAddrName;
    bool groupAddrSend;
    Dpt_t dpt;
    
    /*
     *  Used as M2M Application code
     */
    uint8_t deviceType;
#define DEVICE_TYPE_NOT_VALID           0
#define DEVICE_TYPE_GROUP_ADDR_DEVICE   1
#define DEVICE_TYPE_REAL_DEVICE         2
#define DEVICE_TYPE_NETWORK             3

    // The DPT-ID of the group Addr
    Dpt_t dptId;
    
    // Report
    struct {
        int minInterval;
        int maxInterval;
        bool waitingDia;          // request sended to GSC ?
        void *xo;                 // Obix document to report
        time_t time;              // Time of the obix value
        time_t lastTxTime;        // Date of last send
    } report;
};

#endif /* __KNX_TYPE__ */
