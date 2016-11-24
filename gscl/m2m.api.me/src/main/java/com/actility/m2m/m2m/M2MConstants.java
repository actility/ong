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
 */

package com.actility.m2m.m2m;

public interface M2MConstants {
    // ///////////////////////////////////////////////////////////////////////
    // Encodings / Media types
    // ///////////////////////////////////////////////////////////////////////
    String ENC_UTF8 = "UTF-8";
    String ENC_US_ASCII = "US-ASCII";
    String MT_APPLICATION_XML = "application/xml";
    String MT_APPLICATION_EXI = "application/exi";
    String MT_APPLICATION_XML_OBIX = "application/xml+obix";
    String MT_APPLICATION_EXI_OBIX = "application/exi+obix";
    String MT_TEXT_XML = "text/xml";
    String MT_TEXT = "text/";
    String CT_APPLICATION_XML_UTF8 = "application/xml; charset=utf-8";

    // ///////////////////////////////////////////////////////////////////////
    // M2M Methods
    // ///////////////////////////////////////////////////////////////////////
    int ID_MD_CREATE = 0;
    int ID_MD_RETRIEVE = 1;
    int ID_MD_UPDATE = 2;
    int ID_MD_DELETE = 3;

    String MD_CREATE = "CREATE";
    String MD_RETRIEVE = "RETRIEVE";
    String MD_UPDATE = "UPDATE";
    String MD_DELETE = "DELETE";

    // ///////////////////////////////////////////////////////////////////////
    // M2M Resources
    // ///////////////////////////////////////////////////////////////////////
    String RES_SCL_BASE = "sclBase";
    String RES_SCLS = "scls";
    String RES_SCL = "scl";
    String RES_SCL_ANNCS = "sclAnncs";
    String RES_SCL_ANNC = "sclAnnc";
    String RES_APPLICATIONS = "applications";
    String RES_APPLICATION = "application";
    String RES_APPLICATION_ANNC = "applicationAnnc";
    String RES_ACCESS_RIGHTS = "accessRights";
    String RES_ACCESS_RIGHT = "accessRight";
    String RES_ACCESS_RIGHT_ANNC = "accessRightAnnc";
    String RES_COMMUNICATION_CHANNELS = "communicationChannels";
    String RES_COMMUNICATION_CHANNEL = "communicationChannel";
    String RES_CONTAINERS = "containers";
    String RES_CONTAINER = "container";
    String RES_CONTAINER_ANNC = "containerAnnc";
    String RES_LOCATION_CONTAINER = "locationContainer";
    String RES_LOCATION_CONTAINER_ANNC = "locationContainerAnnc";
    String RES_CONTENT_INSTANCES = "contentInstances";
    String RES_CONTENT_INSTANCE = "contentInstance";
    String RES_GROUPS = "groups";
    String RES_GROUP = "group";
    String RES_GROUP_ANNC = "groupAnnc";
    String RES_SUBCONTAINERS = "subcontainers";
    String RES_SUBSCRIPTIONS = "subscriptions";
    String RES_SUBSCRIPTION = "subscription";
    String RES_M2M_POCS = "m2mPocs";
    String RES_M2M_POC = "m2mPoc";
    String RES_MGMT_OBJS = "mgmtObjs";
    String RES_MGMT_OBJ = "mgmtObj";
    String RES_PARAMETERS = "parameters";
    String RES_MGMT_CMD = "mgmtCmd";
    String RES_EXEC_INSTANCES = "execInstances";
    String RES_EXEC_INSTANCE = "execInstance";
    String RES_ATTACHED_DEVICES = "attachedDevices";
    String RES_ATTACHED_DEVICE = "attachedDevice";
    String RES_NOTIFICATION_CHANNELS = "notificationChannels";
    String RES_NOTIFICATION_CHANNEL = "notificationChannel";
    String RES_DISCOVERY = "discovery";
    String RES_MEMBERS_CONTENT = "membersContent";

