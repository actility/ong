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

//************************************************************************
//************************************************************************
//**
//** File : ./include/azap_api_zcl.h
//**
//** Description  :
//**
//**
//** Date : 11/19/2012
//** Author :  Mathias Louiset, Actility (c), 2012.
//** Modified by :
//**
//** Task :
//**
//** REVISION HISTORY:
//**
//**  Date      Version   Name
//**  --------    -------   ------------
//**
//************************************************************************
//************************************************************************

/**
 * @file azap_api_zcl.h
 * The following defines the contants mandatory for ZCL layer manipulations
 * and the structures of the ZCL messages.
 * It implements what the norm specifies.
 * They are mandatory for using the AZAP library API.
 */
#ifndef _AZAP_API_ZCL__H_
#define _AZAP_API_ZCL__H_

/*********************************************************************
 * CONSTANTS
 */

// General Clusters
#define ZCL_CLUSTER_ID_GEN_BASIC                            0x0000
#define ZCL_CLUSTER_ID_GEN_POWER_CFG                        0x0001
#define ZCL_CLUSTER_ID_GEN_DEVICE_TEMP_CONFIG               0x0002
#define ZCL_CLUSTER_ID_GEN_IDENTIFY                         0x0003
#define ZCL_CLUSTER_ID_GEN_GROUPS                           0x0004
#define ZCL_CLUSTER_ID_GEN_SCENES                           0x0005
#define ZCL_CLUSTER_ID_GEN_ON_OFF                           0x0006
#define ZCL_CLUSTER_ID_GEN_ON_OFF_SWITCH_CONFIG             0x0007
#define ZCL_CLUSTER_ID_GEN_LEVEL_CONTROL                    0x0008
#define ZCL_CLUSTER_ID_GEN_ALARMS                           0x0009
#define ZCL_CLUSTER_ID_GEN_TIME                             0x000A
#define ZCL_CLUSTER_ID_GEN_LOCATION                         0x000B
#define ZCL_CLUSTER_ID_GEN_KEY_ESTABLISHMENT                0x0800

// Closures Clusters
#define ZCL_CLUSTER_ID_CLOSURES_SHADE_CONFIG                0x0100

// HVAC Clusters
#define ZCL_CLUSTER_ID_HVAC_PUMP_CONFIG_CONTROL             0x0200
#define ZCL_CLUSTER_ID_HAVC_THERMOSTAT                      0x0201
#define ZCL_CLUSTER_ID_HAVC_FAN_CONTROL                     0x0202
#define ZCL_CLUSTER_ID_HAVC_DIHUMIDIFICATION_CONTROL        0x0203
#define ZCL_CLUSTER_ID_HAVC_USER_INTERFACE_CONFIG           0x0204

// Lighting Clusters
#define ZCL_CLUSTER_ID_LIGHTING_COLOR_CONTROL               0x0300
#define ZCL_CLUSTER_ID_LIGHTING_BALLAST_CONFIG              0x0301

// Measurement and Sensing Clusters
#define ZCL_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT           0x0400
#define ZCL_CLUSTER_ID_MS_ILLUMINANCE_LEVEL_SENSING_CONFIG  0x0401
#define ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT           0x0402
#define ZCL_CLUSTER_ID_MS_PRESSURE_MEASUREMENT              0x0403
#define ZCL_CLUSTER_ID_MS_FLOW_MEASUREMENT                  0x0404
#define ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY                 0x0405
#define ZCL_CLUSTER_ID_MS_OCCUPANCY_SENSING                 0x0406

// Security and Safety (SS) Clusters
#define ZCL_CLUSTER_ID_SS_IAS_ZONE                          0x0500
#define ZCL_CLUSTER_ID_SS_IAS_ACE                           0x0501
#define ZCL_CLUSTER_ID_SS_IAS_WD                            0x0502

// Protocol Interfaces
#define ZCL_CLUSTER_ID_PI_GENERIC_TUNNEL                    0x0600
#define ZCL_CLUSTER_ID_PI_BACNET_PROTOCOL_TUNNEL            0x0601
#define ZCL_CLUSTER_ID_PI_11073_PROTOCOL_TUNNEL             0x0614

// Advanced Metering Initiative (SE) Clusters
#define ZCL_CLUSTER_ID_SE_PRICING                           0x0700
#define ZCL_CLUSTER_ID_SE_LOAD_CONTROL                      0x0701
#define ZCL_CLUSTER_ID_SE_SIMPLE_METERING                   0x0702
#define ZCL_CLUSTER_ID_SE_MESSAGE                           0x0703
#define ZCL_CLUSTER_ID_SE_REGISTRATION                      0x0704
#define ZCL_CLUSTER_ID_SE_SE_TUNNELING                      0x0705
#define ZCL_CLUSTER_ID_SE_PRE_PAYMENT                       0x0706

/*** Frame Control bit mask ***/
#define ZCL_FRAME_CONTROL_TYPE                              0x03
#define ZCL_FRAME_CONTROL_MANU_SPECIFIC                     0x04
#define ZCL_FRAME_CONTROL_DIRECTION                         0x08
#define ZCL_FRAME_CONTROL_DISABLE_DEFAULT_RSP               0x10

/*** Frame Types ***/
#define ZCL_FRAME_TYPE_PROFILE_CMD                          0x00
#define ZCL_FRAME_TYPE_SPECIFIC_CMD                         0x01

/*** Frame Client/Server Directions ***/
#define ZCL_FRAME_CLIENT_SERVER_DIR                         0x00
#define ZCL_FRAME_SERVER_CLIENT_DIR                         0x01

/*** Chipcon Manufacturer Code ***/
#define CC_MANUFACTURER_CODE                                0x1001

/*** Foundation Command IDs ***/
#define ZCL_CMD_READ                                        0x00
#define ZCL_CMD_READ_RSP                                    0x01
#define ZCL_CMD_WRITE                                       0x02
#define ZCL_CMD_WRITE_UNDIVIDED                             0x03
#define ZCL_CMD_WRITE_RSP                                   0x04
#define ZCL_CMD_WRITE_NO_RSP                                0x05
#define ZCL_CMD_CONFIG_REPORT                               0x06
#define ZCL_CMD_CONFIG_REPORT_RSP                           0x07
#define ZCL_CMD_READ_REPORT_CFG                             0x08
#define ZCL_CMD_READ_REPORT_CFG_RSP                         0x09
#define ZCL_CMD_REPORT                                      0x0a
#define ZCL_CMD_DEFAULT_RSP                                 0x0b
#define ZCL_CMD_DISCOVER                                    0x0c
#define ZCL_CMD_DISCOVER_RSP                                0x0d

