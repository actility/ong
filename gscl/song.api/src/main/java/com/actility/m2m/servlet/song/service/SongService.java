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
 * id $Id: SongService.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song.service;

import java.util.Map;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletConfig;
import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import com.actility.m2m.servlet.NamespaceException;
import com.actility.m2m.servlet.service.ServletService;
import com.actility.m2m.servlet.song.SongDynamicRouter;
import com.actility.m2m.servlet.song.SongServlet;

/**
 * The SONG Service allows other bundles in the OSGi environment to dynamically register {@link SongServlet servlets} into the
 * URI namespace of SONG Service. A bundle may later unregister its servlets.
 */
public interface SongService extends ServletService {
    /**
     * Registers a SONG servlet to a servlet application.
     * <p>
     * After this registration the servlet cannot be accessed directly by SONG network events. The only way to use it is:
     * <ul>
     * <li>to forward a SONG event through a {@link RequestDispatcher} obtained from the
     * {@link ServletContext#getNamedDispatcher(String)}</li>
     * <li>to register a path for this Servlet through the {@link SongDynamicRouter}</li>
     * </ul>
     *
     * @param context The {@link ServletContext} of the servlet application
     * @param servletName The name of the servlet
     * @param servlet The SONG servlet to register
     * @param initParams Initialization arguments for the servlet or null if there are none. This argument is used by the
     *            servlet’s {@link ServletConfig} object.
     * @param configuration Configuration of the registration or null it there is none
     * @throws ServletException If a problem occurs during the registration of the servlet
     */
    void registerServiceServlet(ServletContext context, String servletName, SongServlet servlet, Map initParams,
            Map configuration) throws ServletException;

    /**
     * Registers a SONG servlet to a servlet application. This also registers a path in the SONG container that is linked to
     * that servlet.
     * <p>
     * The path must begin with '/' and must not end with slash '/' or contains any wildcarded '*' with the exception that a
     * path of the form '/' is used to denote the root path. The path is registered with an implicit wildcarded suffix, this
     * means all paths that have the same prefix as the one registered will be sent to that Servlet. The matching process gives
     * a higher priority to the longest prefix found.
     *
     * @param context The {@link ServletContext} of the servlet application
     * @param path The path to register for that servlet
     * @param servletName The name of the servlet
     * @param servlet The SONG servlet to register
     * @param initParams Initialization arguments for the servlet or null if there are none. This argument is used by the
     *            servlet’s {@link ServletConfig} object.
     * @param configuration Configuration of the registration or null it there is none
     * @throws NamespaceException If the path is already bound in the container
     * @throws ServletException If a problem occurs during the registration of the servlet
     */
    void registerServiceServlet(ServletContext context, String path, String servletName, SongServlet servlet, Map initParams,
            Map configuration) throws NamespaceException, ServletException;

    /**
     * Registers a SONG proxy servlet to a servlet application. This also registers a path in the SONG container that is linked
     * to that servlet.
     * <p>
     * The path must begin with '/' and must not end with slash '/' or contains any wildcarded '*' with the exception that a
     * path of the form '/' is used to denote the root path. The path is registered with an implicit wildcarded suffix, this
     * means all paths that have the same prefix as the one registered will be sent to that Servlet. The matching process gives
     * a higher priority to the longest prefix found.
     *
     * @param context The {@link ServletContext} of the servlet application
     * @param servletName The name of the servlet
     * @param servlet The SONG servlet to register
     * @param initParams Initialization arguments for the servlet or null if there are none. This argument is used by the
     *            servlet’s {@link ServletConfig} object.
     * @param configuration Configuration of the registration or null it there is none
     * @throws NamespaceException If the host and port conflicts with an already registered proxy servlet
     * @throws ServletException If a problem occurs during the registration of the servlet
     */
    void registerProxyServlet(ServletContext context, String servletName, SongServlet servlet, Map initParams, Map configuration)
            throws NamespaceException, ServletException;

    /**
     * Unregisters a SONG servlet from the SONG service.
     * <p>
     * This implicitely unregisters all paths registered by this servlet.
     *
     * @param servlet The servlet to unregister
     * @see #unregisterApplication(ServletContext)
     */
    void unregisterServlet(SongServlet servlet);
}
