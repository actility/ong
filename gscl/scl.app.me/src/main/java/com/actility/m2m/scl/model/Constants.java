/*
 * Copyright   Actility, SA. All Rights Reserved.
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
 *
 * id $Id: Constants.java 3419 2012-10-26 13:30:11Z JReich $
 * author $Author: JReich $
 * version $Revision: 3419 $
 * lastrevision $Date: 2012-10-26 15:30:11 +0200 (Fri, 26 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-26 15:30:11 +0200 (Fri, 26 Oct 2012) $
 */

package com.actility.m2m.scl.model;

import com.actility.m2m.m2m.M2MConstants;

public interface Constants {
    long MAX_MAX_INSTANCE_AGE = Long.MAX_VALUE;

    // ///////////////////////////////////////////////////////////////////////
    // Tags and attributes
    // ///////////////////////////////////////////////////////////////////////
    String ATTR_LATEST = "_latest$";

    // ///////////////////////////////////////////////////////////////////////
    // Write Methods
    // ///////////////////////////////////////////////////////////////////////
    int ID_MD_CREATE = 0;
    int ID_MD_UPDATE = 1;
    int ID_MD_DELETE = 2;

    // ///////////////////////////////////////////////////////////////////////
    // M2M Resources
    // ///////////////////////////////////////////////////////////////////////
    int ID_RES_AMBIGUOUS = 0;
    int ID_RES_SCL_BASE = 1;
    int ID_RES_SCLS = 2;
    int ID_RES_SCL = 3;
    int ID_RES_APPLICATIONS = 4;
    int ID_RES_APPLICATION = 5;
    int ID_RES_APPLICATION_ANNC = 6;
    int ID_RES_ACCESS_RIGHTS = 7;
    int ID_RES_ACCESS_RIGHT = 8;
    int ID_RES_ACCESS_RIGHT_ANNC = 9;
    int ID_RES_COMMUNICATION_CHANNELS = 10;
    int ID_RES_COMMUNICATION_CHANNEL = 11;
    int ID_RES_CONTAINERS = 12;
    int ID_RES_CONTAINER = 13;
    int ID_RES_CONTAINER_ANNC = 14;
    int ID_RES_LOCATION_CONTAINER = 15;
    int ID_RES_LOCATION_CONTAINER_ANNC = 16;
    int ID_RES_CONTENT_INSTANCES = 17;
    int ID_RES_CONTENT_INSTANCE = 18;
    int ID_RES_GROUPS = 19;
    int ID_RES_GROUP = 20;
    int ID_RES_GROUP_ANNC = 21;
    int ID_RES_MEMBERS_CONTENT = 22;
    int ID_RES_SUBSCRIPTIONS = 23;
    int ID_RES_SUBSCRIPTION = 24;
    int ID_RES_M2M_POCS = 25;
    int ID_RES_M2M_POC = 26;
    int ID_RES_MGMT_OBJS = 27;
    int ID_RES_MGMT_OBJ = 28;
    int ID_RES_PARAMETERS = 29;
    int ID_RES_MGMT_CMD = 30;
    int ID_RES_EXEC_INSTANCES = 31;
    int ID_RES_EXEC_INSTANCE = 32;
    int ID_RES_ATTACHED_DEVICES = 33;
    int ID_RES_ATTACHED_DEVICE = 34;
    int ID_RES_NOTIFICATION_CHANNELS = 35;
    int ID_RES_NOTIFICATION_CHANNEL = 36;
    int ID_RES_DISCOVERY = 37;
    int ID_RES_SCL_ANNCS = 38;
    int ID_RES_SUBCONTAINERS = 39;