#define ZCL_CMD_MAX                                         ZCL_CMD_DISCOVER_RSP

/*** Data Types ***/
#define ZCL_DATATYPE_NO_DATA                                0x00
#define ZCL_DATATYPE_DATA8                                  0x08
#define ZCL_DATATYPE_DATA16                                 0x09
#define ZCL_DATATYPE_DATA24                                 0x0a
#define ZCL_DATATYPE_DATA32                                 0x0b
#define ZCL_DATATYPE_BOOLEAN                                0x10
#define ZCL_DATATYPE_BITMAP8                                0x18
#define ZCL_DATATYPE_BITMAP16                               0x19
#define ZCL_DATATYPE_BITMAP24                               0x1a
#define ZCL_DATATYPE_BITMAP32                               0x1b
#define ZCL_DATATYPE_UINT8                                  0x20
#define ZCL_DATATYPE_UINT16                                 0x21
#define ZCL_DATATYPE_UINT24                                 0x22
#define ZCL_DATATYPE_UINT32                                 0x23
#define ZCL_DATATYPE_UINT40                                 0x24
#define ZCL_DATATYPE_UINT48                                 0x25
#define ZCL_DATATYPE_INT8                                   0x28
#define ZCL_DATATYPE_INT16                                  0x29
#define ZCL_DATATYPE_INT24                                  0x2a
#define ZCL_DATATYPE_INT32                                  0x2b
#define ZCL_DATATYPE_ENUM8                                  0x30
#define ZCL_DATATYPE_ENUM16                                 0x31
#define ZCL_DATATYPE_SEMI_PREC                              0x38
#define ZCL_DATATYPE_SINGLE_PREC                            0x39
#define ZCL_DATATYPE_DOUBLE_PREC                            0x3a
#define ZCL_DATATYPE_OCTET_STR                              0x41
#define ZCL_DATATYPE_CHAR_STR                               0x42
#define ZCL_DATATYPE_LONG_OCTET_STR                         0x43
#define ZCL_DATATYPE_LONG_CHAR_STR                          0x44
#define ZCL_DATATYPE_TOD                                    0xe0
#define ZCL_DATATYPE_DATE                                   0xe1
#define ZCL_DATATYPE_UTC                                    0xe2
#define ZCL_DATATYPE_CLUSTER_ID                             0xe8
#define ZCL_DATATYPE_ATTR_ID                                0xe9
#define ZCL_DATATYPE_BAC_OID                                0xea
#define ZCL_DATATYPE_IEEE_ADDR                              0xf0
#define ZCL_DATATYPE_UNKNOWN                                0xff

/*** Error Status Codes ***/
#define ZCL_STATUS_SUCCESS                                  0x00
#define ZCL_STATUS_FAILURE                                  0x01
// 0x02-0x7F are reserved.
#define ZCL_STATUS_MALFORMED_COMMAND                        0x80
#define ZCL_STATUS_UNSUP_CLUSTER_COMMAND                    0x81
#define ZCL_STATUS_UNSUP_GENERAL_COMMAND                    0x82
#define ZCL_STATUS_UNSUP_MANU_CLUSTER_COMMAND               0x83
#define ZCL_STATUS_UNSUP_MANU_GENERAL_COMMAND               0x84
#define ZCL_STATUS_INVALID_FIELD                            0x85
#define ZCL_STATUS_UNSUPPORTED_ATTRIBUTE                    0x86
#define ZCL_STATUS_INVALID_VALUE                            0x87
#define ZCL_STATUS_READ_ONLY                                0x88
#define ZCL_STATUS_INSUFFICIENT_SPACE                       0x89
#define ZCL_STATUS_DUPLICATE_EXISTS                         0x8a
#define ZCL_STATUS_NOT_FOUND                                0x8b
#define ZCL_STATUS_UNREPORTABLE_ATTRIBUTE                   0x8c
#define ZCL_STATUS_INVALID_DATA_TYPE                        0x8d
// 0xbd-bf are reserved.
#define ZCL_STATUS_HARDWARE_FAILURE                         0xc0
#define ZCL_STATUS_SOFTWARE_FAILURE                         0xc1
#define ZCL_STATUS_CALIBRATION_ERROR                        0xc2
// 0xc3-0xff are reserved.

/*** Attribute Access Control - bit masks ***/
#define ACCESS_CONTROL_READ                                 0x01
#define ACCESS_CONTROL_WRITE                                0x02
#define ACCESS_CONTROL_COMMAND                              0x04

// Used by Configure Reporting Command
#define ZCL_SEND_ATTR_REPORTS                               0x00
#define ZCL_EXPECT_ATTR_REPORTS                             0x01


/*********************************************************************/
/*    ___                            _______ _           _   _
 *   / __|___ _ __  _ __  ___ _ _   |_  / __| |     __ _| |_| |_ _ _
 *  | (__/ _ \ '  \| '  \/ _ \ ' \   / / (__| |__  / _` |  _|  _| '_|
 *   \___\___/_|_|_|_|_|_\___/_||_| /___\___|____| \__,_|\__|\__|_|
 */
#define ZCL_SCENE_NAME_LEN                                  16

/********************************/
/*** Basic Cluster Attributes ***/
/********************************/
// Basic Device Information
#define ATTRID_BASIC_ZCL_VERSION                            0x0000
#define ATTRID_BASIC_APPL_VERSION                           0x0001
#define ATTRID_BASIC_STACK_VERSION                          0x0002
#define ATTRID_BASIC_HW_VERSION                             0x0003
#define ATTRID_BASIC_MANUFACTURER_NAME                      0x0004
#define ATTRID_BASIC_MODEL_ID                               0x0005
#define ATTRID_BASIC_DATE_CODE                              0x0006
#define ATTRID_BASIC_POWER_SOURCE                           0x0007
// Basic Device Settings
#define ATTRID_BASIC_LOCATION_DESC                          0x0010
#define ATTRID_BASIC_PHYSICAL_ENV                           0x0011
#define ATTRID_BASIC_DEVICE_ENABLED                         0x0012
#define ATTRID_BASIC_ALARM_MASK                             0x0013

/*** Power Source Attribute values ***/
// Bits b0-b6 represent the primary power source of the device
#define POWER_SOURCE_UNKNOWN                                0x00
#define POWER_SOURCE_MAINS_1_PHASE                          0x01
#define POWER_SOURCE_MAINS_3_PHASE                          0x02
#define POWER_SOURCE_BATTERY                                0x03
#define POWER_SOURCE_DC                                     0x04
#define POWER_SOURCE_EMERG_MAINS_CONST_PWR                  0x05
#define POWER_SOURCE_EMERG_MAINS_XFER_SW                    0x06
// Bit b7 indicates whether the device has a secondary power source in the
// form of a battery backup