    // ///////////////////////////////////////////////////////////////////////
    // M2M Short Resources
    // ///////////////////////////////////////////////////////////////////////
    String RES_SHORT_SCLS = "SL";
    String RES_SHORT_SCL_ANNCS = "SLA";
    String RES_SHORT_APPLICATIONS = "AP";
    String RES_SHORT_ACCESS_RIGHTS = "AR";
    String RES_SHORT_COMMUNICATION_CHANNELS = "CC";
    String RES_SHORT_CONTAINERS = "CO";
    String RES_SHORT_CONTENT_INSTANCES = "CI";
    String RES_SHORT_GROUPS = "GR";
    String RES_SHORT_SUBCONTAINERS = "SC";
    String RES_SHORT_SUBSCRIPTIONS = "SU";
    String RES_SHORT_M2M_POCS = "PC";
    String RES_SHORT_MGMT_OBJS = "MO";
    String RES_SHORT_ATTACHED_DEVICES = "AD";
    String RES_SHORT_NOTIFICATION_CHANNELS = "NC";
    String RES_SHORT_DISCOVERY = "DC";
    String RES_SHORT_MEMBERS_CONTENT = "MC";
    String RES_SHORT_EXEC_INSTANCES = "EI";

    // ///////////////////////////////////////////////////////////////////////
    // M2M Attributes
    // ///////////////////////////////////////////////////////////////////////
    String ATTR_ACCESS_RIGHT_ANNC_COLLECTION = "accessRightAnncCollection";
    String ATTR_ACCESS_RIGHT_COLLECTION = "accessRightCollection";
    String ATTR_ACCESS_RIGHT_I_D = "accessRightID";
    String ATTR_ACCESS_RIGHTS_REFERENCE = "accessRightsReference";
    String ATTR_ANNOUNCE_TO = "announceTo";
    String ATTR_APPLICATION_ANNC_COLLECTION = "applicationAnncCollection";
    String ATTR_APPLICATION_COLLECTION = "applicationCollection";
    String ATTR_APPLICATIONS_REFERENCE = "applicationsReference";
    String ATTR_APP_ID = "appId";
    String ATTR_ATTACHED_DEVICE_COLLECTION = "attachedDeviceCollection";
    String ATTR_ATTACHED_DEVICES_REFERENCE = "attachedDevicesReference";
    String ATTR_ATTRIBUTE_ACCESSOR = "attributeAccessor";
    String ATTR_A_PO_C = "aPoC";
    String ATTR_A_PO_C_HANDLING = "aPoCHandling";
    String ATTR_A_PO_C_PATHS = "aPoCPaths";
    String ATTR_CHANNEL_DATA = "channelData";
    String ATTR_CHANNEL_TYPE = "channelType";
    String ATTR_COMMUNICATION_CHANNEL_COLLECTION = "communicationChannelCollection";
    String ATTR_COMMUNICATION_CHANNELS_REFERENCE = "communicationChannelsReference";
    String ATTR_CONTACT = "contact";
    String ATTR_CONTACT_INFO = "contactInfo";
    String ATTR_CONTACT_U_R_I = "contactURI";
    String ATTR_CONTAINER_ANNC_COLLECTION = "containerAnncCollection";
    String ATTR_CONTAINER_COLLECTION = "containerCollection";
    String ATTR_CONTAINERS_REFERENCE = "containersReference";
    String ATTR_CONTENT = "content";
    String ATTR_CONTENT_INSTANCE_COLLECTION = "contentInstanceCollection";
    String ATTR_CONTENT_INSTANCES_REFERENCE = "contentInstancesReference";
    String ATTR_CONTENT_SIZE = "contentSize";
    String ATTR_CONTENT_TYPE = "contentType";
    String ATTR_CONTENT_TYPES = "contentTypes";
    String ATTR_CREATED_AFTER = "createdAfter";
    String ATTR_CREATED_BEFORE = "createdBefore";
    String ATTR_CREATION_TIME = "creationTime";
    String ATTR_CURRENT_BYTE_SIZE = "currentByteSize";
    String ATTR_CURRENT_NR_OF_INSTANCES = "currentNrOfInstances";
    String ATTR_DELAY_TOLERANCE = "delayTolerance";
    String ATTR_DISCOVERY_REFERENCE = "discoveryReference";
    String ATTR_EXPIRATION_TIME = "expirationTime";
    String ATTR_FILTER_CRITERIA = "filterCriteria";
    String ATTR_GROUP_ANNC_COLLECTION = "groupAnncCollection";
    String ATTR_GROUP_COLLECTION = "groupCollection";
    String ATTR_GROUPS_REFERENCE = "groupsReference";
    String ATTR_ID = "id";
    String ATTR_IF_MATCH = "ifMatch";
    String ATTR_IF_MODIFIED_SINCE = "ifModifiedSince";
    String ATTR_IF_NONE_MATCH = "ifNoneMatch";
    String ATTR_IF_UNMODIFIED_SINCE = "ifUnmodifiedSince";
    String ATTR_INTEGRITY_VAL_RESULTS = "integrityValResults";
    String ATTR_IN_TYPE = "inType";
    String ATTR_LAST_MODIFIED_TIME = "lastModifiedTime";
    String ATTR_LATEST = "latest";
    String ATTR_LINK = "link";
    String ATTR_LOCATION_CONTAINER_ANNC_COLLECTION = "locationContainerAnncCollection";
    String ATTR_LOCATION_CONTAINER_COLLECTION = "locationContainerCollection";
    String ATTR_LOCATION_CONTAINER_TYPE = "locationContainerType";
    String ATTR_LOC_REQUESTOR = "locRequestor";
    String ATTR_LOC_TARGET_DEVICE = "locTargetdevice";
    String ATTR_M2M_POC_COLLECTION = "m2mPocCollection";
    String ATTR_M2M_POCS_REFERENCE = "m2mPocsReference";
    String ATTR_MAX_BYTE_SIZE = "maxByteSize";
    String ATTR_MAX_INSTANCE_AGE = "maxInstanceAge";
    String ATTR_MAX_NR_OF_INSTANCES = "maxNrOfInstances";
    String ATTR_MAX_SIZE = "maxSize";
    String ATTR_META_DATA_ONLY = "metaDataOnly";
    String ATTR_MGMT_CMD_COLLECTION = "mgmtCmdCollection";
    String ATTR_MGMT_OBJ_COLLECTION = "mgmtObjCollection";
    String ATTR_MGMT_OBJS_REFERENCE = "mgmtObjsReference";
    String ATTR_MGMT_PROTOCOL_TYPE = "mgmtProtocolType";
    String ATTR_MINIMAL_TIME_BETWEEN_NOTIFICATIONS = "minimalTimeBetweenNotifications";
    String ATTR_NAME = "name";
    String ATTR_NOTIFICATION_CHANNEL_COLLECTION = "notificationChannelCollection";
    String ATTR_NOTIFICATION_CHANNELS_REFERENCE = "notificationChannelsReference";
    String ATTR_OLDEST = "oldest";
    String ATTR_ONLINE_STATUS = "onlineStatus";
    String ATTR_OUT_TYPE = "outType";
    String ATTR_PERMISSIONS = "permissions";
    String ATTR_POC = "poc";
    String ATTR_POCS = "pocs";
    String ATTR_REFERENCE_POINT = "referencePoint";
    String ATTR_REM_TRIGGER_ADDR = "remTriggerAddr";
    String ATTR_SERVER_CAPABILITY = "serverCapability";
    String ATTR_SCHEDULE = "schedule";
    String ATTR_SCL_ANNC_COLLECTION = "sclAnncCollection";
    String ATTR_SCL_COLLECTION = "sclCollection";
    String ATTR_SCL_ID = "sclId";
    String ATTR_SCLS_REFERENCE = "sclsReference";
    String ATTR_SEARCH_PREFIX = "searchPrefix";
    String ATTR_SEARCH_STRING = "searchString";
    String ATTR_SEARCH_STRINGS = "searchStrings";
    String ATTR_SELF_PERMISSIONS = "selfPermissions";
    String ATTR_SIZE_FROM = "sizeFrom";
    String ATTR_SIZE_UNTIL = "sizeUntil";
    String ATTR_SUBCONTAINERS_REFERENCE = "subcontainersReference";
    String ATTR_SUBSCRIPTION_COLLECTION = "subscriptionCollection";
    String ATTR_SUBSCRIPTION_TYPE = "subscriptionType";
    String ATTR_SUBSCRIPTIONS_REFERENCE = "subscriptionsReference";
    String ATTR_VAL = "val";

