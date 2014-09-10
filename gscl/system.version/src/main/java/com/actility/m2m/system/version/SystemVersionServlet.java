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

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import javax.servlet.ServletException;

import org.apache.log4j.Logger;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;

import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.servlet.ApplicationSession;
import com.actility.m2m.servlet.ServletTimer;
import com.actility.m2m.servlet.TimerService;
import com.actility.m2m.servlet.song.SongFactory;
import com.actility.m2m.servlet.song.SongServlet;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.system.config.Configuration;
import com.actility.m2m.system.log.BundleLogger;
import com.actility.m2m.system.monitoring.Indicator;
import com.actility.m2m.system.monitoring.MonitoringManager;
import com.actility.m2m.system.monitoring.MonitoringServiceLauncher;
import com.actility.m2m.system.sshtunnel.SshTunnel;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

/**
 * System application: allows to retrieve ONG information (running bundles, ...).
 *
 * @author mlouiset
 *
 */
public class SystemVersionServlet extends SongServlet implements MonitoringServiceLauncher{

    /**
     * for serialization, required for TimerListener implementation
     */
    private static final long serialVersionUID = 1845993818490508756L;

    private static final Logger LOG = OSGiLogger.getLogger(SystemVersionServlet.class, BundleLogger.LOG);

    private SongFactory factory;
    private TimerService timerService;
    private ApplicationSession appSession;
    private BundleContext context;
    private ServletTimer sshTunnelTimer;

    private ResourcesAccessorService resourcesAccessorService;
    private XoService xoService;

    private MonitoringManager monitoringManager;
    private ResourcesManager resManager;
    private Configuration configuration;
    /**
     * Attribute names
     */
    public static final String REQUEST_CONTEXT = "system-song-request-context";

    public static final String RETARGETING_OP_OPEN_SSH_TUNNEL = "/retargeting1/openSshTunnel";
    public static final String RETARGETING_OP_CLOSE_SSH_TUNNEL = "/retargeting1/closeSshTunnel";
    public static final String RETARGETING_OP_DISK = "/retargeting1/disk";
    public static final String RETARGETING_OP_UPTIME = "/retargeting1/uptime";
    public static final String RETARGETING_OP_MEMORY = "/retargeting1/memory";
    public static final String RETARGETING_LIST_VERSIONS = "/retargeting1/versions";
    public static final String RETARGETING_OP_SERVICES_STATUS = "/retargeting1/services-status";
    public static final String RETARGETING_OP_DISK_IO = "/retargeting1/diskIO";
    public static final String RETARGETING_OP_NET_IO = "/retargeting1/netIO";
    public static final String RETARGETING_OP_POWER = "/retargeting1/power";
    public static final String RETARGETING_OP_TEMPERATURE = "/retargeting1/temperature";
    public static final String RETARGETING_OP_RELOAD_PROFILE = "/retargeting1/reloadProfile";
    
    private static final String DEFAULT_SSH_TARGET_HOSTNAME = "support1.actility.com";
    private static final int DEFAULT_SSH_TARGET_PORT = 81;
    private static final String DEFAULT_SSH_TARGET_LOGIN = "support";
    private static final String DEFAULT_SSH_TARGET_PASSWORD = "support";
    private static final String DEFAULT_SSH_CONNECT_OPTIONS = "-y -K 60";

    /**
     * Constructor
     *
     * @param obixService the oBIX service for resource handling.
     * @param context
     * @param resourcesAccessorService
     * @param xoService
     * @param config
     */
    public SystemVersionServlet(BundleContext context, ResourcesAccessorService resourcesAccessorService, XoService xoService, Configuration configuration) {
        this.context = context;
        this.resourcesAccessorService = resourcesAccessorService;
        this.xoService = xoService;
        this.configuration = configuration;
        
        String shell = resourcesAccessorService.getProperty(context, "com.actility.m2m.framework.config.os.shell");
        if ((shell == null) || "".equals(shell.trim())) {
            shell = "sh";
        }else{
            int space = shell.indexOf(' ');
            if(space!=-1){
                shell = shell.substring(0,space);
            }
        }
        String rootact = resourcesAccessorService.getProperty(context, "com.actility.m2m.framework.config.rootact");
        if ((rootact == null) || "".equals(rootact.trim())) {
            rootact = File.separatorChar+"home"+File.separatorChar+"ong"+File.separatorChar;
        }else{
            int lastChar = rootact.charAt(rootact.length()-1);
            if(lastChar!=File.separatorChar){
                rootact = rootact+File.separatorChar;
            }
        }
        this.monitoringManager = new MonitoringManager(xoService, configuration, shell, rootact);
        setSshTunnelProperties();

    }