/*** Power Source Attribute bits  ***/
#define POWER_SOURCE_PRIMARY                                0x7F
#define POWER_SOURCE_SECONDARY                              0x80

/*** Physical Environment Attribute values ***/
#define PHY_UNSPECIFIED_ENV                                 0x00
// Specified per Profile 0x01-0x7F
#define PHY_UNKNOWN_ENV                                     0xFF

/*** Device Enable Attribute values ***/
#define DEVICE_DISABLED                                     0x00
#define DEVICE_ENABLED                                      0x01

/*** Alarm Mask Attribute bits ***/
#define ALARM_MASK_GEN_HW_FAULT                             0x01
#define ALARM_MASK_GEN_SW_FAULT                             0x02

/******************************/
/*** Basic Cluster Commands ***/
/******************************/
#define COMMAND_BASIC_RESET_FACT_DEFAULT                    0x00

/**********************************************/
/*** Power Configuration Cluster Attributes ***/
/**********************************************/
// Mains Information
#define ATTRID_POWER_CFG_MAINS_VOLTAGE                      0x0000
#define ATTRID_POWER_CFG_MAINS_FREQUENCY                    0x0001
// Mains Settings
#define ATTRID_POWER_CFG_MAINS_ALARM_MASK                   0x0010
#define ATTRID_POWER_CFG_MAINS_VOLT_MIN_THRES               0x0011
#define ATTRID_POWER_CFG_MAINS_VOLT_MAX_THRES               0x0012
#define ATTRID_POWER_CFG_MAINS_DWELL_TRIP_POINT             0x0013
// Battery Information
#define ATTRID_POWER_CFG_BATTERY_VOLTAGE                    0x0020
// Battery Settings
#define ATTRID_POWER_CFG_BAT_MANU                           0x0030
#define ATTRID_POWER_CFG_BAT_SIZE                           0x0031
#define ATTRID_POWER_CFG_BAT_AHR_RATING                     0x0032
#define ATTRID_POWER_CFG_BAT_QUANTITY                       0x0033
#define ATTRID_POWER_CFG_BAT_RATED_VOLTAGE                  0x0034
#define ATTRID_POWER_CFG_BAT_ALARM_MASK                     0x0035
#define ATTRID_POWER_CFG_BAT_VOLT_MIN_THRES                 0x0036

/*** Mains Alarm Mask Attribute bit ***/
#define MAINS_ALARM_MASK_VOLT_2_LOW                         0x00
#define MAINS_ALARM_MASK_VOLT_2_HI                          0x01

/*** Battery Size Attribute values ***/
#define BAT_SIZE_NO_BATTERY                                 0x00
#define BAT_SIZE_BUILT_IN                                   0x01
#define BAT_SIZE_OTHER                                      0x02
#define BAT_SIZE_AA                                         0x03
#define BAT_SIZE_AAA                                        0x04
#define BAT_SIZE_C                                          0x05
#define BAT_SIZE_D                                          0x06
#define BAT_SIZE_UNKNOWN                                    0xFF

/*** Batter Alarm Mask Attribute bit ***/
#define BAT_ALARM_MASK_VOLT_2_LOW                           0x10

/********************************************/
/*** Power Configuration Cluster Commands ***/
/********************************************/
// No cluster specific commands

/***********************************************************/
/*** Device Temperature Configuration Cluster Attributes ***/
/***********************************************************/
// Device Temperature Information
#define ATTRID_DEV_TEMP_CURRENT                             0x0000
#define ATTRID_DEV_TEMP_MIN_EXPERIENCED                     0x0001
#define ATTRID_DEV_TEMP_MAX_EXPERIENCED                     0x0002
#define ATTRID_DEV_TEMP_OVER_TOTAL_DWELL                    0x0003
// Device Temperature Settings
#define ATTRID_DEV_TEMP_ALARM_MASK                          0x0010
#define ATTRID_DEV_TEMP_LOW_THRES                           0x0011
#define ATTRID_DEV_TEMP_HI_THRES                            0x0012
#define ATTRID_DEV_TEMP_LOW_DWELL_TRIP_POINT                0x0013
#define ATTRID_DEV_TEMP_HI_DWELL_TRIP_POINT                 0x0014

/*** Device Temp Alarm_Mask Attribute bits ***/
#define DEV_TEMP_ALARM_MASK_2_LOW                           0x01
#define DEV_TEMP_ALARM_MASK_2_HI                            0x02

/*********************************************************/
/*** Device Temperature Configuration Cluster Commands ***/
/*********************************************************/
// No cluster specific commands

/***********************************/
/*** Identify Cluster Attributes ***/
/***********************************/
#define ATTRID_IDENTIFY_TIME                                0x0000

/*********************************/
/*** Identify Cluster Commands ***/
/*********************************/
#define COMMAND_IDENTIFY                                    0x00
#define COMMAND_IDENTIFY_QUERY                              0x01

#define COMMAND_IDENTIFY_QUERY_RSP                          0x00

/********************************/
/*** Group Cluster Attributes ***/
/********************************/
#define ATTRID_GROUP_NAME_SUPPORT                           0x0000

/******************************/
/*** Group Cluster Commands ***/
/******************************/
#define COMMAND_GROUP_ADD                                   0x00
#define COMMAND_GROUP_VIEW                                  0x01
#define COMMAND_GROUP_GET_MEMBERSHIP                        0x02
#define COMMAND_GROUP_REMOVE                                0x03
#define COMMAND_GROUP_REMOVE_ALL                            0x04
#define COMMAND_GROUP_ADD_IF_IDENTIFYING                    0x05

#define COMMAND_GROUP_ADD_RSP                               0x00
#define COMMAND_GROUP_VIEW_RSP                              0x01
#define COMMAND_GROUP_GET_MEMBERSHIP_RSP                    0x02
#define COMMAND_GROUP_REMOVE_RSP                            0x03

/*********************************/
/*** Scenes Cluster Attributes ***/
/*********************************/
// Scene Management Information
#define ATTRID_SCENES_COUNT                                 0x0000
#define ATTRID_SCENES_CURRENT_SCENE                         0x0001
#define ATTRID_SCENES_CURRENT_GROUP                         0x0002
#define ATTRID_SCENES_SCENE_VALID                           0x0003
#define ATTRID_SCENES_NAME_SUPPORT                          0x0004
#define ATTRID_SCENES_LAST_CFG_BY                           0x0005