    // ///////////////////////////////////////////////////////////////////////
    // M2M Attributes
    // ///////////////////////////////////////////////////////////////////////
    int ID_ATTR_UNKNOWN = 0;
    int ID_ATTR_ACCESS_RIGHTS_REFERENCE = 1;
    int ID_ATTR_ACCESS_RIGHT_ANNC_COLLECTION = 2;
    int ID_ATTR_ACCESS_RIGHT_COLLECTION = 3;
    int ID_ATTR_ACCESS_RIGHT_I_D = 4;
    int ID_ATTR_ANNOUNCE_TO = 5;
    int ID_ATTR_APPLICATIONS_REFERENCE = 6;
    int ID_ATTR_APPLICATION_COLLECTION = 7;
    int ID_ATTR_APPLICATION_ANNC_COLLECTION = 8;
    int ID_ATTR_APP_ID = 9;
    int ID_ATTR_ATTACHED_DEVICES_REFERENCE = 10;
    int ID_ATTR_ATTACHED_DEVICE_COLLECTION = 11;
    int ID_ATTR_A_PO_C = 12;
    int ID_ATTR_A_PO_C_HANDLING = 13;
    int ID_ATTR_A_PO_C_PATHS = 14;
    int ID_ATTR_CHANNEL_DATA = 15;
    int ID_ATTR_CHANNEL_TYPE = 16;
    int ID_ATTR_COMMUNICATION_CHANNELS_REFERENCE = 17;
    int ID_ATTR_COMMUNICATION_CHANNEL_COLLECTION = 18;
    int ID_ATTR_CONTACT = 19;
    int ID_ATTR_CONTACT_INFO = 20;
    int ID_ATTR_CONTACT_U_R_I = 21;
    int ID_ATTR_CONTAINERS_REFERENCE = 22;
    int ID_ATTR_CONTAINER_ANNC_COLLECTION = 23;
    int ID_ATTR_CONTAINER_COLLECTION = 24;
    int ID_ATTR_CONTENT_INSTANCES_REFERENCE = 25;
    int ID_ATTR_CONTENT_INSTANCE_COLLECTION = 26;
    int ID_ATTR_CONTENT = 27;
    int ID_ATTR_CONTENT_SIZE = 28;
    int ID_ATTR_CONTENT_TYPES = 29;
    int ID_ATTR_CREATION_TIME = 30;
    int ID_ATTR_CURRENT_BYTE_SIZE = 31;
    int ID_ATTR_CURRENT_NR_OF_INSTANCES = 32;
    int ID_ATTR_DELAY_TOLERANCE = 33;
    int ID_ATTR_DISCOVERY_REFERENCE = 34;
    int ID_ATTR_EXPIRATION_TIME = 35;
    int ID_ATTR_FILTER_CRITERIA = 36;
    int ID_ATTR_GROUPS_REFERENCE = 37;
    int ID_ATTR_GROUP_ANNC_COLLECTION = 38;
    int ID_ATTR_GROUP_COLLECTION = 39;
    int ID_ATTR_HREF = 40;
    int ID_ATTR_ID = 41;
    int ID_ATTR_INTEGRITY_VAL_RESULTS = 42;
    int ID_ATTR_LAST_MODIFIED_TIME = 43;
    int ID_ATTR_LINK = 44;
    int ID_ATTR_LOCATION_CONTAINER_ANNC_COLLECTION = 45;
    int ID_ATTR_LOCATION_CONTAINER_COLLECTION = 46;
    int ID_ATTR_LOCATION_CONTAINER_TYPE = 47;
    int ID_ATTR_LOC_REQUESTOR = 48;
    int ID_ATTR_LOC_TARGET_DEVICE = 49;
    int ID_ATTR_M2M_POCS_REFERENCE = 50;
    int ID_ATTR_M2M_POC_COLLECTION = 51;
    int ID_ATTR_MAX_BYTE_SIZE = 52;
    int ID_ATTR_MAX_INSTANCE_AGE = 53;
    int ID_ATTR_MAX_NR_OF_INSTANCES = 54;
    int ID_ATTR_MGMT_CMD_COLLECTION = 55;
    int ID_ATTR_MGMT_OBJS_REFERENCE = 56;
    int ID_ATTR_MGMT_OBJ_COLLECTION = 57;
    int ID_ATTR_MGMT_PROTOCOL_TYPE = 58;
    int ID_ATTR_MINIMAL_TIME_BETWEEN_NOTIFICATIONS = 59;
    int ID_ATTR_NOTIFICATION_CHANNELS_REFERENCE = 60;
    int ID_ATTR_NOTIFICATION_CHANNEL_COLLECTION = 61;
    int ID_ATTR_ONLINE_STATUS = 62;
    int ID_ATTR_PERMISSIONS = 63;
    int ID_ATTR_POCS = 64;
    int ID_ATTR_REM_TRIGGER_ADDR = 65;
    int ID_ATTR_SCHEDULE = 66;
    int ID_ATTR_SCL_ANNC_COLLECTION = 67;
    int ID_ATTR_SCLS_REFERENCE = 68;
    int ID_ATTR_SCL_COLLECTION = 69;
    int ID_ATTR_SCL_ID = 70;
    int ID_ATTR_SEARCH_STRINGS = 71;
    int ID_ATTR_SELF_PERMISSIONS = 72;
    int ID_ATTR_SERVER_CAPABILITY = 73;
    int ID_ATTR_SUBCONTAINERS_REFERENCE = 74;
    int ID_ATTR_SUBSCRIPTIONS_REFERENCE = 75;
    int ID_ATTR_SUBSCRIPTION_COLLECTION = 76;
    int ID_ATTR_SUBSCRIPTION_TYPE = 77;

