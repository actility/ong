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

import java.io.IOException;
import java.io.Serializable;
import java.io.UnsupportedEncodingException;
import java.net.URISyntaxException;
import java.util.Iterator;
import java.util.List;

import javax.servlet.ServletException;

import org.apache.log4j.Logger;

import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.TimerService;
import com.actility.m2m.servlet.song.SongFactory;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.system.config.Configuration;
import com.actility.m2m.system.log.BundleLogger;
import com.actility.m2m.system.monitoring.MonitoringListener;
import com.actility.m2m.system.monitoring.MonitoringServiceLauncher;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

public class ResourcesManager implements MonitoringListener,Serializable{

    /**
     *
     */
    private static final long serialVersionUID = 7399088963715646922L;
    private static final Logger LOG = OSGiLogger.getLogger(ResourcesManager.class, BundleLogger.LOG);

    private ApplicationSession appSession;
    private SongFactory factory;
    private TimerService timerService;
    private XoService xoService;
    private Configuration configuration;
    private String lastOperation;
    private String lastTarget;
    private SongURI fakeUri;
    private boolean initialized;
    private ResourcesGenerator resGen;
    private MonitoringServiceLauncher monitoringLauncher;
    private SongServletRequest reloadProfileRequest;
    
    /**
     * Constructor
     * @param appSession
     * @param factory
     * @param xoService
     * @param timerService
     * @param configuration
     * @param monitoringLauncher
     */
    public ResourcesManager(ApplicationSession appSession, SongFactory factory, XoService xoService, TimerService timerService, Configuration configuration, MonitoringServiceLauncher monitoringLauncher) {
        this.appSession = appSession;
        this.factory = factory;
        this.xoService = xoService;
        this.timerService = timerService;
        this.configuration = configuration;
        this.lastOperation = null;
        this.monitoringLauncher = monitoringLauncher;
        this.initialized = false;
        this.resGen = new ResourcesGenerator(this.xoService, configuration);
        try {
            fakeUri = factory.createURI("http://a.com");
        } catch (URISyntaxException e) {
            LOG.error("Impossible to create uri", e);
        }
        this.reloadProfileRequest = null;
    }