/*******************************/
/*** Scenes Cluster Commands ***/
/*******************************/
#define COMMAND_SCENE_ADD                                   0x00
#define COMMAND_SCENE_VIEW                                  0x01
#define COMMAND_SCENE_REMOVE                                0x02
#define COMMAND_SCENE_REMOVE_ALL                            0x03
#define COMMAND_SCENE_STORE                                 0x04
#define COMMAND_SCENE_RECALL                                0x05
#define COMMAND_SCENE_GET_MEMBERSHIP                        0x06

#define COMMAND_SCENE_ADD_RSP                               0x00
#define COMMAND_SCENE_VIEW_RSP                              0x01
#define COMMAND_SCENE_REMOVE_RSP                            0x02
#define COMMAND_SCENE_REMOVE_ALL_RSP                        0x03
#define COMMAND_SCENE_STORE_RSP                             0x04
#define COMMAND_SCENE_GET_MEMBERSHIP_RSP                    0x06

/*********************************/
/*** On/Off Cluster Attributes ***/
/*********************************/
#define ATTRID_ON_OFF                                       0x0000

/*******************************/
/*** On/Off Cluster Commands ***/
/*******************************/
#define COMMAND_OFF                                         0x00
#define COMMAND_ON                                          0x01
#define COMMAND_TOGGLE                                      0x02

/****************************************/
/*** On/Off Switch Cluster Attributes ***/
/****************************************/
// Switch Information
#define ATTRID_ON_OFF_SWITCH_TYPE                           0x0000
// Switch Settings
#define ATTRID_ON_OFF_SWITCH_ACTIONS                        0x0010

/*** On Off Switch Type attribute values ***/
#define ON_OFF_SWITCH_TYPE_TOGGLE                           0x00
#define ON_OFF_SWITCH_TYPE_MOMENTARY                        0x01

/*** On Off Switch Actions attribute values ***/
#define ON_OFF_SWITCH_ACTIONS_0                             0x00
#define ON_OFF_SWITCH_ACTIONS_1                             0x01
#define ON_OFF_SWITCH_ACTIONS_2                             0x02

/**************************************/
/*** On/Off Switch Cluster Commands ***/
/**************************************/
// No cluster specific commands

/****************************************/
/*** Level Control Cluster Attributes ***/
/****************************************/
#define ATTRID_LEVEL_CURRENT_LEVEL                          0x0000
#define ATTRID_LEVEL_REMAINING_TIME                         0x0001
#define ATTRID_LEVEL_ON_OFF_TRANSITION_TIME                 0x0010
#define ATTRID_LEVEL_ON_LEVEL                               0x0011

/**************************************/
/*** Level Control Cluster Commands ***/
/**************************************/
#define COMMAND_LEVEL_MOVE_TO_LEVEL                         0x00
#define COMMAND_LEVEL_MOVE                                  0x01
#define COMMAND_LEVEL_STEP                                  0x02
#define COMMAND_LEVEL_STOP                                  0x03
#define COMMAND_LEVEL_MOVE_TO_LEVEL_WITH_ON_OFF             0x04
#define COMMAND_LEVEL_MOVE_WITH_ON_OFF                      0x05
#define COMMAND_LEVEL_STEP_WITH_ON_OFF                      0x06
#define COMMAND_LEVEL_STOP_WITH_ON_OFF                      0x07

/*** Level Control Move (Mode) Command values ***/
#define LEVEL_MOVE_UP                                       0x00
#define LEVEL_MOVE_DOWN                                     0x01

/*** Level Control Step (Mode) Command values ***/
#define LEVEL_STEP_UP                                       0x00
#define LEVEL_STEP_DOWN                                     0x01

/*********************************/
/*** Alarms Cluster Attributes ***/
/*********************************/
// Alarm Information
#define ATTRID_ALARM_COUNT                                  0x0000

/*******************************/
/*** Alarms Cluster Commands ***/
/*******************************/
#define COMMAND_ALARMS_RESET                                0x00
#define COMMAND_ALARMS_RESET_ALL                            0x01
#define COMMAND_ALARMS_GET                                  0x02
#define COMMAND_ALARMS_RESET_LOG                            0x03

#define COMMAND_ALARMS_ALARM                                0x00
#define COMMAND_ALARMS_GET_RSP                              0x01

/*******************************/
/*** Time Cluster Attributes ***/
/*******************************/
#define ATTRID_TIME_TIME                                    0x00
#define ATTRID_TIME_STATUS                                  0x01

/*** TimeStatus Attribute bits ***/
#define TIME_STATUS_MASTER                                  0x01
#define TIME_STATUS_SYNCH                                   0x02

/*****************************/
/*** Time Cluster Commands ***/
/*****************************/
// No cluster specific commands

/***********************************/
/*** RSSI Location Cluster Attributes ***/
/***********************************/
// Location Information
#define ATTRID_LOCATION_TYPE                                0x0000
#define ATTRID_LOCATION_METHOD                              0x0001
#define ATTRID_LOCATION_AGE                                 0x0002
#define ATTRID_LOCATION_QUALITY_MEASURE                     0x0003
#define ATTRID_LOCATION_NUM_DEVICES                         0x0004

// Location Settings
#define ATTRID_LOCATION_COORDINATE1                         0x0010
#define ATTRID_LOCATION_COORDINATE2                         0x0011
#define ATTRID_LOCATION_COORDINATE3                         0x0012
#define ATTRID_LOCATION_POWER                               0x0013
#define ATTRID_LOCATION_PATH_LOSS_EXPONENT                  0x0014
#define ATTRID_LOCATION_REPORT_PERIOD                       0x0015
#define ATTRID_LOCATION_CALC_PERIOD                         0x0016
#define ATTRID_LOCATION_NUM_RSSI_MEASUREMENTS               0x0017

/*** Location Type attribute bits ***/
#define LOCATION_TYPE_ABSOLUTE                              0x01
#define LOCATION_TYPE_2_D                                   0x02
#define LOCATION_TYPE_COORDINATE_SYSTEM                     0x0C

/*** Location Method attribute values ***/
#define LOCATION_METHOD_LATERATION                          0x00
#define LOCATION_METHOD_SIGNPOSTING                         0x01
#define LOCATION_METHOD_RF_FINGER_PRINT                     0x02
#define LOCATION_METHOD_OUT_OF_BAND                         0x03

