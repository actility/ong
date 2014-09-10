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
    // ///////////////////////////////////////////////////////////////////////
    // Tags and attributes
    // ///////////////////////////////////////////////////////////////////////
    String ATTR_OWNER = "_owner$";
    String ATTR_LATEST = "_latest$";

    // ///////////////////////////////////////////////////////////////////////
    // Primitive types
    // ///////////////////////////////////////////////////////////////////////
    // int ID_PT_AMBIGUOUS = 0;
    // int ID_PT_SCL_BASE_RETRIEVE = 1;
    // int ID_PT_SCL_BASE_UPDATE = 2;
    // int ID_PT_SCLS_RETRIEVE = 3;
    // int ID_PT_SCLS_UPDATE = 4;
    // int ID_PT_SCL_CREATE = 5;
    // int ID_PT_SCL_RETRIEVE = 6;
    // int ID_PT_SCL_UPDATE = 7;
    // int ID_PT_SCL_DELETE = 8;
    // int ID_PT_APPLICATIONS_RETRIEVE = 9;
    // int ID_PT_APPLICATIONS_UPDATE = 10;
    // int ID_PT_APPLICATION_CREATE = 11;
    // int ID_PT_APPLICATION_RETRIEVE = 12;
    // int ID_PT_APPLICATION_UPDATE = 13;
    // int ID_PT_APPLICATION_DELETE = 14;
    // int ID_PT_APPLICATION_ANNC_CREATE = 15;
    // int ID_PT_APPLICATION_ANNC_RETRIEVE = 16;
    // int ID_PT_APPLICATION_ANNC_UPDATE = 17;
    // int ID_PT_APPLICATION_ANNC_DELETE = 18;
    // int ID_PT_ACCESS_RIGHTS_RETRIEVE = 19;
    // int ID_PT_ACCESS_RIGHTS_UPDATE = 20;
    // int ID_PT_ACCESS_RIGHT_CREATE = 21;
    // int ID_PT_ACCESS_RIGHT_RETRIEVE = 22;
    // int ID_PT_ACCESS_RIGHT_UPDATE = 23;
    // int ID_PT_ACCESS_RIGHT_DELETE = 24;
    // int ID_PT_ACCESS_RIGHT_ANNC_CREATE = 25;
    // int ID_PT_ACCESS_RIGHT_ANNC_RETRIEVE = 26;
    // int ID_PT_ACCESS_RIGHT_ANNC_UPDATE = 27;
    // int ID_PT_ACCESS_RIGHT_ANNC_DELETE = 28;
    // int ID_PT_CONTAINERS_RETRIEVE = 29;
    // int ID_PT_CONTAINERS_UPDATE = 30;
    // int ID_PT_CONTAINER_CREATE = 31;
    // int ID_PT_CONTAINER_RETRIEVE = 32;
    // int ID_PT_CONTAINER_UPDATE = 33;
    // int ID_PT_CONTAINER_DELETE = 34;
    // int ID_PT_CONTAINER_ANNC_CREATE = 35;
    // int ID_PT_CONTAINER_ANNC_RETRIEVE = 36;
    // int ID_PT_CONTAINER_ANNC_UPDATE = 37;
    // int ID_PT_CONTAINER_ANNC_DELETE = 38;
    // int ID_PT_LOCATION_CONTAINER_CREATE = 39;
    // int ID_PT_LOCATION_CONTAINER_RETRIEVE = 40;
    // int ID_PT_LOCATION_CONTAINER_UPDATE = 41;
    // int ID_PT_LOCATION_CONTAINER_DELETE = 42;
    // int ID_PT_LOCATION_CONTAINER_ANNC_CREATE = 43;
    // int ID_PT_LOCATION_CONTAINER_ANNC_RETRIEVE = 44;
    // int ID_PT_LOCATION_CONTAINER_ANNC_UPDATE = 45;
    // int ID_PT_LOCATION_CONTAINER_ANNC_DELETE = 46;
    // int ID_PT_CONTENT_INSTANCES_RETRIEVE = 47;
    // int ID_PT_CONTENT_INSTANCE_CREATE = 48;
    // int ID_PT_CONTENT_INSTANCE_RETRIEVE = 49;
    // int ID_PT_CONTENT_INSTANCE_DELETE = 50;
    // int ID_PT_GROUPS_RETRIEVE = 51;
    // int ID_PT_GROUPS_UPDATE = 52;
    // int ID_PT_GROUP_CREATE = 53;
    // int ID_PT_GROUP_RETRIEVE = 54;
    // int ID_PT_GROUP_UPDATE = 55;
    // int ID_PT_GROUP_DELETE = 56;
    // int ID_PT_GROUP_ANNC_CREATE = 57;
    // int ID_PT_GROUP_ANNC_RETRIEVE = 58;
    // int ID_PT_GROUP_ANNC_UPDATE = 59;
    // int ID_PT_GROUP_ANNC_DELETE = 60;
    // int ID_PT_MEMBERS_CONTENT_CREATE = 61;
    // int ID_PT_MEMBERS_CONTENT_RETRIEVE = 62;
    // int ID_PT_MEMBERS_CONTENT_UPDATE = 63;
    // int ID_PT_MEMBERS_CONTENT_DELETE = 64;
    // int ID_PT_SUBSCRIPTIONS_RETRIEVE = 65;
    // int ID_PT_SUBSCRIPTION_CREATE = 66;
    // int ID_PT_SUBCSRIPTION_RETRIEVE = 67;
    // int ID_PT_SUBSCRIPTION_UPDATE = 68;
    // int ID_PT_SUBSCRIPTION_DELETE = 69;
    // int ID_PT_SUBSCRIPTION_NOTIFY = 70;
    // int ID_PT_M2M_POCS_RETRIEVE = 71;
    // int ID_PT_M2M_POC_CREATE = 72;
    // int ID_PT_M2M_POC_RETRIEVE = 73;
    // int ID_PT_M2M_POC_UPDATE = 74;
    // int ID_PT_M2M_POC_DELETE = 75;
    // int ID_PT_MGMT_OBJS_RETRIEVE = 76;
    // int ID_PT_MGMT_OBJS_UPDATE = 77;
    // int ID_PT_MGMT_OBJ_CREATE = 78;
    // int ID_PT_MGMT_OBJ_RETRIEVE = 79;
    // int ID_PT_MGMT_OBJ_UPDATE= 80;
    // int ID_PT_MGMT_OBJ_DELETE = 81;
    // int ID_PT_MGMT_OBJ_EXECUTE = 82;
    // int ID_PT_PARAMETERS_CREATE = 83;
    // int ID_PT_PARAMETERS_RETRIEVE = 84;
    // int ID_PT_PARAMETERS_UPDATE = 85;
    // int ID_PT_PARAMETERS_DELETE = 86;
    // int ID_PT_PARAMETERS_EXECUTE = 87;
    // int ID_PT_MGMT_CMD_CREATE = 88;
    // int ID_PT_MGMT_CMD_RETRIEVE = 89;
    // int ID_PT_MGMT_CMD_UPDATE = 90;
    // int ID_PT_MGMT_CMD_DELETE = 91;
    // int ID_PT_MGMT_CMD_EXECUTE = 92;
    // int ID_PT_EXEC_INSTANCES_RETRIEVE = 93;
    // int ID_PT_EXEC_INSTANCE_RETRIEVE = 94;
    // int ID_PT_EXEC_INSTANCE_DELETE = 95;
    // int ID_PT_EXEC_INSTANCE_EXECUTE = 96;
    // int ID_PT_ATTACHED_DEVICES_RETRIEVE = 97;
    // int ID_PT_ATTACHED_DEVICES_UPDATE = 98;
    // int ID_PT_ATTACHED_DEVICE_CREATE = 99;
    // int ID_PT_ATTACHED_DEVICE_RETRIEVE = 100;
    // int ID_PT_ATTACHED_DEVICE_UPDATE = 101;
    // int ID_PT_ATTACHED_DEVICE_DELETE = 102;
    // int ID_PT_NOTIFICATION_CHANNELS_RETRIEVE = 103;
    // int ID_PT_NOTIFICATION_CHANNEL_CREATE = 104;
    // int ID_PT_NOTIFICATION_CHANNEL_RETRIEVE = 105;
    // int ID_PT_NOTIFICATION_CHANNEL_DELETE = 106;
    // int ID_PT_NOTIFICATION_CHANNEL_NOTIFY = 107;
    // int ID_PT_DISCOVERY_RETRIEVE = 108;

    String PT_SCL_BASE_RETRIEVE_REQUEST = "sclBaseRetrieveRequestIndication";
    String PT_SCL_BASE_UPDATE_REQUEST = "sclBaseUpdateRequestIndication";
    String PT_SCLS_RETRIEVE_REQUEST = "sclsRetrieveRequestIndication";
    String PT_SCLS_UPDATE_REQUEST = "sclsUpdateRequestIndication";
    String PT_SCL_CREATE_REQUEST = "sclCreateRequestIndication";
    String PT_SCL_RETRIEVE_REQUEST = "sclRetrieveRequestIndication";
    String PT_SCL_UPDATE_REQUEST = "sclUpdateRequestIndication";
    String PT_SCL_DELETE_REQUEST = "sclDeleteRequestIndication";
    String PT_APPLICATIONS_RETRIEVE_REQUEST = "applicationsRetrieveRequestIndication";
    String PT_APPLICATIONS_UPDATE_REQUEST = "applicationsUpdateRequestIndication";
    String PT_APPLICATION_CREATE_REQUEST = "applicationCreateRequestIndication";
    String PT_APPLICATION_RETRIEVE_REQUEST = "applicationRetrieveRequestIndication";
    String PT_APPLICATION_UPDATE_REQUEST = "applicationUpdateRequestIndication";
    String PT_APPLICATION_DELETE_REQUEST = "applicationDeleteRequestIndication";
    String PT_APPLICATION_ANNC_CREATE_REQUEST = "applicationAnncCreateRequestIndication";
    String PT_APPLICATION_ANNC_RETRIEVE_REQUEST = "applicationAnncRetrieveRequestIndication";
    String PT_APPLICATION_ANNC_UPDATE_REQUEST = "applicationAnncUpdateRequestIndication";
    String PT_APPLICATION_ANNC_DELETE_REQUEST = "applicationAnncDeleteRequestIndication";
    String PT_ACCESS_RIGHTS_RETRIEVE_REQUEST = "accessRightsRetrieveRequestIndication";
    String PT_ACCESS_RIGHTS_UPDATE_REQUEST = "accessRightsUpdateRequestIndication";
    String PT_ACCESS_RIGHT_CREATE_REQUEST = "accessRightCreateRequestIndication";
    String PT_ACCESS_RIGHT_RETRIEVE_REQUEST = "accessRightRetrieveRequestIndication";
    String PT_ACCESS_RIGHT_UPDATE_REQUEST = "accessRightUpdateRequestIndication";
    String PT_ACCESS_RIGHT_DELETE_REQUEST = "accessRightDeleteRequestIndication";
    String PT_ACCESS_RIGHT_ANNC_CREATE_REQUEST = "accessRightAnncCreateRequestIndication";
    String PT_ACCESS_RIGHT_ANNC_RETRIEVE_REQUEST = "accessRightAnncRetrieveRequestIndication";
    String PT_ACCESS_RIGHT_ANNC_UPDATE_REQUEST = "accessRightAnncUpdateRequestIndication";
    String PT_ACCESS_RIGHT_ANNC_DELETE_REQUEST = "accessRightAnncDeleteRequestIndication";
    String PT_CONTAINERS_RETRIEVE_REQUEST = "containersRetrieveRequestIndication";
    String PT_CONTAINERS_UPDATE_REQUEST = "containersUpdateRequestIndication";
    String PT_CONTAINER_CREATE_REQUEST = "containerCreateRequestIndication";
    String PT_CONTAINER_RETRIEVE_REQUEST = "containerRetrieveRequestIndication";
    String PT_CONTAINER_UPDATE_REQUEST = "containerUpdateRequestIndication";
    String PT_CONTAINER_DELETE_REQUEST = "containerDeleteRequestIndication";
    String PT_CONTAINER_ANNC_CREATE_REQUEST = "containerAnncCreateRequestIndication";
    String PT_CONTAINER_ANNC_RETRIEVE_REQUEST = "containerAnncRetrieveRequestIndication";
    String PT_CONTAINER_ANNC_UPDATE_REQUEST = "containerAnncUpdateRequestIndication";
    String PT_CONTAINER_ANNC_DELETE_REQUEST = "containerAnncDeleteRequestIndication";
    String PT_LOCATION_CONTAINER_CREATE_REQUEST = "locationContainerCreateRequestIndication";
    String PT_LOCATION_CONTAINER_RETRIEVE_REQUEST = "locationContainerRetrieveRequestIndication";
    String PT_LOCATION_CONTAINER_UPDATE_REQUEST = "locationContainerUpdateRequestIndication";
    String PT_LOCATION_CONTAINER_DELETE_REQUEST = "locationContainerDeleteRequestIndication";
    String PT_LOCATION_CONTAINER_ANNC_CREATE_REQUEST = "locationContainerAnncCreateRequestIndication";
    String PT_LOCATION_CONTAINER_ANNC_RETRIEVE_REQUEST = "locationContainerAnncRetrieveRequestIndication";
    String PT_LOCATION_CONTAINER_ANNC_UPDATE_REQUEST = "locationContainerAnncUpdateRequestIndication";
    String PT_LOCATION_CONTAINER_ANNC_DELETE_REQUEST = "locationContainerAnncDeleteRequestIndication";
    String PT_CONTENT_INSTANCES_RETRIEVE_REQUEST = "contentInstancesRetrieveRequestIndication";
    String PT_CONTENT_INSTANCE_CREATE_REQUEST = "contentInstanceCreateRequestIndication";
    String PT_CONTENT_INSTANCE_RETRIEVE_REQUEST = "contentInstanceRetrieveRequestIndication";
    String PT_CONTENT_INSTANCE_DELETE_REQUEST = "contentInstanceDeleteRequestIndication";
    String PT_GROUPS_RETRIEVE_REQUEST = "groupsRetrieveRequestIndication";
    String PT_GROUPS_UPDATE_REQUEST = "groupsUpdateRequestIndication";
    String PT_GROUP_CREATE_REQUEST = "groupCreateRequestIndication";
    String PT_GROUP_RETRIEVE_REQUEST = "groupRetrieveRequestIndication";
    String PT_GROUP_UPDATE_REQUEST = "groupUpdateRequestIndication";
    String PT_GROUP_DELETE_REQUEST = "groupDeleteRequestIndication";
    String PT_GROUP_ANNC_CREATE_REQUEST = "groupAnncCreateRequestIndication";
    String PT_GROUP_ANNC_RETRIEVE_REQUEST = "groupAnncRetrieveRequestIndication";
    String PT_GROUP_ANNC_UPDATE_REQUEST = "groupAnncUpdateRequestIndication";
    String PT_GROUP_ANNC_DELETE_REQUEST = "groupAnncDeleteRequestIndication";
    String PT_MEMBERS_CONTENT_CREATE_REQUEST = "membersContentCreateRequestIndication";
    String PT_MEMBERS_CONTENT_RETRIEVE_REQUEST = "membersContentRetrieveRequestIndication";
    String PT_MEMBERS_CONTENT_UPDATE_REQUEST = "membersContentUpdateRequestIndication";
    String PT_MEMBERS_CONTENT_DELETE_REQUEST = "membersContentDeleteRequestIndication";
    String PT_SUBSCRIPTIONS_RETRIEVE_REQUEST = "subscriptionsRetrieveRequestIndication";
    String PT_SUBSCRIPTION_CREATE_REQUEST = "subscriptionCreateRequestIndication";
    String PT_SUBSCRIPTION_RETRIEVE_REQUEST = "subscriptionRetrieveRequestIndication";
    String PT_SUBSCRIPTION_UPDATE_REQUEST = "subscriptionUpdateRequestIndication";
    String PT_SUBSCRIPTION_DELETE_REQUEST = "subscriptionDeleteRequestIndication";
    String PT_SUBSCRIPTION_NOTIFY_REQUEST = "subscriptionNotifyRequestIndication";
    String PT_M2M_POCS_RETRIEVE_REQUEST = "m2mPocsRetrieveRequestIndication";
    String PT_M2M_POC_CREATE_REQUEST = "m2mPocCreateRequestIndication";
    String PT_M2M_POC_RETRIEVE_REQUEST = "m2mPocRetrieveRequestIndication";
    String PT_M2M_POC_UPDATE_REQUEST = "m2mPocUpdateRequestIndication";
    String PT_M2M_POC_DELETE_REQUEST = "m2mPocDeleteRequestIndication";
    String PT_MGMT_OBJS_RETRIEVE_REQUEST = "mgmtObjsRetrieveRequestIndication";
    String PT_MGMT_OBJS_UPDATE_REQUEST = "mgmtObjsUpdateRequestIndication";
    String PT_MGMT_OBJ_CREATE_REQUEST = "mgmtObjCreateRequestIndication";
    String PT_MGMT_OBJ_RETRIEVE_REQUEST = "mgmtObjRetrieveRequestIndication";
    String PT_MGMT_OBJ_UPDATE_REQUEST = "mgmtObjUpdateRequestIndication";
    String PT_MGMT_OBJ_DELETE_REQUEST = "mgmtObjDeleteRequestIndication";
    String PT_MGMT_OBJ_EXECUTE_REQUEST = "mgmtObjExecuteRequestIndication";
    String PT_PARAMETERS_CREATE_REQUEST = "parametersCreateRequestIndication";
    String PT_PARAMETERS_RETRIEVE_REQUEST = "parametersRetrieveRequestIndication";
    String PT_PARAMETERS_UPDATE_REQUEST = "parametersUpdateRequestIndication";
    String PT_PARAMETERS_DELETE_REQUEST = "parametersDeleteRequestIndication";
    String PT_PARAMETERS_EXECUTE_REQUEST = "parametersExecuteRequestIndication";
    String PT_MGMT_CMD_CREATE_REQUEST = "mgmtCmdCreateRequestIndication";
    String PT_MGMT_CMD_RETRIEVE_REQUEST = "mgmtCmdRetrieveRequestIndication";
    String PT_MGMT_CMD_UPDATE_REQUEST = "mgmtCmdUpdateRequestIndication";
    String PT_MGMT_CMD_DELETE_REQUEST = "mgmtCmdDeleteRequestIndication";
    String PT_MGMT_CMD_EXECUTE_REQUEST = "mgmtCmdExecuteRequestIndication";
    String PT_EXEC_INSTANCES_RETRIEVE_REQUEST = "execInstancesRetrieveRequestIndication";
    String PT_EXEC_INSTANCE_RETRIEVE_REQUEST = "execInstanceRetrieveRequestIndication";
    String PT_EXEC_INSTANCE_DELETE_REQUEST = "execInstanceDeleteRequestIndication";
    String PT_EXEC_INSTANCE_EXECUTE_REQUEST = "execInstanceExecuteRequestIndication";
    String PT_ATTACHED_DEVICES_RETRIEVE_REQUEST = "attachedDevicesRetrieveRequestIndication";
    String PT_ATTACHED_DEVICES_UPDATE_REQUEST = "attachedDevicesUpdateRequestIndication";
    String PT_ATTACHED_DEVICE_CREATE_REQUEST = "attachedDeviceCreateRequestIndication";
    String PT_ATTACHED_DEVICE_RETRIEVE_REQUEST = "attachedDeviceRetrieveRequestIndication";
    String PT_ATTACHED_DEVICE_UPDATE_REQUEST = "attachedDeviceUpdateRequestIndication";
    String PT_ATTACHED_DEVICE_DELETE_REQUEST = "attachedDeviceDeleteRequestIndication";
    String PT_NOTIFICATION_CHANNELS_RETRIEVE_REQUEST = "notificationChannelsRetrieveRequestIndication";
    String PT_NOTIFICATION_CHANNEL_CREATE_REQUEST = "notificationChannelCreateRequestIndication";
    String PT_NOTIFICATION_CHANNEL_RETRIEVE_REQUEST = "notificationChannelRetrieveRequestIndication";
    String PT_NOTIFICATION_CHANNEL_DELETE_REQUEST = "notificationChannelDeleteRequestIndication";
    String PT_NOTIFICATION_CHANNEL_NOTIFY_REQUEST = "notificationChannelNotifyRequestIndication";
    String PT_DISCOVERY_RETRIEVE_REQUEST = "discoveryRetrieveRequestIndication";

    String PT_SCL_BASE_RETRIEVE_RESPONSE = "sclBaseRetrieveResponseConfirm";
    String PT_SCL_BASE_UPDATE_RESPONSE = "sclBaseUpdateResponseConfirm";
    String PT_SCLS_RETRIEVE_RESPONSE = "sclsRetrieveResponseConfirm";
    String PT_SCLS_UPDATE_RESPONSE = "sclsUpdateResponseConfirm";
    String PT_SCL_CREATE_RESPONSE = "sclCreateResponseConfirm";
    String PT_SCL_RETRIEVE_RESPONSE = "sclRetrieveResponseConfirm";
    String PT_SCL_UPDATE_RESPONSE = "sclUpdateResponseConfirm";
    String PT_SCL_DELETE_RESPONSE = "sclDeleteResponseConfirm";
    String PT_APPLICATIONS_RETRIEVE_RESPONSE = "applicationsRetrieveResponseConfirm";
    String PT_APPLICATIONS_UPDATE_RESPONSE = "applicationsUpdateResponseConfirm";
    String PT_APPLICATION_CREATE_RESPONSE = "applicationCreateResponseConfirm";
    String PT_APPLICATION_RETRIEVE_RESPONSE = "applicationRetrieveResponseConfirm";
    String PT_APPLICATION_UPDATE_RESPONSE = "applicationUpdateResponseConfirm";
    String PT_APPLICATION_DELETE_RESPONSE = "applicationDeleteResponseConfirm";
    String PT_APPLICATION_ANNC_CREATE_RESPONSE = "applicationAnncCreateResponseConfirm";
    String PT_APPLICATION_ANNC_RETRIEVE_RESPONSE = "applicationAnncRetrieveResponseConfirm";
    String PT_APPLICATION_ANNC_UPDATE_RESPONSE = "applicationAnncUpdateResponseConfirm";
    String PT_APPLICATION_ANNC_DELETE_RESPONSE = "applicationAnncDeleteResponseConfirm";
    String PT_ACCESS_RIGHTS_RETRIEVE_RESPONSE = "accessRightsRetrieveResponseConfirm";
    String PT_ACCESS_RIGHTS_UPDATE_RESPONSE = "accessRightsUpdateResponseConfirm";
    String PT_ACCESS_RIGHT_CREATE_RESPONSE = "accessRightCreateResponseConfirm";
    String PT_ACCESS_RIGHT_RETRIEVE_RESPONSE = "accessRightRetrieveResponseConfirm";
    String PT_ACCESS_RIGHT_UPDATE_RESPONSE = "accessRightUpdateResponseConfirm";
    String PT_ACCESS_RIGHT_DELETE_RESPONSE = "accessRightDeleteResponseConfirm";
    String PT_ACCESS_RIGHT_ANNC_CREATE_RESPONSE = "accessRightAnncCreateResponseConfirm";
    String PT_ACCESS_RIGHT_ANNC_RETRIEVE_RESPONSE = "accessRightAnncRetrieveResponseConfirm";
    String PT_ACCESS_RIGHT_ANNC_UPDATE_RESPONSE = "accessRightAnncUpdateResponseConfirm";
    String PT_ACCESS_RIGHT_ANNC_DELETE_RESPONSE = "accessRightAnncDeleteResponseConfirm";
    String PT_CONTAINERS_RETRIEVE_RESPONSE = "containersRetrieveResponseConfirm";
    String PT_CONTAINERS_UPDATE_RESPONSE = "containersUpdateResponseConfirm";
    String PT_CONTAINER_CREATE_RESPONSE = "containerCreateResponseConfirm";
    String PT_CONTAINER_RETRIEVE_RESPONSE = "containerRetrieveResponseConfirm";
    String PT_CONTAINER_UPDATE_RESPONSE = "containerUpdateResponseConfirm";
    String PT_CONTAINER_DELETE_RESPONSE = "containerDeleteResponseConfirm";
    String PT_CONTAINER_ANNC_CREATE_RESPONSE = "containerAnncCreateResponseConfirm";
    String PT_CONTAINER_ANNC_RETRIEVE_RESPONSE = "containerAnncRetrieveResponseConfirm";
    String PT_CONTAINER_ANNC_UPDATE_RESPONSE = "containersAnncUpdateResponseConfirm";
    String PT_CONTAINER_ANNC_DELETE_RESPONSE = "containerAnncDeleteResponseConfirm";
    String PT_LOCATION_CONTAINER_CREATE_RESPONSE = "locationContainerCreateResponseConfirm";
    String PT_LOCATION_CONTAINER_RETRIEVE_RESPONSE = "locationContainerRetrieveResponseConfirm";
    String PT_LOCATION_CONTAINER_UPDATE_RESPONSE = "locationContainerUpdateResponseConfirm";
    String PT_LOCATION_CONTAINERS_DELETE_RESPONSE = "locationContainerDeleteResponseConfirm";
    String PT_LOCATION_CONTAINER_ANNC_CREATE_RESPONSE = "locationContainerAnncCreateResponseConfirm";
    String PT_LOCATION_CONTAINER_ANNC_RETRIEVE_RESPONSE = "locationContainerAnncRetrieveResponseConfirm";
    String PT_LOCATION_CONTAINER_ANNC_UPDATE_RESPONSE = "locationContainerAnncUpdateResponseConfirm";
    String PT_LOCATION_CONTAINER_ANNC_DELETE_RESPONSE = "locationContainerAnncDeleteResponseConfirm";
    String PT_CONTENT_INSTANCES_RETRIEVE_RESPONSE = "contentInstancesRetrieveResponseConfirm";
    String PT_CONTENT_INSTANCE_CREATE_RESPONSE = "contentInstanceCreateResponseConfirm";
    String PT_CONTENT_INSTANCE_RETRIEVE_RESPONSE = "contentInstanceRetrieveResponseConfirm";
    String PT_CONTENT_INSTANCE_DELETE_RESPONSE = "contentInstanceDeleteResponseConfirm";
    String PT_GROUPS_RETRIEVE_RESPONSE = "groupsRetrieveResponseConfirm";
    String PT_GROUPS_UPDATE_RESPONSE = "groupsUpdateResponseConfirm";
    String PT_GROUP_CREATE_RESPONSE = "groupCreateResponseConfirm";
    String PT_GROUP_RETRIEVE_RESPONSE = "groupRetrieveResponseConfirm";
    String PT_GROUP_UPDATE_RESPONSE = "groupUpdateResponseConfirm";
    String PT_GROUP_DELETE_RESPONSE = "groupDeleteResponseConfirm";
    String PT_GROUP_ANNC_CREATE_RESPONSE = "groupAnncCreateResponseConfirm";
    String PT_GROUP_ANNC_RETRIEVE_RESPONSE = "groupAnncRetrieveResponseConfirm";
    String PT_GROUP_ANNC_UPDATE_RESPONSE = "groupAnncUpdateResponseConfirm";
    String PT_GROUP_ANNC_DELETE_RESPONSE = "groupAnncDeleteResponseConfirm";
    String PT_MEMBERS_CONTENT_CREATE_RESPONSE = "membersContentCreateResponseConfirm";
    String PT_MEMBERS_CONTENT_RETRIEVE_RESPONSE = "membersContentRetrieveResponseConfirm";
    String PT_MEMBERS_CONTENT_UPDATE_RESPONSE = "membersContentUpdateResponseConfirm";
    String PT_MEMBERS_CONTENT_DELETE_RESPONSE = "membersContentDeleteResponseConfirm";
    String PT_SUBSCRIPTIONS_RETRIEVE_RESPONSE = "subscriptionsRetrieveResponseConfirm";
    String PT_SUBSCRIPTION_CREATE_RESPONSE = "subscriptionCreateResponseConfirm";
    String PT_SUBSCRIPTION_RETRIEVE_RESPONSE = "subscriptionRetrieveResponseConfirm";
    String PT_SUBSCRIPTION_UPDATE_RESPONSE = "subscriptionUpdateResponseConfirm";
    String PT_SUBSCRIPTION_DELETE_RESPONSE = "subscriptionDeleteResponseConfirm";
    String PT_SUBSCRIPTION_NOTIFY_RESPONSE = "subscriptionNotifyResponseConfirm";
    String PT_M2M_POCS_RETRIEVE_RESPONSE = "m2mPocsRetrieveResponseConfirm";
    String PT_M2M_POC_CREATE_RESPONSE = "m2mPocCreateResponseConfirm";
    String PT_M2M_POC_RETRIEVE_RESPONSE = "m2mPocRetrieveResponseConfirm";
    String PT_M2M_POC_UPDATE_RESPONSE = "m2mPocUpdateResponseConfirm";
    String PT_M2M_POC_DELETE_RESPONSE = "m2mPocDeleteResponseConfirm";
    String PT_MGMT_OBJS_RETRIEVE_RESPONSE = "mgmtObjsRetrieveResponseConfirm";
    String PT_MGMT_OBJS_UPDATE_RESPONSE = "mgmtObjsUpdateResponseConfirm";
    String PT_MGMT_OBJ_CREATE_RESPONSE = "mgmtObjCreateResponseConfirm";
    String PT_MGMT_OBJ_RETRIEVE_RESPONSE = "mgmtObjRetrieveResponseConfirm";
    String PT_MGMT_OBJ_UPDATE_RESPONSE = "mgmtObjUpdateResponseConfirm";
    String PT_MGMT_OBJ_DELETE_RESPONSE = "mgmtObjDeleteResponseConfirm";
    String PT_MGMT_OBJ_EXECUTE_RESPONSE = "mgmtObjExecuteResponseConfirm";
    String PT_PARAMETERS_CREATE_RESPONSE = "parametersCreateResponseConfirm";
    String PT_PARAMETERS_RETRIEVE_RESPONSE = "parametersRetrieveResponseConfirm";
    String PT_PARAMETERS_UPDATE_RESPONSE = "parametersUpdateResponseConfirm";
    String PT_PARAMETERS_DELETE_RESPONSE = "parametersDeleteResponseConfirm";
    String PT_PARAMETERS_EXECUTE_RESPONSE = "parametersExecuteResponseConfirm";
    String PT_MGMT_CMD_CREATE_RESPONSE = "mgmtCmdCreateResponseConfirm";
    String PT_MGMT_CMD_RETRIEVE_RESPONSE = "mgmtCmdRetrieveResponseConfirm";
    String PT_MGMT_CMD_UPDATE_RESPONSE = "mgmtCmdUpdateResponseConfirm";
    String PT_MGMT_CMD_DELETE_RESPONSE = "mgmtCmdDeleteResponseConfirm";
    String PT_MGMT_CMD_EXECUTE_RESPONSE = "mgmtCmdExecuteResponseConfirm";
    String PT_EXEC_INSTANCES_RETRIEVE_RESPONSE = "execInstancesRetrieveResponseConfirm";
    String PT_EXEC_INSTANCE_RETRIEVE_RESPONSE = "execInstanceRetrieveResponseConfirm";
    String PT_EXEC_INSTANCE_DELETE_RESPONSE = "execInstanceDeleteResponseConfirm";
    String PT_EXEC_INSTANCE_EXECUTE_RESPONSE = "execInstanceExecuteResponseConfirm";
    String PT_ATTACHED_DEVICES_RETRIEVE_RESPONSE = "attachedDEvicesRetrieveResponseConfirm";
    String PT_ATTACHED_DEVICES_UPDATE_RESPONSE = "attachedDevicesUpdateResponseConfirm";
    String PT_ATTACHED_DEVICE_CREATE_RESPONSE = "attachedDeviceCreateResponseConfirm";
    String PT_ATTACHED_DEVICE_RETRIEVE_RESPONSE = "attachedDeviceRetrieveResponseConfirm";
    String PT_ATTACHED_DEVICE_UPDATE_RESPONSE = "attachedDeviceUpdateResponseConfirm";
    String PT_ATTACHED_DEVICE_DELETE_RESPONSE = "attachedDeviceDeleteResponseConfirm";
    String PT_NOTIFICATION_CHANNELS_RETRIEVE_RESPONSE = "notificationChannelsRetrieveResponseConfirm";
    String PT_NOTIFICATION_CHANNEL_CREATE_RESPONSE = "notificationChannelCreateResponseConfirm";
    String PT_NOTIFICATION_CHANNEL_RETRIEVE_RESPONSE = "notificationChannelRetrieveResponseConfirm";
    String PT_NOTIFICATION_CHANNEL_DELETE_RESPONSE = "notificationChannelDeleteResponseConfirm";
    String PT_NOTIFICATION_CHANNEL_NOTIFY_RESPONSE = "notificationChannelNotifyResponseConfirm";
    String PT_DISCOVERY_RETRIEVE_RESPONSE = "discoveryRetrieveResponseConfirm";

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
    int ID_RES_CONTAINERS = 10;
    int ID_RES_CONTAINER = 11;
    int ID_RES_CONTAINER_ANNC = 12;
    int ID_RES_LOCATION_CONTAINER = 13;
    int ID_RES_LOCATION_CONTAINER_ANNC = 14;
    int ID_RES_CONTENT_INSTANCES = 15;
    int ID_RES_CONTENT_INSTANCE = 16;
    int ID_RES_GROUPS = 17;
    int ID_RES_GROUP = 18;
    int ID_RES_GROUP_ANNC = 19;
    int ID_RES_MEMBERS_CONTENT = 20;
    int ID_RES_SUBSCRIPTIONS = 21;
    int ID_RES_SUBSCRIPTION = 22;
    int ID_RES_M2M_POCS = 23;
    int ID_RES_M2M_POC = 24;
    int ID_RES_MGMT_OBJS = 25;
    int ID_RES_MGMT_OBJ = 26;
    int ID_RES_PARAMETERS = 27;
    int ID_RES_MGMT_CMD = 28;
    int ID_RES_EXEC_INSTANCES = 29;
    int ID_RES_EXEC_INSTANCE = 30;
    int ID_RES_ATTACHED_DEVICES = 31;
    int ID_RES_ATTACHED_DEVICE = 32;
    int ID_RES_NOTIFICATION_CHANNELS = 33;
    int ID_RES_NOTIFICATION_CHANNEL = 34;
    int ID_RES_DISCOVERY = 35;

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
    int ID_ATTR_CONTACT = 17;
    int ID_ATTR_CONTACT_INFO = 18;
    int ID_ATTR_CONTACT_U_R_I = 19;
    int ID_ATTR_CONTAINERS_REFERENCE = 20;
    int ID_ATTR_CONTAINER_ANNC_COLLECTION = 21;
    int ID_ATTR_CONTAINER_COLLECTION = 22;
    int ID_ATTR_CONTENT_INSTANCES_REFERENCE = 23;
    int ID_ATTR_CONTENT_INSTANCE_COLLECTION = 24;
    int ID_ATTR_CONTENT = 25;
    int ID_ATTR_CONTENT_SIZE = 26;
    int ID_ATTR_CONTENT_TYPES = 27;
    int ID_ATTR_CREATION_TIME = 28;
    int ID_ATTR_CURRENT_BYTE_SIZE = 29;
    int ID_ATTR_CURRENT_NR_OF_INSTANCES = 30;
    int ID_ATTR_DELAY_TOLERANCE = 31;
    int ID_ATTR_DISCOVERY_REFERENCE = 32;
    int ID_ATTR_EXPIRATION_TIME = 33;
    int ID_ATTR_FILTER_CRITERIA = 34;
    int ID_ATTR_GROUPS_REFERENCE = 35;
    int ID_ATTR_GROUP_ANNC_COLLECTION = 36;
    int ID_ATTR_GROUP_COLLECTION = 37;
    int ID_ATTR_HREF = 38;
    int ID_ATTR_ID = 39;
    int ID_ATTR_INTEGRITY_VAL_RESULTS = 40;
    int ID_ATTR_LAST_MODIFIED_TIME = 41;
    int ID_ATTR_LINK = 42;
    int ID_ATTR_LOCATION_CONTAINER_ANNC_COLLECTION = 42;
    int ID_ATTR_LOCATION_CONTAINER_COLLECTION = 43;
    int ID_ATTR_LOCATION_CONTAINER_TYPE = 44;
    int ID_ATTR_LOC_REQUESTOR = 45;
    int ID_ATTR_LOC_TARGET_DEVICE = 46;
    int ID_ATTR_M2M_POCS_REFERENCE = 47;
    int ID_ATTR_M2M_POC_COLLECTION = 48;
    int ID_ATTR_MAX_BYTE_SIZE = 49;
    int ID_ATTR_MAX_INSTANCE_AGE = 50;
    int ID_ATTR_MAX_NR_OF_INSTANCES = 51;
    int ID_ATTR_MGMT_CMD_COLLECTION = 52;
    int ID_ATTR_MGMT_OBJS_REFERENCE = 53;
    int ID_ATTR_MGMT_OBJ_COLLECTION = 54;
    int ID_ATTR_MGMT_PROTOCOL_TYPE = 55;
    int ID_ATTR_MINIMAL_TIME_BETWEEN_NOTIFICATIONS = 56;
    int ID_ATTR_NOTIFICATION_CHANNELS_REFERENCE = 57;
    int ID_ATTR_NOTIFICATION_CHANNEL_COLLECTION = 58;
    int ID_ATTR_ONLINE_STATUS = 59;
    int ID_ATTR_PERMISSIONS = 60;
    int ID_ATTR_POCS = 61;
    int ID_ATTR_REM_TRIGGER_ADDR = 62;
    int ID_ATTR_SCHEDULE = 63;
    int ID_ATTR_SCLS_REFERENCE = 64;
    int ID_ATTR_SCL_COLLECTION = 65;
    int ID_ATTR_SCL_ID = 66;
    int ID_ATTR_SEARCH_STRINGS = 67;
    int ID_ATTR_SELF_PERMISSIONS = 68;
    int ID_ATTR_SERVER_CAPABILITY = 69;
    int ID_ATTR_SUBSCRIPTIONS_REFERENCE = 70;
    int ID_ATTR_SUBSCRIPTION_COLLECTION = 71;
    int ID_ATTR_SUBSCRIPTION_TYPE = 72;

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
    int ID_MGMT_PROTOCOL_TYPE_BBF = 1;
    int ID_MGMT_PROTOCOL_TYPE_TR_069 = 2;

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
    String TYPE_CONTENT_INSTANCE = "contentInstance";
    String TYPE_M2M_POC = "m2mPoc";
    String TYPE_NOTIFICATION_CHANNEL = "notificationChannel";
    String TYPE_SCL = "scl";
    String TYPE_SUBSCRIPTION = "subscription";
    String TYPE_CONTENT_INSTANCES = "contentInstances";

    // ///////////////////////////////////////////////////////////////////////
    // Supported header
    // ///////////////////////////////////////////////////////////////////////
    String HD_X_ACY_SUPPORTED = "X-Acy-Supported";
    String SP_STORAGE_CONF = "storageconf";
}