    // ///////////////////////////////////////////////////////////////////////
    // Representation Prefix, Attributes and Tags
    // ///////////////////////////////////////////////////////////////////////
    String PREFIX_M2M = "m2m";
    String PREFIX_XSI = "xsi";
    String PREFIX_XMIME = "xmime";
    String PREFIX_ACY = "acy";

    String ATTR_HREF = "href";

    String TAG_ACCEPT_HEADER = "acceptHeader";
    String TAG_ATTRIBUTE_ACCESSOR = "attributeAccessor";
    String TAG_CONTENT_TYPE = "contentType";
    String TAG_CONTENT_TYPE_HEADER = "contentTypeHeader";
    String TAG_CREATED_AFTER = "createdAfter";
    String TAG_CREATED_BEFORE = "createdBefore";
    String TAG_CREATOR = "creator";
    String TAG_ETAG_HEADER = "etagHeader";
    String TAG_FILTER_CRITERIA = "filterCriteria";
    String TAG_GROUP_REQUEST_IDENTIFIER = "groupRequestIdentifier";
    String TAG_IF_MATCH = "ifMatch";
    String TAG_IF_MATCH_HEADER = "ifMatchHeader";
    String TAG_IF_MODIFIED_SINCE = "ifModifiedSince";
    String TAG_IF_MODIFIED_SINCE_HEADER = "ifModifiedSinceHeader";
    String TAG_IF_NONE_MATCH = "ifNoneMatch";
    String TAG_IF_NONE_MATCH_HEADER = "ifNoneMatchHeader";
    String TAG_IF_UNMODIFIED_SINCE = "ifUnmodifiedSince";
    String TAG_IF_UNMODIFIED_SINCE_HEADER = "ifUnmodifiedSinceHeader";
    String TAG_IN_TYPE = "inType";
    String TAG_IVAL_RESULTS = "ivalResults";
    String TAG_LAST_MODIFIED_HEADER = "lastModifiedHeader";
    String TAG_LOCATION_HEADER = "locationHeader";
    String TAG_MAX_SIZE = "maxSize";
    String TAG_META_DATA_ONLY = "metaDataOnly";
    String TAG_METHOD = "method";
    String TAG_OUT_TYPE = "outType";
    String TAG_PRIMITIVE_TYPE = "primitiveType";
    String TAG_R_C_A_T = "RCAT";
    String TAG_REFERENCE = "reference";
    String TAG_REPRESENTATION = "representation";
    String TAG_REQUESTING_ENTITY = "requestingEntity";
    String TAG_RESOURCE_U_R_I = "resourceURI";
    String TAG_SEARCH_PREFIX = "searchPrefix";
    String TAG_SEARCH_STRING = "searchString";
    String TAG_SECURE_TIME_STAMP = "secureTimeStamp";
    String TAG_SIGNED_IVAL_RESULTS = "signedIvalResults";
    String TAG_SIZE_FROM = "sizeFrom";
    String TAG_SIZE_UNTIL = "sizeUntil";
    String TAG_SUBSCRIPTION_REFERENCE = "subscriptionReference";
    String TAG_TARGET_I_D = "targetID";
    String TAG_TARGET_ID = "targetId";
    String TAG_TIMEOUT_REASON = "timeoutReason";
    String TAG_TOLERABLE_DELAY = "tolerableDelay";
    String TAG_TOLERABLE_TIME = "tolerableTime";
    String TAG_T_R_P_D_T = "TRPDT";
    String TAG_X_ETSI_CONTACT_URI_HEADER = "xEtsiContactUriHeader";
    String TAG_X_ETSI_CORRELATION_I_D_HEADER = "xEtsiCorrelationIDHeader";