/*********************************/
/*** Location Cluster Commands ***/
/*********************************/
#define COMMAND_LOCATION_SET_ABSOLUTE                       0x00
#define COMMAND_LOCATION_SET_DEV_CFG                        0x01
#define COMMAND_LOCATION_GET_DEV_CFG                        0x02
#define COMMAND_LOCATION_GET_DATA                           0x03

#define COMMAND_LOCATION_DEV_CFG_RSP                        0x00
#define COMMAND_LOCATION_DATA_RSP                           0x01
#define COMMAND_LOCATION_DATA_NOTIF                         0x02
#define COMMAND_LOCATION_COMPACT_DATA_NOTIF                 0x03
#define COMMAND_LOCATION_RSSI_PING                          0x04

// The maximum number of characters to allow in a scene's name
// remember that the first byte is the length
#define ZCL_GEN_SCENE_NAME_LEN                              16

// The maximum length of the scene extension field:
//   2 + 1 + 1 for On/Off cluster (onOff attibute)
//   2 + 1 + 1 for Level Control cluster (currentLevel attribute)
//   2 + 1 + 4 for Color Control cluster (currentX/currentY attributes)
#define ZCL_GEN_SCENE_EXT_LEN                               15

// The maximum number of entries in the Scene table
#define ZCL_GEN_MAX_SCENES                                  16


/*********************************************************************/
/*  _______ _      _  _                 _       _                  _   _
 * |_  / __| |    | || |___ _ __  ___  /_\ _  _| |_ ___ _ __  __ _| |_(_)___ _ _
 *  / / (__| |__  | __ / _ \ '  \/ -_)/ _ \ || |  _/ _ \ '  \/ _` |  _| / _ \ ' \
 * /___\___|____| |_||_\___/_|_|_\___/_/ \_\_,_|\__\___/_|_|_\__,_|\__|_\___/_||_|
 */

// Zigbee Home Automation Profile Identification
#define ZCL_HA_PROFILE_ID                                   0x0104

// Generic Device IDs
#define ZCL_HA_DEVICEID_ON_OFF_SWITCH                       0x0000
#define ZCL_HA_DEVICEID_LEVEL_CONTROL_SWITCH                0x0001
#define ZCL_HA_DEVICEID_ON_OFF_OUTPUT                       0x0002
#define ZCL_HA_DEVICEID_LEVEL_CONTROLLABLE_OUTPUT           0x0003
#define ZCL_HA_DEVICEID_SCENE_SELECTOR                      0x0004
#define ZCL_HA_DEVICEID_CONFIGURATIOPN_TOOL                 0x0005
#define ZCL_HA_DEVICEID_REMOTE_CONTROL                      0x0006
#define ZCL_HA_DEVICEID_COMBINED_INETRFACE                  0x0007
#define ZCL_HA_DEVICEID_RANGE_EXTENDER                      0x0008
#define ZCL_HA_DEVICEID_MAINS_POWER_OUTLET                  0x0009
// temp: nnl
#define ZCL_HA_DEVICEID_TEST_DEVICE                         0x00FF

// Lighting Device IDs
#define ZCL_HA_DEVICEID_ON_OFF_LIGHT                        0x0100
#define ZCL_HA_DEVICEID_DIMMABLE_LIGHT                      0x0101
#define ZCL_HA_DEVICEID_COLORED_DIMMABLE_LIGHT              0x0102
#define ZCL_HA_DEVICEID_ON_OFF_LIGHT_SWITCH                 0x0103
#define ZCL_HA_DEVICEID_DIMMER_SWITCH                       0x0104
#define ZCL_HA_DEVICEID_COLOR_DIMMER_SWITCH                 0x0105
#define ZCL_HA_DEVICEID_LIGHT_SENSOR                        0x0106
#define ZCL_HA_DEVICEID_OCCUPANCY_SENSOR                    0x0107

// Closures Device IDs
#define ZCL_HA_DEVICEID_SHADE                               0x0200
#define ZCL_HA_DEVICEID_SHADE_CONTROLLER                    0x0201

// HVAC Device IDs
#define ZCL_HA_DEVICEID_HEATING_COOLING_UNIT                0x0300
#define ZCL_HA_DEVICEID_THERMOSTAT                          0x0301
#define ZCL_HA_DEVICEID_TEMPERATURE_SENSOR                  0x0302
#define ZCL_HA_DEVICEID_PUMP                                0x0303
#define ZCL_HA_DEVICEID_PUMP_CONTROLLER                     0x0304
#define ZCL_HA_DEVICEID_PRESSURE_SENSOR                     0x0305
#define ZCL_HA_DEVICEID_FLOW_SENSOR                         0x0306

// Intruder Alarm Systems (IAS) Device IDs
#define ZCL_HA_DEVICEID_IAS_CONTROL_INDICATING_EQUIPMENT    0x0400
#define ZCL_HA_DEVICEID_IAS_ANCILLARY_CONTROL_EQUIPMENT     0x0401
#define ZCL_HA_DEVICEID_IAS_ZONE                            0x0402
#define ZCL_HA_DEVICEID_IAS_WARNING_DEVICE                  0x0403

// Measure and sensing attributes

// Illuminance Measurement Information attribute set
#define ATTRID_MS_ILLUMINANCE_MEASURED_VALUE                             0x0000
#define ATTRID_MS_ILLUMINANCE_MIN_MEASURED_VALUE                         0x0001
#define ATTRID_MS_ILLUMINANCE_MAX_MEASURED_VALUE                         0x0002
#define ATTRID_MS_ILLUMINANCE_TOLERANCE                                  0x0003
#define ATTRID_MS_ILLUMINANCE_LIGHT_SENSOR_TYPE                          0x0004
// Illuminance Measurement Settings attribute set
/*** Light Sensor Type attribute values ***/
#define MS_ILLUMINANCE_LIGHT_SENSOR_PHOTODIODE                           0x00
#define MS_ILLUMINANCE_LIGHT_SENSOR_CMOS                                 0x01
#define MS_ILLUMINANCE_LIGHT_SENSOR_UNKNOWN                              0xFF

/*****************************************************************************/
/***    Illuminance Level Sensing Configuration Cluster Attributes         ***/
/*****************************************************************************/
// Illuminance Level Sensing Information attribute set
#define ATTRID_MS_ILLUMINANCE_LEVEL_STATUS                               0x0000
#define ATTRID_MS_ILLUMINANCE_LEVEL_LIGHT_SENSOR_TYPE                    0x0001
/***  Level Status attribute values  ***/
#define MS_ILLUMINANCE_LEVEL_ON_TARGET                                   0x00
#define MS_ILLUMINANCE_LEVEL_BELOW_TARGET                                0x01
#define MS_ILLUMINANCE_LEVEL_ABOVE_TARGET                                0x02
/***  Light Sensor Type attribute values  ***/
#define MS_ILLUMINANCE_LEVEL_LIGHT_SENSOR_PHOTODIODE                     0x00
#define MS_ILLUMINANCE_LEVEL_LIGHT_SENSOR_CMOS                           0x01
#define MS_ILLUMINANCE_LEVEL_LIGHT_SENSOR_UNKNOWN                        0xFF
// Illuminance Level Sensing Settings attribute set
#define ATTRID_MS_ILLUMINANCE_TARGET_LEVEL                               0x0010