    /**
     * Start the servlet
     */
    public void init() throws ServletException {
        LOG.info("init");

        timerService = (TimerService) getServletContext().getAttribute(TIMER_SERVICE);
        factory = (SongFactory) getServletContext().getAttribute(SONG_FACTORY);

        if (null == timerService) {
            throw new ServletException("timer service is not available");
        }
        if (null == factory) {
            throw new ServletException("SONG factory is not available");
        }

        // Create an application session to allow implementing global timers (here needed for SSH connection duration
        // management)
        try {
            appSession = factory.createApplicationSession();
            appSession.setExpires(0); // will never expire
            appSession.setInvalidateWhenReady(false);
        } catch (Exception e) {
            throw new ServletException("Failed to create System application session");
        }
        resManager = new ResourcesManager(appSession, factory, xoService, timerService, configuration, this);
        monitoringManager.addListener(resManager);
        resManager.generate();
    }

    /**
     * Stop the servlet
     */
    public void uninit() {
        LOG.info("uninit");
        SshTunnel.getInstance().closeSshTunnel();

    }
    /**
     * Launches the monitoring service
     */
    public void launchMonitoringService(){
        timerService.createTimer(appSession, 0, 60000, true, false, this.monitoringManager);
    }

    /**
     * This method is called when retrieve request is received
     */
    public void doRetrieve(SongServletRequest request) throws ServletException, IOException {

        String requestPath = request.getPathInfo();
        if (LOG.isInfoEnabled()) {
            LOG.info("doRetrieve (getPathInfo:" + requestPath + ")");
        }
        if (requestPath.equals(RETARGETING_LIST_VERSIONS)) {
            sendConfigVersion(request);
        } else {
            SongServletResponse response = null;
            response = request.createResponse(SongServletResponse.SC_NOT_FOUND);
            byte[] content = Utils.generateErrorContent(xoService, StatusCode.STATUS_NOT_FOUND, "Resource on URI " + request.getTargetID().absoluteURI() + " does not exist");
            if(content!=null){
                response.setContent(content, "application/xml; charset=utf-8");
            }
            response.send();
        }

    }
    /**
     * This method is called when create request is received
     */
    public void doCreate(SongServletRequest request) throws ServletException, IOException {
        String requestPath = request.getPathInfo();
        if (LOG.isInfoEnabled()) {
            LOG.info("doCreate (getPathInfo:" + requestPath + ")");
        }
        List indicators = new ArrayList();
        if (requestPath.equals(RETARGETING_OP_OPEN_SSH_TUNNEL)) {
            long duration = SshTunnel.getInstance().invokeOpenSshTunnel(request);
            resetSshTunnelTimer(duration);
        } else if (requestPath.equals(RETARGETING_OP_CLOSE_SSH_TUNNEL)) {
            resetSshTunnelTimer(0);
            SshTunnel.getInstance().invokeCloseSshTunnel(request);
        } else if (requestPath.equals(RETARGETING_OP_DISK)) {
            indicators.add(Indicator.Name.DISK_PART_USED);
            indicators.add(Indicator.Name.DISK_PART_FREE);
            retargetingResponse(request, indicators);
        } else if (requestPath.equals(RETARGETING_OP_MEMORY)) {
            indicators.add(Indicator.Name.MEM_USED);
            indicators.add(Indicator.Name.MEM_FREE);
            indicators.add(Indicator.Name.MEM_BUFFERS);
            indicators.add(Indicator.Name.MEM_CACHED);
            indicators.add(Indicator.Name.MEM_SWP_USED);
            indicators.add(Indicator.Name.MEM_SWP_FREE);
            retargetingResponse(request, indicators);
        } else if (requestPath.equals(RETARGETING_OP_UPTIME)) {
            indicators.add(Indicator.Name.UPTIME);
            indicators.add(Indicator.Name.LOAD_AVG_PT1M);
            indicators.add(Indicator.Name.LOAD_AVG_PT5M);
            indicators.add(Indicator.Name.LOAD_AVG_PT15M);
            indicators.add(Indicator.Name.PROC_TOTAL);
            retargetingResponse(request, indicators);
        } else if (requestPath.equals(RETARGETING_OP_SERVICES_STATUS)) {
            indicators.add(Indicator.Name.SRV_STATUS);
            indicators.add(Indicator.Name.SRV_PID);
            indicators.add(Indicator.Name.SRV_START_DATE);
            indicators.add(Indicator.Name.SRV_NB_RESTARTS);
            retargetingResponse(request, indicators);
        } else if (requestPath.equals(RETARGETING_OP_DISK_IO)) {
            indicators.add(Indicator.Name.DISK_READ_IO);
            indicators.add(Indicator.Name.DISK_WRITE_IO);
            retargetingResponse(request, indicators);
        } else if (requestPath.equals(RETARGETING_OP_NET_IO)) {
            indicators.add(Indicator.Name.NET_RECV_IO);
            indicators.add(Indicator.Name.NET_SENT_IO);
            retargetingResponse(request, indicators);
        } else if (requestPath.equals(RETARGETING_OP_POWER)) {
            indicators.add(Indicator.Name.MAIN_PWR_ON);
            indicators.add(Indicator.Name.BAT_PWR_LEVEL);
            indicators.add(Indicator.Name.BAT_REMAINING_TIME);
            retargetingResponse(request, indicators);
        } else if (requestPath.equals(RETARGETING_OP_TEMPERATURE)) {
            indicators.add(Indicator.Name.TEMP_CORE);
            indicators.add(Indicator.Name.TEMP_FRAME);
            retargetingResponse(request, indicators);
        } else if (requestPath.equals(RETARGETING_OP_RELOAD_PROFILE)) {
//            reloadProfile(request);
            resManager.reloadProfile(request);
        } else {
            SongServletResponse response = null;
            response = request.createResponse(SongServletResponse.SC_NOT_FOUND);
            byte[] content = Utils.generateErrorContent(xoService, StatusCode.STATUS_NOT_FOUND, "Resource on URI " + request.getTargetID().absoluteURI() + " does not exist");
            if(content!=null){
                response.setContent(content, "application/xml; charset=utf-8");
            }
            response.send();
        }
    }
    /**
     * This method is called when success response is received
     */
    public void doSuccessResponse(SongServletResponse response) throws IOException {
        if (LOG.isDebugEnabled()) {
            LOG.debug("doSuccessResponse");
            LOG.debug("Requesting Entity: " + response.getRequestingEntity().absoluteURI());
            LOG.debug("Target ID: " + response.getTargetID().absoluteURI());
            byte[] content = response.getRawContent();
            LOG.debug("Charset: " + response.getCharacterEncoding());
            LOG.debug("Content-type: " + response.getContentType());
            String value = new String(content, response.getCharacterEncoding());
            LOG.debug("Content: " + value);
        }
        ResourcesManager callback = (ResourcesManager) response.getRequest().getAttribute(REQUEST_CONTEXT);
        if(callback!=null){
            if(!resManager.isInitialized()){
                callback.onResponse(response.getStatus());
            }else{
                    callback.onReloadProfileResponse(response);
            }
        }
    }
    /**
     * This method is called when error response is received
     */
    public void doErrorResponse(SongServletResponse response) throws IOException {
        if (LOG.isDebugEnabled()) {
            LOG.debug("doErrorResponse (StatusCode:" + response.getStatus() + ")");
            LOG.debug("Requesting Entity: " + response.getRequestingEntity().absoluteURI());
            LOG.debug("Target ID: " + response.getTargetID().absoluteURI());
            byte[] content = response.getRawContent();
            LOG.debug("Charset: " + response.getCharacterEncoding());
            LOG.debug("Content-type: " + response.getContentType());
            String value = new String(content, response.getCharacterEncoding());
            LOG.debug("Content: " + value);
        }
        if(!resManager.isInitialized()){
            if ((response.getTargetID().getPath().equals("/m2m/applications/")
                    && response.getStatus() == SongServletResponse.SC_NOT_FOUND) || response.getStatus() == SongServletResponse.SC_SERVICE_UNAVAILABLE ) {
                int timeBetweenInit = ((Integer)configuration.getValue(Configuration.Name.TIME_BETWEEN_INIT/*, Integer.class*/)).intValue();
                LOG.info("scl.app.me has not created m2m resources... Retrying in " + timeBetweenInit / 1000 + " secondes");
                timerService.createTimer(appSession, timeBetweenInit, false, resManager);
            }else{
                ResourcesManager callback = (ResourcesManager) response.getRequest().getAttribute(REQUEST_CONTEXT);
                callback.onResponse(response.getStatus());
            }
        }else{
            ResourcesManager callback = (ResourcesManager) response.getRequest().getAttribute(REQUEST_CONTEXT);
            if(callback!=null){
                callback.onReloadProfileResponse(response);
            }
        }
    }

    
    /**
     * Create a response to a retargeting request from a list of indicators
     * @param request
     * @throws IOException
     */
    private void retargetingResponse(SongServletRequest request, List/*<Indicator>*/ indicators) throws IOException{
        LOG.debug("Requesting uptime informations");
        SongServletResponse response = null;
        try {
            byte[] content = monitoringManager.snapshot(indicators);
            if(content!=null){
                response = request.createResponse(SongServletResponse.SC_OK);
                response.setContent(content, "application/xml; charset=utf-8");
            }else{
                response = request.createResponse(SongServletResponse.SC_NOT_IMPLEMENTED);
            }
        } catch (XoException e) {
            LOG.error("Unable to create JXo obix object", e);
            response = request.createResponse(SongServletResponse.SC_INTERNAL_SERVER_ERROR);
            byte[] content = Utils.generateErrorContent(xoService, StatusCode.STATUS_INTERNAL_SERVER_ERROR, "Error occured during Obix creation");
            if(content!=null){
                response.setContent(content, "application/xml; charset=utf-8");
            }
        }
        response.send();
    }
    
//    private void reloadProfile(SongServletRequest request) throws IOException{
//        resManager.reloadProfile(request);
//        LOG.debug("Requesting reload profile");
//        SongServletResponse retargetingResponse = null;
//        SongServletRequest songRequest = factory.createRequest(appSession, SongServletRequest.MD_CREATE, reqEntity, toUri);
//        resManager.
//        retargetingResponse = request.createResponse(SongServletResponse.SC_NOT_IMPLEMENTED);
//        retargetingResponse.send();
//    }
    /**
     * Process the request when the url points on /retargeting1/versions
     * @param request
     * @throws IOException
     */
    private void sendConfigVersion(SongServletRequest request) throws IOException {
        LOG.debug("Requesting version informations");
        
        SongServletResponse response = null;
        XoObject xoObject = null;
        try {
            xoObject = this.xoService.newObixXoObject("o:obj");
            List xoChilds = xoObject.getXoObjectListAttribute("[]");
            response = request.createResponse(SongServletResponse.SC_OK);

            Bundle[] runningBundles = context.getBundles();

            // GET ONG VERSION
            XoObject xoVersion = this.xoService.newObixXoObject("o:str");
            xoVersion.setStringAttribute("name", "ong.firmware.base");
            String versionONG = response.getHeader("Server");
            versionONG = (versionONG != null && versionONG.length() > 4 ? versionONG.substring(4) : "unknown");
            xoVersion.setStringAttribute("val", versionONG);
            xoVersion.setStringAttribute("status", "ACTIVE");
            xoChilds.add(xoVersion);

            XoObject xoList = this.xoService.newObixXoObject("o:list");
            xoList.setStringAttribute("of", "bundle");
            List bundleList = xoList.getXoObjectListAttribute("[]");
            for (int i = 0; i < runningBundles.length; i++) {
                XoObject xoBundle = this.xoService.newObixXoObject("o:str");
                String bundleState;
                switch (runningBundles[i].getState()) {
                case Bundle.UNINSTALLED:
                    bundleState = "UNINSTALLED";
                    break;
                case Bundle.INSTALLED:
                    bundleState = "INSTALLED";
                    break;
                case Bundle.RESOLVED:
                    bundleState = "RESOLVED";
                    break;
                case Bundle.STARTING:
                    bundleState = "STARTING";
                    break;
                case Bundle.STOPPING:
                    bundleState = "STOPPING";
                    break;
                case Bundle.ACTIVE:
                    bundleState = "ACTIVE";
                    break;
                default:
                    bundleState = "UNKNOWN";
                    break;
                }
                String version = (String) runningBundles[i].getHeaders().get("Bundle-Version");
                xoBundle.setStringAttribute("status", bundleState);
                xoBundle.setStringAttribute("val", version == null ? "unknown" : version);

                String symbolicName = runningBundles[i].getSymbolicName();
                int firstSemi = symbolicName.indexOf(";");
                String name = null;
                if (firstSemi != -1) {
                    name = symbolicName.substring(0, firstSemi);
                } else {
                    name = symbolicName;
                }
                xoBundle.setStringAttribute("name", name);
                bundleList.add(xoBundle);
            }
            xoChilds.add(xoList);
            byte[] content = xoObject.saveXml();
            response.setContent(content, "application/xml; charset=utf-8");
        } catch (XoException e) {
            LOG.error("Unable to create JXo obix object", e);
            response = request.createResponse(SongServletResponse.SC_INTERNAL_SERVER_ERROR);
            byte[] content = Utils.generateErrorContent(xoService, StatusCode.STATUS_INTERNAL_SERVER_ERROR, "Error occured during Obix creation");
            if(content!=null){
                response.setContent(content, "application/xml; charset=utf-8");
            }
        } finally{
            Utils.releaseXoObjectFromMemory(xoObject);
        }
        response.send();
    }