    String ATTR_XSI_TYPE = "xsi:type";

    String TAG_ACY_STORAGE_CONFIGURATION = "acy:storageConfiguration";
    String TAG_ACY_PARAM = "acy:param";

    String ATTR_M2M_ID = "m2m:id";

    String TAG_M2M_AGGREGATE_U_R_I = "m2m:aggregateURI";
    String TAG_M2M_A_PO_C = "m2m:aPoC";
    String TAG_M2M_A_PO_C_PATH = "m2m:aPoCPath";
    String TAG_M2M_A_PO_C_PATHS = "m2m:aPoCPaths";
    String TAG_M2M_A_POC_HANDLING = "m2m:aPocHandling";
    String TAG_M2M_ACCESS_RIGHT = "m2m:accessRight";
    String TAG_M2M_ACCESS_RIGHT_ANNC = "m2m:accessRightAnnc";
    String TAG_M2M_ACCESS_RIGHT_ANNC_COLLECTION = "m2m:accessRightAnncCollection";
    String TAG_M2M_ACCESS_RIGHT_COLLECTION = "m2m:accessRightCollection";
    String TAG_M2M_ACCESS_RIGHT_I_D = "m2m:accessRightID";
    String TAG_M2M_ACCESS_RIGHTS = "m2m:accessRights";
    String TAG_M2M_ACCESS_RIGHTS_REFERENCE = "m2m:accessRightsReference";
    String TAG_M2M_ACTIVATED = "m2m:activated";
    String TAG_M2M_ADDITIONAL_INFO = "m2m:additionalInfo";
    String TAG_M2M_ALL = "m2m:all";
    String TAG_M2M_ANNOUNCE_TO = "m2m:announceTo";
    String TAG_M2M_APPLICATION = "m2m:application";
    String TAG_M2M_APPLICATION_ANNC = "m2m:applicationAnnc";
    String TAG_M2M_APPLICATION_ANNC_COLLECTION = "m2m:applicationAnncCollection";
    String TAG_M2M_APPLICATION_COLLECTION = "m2m:applicationCollection";
    String TAG_M2M_APPLICATION_I_D = "m2m:applicationID";
    String TAG_M2M_APPLICATION_I_DS = "m2m:applicationIDs";
    String TAG_M2M_APPLICATION_STATUS = "m2m:applicationStatus";
    String TAG_M2M_APPLICATIONS = "m2m:applications";
    String TAG_M2M_APPLICATIONS_REFERENCE = "m2m:applicationsReference";
    String TAG_M2M_ATTACHED_DEVICES_REFERENCE = "m2m:attachedDevicesReference";
    String TAG_M2M_BINARY_CONTENT = "m2m:binaryContent";
    String TAG_M2M_CHANNEL_DATA = "m2m:channelData";
    String TAG_M2M_CHANNEL_TYPE = "m2m:channelType";
    String TAG_M2M_COMMUNICATION_CHANNEL = "m2m:communicationChannel";
    String TAG_M2M_COMMUNICATION_CHANNEL_COLLECTION = "m2m:communicationChannelCollection";
    String TAG_M2M_COMMUNICATION_CHANNELS = "m2m:communicationChannels";
    String TAG_M2M_COMMUNICATION_CHANNELS_REFERENCE = "m2m:communicationChannelsReference";
    String TAG_M2M_CONTACT = "m2m:contact";
    String TAG_M2M_CONTACT_INFO = "m2m:contactInfo";
    String TAG_M2M_CONTACT_U_R_I = "m2m:contactURI";
    String TAG_M2M_CONTAINER = "m2m:container";
    String TAG_M2M_CONTAINER_ANNC = "m2m:containerAnnc";
    String TAG_M2M_CONTAINER_ANNC_COLLECTION = "m2m:containerAnncCollection";
    String TAG_M2M_CONTAINER_COLLECTION = "m2m:containerCollection";
    String TAG_M2M_CONTAINERS = "m2m:containers";
    String TAG_M2M_CONTAINERS_REFERENCE = "m2m:containersReference";
    String TAG_M2M_CONTENT = "m2m:content";
    String TAG_M2M_CONTENT_INSTANCE = "m2m:contentInstance";
    String TAG_M2M_CONTENT_INSTANCE_COLLECTION = "m2m:contentInstanceCollection";
    String TAG_M2M_CONTENT_INSTANCES = "m2m:contentInstances";
    String TAG_M2M_CONTENT_INSTANCES_REFERENCE = "m2m:contentInstancesReference";
    String TAG_M2M_CONTENT_SIZE = "m2m:contentSize";
    String TAG_M2M_CONTENT_TYPE = "m2m:contentType";
    String TAG_M2M_CONTENT_TYPES = "m2m:contentTypes";
    String TAG_M2M_CREATION_TIME = "m2m:creationTime";
    String TAG_M2M_CURRENT_ACCU_CREATES = "m2m:currentAccuCreates";
    String TAG_M2M_CURRENT_BYTE_SIZE = "m2m:currentByteSize";
    String TAG_M2M_CURRENT_NR_OF_INSTANCES = "m2m:currentNrOfInstances";
    String TAG_M2M_CURRENT_NR_OF_MEMBERS = "m2m:currentNrOfMembers";
    String TAG_M2M_DELAY_TOLERANCE = "m2m:delayTolerance";
    String TAG_M2M_DISCOVERY = "m2m:discovery";
    String TAG_M2M_DISCOVERY_REFERENCE = "m2m:discoveryReference";
    String TAG_M2M_DISCOVERY_U_R_I = "m2m:discoveryURI";
    String TAG_M2M_DOMAIN = "m2m:domain";
    String TAG_M2M_DOMAINS = "m2m:domains";
    String TAG_M2M_ERROR_INFO = "m2m:errorInfo";
    String TAG_M2M_EXPIRATION_TIME = "m2m:expirationTime";
    String TAG_M2M_FILTER_CRITERIA = "m2m:filterCriteria";
    String TAG_M2M_FLAG = "m2m:flag";
    String TAG_M2M_GLOBAL = "m2m:global";
    String TAG_M2M_GROUP = "m2m:group";
    String TAG_M2M_GROUP_ANNC = "m2m:groupAnnc";
    String TAG_M2M_GROUP_ANNC_COLLECTION = "m2m:groupAnncCollection";
    String TAG_M2M_GROUP_COLLECTION = "m2m:groupCollection";
    String TAG_M2M_GROUPS = "m2m:groups";
    String TAG_M2M_GROUPS_REFERENCE = "m2m:groupsReference";
    String TAG_M2M_HOLDER_REF = "m2m:holderRef";
    String TAG_M2M_HOLDER_REFS = "m2m:holderRefs";
    String TAG_M2M_INTEGRITY_VAL_RESULTS = "m2m:integrityValResults";
    String TAG_M2M_LAST_MODIFIED_TIME = "m2m:lastModifiedTime";
    String TAG_M2M_LATEST = "m2m:latest";
    String TAG_M2M_LINK = "m2m:link";
    String TAG_M2M_LOC_REQUESTOR = "m2m:locRequestor";
    String TAG_M2M_LOC_TARGET_DEVICE = "m2m:locTargetDevice";
    String TAG_M2M_LOCATION_CONTAINER = "m2m:locationContainer";
    String TAG_M2M_LOCATION_CONTAINER_ANNC = "m2m:locationContainerAnnc";
    String TAG_M2M_LOCATION_CONTAINER_ANNC_COLLECTION = "m2m:locationContainerAnncCollection";
    String TAG_M2M_LOCATION_CONTAINER_COLLECTION = "m2m:locationContainerCollection";
    String TAG_M2M_LOCATION_CONTAINER_TYPE = "m2m:locationContainerType";
    String TAG_M2M_LONG_POLLING_U_R_I = "m2m:longPollingURI";
    String TAG_M2M_M2M_POC = "m2m:m2mPoc";
    String TAG_M2M_M2M_POC_COLLECTION = "m2m:m2mPocCollection";
    String TAG_M2M_M2M_POCS = "m2m:m2mPocs";
    String TAG_M2M_M2M_POCS_REFERENCE = "m2m:m2mPocsReference";
    String TAG_M2M_MATCH_SIZE = "m2m:matchSize";
    String TAG_M2M_MAX_ACCU_CREATES = "m2m:maxAccuCreates";
    String TAG_M2M_MAX_BYTE_SIZE = "m2m:maxByteSize";
    String TAG_M2M_MAX_INSTANCE_AGE = "m2m:maxInstanceAge";
    String TAG_M2M_MAX_NR_OF_INSTANCES = "m2m:maxNrOfInstances";
    String TAG_M2M_MAX_NR_OF_MEMBERS = "m2m:maxNrOfMembers";
    String TAG_M2M_MEMBER_TYPE = "m2m:memberType";
    String TAG_M2M_MEMBERS = "m2m:members";
    String TAG_M2M_MEMBERS_CONTENT_REFERENCE = "m2m:membersContentReference";
    String TAG_M2M_MEMBERS_CONTENT_RESPONSES = "m2m:membersContentResponses";
    String TAG_M2M_MGMT_OBJS_REFERENCE = "m2m:mgmtObjsReference";
    String TAG_M2M_MGMT_PROTOCOL_TYPE = "m2m:mgmtProtocolType";
    String TAG_M2M_MINIMAL_TIME_BETWEEN_NOTIFICATIONS = "m2m:minimalTimeBetweenNotifications";
    String TAG_M2M_NAMED_REFERENCE = "m2m:namedReference";
    String TAG_M2M_NO_REPRESENTATION = "m2m:noRepresentation";
    String TAG_M2M_NOTIFICATION_CHANNEL = "m2m:notificationChannel";
    String TAG_M2M_NOTIFICATION_CHANNEL_COLLECTION = "m2m:notificationChannelCollection";
    String TAG_M2M_NOTIFICATION_CHANNELS = "m2m:notificationChannels";
    String TAG_M2M_NOTIFICATION_CHANNELS_REFERENCE = "m2m:notificationChannelsReference";
    String TAG_M2M_NOTIFY = "m2m:notify";
    String TAG_M2M_OLDEST = "m2m:oldest";
    String TAG_M2M_ONLINE_STATUS = "m2m:onlineStatus";
    String TAG_M2M_OTHER = "m2m:other";
    String TAG_M2M_PATH = "m2m:path";
    String TAG_M2M_PERMISSION = "m2m:permission";
    String TAG_M2M_PERMISSION_FLAGS = "m2m:permissionFlags";
    String TAG_M2M_PERMISSION_HOLDERS = "m2m:permissionHolders";
    String TAG_M2M_PERMISSIONS = "m2m:permissions";
    String TAG_M2M_POC = "m2m:poc";
    String TAG_M2M_POCS = "m2m:pocs";
    String TAG_M2M_PUBLIC_DOMAIN = "m2m:publicDomain";
    String TAG_M2M_REFERENCE_POINT = "m2m:referencePoint";
    String TAG_M2M_REM_TRIGGER_ADDR = "m2m:remTriggerAddr";
    String TAG_M2M_REQUEST_NOTIFY = "m2m:requestNotify";
    String TAG_M2M_RESPONSE_NOTIFY = "m2m:responseNotify";
    String TAG_M2M_SCHEDULE = "m2m:schedule";
    String TAG_M2M_SCL = "m2m:scl";
    String TAG_M2M_SCL_ANNCS_REFERENCE = "m2m:sclAnncsReference";
    String TAG_M2M_SCL_BASE = "m2m:sclBase";
    String TAG_M2M_SCL_COLLECTION = "m2m:sclCollection";
    String TAG_M2M_SCL_I_D = "m2m:sclID";
    String TAG_M2M_SCL_I_DS = "m2m:sclIDs";
    String TAG_M2M_SCL_ID = "m2m:sclId";
    String TAG_M2M_SCL_LIST = "m2m:sclList";
    String TAG_M2M_SCLS = "m2m:scls";
    String TAG_M2M_SCLS_REFERENCE = "m2m:sclsReference";
    String TAG_M2M_SCL_TYPE = "m2m:sclType";
    String TAG_M2M_SEARCH_STRING = "m2m:searchString";
    String TAG_M2M_SEARCH_STRINGS = "m2m:searchStrings";
    String TAG_M2M_SELF_PERMISSIONS = "m2m:selfPermissions";
    String TAG_M2M_SERVER_CAPABILITY = "m2m:serverCapability";
    String TAG_M2M_STATUS_CODE = "m2m:statusCode";
    String TAG_M2M_SUBCONTAINERS = "m2m:subcontainers";
    String TAG_M2M_SUBCONTAINERS_REFERENCE = "m2m:subcontainersReference";
    String TAG_M2M_SUBSCRIBER_ID = "m2m:subscriberId";
    String TAG_M2M_SUBSCRIPTION = "m2m:subscription";
    String TAG_M2M_SUBSCRIPTION_COLLECTION = "m2m:subscriptionCollection";
    String TAG_M2M_SUBSCRIPTION_TYPE = "m2m:subscriptionType";
    String TAG_M2M_SUBSCRIPTIONS = "m2m:subscriptions";
    String TAG_M2M_SUBSCRIPTIONS_REFERENCE = "m2m:subscriptionsReference";
    String TAG_M2M_TEXT_CONTENT = "m2m:textContent";
    String TAG_M2M_TIMEOUT_REASON = "m2m:timeoutReason";
    String TAG_M2M_TRUNCATED = "m2m:truncated";