/*****************************************************************************/
/***    Temperature Measurement Cluster Attributes                         ***/
/*****************************************************************************/
// Temperature Measurement Information attributes set
#define ATTRID_MS_TEMPERATURE_MEASURED_VALUE                             0x0000
#define ATTRID_MS_TEMPERATURE_MIN_MEASURED_VALUE                         0x0001
#define ATTRID_MS_TEMPERATURE_MAX_MEASURED_VALUE                         0x0002
#define ATTRID_MS_TEMPERATURE_TOLERANCE                                  0x0003
// Temperature Measurement Settings attributes set
#define ATTRID_MS_TEMPERATURE_MIN_PERCENT_CHANGE                         0x0010
#define ATTRID_MS_TEMPERATURE_MIN_ABSOLUTE_CHANGE                        0x0011

/*****************************************************************************/
/***    Pressure Measurement Cluster Attributes                            ***/
/*****************************************************************************/
// Pressure Measurement Information attribute set
#define ATTRID_MS_PRESSURE_MEASUREMENT_MEASURED_VALUE                    0x0000
#define ATTRID_MS_PRESSURE_MEASUREMENT_MIN_MEASURED_VALUE                0x0001
#define ATTRID_MS_PRESSURE_MEASUREMENT_MAX_MEASURED_VALUE                0x0002
#define ATTRID_MS_PRESSURE_MEASUREMENT_TOLERANCE                         0x0003
// Pressure Measurement Settings attribute set

/*****************************************************************************/
/***        Flow Measurement Cluster Attributes                            ***/
/*****************************************************************************/
// Flow Measurement Information attribute set
#define ATTRID_MS_FLOW_MEASUREMENT_MEASURED_VALUE                        0x0000
#define ATTRID_MS_FLOW_MEASUREMENT_MIN_MEASURED_VALUE                    0x0001
#define ATTRID_MS_FLOW_MEASUREMENT_MAX_MEASURED_VALUE                    0x0002
#define ATTRID_MS_FLOW_MEASUREMENT_TOLERANCE                             0x0003
// Flow Measurement Settings attribute set

/*****************************************************************************/
/***        Relative Humidity Cluster Attributes                           ***/
/*****************************************************************************/
// Relative Humidity Information attribute set
#define ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE                       0x0000
#define ATTRID_MS_RELATIVE_HUMIDITY_MIN_MEASURED_VALUE                   0x0001
#define ATTRID_MS_RELATIVE_HUMIDITY_MAX_MEASURED_VALUE                   0x0002
#define ATTRID_MS_RELATIVE_HUMIDITY_TOLERANCE                            0x0003

/*****************************************************************************/
/***         Occupancy Sensing Cluster Attributes                          ***/
/*****************************************************************************/
// Occupancy Sensor Configuration attribute set
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_OCCUPANCY                     0x0000
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_OCCUPANCY_SENSOR_TYPE         0x0001
/*** Occupancy Sensor Type Attribute values ***/
#define MS_OCCUPANCY_SENSOR_TYPE_PIR                                     0x00
#define MS_OCCUPANCY_SENSOR_TYPE_ULTRASONIC                              0x01
#define MS_OCCUPANCY_SENSOR_TYPE_PIR_AND_ULTRASONIC                      0x02
// PIR Configuration attribute set
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_PIR_O_TO_U_DELAY              0x0010
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_PIR_U_TO_O_DELAY              0x0011
// Ultrasonic Configuration attribute set
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_ULTRASONIC_O_TO_U_DELAY       0x0020
#define ATTRID_MS_OCCUPANCY_SENSING_CONFIG_ULTRASONIC_U_TO_O_DELAY       0x0021



/*********************************************************************/
/*  _______ _      ___                _   ___
 * |_  / __| |    / __|_ __  __ _ _ _| |_| __|_ _  ___ _ _ __ _ _  _
 *  / / (__| |__  \__ \ '  \/ _` | '_|  _| _|| ' \/ -_) '_/ _` | || |
 * /___\___|____| |___/_|_|_\__,_|_|  \__|___|_||_\___|_| \__, |\_, |
 *                                                        |___/ |__/
 */

// Zigbee SE Profile Identification
#define ZCL_SE_PROFILE_ID                                   0x0109

// Generic Device IDs
#define ZCL_SE_DEVICEID_RANGE_EXTENDER                      0x0008

// SE Device IDs
#define ZCL_SE_DEVICEID_ESP                                 0x0500
#define ZCL_SE_DEVICEID_METER                               0x0501
#define ZCL_SE_DEVICEID_IN_PREMISE_DISPLAY                  0x0502
#define ZCL_SE_DEVICEID_PCT                                 0x0503
#define ZCL_SE_DEVICEID_LOAD_CTRL_EXTENSION                 0x0504
#define ZCL_SE_DEVICEID_SMART_APPLIANCE                     0x0505
#define ZCL_SE_DEVICEID_PREPAY_TERMINAL                     0x0506

// SE Profile Attributes List
// Simple Metering Attribute Set
#define ATTRID_MASK_SE_READ_INFO_SET                        0x0000
#define ATTRID_MASK_SE_TOU_INFO_SET                         0x0100
#define ATTRID_MASK_SE_METER_STATUS                         0x0200
#define ATTRID_MASK_SE_FORMATTING                           0x0300
#define ATTRID_MASK_SE_ESP_HISTO_CONSUMPT                   0x0400
#define ATTRID_MASK_SE_LOAD_PROFILE_CONFIG                  0x0500