    // ///////////////////////////////////////////////////////////////////////
    // Enum values
    // ///////////////////////////////////////////////////////////////////////
    int ID_ENUM_UNDEFINED_VALUE = -1;

    int ID_MODE_FORBIDDEN = 0;
    int ID_MODE_REQUIRED = 1;
    int ID_MODE_OPTIONAL = 2;

    int ID_CHANNEL_TYPE_LONG_POLLING = 0;

    int ID_APOC_HANDLING_SHALLOW = 0;
    int ID_APOC_HANDLING_DEEP = 1;

    int ID_FLAG_READ = 0;
    int ID_FLAG_WRITE = 1;
    int ID_FLAG_DISCOVER = 2;
    int ID_FLAG_DELETE = 3;
    int ID_FLAG_CREATE = 4;

    int ID_ONLINE_STATUS_ONLINE = 0;
    int ID_ONLINE_STATUS_OFFLINE = 1;
    int ID_ONLINE_STATUS_NOT_REACHABLE = 2;

    int ID_MGMT_PROTOCOL_TYPE_OMA_DM = 0;
    int ID_MGMT_PROTOCOL_TYPE_BBF_TR069 = 1;

    int ID_LOCATION_CONTAINER_TYPE_LOCATION_SERVER_BASED = 0;
    int ID_LOCATION_CONTAINER_TYPE_APPLICATION_GENERATED = 1;

    // ///////////////////////////////////////////////////////////////////////
    // Filter criteria mode
    // ///////////////////////////////////////////////////////////////////////
    int ID_NO_FILTER_CRITERIA_MODE = 0;
    int ID_FILTER_CRITERIA_MODE = 1;
    int ID_CONTENT_INSTANCE_FILTER_CRITERIA_MODE = 2;

    // ///////////////////////////////////////////////////////////////////////
    // Log flags
    // ///////////////////////////////////////////////////////////////////////
    int ID_LOG_REPRESENTATION = 1;
    int ID_LOG_RAW_REPRESENTATION = 2;

    // ///////////////////////////////////////////////////////////////////////
    // Slash management flags for URIs
    // ///////////////////////////////////////////////////////////////////////
    int ID_SLASH_MGMT_LEAVE = 0;
    int ID_SLASH_MGMT_ADD = 1;
    int ID_SLASH_MGMT_REMOVE = 2;

    // ///////////////////////////////////////////////////////////////////////
    // Attributes
    // ///////////////////////////////////////////////////////////////////////
    String AT_PT_RESPONSE = "ptResponse";
    String AT_INDICATION = "indication";
    String AT_REQUESTOR = "requestor";
    String AT_STATUS = "status";

    // ///////////////////////////////////////////////////////////////////////
    // Paths + names
    // ///////////////////////////////////////////////////////////////////////
    String NAME_NSCL = "nscl";
    String PATH_ROOT = "";
    String PATH_SCLS = M2MConstants.URI_SEP + M2MConstants.RES_SCLS;
    String PATH_APPLICATIONS = M2MConstants.URI_SEP + M2MConstants.RES_APPLICATIONS;
    String PATH_NSCL = PATH_SCLS + M2MConstants.URI_SEP + NAME_NSCL;
    String ATTR_TYPE = "type";
    String ATTR_MEDIA_TYPE = "mediaType";

    // ///////////////////////////////////////////////////////////////////////
    // Types
    // ///////////////////////////////////////////////////////////////////////
    String TYPE_ACCESS_RIGHT = "accessRight";
    String TYPE_APPLICATION = "application";
    String TYPE_CONTAINER = "container";
    String TYPE_LOCATION_CONTAINER = "locationContainer";
    String TYPE_CONTENT_INSTANCE = "contentInstance";
    String TYPE_M2M_POC = "m2mPoc";
    String TYPE_NOTIFICATION_CHANNEL = "notificationChannel";
    String TYPE_COMMUNICATION_CHANNEL = "communicationChannel";
    String TYPE_SCL = "scl";
    String TYPE_SUBSCRIPTION = "subscription";
    String TYPE_CONTENT_INSTANCES = "contentInstances";

    // ///////////////////////////////////////////////////////////////////////
    // Supported header
    // ///////////////////////////////////////////////////////////////////////
    String HD_X_ACY_SUPPORTED = "X-Acy-Supported";
    String SP_STORAGE_CONF = "storageconf";
}