    String TAG_XMIME_CONTENT_TYPE = "xmime:contentType";

    String TYPE_M2M_REFERENCE_TO_NAMED_RESOURCE = "m2m:ReferenceToNamedResource";
    String TYPE_M2M_FILTER_CRITERIA_TYPE = "m2m:FilterCriteriaType";
    String TYPE_M2M_CONTENT_INSTANCE_FILTER_CRITERIA_TYPE = "m2m:ContentInstanceFilterCriteriaType";
    String TYPE_M2M_CHANNEL_DATA = "m2m:ChannelData";
    String TYPE_M2M_LONG_POLLING_CHANNEL_DATA = "m2m:LongPollingChannelData";

    String TYPE_XMIME_BASE64_BINARY = "xmime:base64Binary";

    // ///////////////////////////////////////////////////////////////////////
    // Enums
    // ///////////////////////////////////////////////////////////////////////
    String APOC_HANDLING_SHALLOW = "SHALLOW";
    String APOC_HANDLING_DEEP = "DEEP";

    String ONLINE_STATUS_ONLINE = "ONLINE";
    String ONLINE_STATUS_OFFLINE = "OFFLINE";
    String ONLINE_STATUS_NOT_REACHABLE = "NOT_REACHABLE";

    String CHANNEL_TYPE_LONG_POLLING = "LONG_POLLING";

