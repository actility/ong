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
 * id $Id: SongBindingService.java 6084 2013-10-15 13:54:23Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6084 $
 * lastrevision $Date: 2013-10-15 15:54:23 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:54:23 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song.service;

import java.net.InetAddress;
import java.util.Map;

import javax.servlet.ServletContext;
import javax.servlet.ServletException;

import com.actility.m2m.servlet.NamespaceException;
import com.actility.m2m.servlet.song.SongBindingFacade;
import com.actility.m2m.servlet.song.SongServlet;

/**
 * This is a SONG service which allows to register a {@link SongServlet} Binding to a servlet application.
 */
public interface SongBindingService extends SongService {
    /**
     * Registers a SONG binding servlet to a servlet application.
     *
     * @param context The {@link ServletContext} of the servlet application
     * @param servletName The name of the servlet
     * @param servlet The SONG servlet to register
     * @param initParams The initial configuration of the servlet
     * @param facade The SONG Binding facade to use for that servlet
     * @param serverScheme The scheme of the server socket
     * @param managedSchemes List of schemes of the binding
     * @param longPollSupported Whether the binding supports long poll connections
     * @param defaultProtocol Default PROTOCOL/VERSION used for the binding (ex. HTTP/1.1)
     * @param address The InetAddres of the binding server
     * @param port The port of the binding server
     * @param configuration Configuration of the registration
     * @throws NamespaceException If the binding is already registered
     * @throws ServletException If a problem occurs during the registration
     */
    void registerBindingServlet(ServletContext context, String servletName, SongServlet servlet, Map initParams,
            SongBindingFacade facade, String serverScheme, String[] managedSchemes, boolean longPollSupported,
            String defaultProtocol, InetAddress address, int port, Map configuration) throws NamespaceException,
            ServletException;
}