    /**
     * This callback is called when a response arrives from a request sent by this class
     * @param status The status of response
     */
    public void onResponse(int status){
        try {
            LOG.info("Resource creation response status:" + status);
            if (lastOperation != null && lastTarget != null) {
                switch (status) {
                case SongServletResponse.SC_CREATED:
                    if (lastOperation.equals(SongServletRequest.MD_CREATE)) {
                        if (lastTarget.equals("/m2m/applications/SYSTEM")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/DESCRIPTOR"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/DESCRIPTOR")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/DESCRIPTOR/contentInstances"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/DESCRIPTOR/contentInstances/")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/resources"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/resources")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/highRateLogs"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/highRateLogs")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/mediumRateLogs"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/mediumRateLogs")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/lowRateLogs"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/lowRateLogs")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/alarmLogs"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/alarmLogs")) {
                            this.launchMonitoringService();
                        }
                    }
                    break;
                case SongServletResponse.SC_OK:
                case SongServletResponse.SC_NO_CONTENT:
                    if (lastOperation.equals(SongServletRequest.MD_UPDATE)) {
                        if (lastTarget.equals("/m2m/applications/SYSTEM")) {
                            this.sendUpdateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/DESCRIPTOR"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/DESCRIPTOR")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/DESCRIPTOR/contentInstances"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/DESCRIPTOR/contentInstances/")) {
                            this.sendUpdateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/resources"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/resources")) {
                            this.sendUpdateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/highRateLogs"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/highRateLogs")) {
                            this.sendUpdateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/mediumRateLogs"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/mediumRateLogs")) {
                            this.sendUpdateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/lowRateLogs"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/lowRateLogs")) {
                            this.sendUpdateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/alarmLogs"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/alarmLogs")) {
                            this.launchMonitoringService();
                        }
                    }
                    break;
                case SongServletResponse.SC_NOT_FOUND:
                    if (lastOperation.equals(SongServletRequest.MD_UPDATE)) {
                        if (lastTarget.equals("/m2m/applications/SYSTEM")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/DESCRIPTOR")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/DESCRIPTOR"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/resources")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/resources"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/highRateLogs")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/highRateLogs"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/mediumRateLogs")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/mediumRateLogs"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/lowRateLogs")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/lowRateLogs"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/alarmLogs")) {
                            this.sendCreateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/alarmLogs"));
                        }
                    }
                    break;
                case SongServletResponse.SC_CONFLICT:
                    if (lastOperation.equals(SongServletRequest.MD_CREATE)) {
                        if (lastTarget.equals("/m2m/applications/SYSTEM")) {
                            this.sendUpdateRequest(resGen.getResource("/m2m/applications/SYSTEM"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/DESCRIPTOR")) {
                            this.sendUpdateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/DESCRIPTOR"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/resources")) {
                            this.sendUpdateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/resources"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/highRateLogs")) {
                            this.sendUpdateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/highRateLogs"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/mediumRateLogs")) {
                            this.sendUpdateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/mediumRateLogs"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/lowRateLogs")) {
                            this.sendUpdateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/lowRateLogs"));
                        } else if (lastTarget.equals("/m2m/applications/SYSTEM/containers/alarmLogs")) {
                            this.sendUpdateRequest(resGen.getResource("/m2m/applications/SYSTEM/containers/alarmLogs"));
                        }
                    }
                    break;
                default:
                    break;
                }
            }
        }catch (XoException e) {
            LOG.error("fail to export XoObject as xml", e);
        }
    }
    /**
     * Launches the monitoring service
     */
    private void launchMonitoringService(){
        if(LOG.isInfoEnabled()){
            LOG.info("Preparing report service");
        }
        initialized = true;
        this.monitoringLauncher.launchMonitoringService();
    }
    /**
     * Sends a monitoring report
     */
    public void sendMonitoringReport(String target, byte[] content) {
        sendRequest(SongServletRequest.MD_CREATE, target, content, false);
    }

    /**
     * Sends the SONG CREATE request
     *
     * @param xobject The Xo Object to send
     */
    public void sendCreateRequest(XoObject xobject) throws IllegalArgumentException {
        try {
            String tmpPath;
            String target;
            if (xobject.getName().equals(M2MConstants.TAG_M2M_APPLICATION)) {
                tmpPath = "/m2m/applications/";
                target = "/m2m/applications/" + xobject.getStringAttribute(M2MConstants.ATTR_APP_ID);
            } else if (xobject.getName().equals(M2MConstants.TAG_M2M_CONTAINER)) {
                tmpPath = "/m2m/applications/SYSTEM/containers/";
                target = "/m2m/applications/SYSTEM/containers/" + xobject.getStringAttribute(M2MConstants.ATTR_M2M_ID);
            } else if (xobject.getName().equals("o:obj")) {
                tmpPath = "/m2m/applications/SYSTEM/containers/DESCRIPTOR/contentInstances/";
                target = tmpPath;
            } else {
                throw new IllegalArgumentException("The current resource (" + xobject.getName() + ") is not managed");
            }
            byte[] content = xobject.saveXml();
            sendRequest(SongServletRequest.MD_CREATE, tmpPath, content, target);
        } catch (XoException e) {
            LOG.error("sendUpdateRequest - fail to export XoObject as xml", e);
        }finally{
            Utils.releaseXoObjectFromMemory(xobject);
        }
    }

    /**
     * Sends the SONG UPDATE request
     *
     * @param xobject
     */
    public void sendUpdateRequest(XoObject xobject) throws IllegalArgumentException {
        try {
            String tmpPath;
            if (xobject.getName().equals(M2MConstants.TAG_M2M_APPLICATION)) {
                tmpPath = "/m2m/applications/" + xobject.getStringAttribute(M2MConstants.ATTR_APP_ID);
                xobject.setStringAttribute(M2MConstants.ATTR_APP_ID, null);
            } else if (xobject.getName().equals(M2MConstants.TAG_M2M_CONTAINER)) {
                tmpPath = "/m2m/applications/SYSTEM/containers/" + xobject.getStringAttribute(M2MConstants.ATTR_M2M_ID);
                xobject.setStringAttribute(M2MConstants.ATTR_M2M_ID, null);
            } else if (xobject.getName().equals(M2MConstants.TAG_M2M_CONTENT_INSTANCE)) {
                tmpPath = "/m2m/applications/SYSTEM/containers/DESCRIPTOR/contentInstances/";
            } else {
                throw new IllegalArgumentException("The current resource (" + xobject.getName() + ") is not managed");
            }
            byte[] content = xobject.saveXml();
            sendRequest(SongServletRequest.MD_UPDATE, tmpPath, content);
        } catch (XoException e) {
            LOG.error("sendUpdateRequest - fail to export XoObject as xml", e);
        }finally{
            Utils.releaseXoObjectFromMemory(xobject);
        }
    }
    /**
     * Sends a request
     * @param operation
     * @param path
     * @param content
     * @param context
     */
    private void sendRequest(String operation, String path, byte[] content, boolean context){
        this.sendRequest(operation, path, content, null,context);
    }
    /**
     * Sends a request
     * @param operation
     * @param path
     * @param content
     * @param target
     */
    private void sendRequest(String operation, String path, byte[] content, String target){
        this.sendRequest(operation, path, content, target,true);
    }
    /**
     * Sends a request
     * @param operation
     * @param path
     * @param content
     */
    private void sendRequest(String operation, String path, byte[] content){
        this.sendRequest(operation, path, content, null, true);
    }
    /**
     * Sends a request
     * @param operation
     * @param path
     * @param content
     * @param target
     * @param context
     */
    private void sendRequest(String operation, String path, byte[] content, String target, boolean context){
        try {
            SongURI toUri = this.generateUri(path);
            SongURI reqEntity = this.generateUri("/m2m/applications/GA");
            SongServletRequest songRequest = factory.createRequest(appSession, operation, reqEntity, toUri);
            songRequest.setContent(content, "application/xml; charset=utf-8");
            if(context){
                songRequest.setAttribute(SystemVersionServlet.REQUEST_CONTEXT, this);
            }
            if (LOG.isDebugEnabled()) {
                LOG.debug("Send SONG "+ operation +" request (To:" + toUri.absoluteURI() + ") (content:\n"
                        + new String(songRequest.getRawContent()) + ")");
            }
            if(!initialized){
                this.lastOperation = operation;
                if(target!=null){
                    this.lastTarget = target;
                }else{
                    this.lastTarget = path;
                }
            }else{
                this.lastOperation = null;
                this.lastTarget = null;
            }
            songRequest.send();
        } catch (ServletException e) {
            LOG.error("sendRequest - fail to create local Uri from fake uri", e);
            timerService.createTimer(appSession, ((Integer)configuration.getValue(Configuration.Name.TIME_BETWEEN_INIT/*, Integer.class*/)).intValue(), false, this);
        } catch(IOException e) {
            LOG.error("sendRequest - fail to send request", e);
        }

    }
    /**
     * Generates the SYSTEM application resource and its sub resources
     */
    public void generate() {
        initialized = false;
        if (LOG.isInfoEnabled()) {
            LOG.info("Beginning of resources generation");
        }
        try {
            this.sendUpdateRequest(resGen.getResource("/m2m/applications/SYSTEM"));
        } catch (XoException e) {
            LOG.error("Unable to create JXo object", e);
        } catch (IllegalArgumentException e) {
            LOG.error("The type of resource isn't supported", e);
        }
    }
    
    /**
     * Executes a profile reloading operation
     * @param request
     * @throws IOException
     */
    public void reloadProfile(SongServletRequest request) throws IOException{
        reloadProfileRequest = null;
        SongServletResponse retargetingResponse = null;
        if(request.getContentLength()>0){
            byte[] byteTab = request.getRawContent();
            if(byteTab!=null){
//                LOG.info("content type:"+request.getContentType());
//                LOG.info("char encoding:"+request.getCharacterEncoding());
                try {
                    XoObject xoUrl = xoService.readXmlObixXoObject(byteTab, request.getCharacterEncoding());
                    List xoUrlList =  xoUrl.getXoObjectListAttribute("[]");
                    Iterator iterator = xoUrlList.iterator();
                    while(iterator.hasNext()){
                        XoObject xoChild = (XoObject) iterator.next();
                        String xoType = xoChild.getType();
                        String xoName = xoChild.getStringAttribute("name");
//                        LOG.info("xoType:"+xoType+";xoName:"+xoName);
                        if(xoName!=null && xoName.equals("profileUrl")&& xoType!=null && xoType.equals("o:ref")){
                            String url = xoChild.getStringAttribute("href");
                            LOG.info("Profile Url :"+url);
                            try {
                                SongURI toUri = factory.createURI(url);
                                SongURI reqEntity = this.generateUri("/m2m/applications/GA");
                                SongServletRequest songRequest = factory.createRequest(appSession, SongServletRequest.MD_RETRIEVE, reqEntity, toUri);
                                songRequest.setAttribute(SystemVersionServlet.REQUEST_CONTEXT, this);
                                songRequest.send();
                                reloadProfileRequest = request;
                            } catch (URISyntaxException e) {
                                LOG.error("The uri isn't valid",e);
                                retargetingResponse = request.createResponse(SongServletResponse.SC_BAD_REQUEST);
                                byte[] content = Utils.generateErrorContent(xoService, StatusCode.STATUS_BAD_REQUEST, "The given url isn't valid");
                                if(content!=null){
                                    retargetingResponse.setContent(content, "application/xml; charset=utf-8");
                                }
                            } catch (ServletException e) {
                                LOG.error("An exception occured during request creation",e);
                                retargetingResponse = request.createResponse(SongServletResponse.SC_INTERNAL_SERVER_ERROR);
                                byte[] content = Utils.generateErrorContent(xoService, StatusCode.STATUS_INTERNAL_SERVER_ERROR, "Error during request creation");
                                if(content!=null){
                                    retargetingResponse.setContent(content, "application/xml; charset=utf-8");
                                }
                            }
                            break;
                        }
                    }
                    Utils.releaseXoObjectFromMemory(xoUrl);
                    if(retargetingResponse==null && reloadProfileRequest==null){
                        retargetingResponse = request.createResponse(SongServletResponse.SC_BAD_REQUEST);
                        byte[] content = Utils.generateErrorContent(xoService, StatusCode.STATUS_BAD_REQUEST, "Obix not contains the profile url");
                        if(content!=null){
                            retargetingResponse.setContent(content, "application/xml; charset=utf-8");
                        }
                    }
                } catch (XoException e) {
                    LOG.error("An error occured during obix decoding",e);
                    retargetingResponse = request.createResponse(SongServletResponse.SC_BAD_REQUEST);
                    byte[] content = Utils.generateErrorContent(xoService, StatusCode.STATUS_BAD_REQUEST, "Xml content is not formatted correctly");
                    if(content!=null){
                        retargetingResponse.setContent(content, "application/xml; charset=utf-8");
                    }
                }
            }else{
                retargetingResponse = request.createResponse(SongServletResponse.SC_BAD_REQUEST);
                byte[] content = Utils.generateErrorContent(xoService, StatusCode.STATUS_BAD_REQUEST, "Content is empty");
                if(content!=null){
                    retargetingResponse.setContent(content, "application/xml; charset=utf-8");
                }
            }
        }else{
            retargetingResponse = request.createResponse(SongServletResponse.SC_BAD_REQUEST);
            byte[] content = Utils.generateErrorContent(xoService, StatusCode.STATUS_BAD_REQUEST, "Content is empty");
            if(content!=null){
                retargetingResponse.setContent(content, "application/xml; charset=utf-8");
            }
        }
        if(reloadProfileRequest==null){
            retargetingResponse.send();
        }
    }
    /**
     * This method is called when the response of the profile's reloading is received
     * @param response
     * @throws IOException
     */
    public void onReloadProfileResponse(SongServletResponse response) throws IOException{
        if(reloadProfileRequest!=null){
            SongServletResponse retargetingResponse;
            int status = response.getStatus();
            if(status==200){
                if(response.getContentLength()>0){
                    byte[] byteTab = response.getRawContent();
                    if(byteTab!=null){
                        try {
                            XoObject xoUrl = xoService.readXmlObixXoObject(byteTab, response.getCharacterEncoding());
                            List xoUrlList =  xoUrl.getXoObjectListAttribute("[]");
                            Iterator iterator = xoUrlList.iterator();
                            while(iterator.hasNext()){
                                XoObject xoChild = (XoObject) iterator.next();
                                configuration.updateValue(xoChild);
                            }
                            configuration.saveConfig();
                            Utils.releaseXoObjectFromMemory(xoUrl);
                            retargetingResponse = reloadProfileRequest.createResponse(SongServletResponse.SC_OK);
                        } catch (XoException e) {
                            LOG.error("An error occured during obix decoding",e);
                            retargetingResponse = reloadProfileRequest.createResponse(SongServletResponse.SC_BAD_REQUEST);
                            byte[] content = Utils.generateErrorContent(xoService, StatusCode.STATUS_BAD_REQUEST, "Xml content is not formatted correctly");
                            if(content!=null){
                                retargetingResponse.setContent(content, "application/xml; charset=utf-8");
                            }
                        }
                    }else{
                        retargetingResponse = reloadProfileRequest.createResponse(SongServletResponse.SC_NO_CONTENT);
                        byte[] content = Utils.generateErrorContent(xoService, StatusCode.STATUS_BAD_REQUEST, "Response content is empty");
                        if(content!=null){
                            retargetingResponse.setContent(content, "application/xml; charset=utf-8");
                        }
                    }
                }else{
                    retargetingResponse = reloadProfileRequest.createResponse(SongServletResponse.SC_NO_CONTENT);
                    byte[] content = Utils.generateErrorContent(xoService, StatusCode.STATUS_BAD_REQUEST, "Response content is empty");
                    if(content!=null){
                        retargetingResponse.setContent(content, "application/xml; charset=utf-8");
                    }
                }
            }else{
                retargetingResponse = reloadProfileRequest.createResponse(status);
                byte[] content = Utils.generateErrorContent(xoService, response.getReasonPhrase(), "The response given by the reload profile url");
                if(content!=null){
                    retargetingResponse.setContent(content, "application/xml; charset=utf-8");
                }
           }
            retargetingResponse.send();
            reloadProfileRequest = null;
        }
    }
    /**
     * Returns true if m2m resources have been initialized
     * @return
     */
    public boolean isInitialized(){
        return initialized;
    }

    /**
     * Generates an uri which begins with the current ong base uri and ends with the given path
     * @param path
     * @return
     * @throws ServletException
     */
    private SongURI generateUri(String path) throws ServletException {
        if(LOG.isDebugEnabled()){
            LOG.debug("Creating uri from : "+path+" and from :"+fakeUri.absoluteURI());
        }
        SongURI uri = factory.createLocalURIFrom(fakeUri, path);
        uri.setPath(path);
        return uri;
    }
}