    String SUBSCRIPTION_TYPE_ASYNCHRONOUS = "ASYNCHRONOUS";
    String SUBSCRIPTION_TYPE_SYNCHRONOUS = "SYNCHRONOUS";

    String REFERENCE_POINT_MIA = "MIA_REFERENCE_POINT";
    String REFERENCE_POINT_DIA = "DIA_REFERENCE_POINT";

    String FLAG_READ = "READ";
    String FLAG_WRITE = "WRITE";
    String FLAG_DISCOVER = "DISCOVER";
    String FLAG_DELETE = "DELETE";
    String FLAG_CREATE = "CREATE";

    String MGMT_PROTOCOL_TYPE_OMA_DM = "OMA_DM";
    String MGMT_PROTOCOL_TYPE_BBF_TR069 = "BBF_TR069";

    String SCL_TYPE_NSCL = "NSCL";
    String SCL_TYPE_GSCL = "GSCL";
    String SCL_TYPE_DSCL = "DSCL";

    String BOOLEAN_TRUE = "true";
    String BOOLEAN_FALSE = "false";

    String RCAT_TYPE_RCAT_0 = "RCAT_0";
    String RCAT_TYPE_RCAT_1 = "RCAT_1";
    String RCAT_TYPE_RCAT_2 = "RCAT_2";
    String RCAT_TYPE_RCAT_3 = "RCAT_3";
    String RCAT_TYPE_RCAT_4 = "RCAT_4";
    String RCAT_TYPE_RCAT_5 = "RCAT_5";
    String RCAT_TYPE_RCAT_6 = "RCAT_6";
    String RCAT_TYPE_RCAT_7 = "RCAT_7";