// Simple Metering Cluster - Reading Information Set Attributes 0x00
#define ATTRID_SE_CURRENT_SUMMATION_DELIVERED        ( 0x0000 | ATTRID_MASK_SE_READ_INFO_SET )
#define ATTRID_SE_CURRENT_SUMMATION_RECEIVED         ( 0x0001 | ATTRID_MASK_SE_READ_INFO_SET )
#define ATTRID_SE_CURRENT_MAX_DEMAND_DELIVERED       ( 0x0002 | ATTRID_MASK_SE_READ_INFO_SET )
#define ATTRID_SE_CURRENT_MAX_DEMAND_RECEIVED        ( 0x0003 | ATTRID_MASK_SE_READ_INFO_SET )
#define ATTRID_SE_DFT_SUMMATION                      ( 0x0004 | ATTRID_MASK_SE_READ_INFO_SET )
#define ATTRID_SE_DAILY_FREEZE_TIME                  ( 0x0005 | ATTRID_MASK_SE_READ_INFO_SET )
#define ATTRID_SE_POWER_FACTOR                       ( 0x0006 | ATTRID_MASK_SE_READ_INFO_SET )
#define ATTRID_SE_READING_SNAPSHOT_TIME              ( 0x0007 | ATTRID_MASK_SE_READ_INFO_SET )
#define ATTRID_SE_CURRENT_MAX_DEMAND_DELIVERD_TIME   ( 0x0008 | ATTRID_MASK_SE_READ_INFO_SET )
#define ATTRID_SE_CURRENT_MAX_DEMAND_RECEIVED_TIME   ( 0x0009 | ATTRID_MASK_SE_READ_INFO_SET )
// Simple Metering Cluster - TOU Information Set Attributes 0x01
#define ATTRID_SE_CURRENT_TIER1_SUMMATION_DELIVERED  ( 0x0000 | ATTRID_MASK_SE_TOU_INFO_SET )
#define ATTRID_SE_CURRENT_TIER1_SUMMATION_RECEIVED   ( 0x0001 | ATTRID_MASK_SE_TOU_INFO_SET )
#define ATTRID_SE_CURRENT_TIER2_SUMMATION_DELIVERED  ( 0x0002 | ATTRID_MASK_SE_TOU_INFO_SET )
#define ATTRID_SE_CURRENT_TIER2_SUMMATION_RECEIVED   ( 0x0003 | ATTRID_MASK_SE_TOU_INFO_SET )
#define ATTRID_SE_CURRENT_TIER3_SUMMATION_DELIVERED  ( 0x0004 | ATTRID_MASK_SE_TOU_INFO_SET )
#define ATTRID_SE_CURRENT_TIER3_SUMMATION_RECEIVED   ( 0x0005 | ATTRID_MASK_SE_TOU_INFO_SET )
#define ATTRID_SE_CURRENT_TIER4_SUMMATION_DELIVERED  ( 0x0006 | ATTRID_MASK_SE_TOU_INFO_SET )
#define ATTRID_SE_CURRENT_TIER4_SUMMATION_RECEIVED   ( 0x0007 | ATTRID_MASK_SE_TOU_INFO_SET )
#define ATTRID_SE_CURRENT_TIER5_SUMMATION_DELIVERED  ( 0x0008 | ATTRID_MASK_SE_TOU_INFO_SET )
#define ATTRID_SE_CURRENT_TIER5_SUMMATION_RECEIVED   ( 0x0009 | ATTRID_MASK_SE_TOU_INFO_SET )
#define ATTRID_SE_CURRENT_TIER6_SUMMATION_DELIVERED  ( 0x000A | ATTRID_MASK_SE_TOU_INFO_SET )
#define ATTRID_SE_CURRENT_TIER6_SUMMATION_RECEIVED   ( 0x000B | ATTRID_MASK_SE_TOU_INFO_SET )

// Simple Metering Cluslter - Meter Status Attributes 0x02
#define ATTRID_SE_STATUS                             ( 0x0000 | ATTRID_MASK_SE_METER_STATUS )

// Simple Metering Cluster - Formatting Attributes 0x03
#define ATTRID_SE_UNIT_OF_MEASURE                    ( 0x0000 | ATTRID_MASK_SE_FORMATTING )
#define ATTRID_SE_MULTIPLIER                         ( 0x0001 | ATTRID_MASK_SE_FORMATTING )
#define ATTRID_SE_DIVISOR                            ( 0x0002 | ATTRID_MASK_SE_FORMATTING )
#define ATTRID_SE_SUMMATION_FORMATTING               ( 0x0003 | ATTRID_MASK_SE_FORMATTING )
#define ATTRID_SE_DEMAND_FORMATTING                  ( 0x0004 | ATTRID_MASK_SE_FORMATTING )
#define ATTRID_SE_HISTORICAL_CONSUMPTION_FORMATTING  ( 0x0005 | ATTRID_MASK_SE_FORMATTING )
#define ATTRID_SE_METERING_DEVICE_TYPE               ( 0x0006 | ATTRID_MASK_SE_FORMATTING )

// Simple Metering Cluster - ESP Historical Comsumption Attributes 0x04
#define ATTRID_SE_INSTANTANEOUS_DEMAND               ( 0x0000 | ATTRID_MASK_SE_ESP_HISTO_CONSUMPT )
#define ATTRID_SE_CURRENTDAY_CONSUMPTION_DELIVERED   ( 0x0001 | ATTRID_MASK_SE_ESP_HISTO_CONSUMPT )
#define ATTRID_SE_CURRENTDAY_CONSUMPTION_RECEIVED    ( 0x0002 | ATTRID_MASK_SE_ESP_HISTO_CONSUMPT )
#define ATTRID_SE_PREVIOUSDAY_CONSUMPTION_DELIVERED  ( 0x0003 | ATTRID_MASK_SE_ESP_HISTO_CONSUMPT )
#define ATTRID_SE_PREVIOUSDAY_CONSUMPTION_RECEIVED   ( 0x0004 | ATTRID_MASK_SE_ESP_HISTO_CONSUMPT )
#define ATTRID_SE_CURR_PARTIAL_PROFIL_INT_START_TIME ( 0x0005 | ATTRID_MASK_SE_ESP_HISTO_CONSUMPT )
#define ATTRID_SE_CURR_PARTIAL_PROFIL_INT_VALUE      ( 0x0006 | ATTRID_MASK_SE_ESP_HISTO_CONSUMPT )

// Simple Metering Cluster - Load Profile Configuration Attributes 0x05
#define ATTRID_SE_MAX_NUMBER_OF_PERIODS_DELIVERED    ( 0x0000 | ATTRID_MASK_SE_LOAD_PROFILE_CONFIG )

// Demand Response Client Cluster Attributes
#define ATTRID_SE_UTILITY_DEFINED_GROUP               0x0000
#define ATTRID_SE_START_RANDOMIZE_MINUTES             0x0001
#define ATTRID_SE_STOP_RANDOMIZE_MINUTES              0x0002



/*********************************************************************
 * TYPEDEFS
 */


