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
 * Please contact Actility, SA.,  4, rue Ampère 22300 LANNION FRANCE
 * or visit www.actility.com if you need additional
 * information or have any questions.
 *
 * id $Id: $
 * author $Author: $
 * version $Revision: $
 * lastrevision $Date: $
 * modifiedby $LastChangedBy: $
 * lastmodified $LastChangedDate: $
 */

package com.actility.m2m.system.version;

import java.util.List;

import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.system.config.Configuration;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

public final class ResourcesGenerator {
    private final XoService xoService;
    private final Configuration configuration;

    /**
     * Constructor
     *
     * @param xoService The XO service in charge of serializing objects
     * @param configuration The configuration object
     */
    public ResourcesGenerator(XoService xoService, Configuration configuration) {
        this.xoService = xoService;
        this.configuration = configuration;
    }

    public XoObject generateCreateSystemApplication() throws XoException {
        XoObject xoApp = generateUpdateSystemApplication();
        xoApp.setStringAttribute(M2MConstants.ATTR_APP_ID, "SYSTEM");
        return xoApp;
    }

    public XoObject generateUpdateSystemApplication() throws XoException {
        XoObject xoApp = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_APPLICATION);
        try {
            xoApp.setNameSpace(M2MConstants.PREFIX_M2M);

            XoObject xoSearchStrings = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_SEARCH_STRINGS);
            xoApp.setXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS, xoSearchStrings);
            List searchStrings = xoSearchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
            searchStrings.add("ETSI.ObjectTechnology/ACTILITY.SYSTEM");
            searchStrings.add("ETSI.ObjectType/ETSI.GA");

            XoObject xoApocPaths = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_A_PO_C_PATHS);
            xoApp.setXoObjectAttribute(M2MConstants.TAG_M2M_A_PO_C_PATHS, xoApocPaths);
            List apocPaths = xoApocPaths.getXoObjectListAttribute(M2MConstants.TAG_M2M_A_PO_C_PATH);
            XoObject xoApocPath = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_A_PO_C_PATH);
            apocPaths.add(xoApocPath);
            xoApocPath.setStringAttribute(M2MConstants.TAG_M2M_PATH, "retargeting1");
            xoApocPath.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR2");
            xoApp.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR");
            xoApp.setStringAttribute(M2MConstants.TAG_M2M_A_PO_C, "/SYSTEM");
        } catch (XoException e) {
            Utils.releaseXoObjectFromMemory(xoApp);
            throw e;
        }
        return xoApp;
    }

    public XoObject generateCreateDescriptorContainer() throws XoException {
        XoObject xoCtn = generateUpdateDescriptorContainer();
        xoCtn.setStringAttribute(M2MConstants.ATTR_M2M_ID, "DESCRIPTOR");
        return xoCtn;
    }

    public XoObject generateUpdateDescriptorContainer() throws XoException {
        XoObject xoCtn = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_CONTAINER);
        try {
            xoCtn.setNameSpace(M2MConstants.PREFIX_M2M);

            XoObject xoSearchStrings = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_SEARCH_STRINGS);
            xoCtn.setXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS, xoSearchStrings);
            List searchStrings = xoSearchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
            searchStrings.add("ETSI.ObjectSemantic/OASIS.OBIX_1_1");
            xoCtn.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR");
            xoCtn.setStringAttribute(M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES, Integer.toString(1));
        } catch (XoException e) {
            Utils.releaseXoObjectFromMemory(xoCtn);
            throw e;
        }
        return xoCtn;
    }

    public XoObject generateCreateDescriptorContentInstance() throws XoException {
        XoObject xoObj = this.xoService.newObixXoObject("o:obj");
        try {
            xoObj.setStringAttribute("href", "/SYSTEM");
            List childs = xoObj.getXoObjectListAttribute("[]");
            XoObject xoStr = this.xoService.newObixXoObject("o:str");
            childs.add(xoStr);
            xoStr.setStringAttribute("name", "thingletID");
            xoStr.setStringAttribute("val", "SYSTEM");
            XoObject xoList = this.xoService.newObixXoObject("o:list");
            childs.add(xoList);
            xoList.setStringAttribute("name", "versions");
            xoList.setStringAttribute("href", "/m2m/applications/SYSTEM" + SystemVersionServlet.RETARGETING_LIST_VERSIONS);
            XoObject xoOp1 = this.xoService.newObixXoObject("o:op");
            childs.add(xoOp1);
            xoOp1.setStringAttribute("name", "openSshTunnel");
            xoOp1.setStringAttribute("href", "/m2m/applications/SYSTEM" + SystemVersionServlet.RETARGETING_OP_OPEN_SSH_TUNNEL);
            XoObject xoOp2 = this.xoService.newObixXoObject("o:op");
            childs.add(xoOp2);
            xoOp2.setStringAttribute("name", "closeSshTunnel");
            xoOp2.setStringAttribute("href", "/m2m/applications/SYSTEM" + SystemVersionServlet.RETARGETING_OP_CLOSE_SSH_TUNNEL);
            XoObject xoOp3 = this.xoService.newObixXoObject("o:op");
            childs.add(xoOp3);
            xoOp3.setStringAttribute("name", "uptime");
            xoOp3.setStringAttribute("href", "/m2m/applications/SYSTEM" + SystemVersionServlet.RETARGETING_OP_UPTIME);
            XoObject xoOp4 = this.xoService.newObixXoObject("o:op");
            childs.add(xoOp4);
            xoOp4.setStringAttribute("name", "memory");
            xoOp4.setStringAttribute("href", "/m2m/applications/SYSTEM" + SystemVersionServlet.RETARGETING_OP_MEMORY);
            XoObject xoOp5 = this.xoService.newObixXoObject("o:op");
            childs.add(xoOp5);
            xoOp5.setStringAttribute("name", "disk");
            xoOp5.setStringAttribute("href", "/m2m/applications/SYSTEM" + SystemVersionServlet.RETARGETING_OP_DISK);
            XoObject xoOp6 = this.xoService.newObixXoObject("o:op");
            childs.add(xoOp6);
            xoOp6.setStringAttribute("name", "diskIO");
            xoOp6.setStringAttribute("href", "/m2m/applications/SYSTEM" + SystemVersionServlet.RETARGETING_OP_DISK_IO);
            XoObject xoOp7 = this.xoService.newObixXoObject("o:op");
            childs.add(xoOp7);
            xoOp7.setStringAttribute("name", "netIO");
            xoOp7.setStringAttribute("href", "/m2m/applications/SYSTEM" + SystemVersionServlet.RETARGETING_OP_NET_IO);
            XoObject xoOp8 = this.xoService.newObixXoObject("o:op");
            childs.add(xoOp8);
            xoOp8.setStringAttribute("name", "power");
            xoOp8.setStringAttribute("href", "/m2m/applications/SYSTEM" + SystemVersionServlet.RETARGETING_OP_POWER);
            XoObject xoOp9 = this.xoService.newObixXoObject("o:op");
            childs.add(xoOp9);
            xoOp9.setStringAttribute("name", "temperature");
            xoOp9.setStringAttribute("href", "/m2m/applications/SYSTEM" + SystemVersionServlet.RETARGETING_OP_TEMPERATURE);
            XoObject xoOp10 = this.xoService.newObixXoObject("o:op");
            childs.add(xoOp10);
            xoOp10.setStringAttribute("name", "service-status");
            xoOp10.setStringAttribute("href", "/m2m/applications/SYSTEM" + SystemVersionServlet.RETARGETING_OP_SERVICES_STATUS);
            XoObject xoOp11 = this.xoService.newObixXoObject("o:op");
            childs.add(xoOp11);
            xoOp11.setStringAttribute("name", "reloadProfile");
            xoOp11.setStringAttribute("href", "/m2m/applications/SYSTEM" + SystemVersionServlet.RETARGETING_OP_RELOAD_PROFILE);
        } catch (XoException e) {
            Utils.releaseXoObjectFromMemory(xoObj);
            throw e;
        }
        return xoObj;
    }

    public XoObject generateCreateHighRateLogsContainer() throws XoException {
        XoObject xoCtn = generateUpdateHighRateLogsContainer();
        xoCtn.setStringAttribute(M2MConstants.ATTR_M2M_ID, "highRateLogs");
        return xoCtn;
    }

    public XoObject generateUpdateHighRateLogsContainer() throws XoException {
        XoObject xoCtn = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_CONTAINER);
        xoCtn.setNameSpace(M2MConstants.PREFIX_M2M);

        xoCtn.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR");
        xoCtn.setStringAttribute(M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES,
                ((Integer) configuration.getValue(Configuration.Name.HIGH_RATE_LOGGING_SIZE/* , Integer.class */)).toString());
        return xoCtn;
    }

    public XoObject generateCreateMediumRateLogsContainer() throws XoException {
        XoObject xoCtn = generateUpdateMediumRateLogsContainer();
        xoCtn.setStringAttribute(M2MConstants.ATTR_M2M_ID, "mediumRateLogs");
        return xoCtn;
    }

    public XoObject generateUpdateMediumRateLogsContainer() throws XoException {
        XoObject xoCtn = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_CONTAINER);
        xoCtn.setNameSpace(M2MConstants.PREFIX_M2M);

        xoCtn.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR");
        xoCtn.setStringAttribute(M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES,
                ((Integer) configuration.getValue(Configuration.Name.MEDIUM_RATE_LOGGING_SIZE/* , Integer.class */)).toString());
        return xoCtn;
    }

    public XoObject generateCreateLowRateLogsContainer() throws XoException {
        XoObject xoCtn = generateUpdateMediumRateLogsContainer();
        xoCtn.setStringAttribute(M2MConstants.ATTR_M2M_ID, "lowRateLogs");
        return xoCtn;
    }

    public XoObject generateUpdateLowRateLogsContainer() throws XoException {
        XoObject xoCtn = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_CONTAINER);
        xoCtn.setNameSpace(M2MConstants.PREFIX_M2M);

        xoCtn.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR");
        xoCtn.setStringAttribute(M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES,
                ((Integer) configuration.getValue(Configuration.Name.LOW_RATE_LOGGING_SIZE/* , Integer.class */)).toString());
        return xoCtn;
    }

    public XoObject generateCreateAlarmLogsContainer() throws XoException {
        XoObject xoCtn = generateUpdateAlarmLogsContainer();
        xoCtn.setStringAttribute(M2MConstants.ATTR_M2M_ID, "alarmLogs");
        return xoCtn;
    }

    public XoObject generateUpdateAlarmLogsContainer() throws XoException {
        XoObject xoCtn = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_CONTAINER);
        xoCtn.setNameSpace(M2MConstants.PREFIX_M2M);

        xoCtn.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR");
        xoCtn.setStringAttribute(M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES,
                ((Integer) configuration.getValue(Configuration.Name.ALARMS_LOGGING_SIZE/* , Integer.class */)).toString());
        return xoCtn;
    }

    public XoObject generateCreateResourcesContainer() throws XoException {
        XoObject xoCtn = generateUpdateResourcesContainer();
        xoCtn.setStringAttribute(M2MConstants.ATTR_M2M_ID, "resources");
        return xoCtn;
    }

    public XoObject generateUpdateResourcesContainer() throws XoException {
        XoObject xoCtn = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_CONTAINER);
        xoCtn.setNameSpace(M2MConstants.PREFIX_M2M);
        xoCtn.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR");
        xoCtn.setStringAttribute(M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES, "1");
        return xoCtn;
    }
}