    String METHOD_TYPE_CREATE = "CREATE";
    String METHOD_TYPE_DELETE = "DELETE";
    String METHOD_TYPE_EXECUTE = "EXECUTE";
    String METHOD_TYPE_NOTIFY = "NOTIFY";
    String METHOD_TYPE_RETRIEVE = "RETRIEVE";
    String METHOD_TYPE_UPDATE = "UPDATE";

    String CONSISTENCY_STRATEGY_ABANDON_GROUP = "ABANDON_GROUP";
    String CONSISTENCY_STRATEGY_ABANDON_MEMBER = "ABANDON_MEMBER";
    String CONSISTENCY_STRATEGY_ABANDON_TYPE = "ABANDON_TYPE";

    String MEMBER_TYPE_ACCESS_RIGHT = "ACCESS_RIGHT";
    String MEMBER_TYPE_APPLICATION = "APPLICATION";
    String MEMBER_TYPE_ATTACHED_DEVICE = "ATTACHED_DEVICE";
    String MEMBER_TYPE_CONTAINER = "CONTAINER";
    String MEMBER_TYPE_LOCATION_CONTAINER = "LOCATION_CONTAINER";
    String MEMBER_TYPE_MGMT_CMD = "MGMT_CMD";
    String MEMBER_TYPE_MGMT_OBJ = "MGMT_OBJ";
    String MEMBER_TYPE_MIXED = "MIXED";
    String MEMBER_TYPE_SCL = "SCL";
    String MEMBER_TYPE_SCL_BASE = "SCL_BASE";

    String RESOURCE_TYPE_AN = "AN";
    String RESOURCE_TYPE_AP = "AP";
    String RESOURCE_TYPE_AR = "AR";
    String RESOURCE_TYPE_CI = "CI";
    String RESOURCE_TYPE_CS = "CS";
    String RESOURCE_TYPE_CT = "CT";
    String RESOURCE_TYPE_GP = "GP";
    String RESOURCE_TYPE_MT = "MT";
    String RESOURCE_TYPE_PM = "PM";
    String RESOURCE_TYPE_RS = "RS";
    String RESOURCE_TYPE_SB = "SB";
    String RESOURCE_TYPE_SL = "SL";

    String LOCATION_CONTAINER_TYPE_APPLICATION_GENERATED = "APPLICATION_GENERATED";
    String LOCATION_CONTAINER_TYPE_LOCATION_SERVER_BASED = "LOCATION_SERVER_BASED";

    // ///////////////////////////////////////////////////////////////////////
    // Misc
    // ///////////////////////////////////////////////////////////////////////
    String URI_SEP = "/";
    char URI_SEP_CHAR = URI_SEP.charAt(0);
}
