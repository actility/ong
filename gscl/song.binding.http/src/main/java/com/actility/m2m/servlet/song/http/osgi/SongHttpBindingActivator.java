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
 * id $Id: SongHttpBindingActivator.java 9285 2014-08-19 12:28:15Z JReich $
 * author $Author: JReich $
 * version $Revision: 9285 $
 * lastrevision $Date: 2014-08-19 14:28:15 +0200 (Tue, 19 Aug 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-08-19 14:28:15 +0200 (Tue, 19 Aug 2014) $
 */

package com.actility.m2m.servlet.song.http.osgi;

import java.io.IOException;
import java.io.InputStream;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.text.ParseException;
import java.util.Dictionary;
import java.util.Hashtable;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.Constants;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.ServiceRegistration;
import org.osgi.service.cm.ConfigurationException;
import org.osgi.service.cm.ManagedService;
import org.osgi.util.tracker.ServiceTracker;
import org.osgi.util.tracker.ServiceTrackerCustomizer;

import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.http.client.ni.HttpClient;
import com.actility.m2m.http.client.ni.HttpClientNiService;
import com.actility.m2m.http.server.HttpServerManager;
import com.actility.m2m.m2m.M2MService;
import com.actility.m2m.servlet.NamespaceException;
import com.actility.m2m.servlet.song.binding.SongBindingFacade;
import com.actility.m2m.servlet.song.binding.service.SongBindingService;
import com.actility.m2m.servlet.song.http.HttpBindingFacade;
import com.actility.m2m.servlet.song.http.SongHttpBinding;
import com.actility.m2m.servlet.song.http.log.BundleLogger;
import com.actility.m2m.song.binding.http.SongHttpBindingStatsService;
import com.actility.m2m.util.ByteArrayOutputStream;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.log.OSGiLogger;
import com.actility.m2m.xo.XoService;

/**
 * OSGi activator for the SONG HTTP Binding
 */
public final class SongHttpBindingActivator implements BundleActivator, ManagedService {
    private static final Logger LOG = OSGiLogger.getLogger(SongHttpBindingActivator.class, BundleLogger.LOG);

    /**
     * Maximum authorized length for an HTTP message
     */
    private static final Integer DEFAULT_MAX_CONTENT_LENGTH = new Integer(8192);
    private static final Long DEFAULT_REQUEST_EXPIRATION_TIMER = new Long(32000L);
    private static final Long DEFAULT_LONG_POLLING_SERVER_REQUEST_EXPIRATION_TIMER = new Long(300000L); // 5 mins
    private static final Long DEFAULT_LONG_POLLING_CLIENT_REQUEST_EXPIRATION_TIMER = new Long(420000L); // 7 mins
    private static final Long DEFAULT_CONTACT_REQUEST_WAITING_TIMER = new Long(5000L);
    private static final Integer DEFAULT_PROXY_PORT = new Integer(3128);
    private static final Integer DEFAULT_SERVER_PORT = new Integer(8080);
    private static final Integer DEFAULT_SERVER_MAX_SOCKETS = new Integer(20);
    private static final Long DEFAULT_SERVER_CONNECTION_EXPIRATION_TIMER = new Long(10000L);
    private static final Integer DEFAULT_SERVER_MAX_NB_OF_HEADERS = new Integer(32);
    private static final Integer DEFAULT_SERVER_MAX_HEADER_LINE_LENGTH = new Integer(2048);
    private static final Integer DEFAULT_SERVER_MAX_HEADERS_LENGTH = new Integer(8192);
    private static final Integer DEFAULT_CLIENT_MAX_SOCKETS_PER_HOST = new Integer(10);
    private static final Integer DEFAULT_CLIENT_MAX_SOCKETS = new Integer(50);

    private static final String[] PROTOCOLS = new String[] { "http", "https" };
    private static final String DEFAULT_PROTOCOL = "HTTP/1.1";

    private BundleContext context;

    private ServiceRegistration songHttpBindingRegistration;

    private ServiceTracker songBindingServiceTracker;
    private ServiceTracker xoServiceTracker;
    private ServiceTracker m2mServiceTracker;
    private ServiceTracker resourcesAccessorServiceTracker;
    private ServiceTracker httpClientNativeNiServiceTracker;
    private ServiceRegistration managedServiceRegistration;

    private Dictionary config;

    private XoService xoService;
    private M2MService m2mService;
    private SongBindingService songBindingService;
    private ResourcesAccessorService resourcesAccessorService;
    private HttpClientNiService httpClientNiService;

    private SongHttpBinding songHttpBinding;
    private String bindingPath;
    private ServletContext applicationContext;
    private HttpServerManager httpServer;
    private Thread httpServerThread;

    private Object checkWithDefault(Dictionary config, String name, Class type, Object defaultValue) {
        Object result = defaultValue;
        Object value = config.get(name);
        if (value == null) {
            if (LOG.isEnabledFor(Level.WARN)) {
                LOG.warn("Configuration property " + name + " is null -> use default value instead " + defaultValue);
            }
        } else if (!type.equals(value.getClass())) {
            LOG.error("Configuration property " + name + " must be of type " + type.toString() + " while it is of type "
                    + value.getClass() + " -> use default value instead " + defaultValue);
        } else if (type != String.class || ((String) value).length() != 0) {
            result = value;
        }
        return result;
    }

    private int getMaxContentLength(Dictionary config) {
        return ((Integer) checkWithDefault(config, "maxContentLength", Integer.class, DEFAULT_MAX_CONTENT_LENGTH)).intValue();
    }

    private long getRequestExpirationTimer(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_REQUEST_EXPIRATION_TIMER.longValue());
        String duration = (String) checkWithDefault(config, "requestExpirationTimer", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error("Configuration property requestExpirationTimer cannot be decoded as a duration, use default value "
                    + defaultStr, e);
            return DEFAULT_REQUEST_EXPIRATION_TIMER.longValue();
        }
    }

    private long getLongPollingClientRequestExpirationTimer(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_LONG_POLLING_CLIENT_REQUEST_EXPIRATION_TIMER.longValue());
        String duration = (String) checkWithDefault(config, "longPollingClientRequestExpirationTimer", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error(
                    "Configuration property longPollingClientRequestExpirationTimer cannot be decoded as a duration, use default value "
                            + defaultStr, e);
            return DEFAULT_LONG_POLLING_CLIENT_REQUEST_EXPIRATION_TIMER.longValue();
        }
    }

    private long getLongPollingServerRequestExpirationTimer(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_LONG_POLLING_SERVER_REQUEST_EXPIRATION_TIMER.longValue());
        String duration = (String) checkWithDefault(config, "longPollingServerRequestExpirationTimer", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error(
                    "Configuration property longPollingServerRequestExpirationTimer cannot be decoded as a duration, use default value "
                            + defaultStr, e);
            return DEFAULT_LONG_POLLING_SERVER_REQUEST_EXPIRATION_TIMER.longValue();
        }
    }

    private long getContactRequestWaitingTimer(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_CONTACT_REQUEST_WAITING_TIMER.longValue());
        String duration = (String) checkWithDefault(config, "contactRequestWaitingTimer", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error("Configuration property contactRequestWaitingTimer cannot be decoded as a duration, use default value "
                    + defaultStr, e);
            return DEFAULT_CONTACT_REQUEST_WAITING_TIMER.longValue();
        }
    }

    private String getProxyHost(BundleContext context, Dictionary config) {
        String defaultProxyHost = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                + ".config.proxyHost");
        if (defaultProxyHost != null) {
            defaultProxyHost = defaultProxyHost.trim();
            if (defaultProxyHost.length() == 0) {
                defaultProxyHost = null;
            }
        }
        return (String) checkWithDefault(config, "proxyHost", String.class, defaultProxyHost);
    }

    private int getProxyPort(BundleContext context, Dictionary config) {
        Integer defaultPortValue = DEFAULT_PROXY_PORT;
        String defaultProxyPort = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                + ".config.proxyPort");
        if (defaultProxyPort != null) {
            defaultProxyPort = defaultProxyPort.trim();
            if (defaultProxyPort.length() != 0) {
                try {
                    defaultPortValue = new Integer(Integer.parseInt(defaultProxyPort));
                } catch (NumberFormatException e) {
                    LOG.error("Framework property " + context.getBundle().getSymbolicName()
                            + ".config.proxyPort is not a valid number: " + defaultProxyPort + " use default value "
                            + defaultPortValue + " instead");
                }
            }
        }
        return ((Integer) checkWithDefault(config, "proxyPort", Integer.class, defaultPortValue)).intValue();
    }

    private String getProxyUsername(BundleContext context, Dictionary config) {
        String defaultProxyUsername = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                + ".config.proxyUsername");
        if (defaultProxyUsername != null) {
            defaultProxyUsername = defaultProxyUsername.trim();
            if (defaultProxyUsername.length() == 0) {
                defaultProxyUsername = null;
            }
        }
        return (String) checkWithDefault(config, "proxyUsername", String.class, defaultProxyUsername);
    }

    private String getProxyPassword(BundleContext context, Dictionary config) {
        String defaultProxyPassword = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                + ".config.proxyPassword");
        if (defaultProxyPassword != null) {
            defaultProxyPassword = defaultProxyPassword.trim();
            if (defaultProxyPassword.length() == 0) {
                defaultProxyPassword = null;
            }
        }
        return (String) checkWithDefault(config, "proxyPassword", String.class, defaultProxyPassword);
    }

    private int getServerPort(BundleContext context, Dictionary config) {
        Integer defaultPortValue = DEFAULT_SERVER_PORT;
        String defaultServerPort = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                + ".config.serverPort");
        if (defaultServerPort != null) {
            defaultServerPort = defaultServerPort.trim();
            if (defaultServerPort.length() != 0) {
                try {
                    defaultPortValue = new Integer(Integer.parseInt(defaultServerPort));
                } catch (NumberFormatException e) {
                    LOG.error("Framework property " + context.getBundle().getSymbolicName()
                            + ".config.serverPort is not a valid number: " + defaultServerPort + " use default value "
                            + defaultPortValue + " instead");
                }
            }
        }
        return ((Integer) checkWithDefault(config, "serverPort", Integer.class, defaultPortValue)).intValue();
    }

    private Integer getVirtualServerPort(BundleContext context, Dictionary config) {
        Integer virtualPortValue = null;
        String defaultVirtualServerPort = resourcesAccessorService.getProperty(context, context.getBundle().getSymbolicName()
                + ".config.virtualServerPort");
        if (defaultVirtualServerPort != null) {
            defaultVirtualServerPort = defaultVirtualServerPort.trim();
            if (defaultVirtualServerPort.length() != 0) {
                try {
                    virtualPortValue = new Integer(Integer.parseInt(defaultVirtualServerPort));
                } catch (NumberFormatException e) {
                    LOG.error("Framework property " + context.getBundle().getSymbolicName()
                            + ".config.virtualServerPort is not a valid number: " + defaultVirtualServerPort);
                }
            }
        }
        return (Integer) checkWithDefault(config, "virtualServerPort", Integer.class, virtualPortValue);
    }

    private int getServerMaxSockets(Dictionary config) {
        return ((Integer) checkWithDefault(config, "serverMaxSockets", Integer.class, DEFAULT_SERVER_MAX_SOCKETS)).intValue();
    }

    private int getServerConnectionExpirationTimer(Dictionary config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_SERVER_CONNECTION_EXPIRATION_TIMER.longValue());
        String duration = (String) checkWithDefault(config, "serverConnectionExpirationTimer", String.class, defaultStr);
        try {
            return (int) FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error(
                    "Configuration property serverConnectionExpirationTimer cannot be decoded as a duration, use default value "
                            + defaultStr, e);
            return (int) DEFAULT_SERVER_CONNECTION_EXPIRATION_TIMER.longValue();
        }
    }

    private int getServerMaxNbOfHeaders(Dictionary config) {
        return ((Integer) checkWithDefault(config, "serverMaxNbOfHeaders", Integer.class, DEFAULT_SERVER_MAX_NB_OF_HEADERS))
                .intValue();
    }

    private int getServerMaxHeaderLineLength(Dictionary config) {
        return ((Integer) checkWithDefault(config, "serverMaxHeaderLineLength", Integer.class,
                DEFAULT_SERVER_MAX_HEADER_LINE_LENGTH)).intValue();
    }

    private int getServerMaxHeadersLength(Dictionary config) {
        return ((Integer) checkWithDefault(config, "serverMaxHeadersLength", Integer.class, DEFAULT_SERVER_MAX_HEADERS_LENGTH))
                .intValue();
    }

    private int getClientMaxSocketsPerHost(Dictionary config) {
        return ((Integer) checkWithDefault(config, "clientMaxSocketsPerHost", Integer.class,
                DEFAULT_CLIENT_MAX_SOCKETS_PER_HOST)).intValue();
    }

    private int getClientMaxSockets(Dictionary config) {
        return ((Integer) checkWithDefault(config, "clientMaxSockets", Integer.class, DEFAULT_CLIENT_MAX_SOCKETS)).intValue();
    }

    public void start(BundleContext context) throws Exception {
        this.context = context;
        BundleLogger.LOG.init(context);

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Starting tracker for " + XoService.class.getName() + " service...");
        }

        // init and start ServiceTrackers to tracks elements
        // for XoService
        xoServiceTracker = new ServiceTracker(context, XoService.class.getName(), new XoServiceCustomizer());
        xoServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + SongBindingService.class.getName() + " service...");
        }

        // for SongBindingService
        songBindingServiceTracker = new ServiceTracker(context, SongBindingService.class.getName(),
                new SongBindingServiceCustomizer());
        songBindingServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + M2MService.class.getName() + " service...");
        }

        // for M2MService
        m2mServiceTracker = new ServiceTracker(context, M2MService.class.getName(), new M2MServiceCustomizer());
        m2mServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }

        // for ResourcesAccessorService
        resourcesAccessorServiceTracker = new ServiceTracker(context, ResourcesAccessorService.class.getName(),
                new ResourcesAccessorServiceCustomizer());
        resourcesAccessorServiceTracker.open();

        if (LOG.isInfoEnabled()) {
            LOG.info("Starting tracker for " + HttpClientNiService.class.getName() + " service...");
        }

        // for HttpClientNiService
        httpClientNativeNiServiceTracker = new ServiceTracker(context, HttpClientNiService.class.getName(),
                new HttpClientNiServiceCustomizer());
        httpClientNativeNiServiceTracker.open();

        String pid = context.getBundle().getSymbolicName() + ".config";
        if (LOG.isInfoEnabled()) {
            LOG.info("Registering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        config = null;
        Dictionary props = new Hashtable();
        props.put(Constants.SERVICE_PID, pid);
        managedServiceRegistration = context.registerService(ManagedService.class.getName(), this, props);
    }

    public void stop(BundleContext context) throws Exception {
        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping bundle " + context.getBundle().getSymbolicName() + "...");
            LOG.info("Stopping tracker for " + XoService.class.getName() + " service...");
        }
        xoServiceTracker.close();
        xoServiceTracker = null;

        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + SongBindingService.class.getName() + " service...");
        }
        songBindingServiceTracker.close();
        songBindingServiceTracker = null;

        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + M2MService.class.getName() + " service...");
        }
        m2mServiceTracker.close();
        m2mServiceTracker = null;

        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + ResourcesAccessorService.class.getName() + " service...");
        }
        resourcesAccessorServiceTracker.close();
        resourcesAccessorServiceTracker = null;

        if (LOG.isInfoEnabled()) {
            LOG.info("Stopping tracker for " + HttpClientNiService.class.getName() + " service...");
        }
        httpClientNativeNiServiceTracker.close();
        httpClientNativeNiServiceTracker = null;

        if (LOG.isInfoEnabled()) {
            String pid = context.getBundle().getSymbolicName() + ".config";
            LOG.info("Unregistering service " + ManagedService.class.getName() + " for pid " + pid + "...");
        }
        managedServiceRegistration.unregister();
        managedServiceRegistration = null;

        stopSongBindingHttp();
        config = null;
        this.context = null;
        BundleLogger.LOG.init(null);
    }

    private synchronized void startSongBindingHttp() {
        if ((config != null) && (xoService != null) && (m2mService != null) && (songBindingService != null)
                && (resourcesAccessorService != null) && (httpClientNiService != null)) {
            LOG.info("Starting song binding http...");
            InputStream is = null;
            boolean error = false;

            try {
                bindingPath = "/http";

                InetAddress localAddress = InetAddress.getLocalHost();
                HttpClient httpClient = httpClientNiService.createHttpClient();
                applicationContext = songBindingService.createApplication(bindingPath, "HTTP", null, null, null);

                is = resourcesAccessorService.getResourceAsStream(context.getBundle(), "images/favicon.ico");
                // Create the byte array to hold the data
                ByteArrayOutputStream buffer = new ByteArrayOutputStream();
                try {
                    int nRead;
                    byte[] data = new byte[128];
                    while ((nRead = is.read(data, 0, data.length)) != -1) {
                        buffer.write(data, 0, nRead);
                    }
                    buffer.flush();
                } finally {
                    buffer.close();
                }

                int maxContentLength = getMaxContentLength(config);
                long requestExpirationTimer = getRequestExpirationTimer(config);
                long longPollingServerRequestExpirationTimer = getLongPollingServerRequestExpirationTimer(config);
                long longPollingClientRequestExpirationTimer = getLongPollingClientRequestExpirationTimer(config);
                long contactRequestWaitingTimer = getContactRequestWaitingTimer(config);
                String proxyHost = getProxyHost(context, config);
                int proxyPort = getProxyPort(context, config);
                String proxyUsername = getProxyUsername(context, config);
                String proxyPassword = getProxyPassword(context, config);
                int serverPort = getServerPort(context, config);
                Integer configVirtualServerPort = getVirtualServerPort(context, config);
                int virtualServerPort = serverPort;
                if (configVirtualServerPort != null) {
                    virtualServerPort = configVirtualServerPort.intValue();
                }
                int serverMaxSockets = getServerMaxSockets(config);
                int serverConnectionExpirationTimer = getServerConnectionExpirationTimer(config);
                int serverMaxNbOfHeaders = getServerMaxNbOfHeaders(config);
                int serverMaxHeaderLineLength = getServerMaxHeaderLineLength(config);
                int serverMaxHeadersLength = getServerMaxHeadersLength(config);
                int clientMaxSocketsPerHost = getClientMaxSocketsPerHost(config);
                int clientMaxSockets = getClientMaxSockets(config);

                songHttpBinding = new SongHttpBinding(xoService, m2mService, httpClient, localAddress, serverPort,
                        serverMaxSockets, maxContentLength, requestExpirationTimer, longPollingServerRequestExpirationTimer,
                        longPollingClientRequestExpirationTimer, contactRequestWaitingTimer, proxyHost, proxyPort,
                        proxyUsername, proxyPassword, clientMaxSocketsPerHost, clientMaxSockets, applicationContext,
                        buffer.toByteArray());
                httpClientNiService.init(songHttpBinding);
                SongBindingFacade bindingFacade = new HttpBindingFacade(songHttpBinding);

                songBindingService.registerBindingServlet(applicationContext, "SONGBinding", songHttpBinding, null,
                        bindingFacade, "http", (virtualServerPort == 80) ? -1 : virtualServerPort, PROTOCOLS, true,
                        DEFAULT_PROTOCOL, localAddress, virtualServerPort, null);
                songBindingService.registerServiceServlet(applicationContext, "/", "LPService", songHttpBinding, null, null);
                httpServer = new HttpServerManager(songHttpBinding, false, serverPort, serverMaxSockets,
                        serverConnectionExpirationTimer, serverMaxNbOfHeaders, serverMaxHeaderLineLength,
                        serverMaxHeadersLength, maxContentLength);
                httpServerThread = new Thread(httpServer, "SONG binding HTTP Server:" + serverPort);
                httpServerThread.start();

                songHttpBindingRegistration = context.registerService(SongHttpBindingStatsService.class.getName(),
                        songHttpBinding, null);
            } catch (UnknownHostException e) {
                error = true;
                LOG.error("Can't get the localhost address", e);
                // throw e;
            } catch (ServletException e) {
                error = true;
                LOG.error("Can't initialize servlet", e);
                // throw e;
            } catch (NamespaceException e) {
                error = true;
                LOG.error("Problem with the name of the SONG Binding Servlet", e);
                // throw e;
            } catch (IOException e) {
                error = true;
                LOG.error("Can't start HTTP server", e);
                // throw e;
            } catch (Throwable e) {
                error = true;
                LOG.error("Can't start HTTP server", e);
            } finally {
                if (error && applicationContext != null) {
                    songBindingService.unregisterApplication(applicationContext);
                }
                if (is != null) {
                    try {
                        is.close();
                    } catch (IOException e) {
                        LOG.error("Unable to close ", e);
                    }
                }
            }
        }
    }

    private synchronized void stopSongBindingHttp() {
        if (songHttpBinding != null) {
            LOG.info("Stopping song binding http...");
            try {
                LOG.info("Destroy http server");
                httpServer.destroy();
            } catch (Exception e) {
                LOG.error("Problem while unregistering HTTP SONG Binding", e);
            }
            try {
                LOG.info("Join http server thread");
                httpServerThread.join();
            } catch (InterruptedException e) {
                LOG.error("Cannot join HTTP server thread", e);
            }
            LOG.info("Unregister song binding http service");
            songHttpBindingRegistration.unregister();
            try {
                LOG.info("Unregister song application");
                songBindingService.unregisterApplication(applicationContext);
            } catch (Exception e) {
                LOG.error("Problem while unregistering HTTP SONG Binding", e);
            }
            try {
                LOG.info("Destroy song binding http");
                songHttpBinding.destroy();
                // TODO ? LOG.info("Shutdown http client");
            } catch (Exception e) {
                LOG.error("Problem while deleting the song http binding", e);
            }
            songHttpBinding = null;
            songHttpBindingRegistration = null;
        }
    }

    public synchronized void updated(Dictionary properties) throws ConfigurationException {
        LOG.info("Configuration updated...");

        config = properties;
        if (config == null) {
            config = new Hashtable();
        }
        if (songHttpBinding != null) {
            songHttpBinding.setMaxContentLength(getMaxContentLength(config));
            songHttpBinding.setRequestExpirationTimer(getRequestExpirationTimer(config));
            songHttpBinding.setLongPollingServerRequestExpirationTimer(getLongPollingServerRequestExpirationTimer(config));
            songHttpBinding.setLongPollingClientRequestExpirationTimer(getLongPollingClientRequestExpirationTimer(config));
            songHttpBinding.setContactRequestWaitingTimer(getContactRequestWaitingTimer(config));
            songHttpBinding.setProxyConfiguration(getProxyHost(context, config), getProxyPort(context, config),
                    getProxyUsername(context, config), getProxyPassword(context, config));
            httpServer.setConnectionTimeout(getServerConnectionExpirationTimer(config));
            httpServer.setMaxNbOfHeaders(getServerMaxNbOfHeaders(config));
            httpServer.setMaxSockets(getServerMaxSockets(config));
        } else {
            startSongBindingHttp();
        }
    }

    // customizer that handles registration/modification/unregistration events for XoService
    private class XoServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + XoService.class.getName() + "...");
            }
            if (xoService != null) {
                return null;
            }
            xoService = (XoService) context.getService(reference);

            startSongBindingHttp();
            // Return the service to track it
            return xoService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + XoService.class.getName() + "...");
            }
            if (service == xoService) {
                stopSongBindingHttp();
                xoService = null;
            }
        }
    }

    // customizer that handles registration/modification/unregistration events for M2MService
    private class M2MServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + M2MService.class.getName() + "...");
            }
            if (m2mService != null) {
                return null;
            }
            m2mService = (M2MService) context.getService(reference);

            startSongBindingHttp();
            // Return the service to track it
            return m2mService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + M2MService.class.getName() + "...");
            }
            if (service == m2mService) {
                stopSongBindingHttp();
                m2mService = null;
            }
        }
    }

    // customizer that handles registration/modification/unregistration events for SongBindingService
    private class SongBindingServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + SongBindingService.class.getName() + "...");
            }
            if (songBindingService != null) {
                return null;
            }
            songBindingService = (SongBindingService) context.getService(reference);

            startSongBindingHttp();
            // Return the service to track it
            return songBindingService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + SongBindingService.class.getName() + "...");
            }
            if (service == songBindingService) {
                stopSongBindingHttp();
                songBindingService = null;
            }
        }
    }

    // customizer that handles registration/modification/unregistration events for ResourcesAccessorService
    private class ResourcesAccessorServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + ResourcesAccessorService.class.getName() + "...");
            }
            if (resourcesAccessorService != null) {
                return null;
            }
            resourcesAccessorService = (ResourcesAccessorService) context.getService(reference);

            startSongBindingHttp();
            // Return the service to track it
            return resourcesAccessorService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + ResourcesAccessorService.class.getName() + "...");
            }
            if (service == resourcesAccessorService) {
                stopSongBindingHttp();
                resourcesAccessorService = null;
            }
        }
    }

    // customizer that handles registration/modification/unregistration events for HttpClientNiService
    private class HttpClientNiServiceCustomizer implements ServiceTrackerCustomizer {

        public Object addingService(ServiceReference reference) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Adding service " + HttpClientNiService.class.getName() + "...");
            }
            if (httpClientNiService != null) {
                return null;
            }
            httpClientNiService = (HttpClientNiService) context.getService(reference);

            startSongBindingHttp();
            // Return the service to track it
            return httpClientNiService;
        }

        public void modifiedService(ServiceReference reference, Object service) {
        }

        public void removedService(ServiceReference reference, Object service) {
            if (LOG.isInfoEnabled()) {
                LOG.info("Removing service " + HttpClientNiService.class.getName() + "...");
            }
            if (service == httpClientNiService) {
                stopSongBindingHttp();
                httpClientNiService = null;
            }
        }

    }
}
