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
 * id $Id: TelnetConfig.java 8750 2014-05-21 15:18:02Z JReich $
 * author $Author: JReich $
 * version $Revision: 8750 $
 * lastrevision $Date: 2014-05-21 17:18:02 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:18:02 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.felix.shell.telnet;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Dictionary;
import java.util.Enumeration;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleContext;
import org.osgi.service.cm.ConfigurationException;

import com.actility.m2m.felix.shell.telnet.log.BundleLogger;
import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.util.log.OSGiLogger;

public class TelnetConfig {
    private static final Logger LOG = OSGiLogger.getLogger(TelnetConfig.class, BundleLogger.getStaticLogger());

    private static final String PORT_KEY = "port";

    private static final String HOST_KEY = "host";

    private int port = 2323;

    private String host = null;

    private String user = "admin";

    private String password = "admin";

    private boolean busyWait = false;

    public TelnetConfig(ResourcesAccessorService resourcesAccessorService, BundleContext context, Dictionary configuration)
            throws ConfigurationException {
        String symbolicName = context.getBundle().getSymbolicName();
        String user = resourcesAccessorService.getProperty(context, symbolicName + ".config.user");
        if (null != user && 0 < user.length()) {
            this.user = user;
        }

        String password = resourcesAccessorService.getProperty(context, symbolicName + ".config.password");
        if (null != password && 0 < password.length()) {
            this.password = password;
        }

        String busyWait = resourcesAccessorService.getProperty(context, symbolicName + ".config.busyWait");
        if (null != busyWait) {
            this.busyWait = busyWait.trim().equalsIgnoreCase("true");
        }

        String port = resourcesAccessorService.getProperty(context, symbolicName + ".config.port");
        if (null != port && 0 < port.length()) {
            try {
                this.port = Integer.parseInt(port);
            } catch (NumberFormatException e) {
                LOG.error("Bad framework property " + symbolicName + ".config.port", e);
            }
        }

        String host = resourcesAccessorService.getProperty(context, symbolicName + ".config.host");
        if (null != host) {
            host = host.trim();
            if ("".equals(host)) {
                host = null;
            }
            this.host = host;
        }

        updated(configuration);
    }

    public void updated(Dictionary configuration) throws ConfigurationException {
        if (configuration == null) {
            return;
        }

        Enumeration e = configuration.keys();
        while (e.hasMoreElements()) {
            String key = (String) e.nextElement();
            Object value = configuration.get(key);
            try {
                if (key.equals(PORT_KEY)) {
                    port = ((Integer) value).intValue();
                } else if (key.equals(HOST_KEY)) {
                    host = ((String) value);
                    host = host.trim();
                    if ("".equals(host)) {
                        host = null;
                    }
                }
            } catch (ClassCastException cce) {
                throw new ConfigurationException(key, "Wrong type: " + value.getClass().getName());
            }
        }
    }

    public int getPort() {
        return port;
    }

    public InetAddress getAddress() throws UnknownHostException {
        InetAddress inetAddress = null;
        if (host != null) {
            inetAddress = InetAddress.getByName(host);
        }

        return inetAddress;
    }

    public int getSocketTimeout() {
        return 30000;
    }

    public int getBacklog() {
        return 100;
    }

    public String getUser() {
        return user;
    }

    public String getPassword() {
        return password;
    }

    String getInputPath() {
        return "telnet"; // To be defined
    }

    String getAuthorizationMethod() {
        return "passwd"; // To be defined
    }

    boolean getBusyWait() {
        return busyWait;
    }

} // TelnetConfig