/*
 * Structures for ZCL foundation commands
 */

/** Read Attribute Command format */
typedef struct
{
  azap_uint8 numAttr;    /**< number of attributes in the list */
  azap_uint16 attrId[];  /**< supported attributes list - this structure should
                           * be allocated with the appropriate number of attributes.*/
} azapZclReadCommand_t;

/** Read Attribute Response Status record */
typedef struct
{
  azap_uint16 attrId;    /**< attribute ID */
  azap_uint8 status;     /**< should be ZCL_STATUS_SUCCESS or error */
  azap_uint8 dataType;   /**< attribute data type */
  azap_uint8 *data;      /**< this structure is allocated, so the data is HERE
                           *  - the size depends on the attribute data type */
} azapZclReadRspAttr_t;

/** Read Attribute Response Command format */
typedef struct
{
  azap_uint8 numAttr;            /**< number of attributes in the list */
  azapZclReadRspAttr_t attrList[]; /**< attribute status list */
} azapZclReadRspCommand_t;

/** Write Attribute record */
typedef struct
{
  azap_uint16 attrId;     /**< attribute ID */
  azap_uint8 dataType;    /**< attribute data type */
  azap_uint8 *attrData;   /**< this structure is allocated, so the data is HERE
                            *  - the size depends on the attribute data type */
} azapZclWriteAttr_t;

/** Write Attribute Command format */
typedef struct
{
  azap_uint8 numAttr;       /**< number of attribute records in the list */
  azapZclWriteAttr_t attrList[]; /**< attribute records */
} azapZclWriteCommand_t;

/** Write Attribute Status record */
typedef struct
{
  azap_uint8 status;        /**< should be ZCL_STATUS_SUCCESS or error */
  azap_uint16 attrId;       /**< attribute ID */
} STRUCTPACKED azapZclWriteRspAttr_t;

/** Write Attribute Response Command format */
typedef struct
{
  azap_uint8 numAttr;             /**< number of attribute status in the list */
  azapZclWriteRspAttr_t attrList[]; /**< attribute status records */
} STRUCTPACKED azapZclWriteRspCommand_t;

/** Configure Reporting Command format */
typedef struct
{
  azap_uint8 direction;         /**< to send or receive reports of the attribute */
  azap_uint16 attrId;           /**< attribute ID */
  azap_uint8 dataType;          /**< attribute data type */
  azap_uint16 minReportInt;     /**< minimum reporting interval */
  azap_uint16 maxReportInt;     /**< maximum reporting interval */
  azap_uint16 timeoutPeriod;    /**< timeout period */
  azap_uint8 *reportableChange; /**< reportable change (only applicable to analog data type)
                                 * - the size depends on the attribute data type */
} azapZclCfgReportAttr_t;

typedef struct
{
  azap_uint8 numAttr;           /**< number of attribute IDs in the list */
  azapZclCfgReportAttr_t attrList[]; /**< attribute ID list */
} azapZclCfgReportCommand_t;

/** Attribute Status record */
typedef struct
{
  azap_uint8 status;            /**< should be ZCL_STATUS_SUCCESS or error */
  azap_uint8 direction;         /**< whether attributes are reported or reports
                                  *  of attributes are received */
  azap_uint16 attrId;           /**< attribute ID */
} azapZclCfgReportRspAttr_t;

/** Configure Reporting Response Command format */
typedef struct
{
  azap_uint8 numAttr;               /**< number of attribute status in the list */
  azapZclCfgReportRspAttr_t attrList[]; /**< attribute status records */
} azapZclCfgReportRspCommand_t;

/** Attribute Report */
typedef struct
{
  azap_uint16 attrId;           /**< atrribute ID */
  azap_uint8 dataType;          /**< attribute data type */
  azap_uint8 *attrData;         /**< this structure is allocated, so the data is HERE
                                 *   - the size depends on the data type of attrID */
} azapZclReportAttr_t;

/** Report Attributes Command format */
typedef struct
{
  azap_uint8 numAttr;           /**< number of reports in the list */
  azapZclReportAttr_t attrList[];       /**< attribute report list */
} azapZclReportCommand_t;

// Read Reporting Configuration Command format
typedef struct
{
  azap_uint8 direction;         /**< to send or receive reports of the attribute */
  azap_uint16 attrId;           /**< atrribute ID */
} azapZclReadReportCfgAttr_t;

typedef struct
{
  azap_uint8 numAttr;           /**< number of attributes in the list */
  azapZclReadReportCfgAttr_t attrList[]; /**< list of read configs */
} azapZclReadReportCfgCommand_t;

// Attribute Reporting Configuration record
typedef struct
{
  azap_uint8 status;            /**< status field */
  azap_uint8 direction;         /**< to send or receive reports of the attribute */
  azap_uint16 attrId;           /**< attribute ID */
  azap_uint8 dataType;          /**< attribute data type */
  azap_uint16 minReportInt;     /**< minimum reporting interval */
  azap_uint16 maxReportInt;     /**< maximum reporting interval */
  azap_uint16 timeoutPeriod;    /**< timeout period */
  azap_uint8 *reportableChange; /**< reportable change (only applicable to analog data type)
                                 *   - the size depends on the attribute data type */
} azapZclReportCfgRspAttr_t;

// Read Reporting Configuration Response Command format
typedef struct
{
  azap_uint8 numAttr;           /**< number of records in the list */
  azapZclReportCfgRspAttr_t attrList[];   /**< attribute reporting configuration list */
} azapZclReadReportCfgRspCommand_t;


/** Default Response Command format */
typedef struct
{
  azap_uint8 commandId;
  azap_uint8 statusCode;
} azapZclDefaultRspCommand_t;

/** Discover Attributes Command format */
typedef struct
{
  azap_uint16 startAttrId;      /**< specifies the minimum value of the identifier
                                 *   to begin attribute discovery. */
  azap_uint8 maxNumAttrIds;     /**< maximum number of attribute IDs that are to be
                                     returned in the resulting response command. */
} azapZclDiscoverCommand_t;

/** Attribute Report info */
typedef struct
{
  azap_uint16 attrId;          /**< attribute ID */
  azap_uint8 dataType;         /**< attribute data type (see Table 17 in Spec) */
} azapZclDiscoverAttr_t;

/** Discover Attributes Response Command format */
typedef struct
{
  azap_uint8 discComplete;      /**< whether or not there're more attributes to be discovered */
  azap_uint8 numAttr;           /**< number of attributes in the list */
  azapZclDiscoverAttr_t attrList[]; /**< supported attributes list */
} azapZclDiscoverRspCommand_t;


#endif