    /**
     * Retrieve the system properties required for SSH tunnel resource
     */
    private void setSshTunnelProperties() {

        // Set properties of SSH tunnel resource

        // The shell
        String shell = resourcesAccessorService.getProperty(context, "com.actility.m2m.framework.config.os.shell");
        if ((shell == null) || "".equals(shell.trim())) {
            shell = "sh -c";
        }
        SshTunnel.getInstance().setShellCmd(shell);

        // The target host name
        String sshTargetHostname = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                + ".config.supportServerName");
        if ((sshTargetHostname == null) || "".equals(sshTargetHostname.trim())) {
            sshTargetHostname = DEFAULT_SSH_TARGET_HOSTNAME;
        }
        SshTunnel.getInstance().setSshTargetHostname(sshTargetHostname);

        // The target port
        String sshTargetPortStr = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                + ".config.supportServerPort");
        int sshTargetPort = DEFAULT_SSH_TARGET_PORT;
        if ((sshTargetPortStr != null) && (!"".equals(sshTargetPortStr.trim()))) {
            sshTargetPort = Integer.parseInt(sshTargetPortStr);
        }
        SshTunnel.getInstance().setSshTargetPort(sshTargetPort);

        // The target login
        String sshTargetLogin = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                + ".config.supportSshLogin");
        if ((sshTargetLogin == null) || "".equals(sshTargetLogin.trim())) {
            sshTargetLogin = DEFAULT_SSH_TARGET_LOGIN;
        }
        SshTunnel.getInstance().setSshTargetLogin(sshTargetLogin);

        // The target password
        String sshTargetPassword = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                + ".config.supportSshPassword");
        if ((sshTargetPassword == null) || "".equals(sshTargetPassword.trim())) {
            sshTargetPassword = DEFAULT_SSH_TARGET_PASSWORD;
        }
        SshTunnel.getInstance().setSshTargetPassword(sshTargetPassword);

        // The SSH connect options
        String sshOptions = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                + ".config.supportSshOptions");
        if ((sshOptions == null) || "".equals(sshOptions.trim())) {
            sshOptions = DEFAULT_SSH_CONNECT_OPTIONS;
        }
        SshTunnel.getInstance().setSshConnectOptions(sshOptions);

    }

    /**
     * Reset the SSH tunnel connection timer
     *
     * @param duration the maximum connection duration allowed for the new SSH connection. If duration is less than 0, timer is
     *            not restarted.
     */
    private void resetSshTunnelTimer(long duration) {
        if (sshTunnelTimer != null) {
            sshTunnelTimer.cancel();
            sshTunnelTimer = null;
        }
        if (duration > 0) {
            sshTunnelTimer = timerService.createTimer(appSession, duration, false, SshTunnel.getInstance());
        }
    }

}
