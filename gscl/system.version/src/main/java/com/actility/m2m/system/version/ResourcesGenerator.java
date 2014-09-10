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

import java.util.HashMap;
import java.util.List;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.system.config.Configuration;
import com.actility.m2m.system.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

public class ResourcesGenerator {

    private static final Logger LOG = OSGiLogger.getLogger(ResourcesManager.class, BundleLogger.LOG);

    private XoService xoService;
    private Configuration configuration;

    private HashMap/*<String,XoObject>*/ resources;
//    private XoObject

    /**
     * Constructor
     * @param xoService
     * @param configuration
     */
    public ResourcesGenerator(XoService xoService, Configuration configuration){
        this.xoService = xoService;
        this.configuration = configuration;
        this.resources = new HashMap();
    }
    
    /**
     * Gets a m2m resource from its url
     * @param url
     * @return
     * @throws XoException
     */
    public XoObject getResource(String url) throws XoException{
        LOG.debug("Getting resource : "+url);
        XoObject xoObject = null;
        Object resource = this.resources.get(url);
        if(resource == null){
            xoObject = this.generateResource(url);
            this.resources.put(url, xoObject);
        }else{
            xoObject = (XoObject) resource;
            if(xoObject.getName() == null){
                xoObject = this.generateResource(url);
                this.resources.put(url, xoObject);
            }
        }
        return xoObject;
    }
    /**
     * Generates a resource from its url
     * @param url
     * @return
     * @throws XoException
     */
    private XoObject generateResource(String url) throws XoException{
        XoObject xoObject = null;
        if(url.equals("/m2m/applications/SYSTEM")){
            XoObject xoApp = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_APPLICATION);
            xoApp.setNameSpace(M2MConstants.PREFIX_M2M);
            xoApp.setStringAttribute(M2MConstants.ATTR_APP_ID, "SYSTEM");

            XoObject xoSearchStrings = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_SEARCH_STRINGS);
            List searchStrings = xoSearchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
            searchStrings.add("ETSI.ObjectTechnology/ACTILITY.SYSTEM");
            searchStrings.add("ETSI.ObjectType/ETSI.GA");
            xoApp.setXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS, xoSearchStrings);

            XoObject xoApocPaths = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_A_PO_C_PATHS);
            List apocPaths = xoApocPaths.getXoObjectListAttribute(M2MConstants.TAG_M2M_A_PO_C_PATH);
            XoObject xoApocPath = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_A_PO_C_PATH);
            xoApocPath.setStringAttribute(M2MConstants.TAG_M2M_PATH, "retargeting1");
            xoApocPath.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR2");
            apocPaths.add(xoApocPath);
            xoApp.setXoObjectAttribute(M2MConstants.TAG_M2M_A_PO_C_PATHS, xoApocPaths);
            xoApp.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR");
            xoApp.setStringAttribute(M2MConstants.TAG_M2M_A_PO_C, "/SYSTEM");
            xoObject = xoApp;
        }else if(url.equals("/m2m/applications/SYSTEM/containers/highRateLogs")){
            XoObject xoCtn = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_CONTAINER);
            xoCtn.setNameSpace(M2MConstants.PREFIX_M2M);
            xoCtn.setStringAttribute(M2MConstants.ATTR_M2M_ID, "highRateLogs");

            xoCtn.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR");
            xoCtn.setStringAttribute(M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES, ((Integer)configuration.getValue(Configuration.Name.HIGH_RATE_LOGGING_SIZE/*, Integer.class*/)).toString());
            xoObject = xoCtn;
        }else if(url.equals("/m2m/applications/SYSTEM/containers/mediumRateLogs")){
            XoObject xoCtn = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_CONTAINER);
            xoCtn.setNameSpace(M2MConstants.PREFIX_M2M);
            xoCtn.setStringAttribute(M2MConstants.ATTR_M2M_ID, "mediumRateLogs");

            xoCtn.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR");
            xoCtn.setStringAttribute(M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES, ((Integer)configuration.getValue(Configuration.Name.MEDIUM_RATE_LOGGING_SIZE/*, Integer.class*/)).toString());
            xoObject = xoCtn;
        }else if(url.equals("/m2m/applications/SYSTEM/containers/lowRateLogs")){
            XoObject xoCtn = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_CONTAINER);
            xoCtn.setNameSpace(M2MConstants.PREFIX_M2M);
            xoCtn.setStringAttribute(M2MConstants.ATTR_M2M_ID, "lowRateLogs");

            xoCtn.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR");
            xoCtn.setStringAttribute(M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES, ((Integer)configuration.getValue(Configuration.Name.LOW_RATE_LOGGING_SIZE/*, Integer.class*/)).toString());
            xoObject = xoCtn;
        }else if(url.equals("/m2m/applications/SYSTEM/containers/alarmLogs")){
            XoObject xoCtn = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_CONTAINER);
            xoCtn.setNameSpace(M2MConstants.PREFIX_M2M);
            xoCtn.setStringAttribute(M2MConstants.ATTR_M2M_ID, "alarmLogs");

            xoCtn.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR");
            xoCtn.setStringAttribute(M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES, ((Integer)configuration.getValue(Configuration.Name.ALARMS_LOGGING_SIZE/*, Integer.class*/)).toString());
            xoObject = xoCtn;
        }else if(url.equals("/m2m/applications/SYSTEM/containers/DESCRIPTOR")){
            XoObject xoCtn = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_CONTAINER);
            xoCtn.setNameSpace(M2MConstants.PREFIX_M2M);
            xoCtn.setStringAttribute(M2MConstants.ATTR_M2M_ID, "DESCRIPTOR");

            XoObject xoSearchStrings = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_SEARCH_STRINGS);
            List searchStrings = xoSearchStrings.getStringListAttribute(M2MConstants.TAG_M2M_SEARCH_STRING);
            searchStrings.add("ETSI.ObjectSemantic/OASIS.OBIX_1_1");
            xoCtn.setXoObjectAttribute(M2MConstants.TAG_M2M_SEARCH_STRINGS, xoSearchStrings);
            xoCtn.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR");
            xoCtn.setStringAttribute(M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES, Integer.toString(1));
            xoObject = xoCtn;
        }else if(url.equals("/m2m/applications/SYSTEM/containers/DESCRIPTOR/contentInstances")){
            XoObject xoObj = this.xoService.newObixXoObject("o:obj");
            xoObj.setStringAttribute("href", "/SYSTEM");
            List childs = xoObj.getXoObjectListAttribute("[]");
            XoObject xoStr = this.xoService.newObixXoObject("o:str");
            xoStr.setStringAttribute("name", "thingletID");
            xoStr.setStringAttribute("val", "SYSTEM");
            childs.add(xoStr);
            XoObject xoList = this.xoService.newObixXoObject("o:list");
            xoList.setStringAttribute("name", "versions");
            xoList.setStringAttribute("href", "/m2m/applications/SYSTEM"+SystemVersionServlet.RETARGETING_LIST_VERSIONS);
            childs.add(xoList);
            XoObject xoOp1 = this.xoService.newObixXoObject("o:op");
            xoOp1.setStringAttribute("name", "openSshTunnel");
            xoOp1.setStringAttribute("href", "/m2m/applications/SYSTEM"+SystemVersionServlet.RETARGETING_OP_OPEN_SSH_TUNNEL);
            childs.add(xoOp1);
            XoObject xoOp2 = this.xoService.newObixXoObject("o:op");
            xoOp2.setStringAttribute("name", "closeSshTunnel");
            xoOp2.setStringAttribute("href", "/m2m/applications/SYSTEM"+SystemVersionServlet.RETARGETING_OP_CLOSE_SSH_TUNNEL);
            childs.add(xoOp2);
            XoObject xoOp3 = this.xoService.newObixXoObject("o:op");
            xoOp3.setStringAttribute("name", "uptime");
            xoOp3.setStringAttribute("href", "/m2m/applications/SYSTEM"+SystemVersionServlet.RETARGETING_OP_UPTIME);
            childs.add(xoOp3);
            XoObject xoOp4 = this.xoService.newObixXoObject("o:op");
            xoOp4.setStringAttribute("name", "memory");
            xoOp4.setStringAttribute("href", "/m2m/applications/SYSTEM"+SystemVersionServlet.RETARGETING_OP_MEMORY);
            childs.add(xoOp4);
            XoObject xoOp5 = this.xoService.newObixXoObject("o:op");
            xoOp5.setStringAttribute("name", "disk");
            xoOp5.setStringAttribute("href", "/m2m/applications/SYSTEM"+SystemVersionServlet.RETARGETING_OP_DISK);
            childs.add(xoOp5);
            XoObject xoOp6 = this.xoService.newObixXoObject("o:op");
            xoOp6.setStringAttribute("name", "diskIO");
            xoOp6.setStringAttribute("href", "/m2m/applications/SYSTEM"+SystemVersionServlet.RETARGETING_OP_DISK_IO);
            childs.add(xoOp6);
            xoObject = xoObj;
            XoObject xoOp7 = this.xoService.newObixXoObject("o:op");
            xoOp7.setStringAttribute("name", "netIO");
            xoOp7.setStringAttribute("href", "/m2m/applications/SYSTEM"+SystemVersionServlet.RETARGETING_OP_NET_IO);
            childs.add(xoOp7);
            XoObject xoOp8 = this.xoService.newObixXoObject("o:op");
            xoOp8.setStringAttribute("name", "power");
            xoOp8.setStringAttribute("href", "/m2m/applications/SYSTEM"+SystemVersionServlet.RETARGETING_OP_POWER);
            childs.add(xoOp8);
            XoObject xoOp9 = this.xoService.newObixXoObject("o:op");
            xoOp9.setStringAttribute("name", "temperature");
            xoOp9.setStringAttribute("href", "/m2m/applications/SYSTEM"+SystemVersionServlet.RETARGETING_OP_TEMPERATURE);
            childs.add(xoOp9);
            XoObject xoOp10 = this.xoService.newObixXoObject("o:op");
            xoOp10.setStringAttribute("name", "service-status");
            xoOp10.setStringAttribute("href", "/m2m/applications/SYSTEM"+SystemVersionServlet.RETARGETING_OP_SERVICES_STATUS);
            childs.add(xoOp10);
            XoObject xoOp11 = this.xoService.newObixXoObject("o:op");
            xoOp11.setStringAttribute("name", "reloadProfile");
            xoOp11.setStringAttribute("href", "/m2m/applications/SYSTEM"+SystemVersionServlet.RETARGETING_OP_RELOAD_PROFILE);
            childs.add(xoOp11);
        }else if(url.equals("/m2m/applications/SYSTEM/containers/resources")){
            XoObject xoCtn = this.xoService.newXmlXoObject(M2MConstants.TAG_M2M_CONTAINER);
            xoCtn.setNameSpace(M2MConstants.PREFIX_M2M);
            xoCtn.setStringAttribute(M2MConstants.ATTR_M2M_ID, "resources");
            xoCtn.setStringAttribute(M2MConstants.TAG_M2M_ACCESS_RIGHT_I_D, "/m2m/accessRights/Locadmin_AR");
            xoCtn.setStringAttribute(M2MConstants.TAG_M2M_MAX_NR_OF_INSTANCES, "1");
            xoObject = xoCtn;
        }else{
            throw new UnsupportedOperationException("The resource '"+url+"' not exists");
        }
        return xoObject;
    }

}
