#ifndef __MODBUS_TYPE__
#define __MODBUS_TYPE__

#include <stdint.h>
#include <stdbool.h>
#include <rtllist.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <signal.h>

#include "cmnstruct.h"
#include "modbus/modbus-rtu.h"
#include "modbus/modbus-tcp.h"

typedef struct _sensor_t t_sensor;
typedef struct _sensor_t Sensor_t;
typedef struct _network_t Network_t;
typedef struct _product_t Product_t;
typedef struct _Point_t Point_t;
typedef struct _Attr_t Attr_t;
typedef struct _Value_t Value_t;


/*
 * From Sensor header
 */
#define cp_self       cp_cmn.cmn_self
#define cp_num        cp_cmn.cmn_num
#define cp_serial     cp_cmn.cmn_serial
#define cp_ieee       cp_cmn.cmn_ieee
#define cp_dia_devok  cp_cmn.cmn_dia_devok
#define cp_dia_appok  cp_cmn.cmn_dia_appok
#define cp_dia_datok  cp_cmn.cmn_dia_datok
#define cp_dia_metok  cp_cmn.cmn_dia_metok
#define cp_dia_accok  cp_cmn.cmn_dia_accok
#define cp_dia_repok  cp_cmn.cmn_dia_repok
#define cp_dia_reperr cp_cmn.cmn_dia_reperr

#define CP_NUM_NETWORK  1
#define CP_NUM_DEVICE   2

struct _sensor_t
{
	t_cmn_sensor	cp_cmn;
	struct list_head list;    // List the device into the network
	struct list_head M2Mlist;   // List the device into M2M pending action
	Network_t *network;
	
	char *addr;
	char *product;
	char *name;
	void **xo;          // Ref to a product->xo
	
	// Serial
	int serial_addr;
	
	// Ethernet
	int tcp_port;
	char *tcp_ip;
	modbus_t *modbusCtx;
	
	// M2M
	bool isInit;
	bool toDelete;
	bool isARInit;
	bool isAR2Init;
	bool isAR3Init;
	bool isAppInit;
	struct list_head cnts;
	struct list_head ops;
};

struct _product_t {
  struct list_head list;
  int id;
  char *ref;
  void *xo;
  bool fetching;
};

struct _network_t {
  struct list_head list;
  
  bool isInit;      // Create on ONG ?
  bool toDelete;      // Delete pending on ONG ?
  
  t_cmn_sensor cp_cmn;  // Dia context
  struct list_head device;

  int type;
#define NETWORK_SERIAL    1
#define NETWORK_ETHERNET  2

  int scan;
#define NETWORK_NO_SCAN     0
#define NETWORK_SCAN_REQUESTED  1
#define NETWORK_SCAN_DOING    2

  char uart[NAME_MAX];
  char name[NAME_MAX];  // Name of the XML file without suffix
  
  // For RTU only
  modbus_t *modbusCtx;
  int baudrate;
  int datasize;
  int stopsize;
  char parity;
};


#define ACCESS_VOID       0
#define ACCESS_BOOL       1
#define ACCESS_UINT8      2
#define ACCESS_INT8       3
#define ACCESS_UINT16_LE  4
#define ACCESS_UINT16_BE  5
#define ACCESS_INT16_LE   6
#define ACCESS_INT16_BE   7
#define ACCESS_INT        8
#define ACCESS_UINT       9
#define ACCESS_UINT32_LE  10
#define ACCESS_UINT32_BE  11
#define ACCESS_INT32_LE   12
#define ACCESS_INT32_BE   13
#define ACCESS_FLOAT      14
#define ACCESS_FLOAT_LE   15
#define ACCESS_FLOAT_BE   16
#define ACCESS_DOUBLE     17
#define ACCESS_DOUBLE_LE  18
#define ACCESS_DOUBLE_BE  19

/*
Discretes Input
Single bit
Read-Only
his type of data can be provided by an I/O system. 

Coils
Single bit
Read-Write
This type of data can be alterable by an application program. 

Input Registers
16-bit word
Read-Only
This type of data can be provided by an I/O system 

Holding Registers
16-bit word
Read-Write
This type of data 
*/
#define MAP_NOT_MAPPED 0
#define MAP_DISCRETES  1
#define MAP_COILS      2
#define MAP_INPUT      3
#define MAP_HOLDING    4

/*
 * Describe Attr in retargeting config
 */
struct _Attr_t {
  struct list_head list;

  char *modbusType;
  int modbusTypeID;
  char *modbusAccess;
  bool isReadable;
  bool isWritable;
  
  // Decoded version of modbusAccess
  uint16_t mask;
  int reg, map;
};

struct _Value_t {
  union {
    bool boolValue;
  } u;
};

/*
 * Describe Point in logging config
 */
struct _Point_t {
  struct list_head list;
  bool isInit;
  bool toDelete;

  time_t nextMaxInterval;
  time_t nextRead;
  int minInterval;
  int maxInterval;
  char *containerID;
  Value_t lastVal;
 
  uint16_t rq_cluster, rq_attribut, rq_member;
  void *xo;
  
  Attr_t attr;
};

#endif /* __MODBUS_TYPE__ */
