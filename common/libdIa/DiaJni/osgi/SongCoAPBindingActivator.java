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
 * id $Id: $
 * author $Author: $
 * version $Revision: 17 $
 * lastrevision $Date: 2011-03-04 15:12:54 +0100 (ven., 04 mars 2011) $
 * modifiedby $LastChangedBy: $
 * lastmodified $LastChangedDate: 2011-03-04 15:12:54 +0100 (ven., 04 mars 2011) $
 */

package com.actility.servlet.song.coap.osgi;

import java.net.InetAddress;
import java.net.UnknownHostException;
import java.util.Map;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleContext;
import org.osgi.service.component.ComponentContext;

import com.actility.servlet.NamespaceException;
import com.actility.servlet.song.SongBindingFacade;
import com.actility.servlet.song.coap.CoAPBindingFacade;
import com.actility.servlet.song.coap.SongCoAPBinding;
import com.actility.servlet.song.service.SongBindingService;
import com.actility.util.log.LogUtil;

/**
 * OSGi activator for the SONG HTTP Binding
 */
public final class SongCoAPBindingActivator {
    private static final Logger LOG = Logger.getLogger(SongCoAPBindingActivator.class);

    private static final String[] PROTOCOLS = new String[] { "coap", "coaps" };
    private static final String DEFAULT_PROTOCOL = "CoAP/1.0";

    private String bindingPath;
    private ServletContext applicationContext;
    private SongBindingService songBindingService;
    private SongCoAPBinding songCoAPBinding;

    /**
     * Activates the SONG CoAP Binding.
     *
     * @param componentContext Component context provided by SCR
     * @param config Configuration provided by SCR
     * @throws UnknownHostException In case the local host name could be determined
     * @throws ServletException In case a problem occurs while registering servlets
     * @throws NamespaceException In case a conflict of paths occurs while registering a SONG servlet
     */
    public void activate(ComponentContext componentContext, Map config) throws UnknownHostException, ServletException,
            NamespaceException {
        BundleContext context = componentContext.getBundleContext();
/*	2.4.0
        LogUtil.init(this.getClass().getClassLoader(), context, true);
*/
//	3.0.0
	LogUtil.init(context);

        LOG.info("Starting...");
        try {
            bindingPath = "/coap";
            songBindingService = (SongBindingService) componentContext.locateService("SongBindingService");

            InetAddress localAddress = InetAddress.getLocalHost();
            int localPort = 8080;
            applicationContext = songBindingService.createApplication(bindingPath, "CoAP", null, null, null);

            songCoAPBinding = new SongCoAPBinding();
            SongBindingFacade bindingFacade = new CoAPBindingFacade();

            // TODO Configure CoAP client

            songBindingService.registerBindingServlet(applicationContext, "SONGBinding", songCoAPBinding, null, bindingFacade,
                    "coap", PROTOCOLS, true, DEFAULT_PROTOCOL, localAddress, localPort, null);

            // TODO Start CoAP server
        } catch (UnknownHostException e) {
            LOG.error("Can't get the localhost address", e);
            throw e;
        } catch (ServletException e) {
            if (applicationContext != null) {
                songBindingService.unregisterApplication(applicationContext);
            }
            LOG.error("Can't initialize servlet", e);
            throw e;
        } catch (NamespaceException e) {
            songBindingService.unregisterApplication(applicationContext);
            LOG.error("Problem with the name of the SONG Binding Servlet", e);
            throw e;
        }
    }

    /**
     * Deactivates the SONG HTTP Binding.
     */
    public void deactivate(BundleContext context) {
        LOG.info("Stopping...");
        try {
            songBindingService.unregisterApplication(applicationContext);
        } catch (RuntimeException e) {
            LOG.error("Problem while unregistering CoAP SONG Binding", e);
        }
        try {
            // TODO stop CoAP server

            songCoAPBinding.destroy();
        } catch (RuntimeException e) {
            LOG.error("Problem while deleting the CoAP SONG binding", e);
        } finally {
/*		2.4.0
            LogUtil.close(this.getClass().getClassLoader());
*/
		// 3.0.0
		LogUtil.close(context);
        }
    }
}
