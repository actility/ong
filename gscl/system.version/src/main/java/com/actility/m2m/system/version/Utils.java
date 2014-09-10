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

import java.util.ArrayList;
import java.util.Calendar;
import java.util.Date;
import java.util.Dictionary;
import java.util.GregorianCalendar;
import java.util.List;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;

import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.system.log.BundleLogger;
import com.actility.m2m.system.monitoring.alarm.AlarmResult;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

public class Utils {

    private static final Logger LOG = OSGiLogger.getLogger(Utils.class, BundleLogger.LOG);

    private static Calendar calendar = new GregorianCalendar();

    /**
     * Release the xoObject from the memory
     * @param object the object to release
     */
    public static void releaseXoObjectFromMemory(XoObject object){
        if(object!=null){
            if(LOG.isDebugEnabled()){
                LOG.debug("Desintegration of :"+object.getName());
            }
            object.free(true);
            object=null;
        }
    }
    /**
     * Generates a errorInfo m2m xml document to send in response
     *
     * @param xoService the service needs to generate xml document
     * @param status The status error code
     * @param additionalInfo The additional information to put into the document
     * @return A content which defines an errorInfo document
     */
    public static byte[] generateErrorContent(XoService xoService, StatusCode status, String additionalInfo) {
        return Utils.generateErrorContent(xoService, status.toString(), additionalInfo);
    }
    /**
     * Generates a errorInfo m2m xml document to send in response
     *
     * @param xoService the service needs to generate xml document
     * @param status The status error code
     * @param additionalInfo The additional information to put into the document
     * @return A content which defines an errorInfo document
     */
    public static byte[] generateErrorContent(XoService xoService, String status, String additionalInfo) {
        byte[] content = null;
        XoObject xoError = null;
        try{
            xoError = xoService.newXmlXoObject(M2MConstants.TAG_M2M_ERROR_INFO);
            xoError.setNameSpace(M2MConstants.PREFIX_M2M);
            xoError.setStringAttribute(M2MConstants.TAG_M2M_STATUS_CODE, status);
            xoError.setStringAttribute(M2MConstants.TAG_M2M_ADDITIONAL_INFO, additionalInfo);
            content = xoError.saveXml();
        } catch (XoException e1) {
            LOG.error("Unable to create m2m error object", e1);
        } finally {
            Utils.releaseXoObjectFromMemory(xoError);
        }
        return content;
    }
    /**
     * Generate a alarm report m2m document to send
     * @param xoService
     * @param alarmResult
     * @return
     * @throws XoException
     */
    public static byte[] generateAlarmResultContent(XoService xoService, AlarmResult alarmResult) throws XoException{
        XoObject xoObj = xoService.newObixXoObject("o:obj");
        List childs = xoObj.getXoObjectListAttribute("[]");

        XoObject xoTimestamp = xoService.newObixXoObject("o:abstime");
        xoTimestamp.setStringAttribute("name", "timestamp");
        xoTimestamp.setStringAttribute("val", FormatUtils.formatDateTime(new Date(alarmResult.getTimestamp()), calendar));
        childs.add(xoTimestamp);

        XoObject xoOid = xoService.newObixXoObject("o:str");
        xoOid.setStringAttribute("name", "oid");
        xoOid.setStringAttribute("val", alarmResult.getName());
        childs.add(xoOid);

        if(alarmResult.getIndex()!=-1){
            XoObject xoVarIndex = xoService.newObixXoObject("o:int");
            xoVarIndex.setStringAttribute("name", "varIndex");
            xoVarIndex.setStringAttribute("val", Integer.toString(alarmResult.getIndex()));
            childs.add(xoVarIndex);
        }

        if(alarmResult.getInfo()!=null){
            XoObject xoVarInfo = xoService.newObixXoObject("o:str");
            xoVarInfo.setStringAttribute("name", "varInfo");
            xoVarInfo.setStringAttribute("val", alarmResult.getInfo());
            childs.add(xoVarInfo);
        }

        XoObject xoState = xoService.newObixXoObject("o:int");
        xoState.setStringAttribute("name", "state");
        xoState.setStringAttribute("val", Integer.toString(alarmResult.getState()));
        childs.add(xoState);
        byte[] result =  xoObj.saveXml();
        Utils.releaseXoObjectFromMemory(xoObj);
        return result;
    }
    /**
     * Gets the index of the given object in a list
     * @param list
     * @param object
     * @return
     */
    public static int getIndexOf(List/*<Indicator>*/ list, Object object){
        for(int i=0;i<list.size();i++){
            Object element = list.get(i);
            if(element==null ? object==null : element.equals(object)){
                return i;
            }
        }
        return -1;
    }
}
