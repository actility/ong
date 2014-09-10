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
 * id $Id: SongRequestDispatcher.java 6085 2013-10-15 14:08:29Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6085 $
 * lastrevision $Date: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song.impl;

import java.io.IOException;

import javax.servlet.RequestDispatcher;
import javax.servlet.ServletException;
import javax.servlet.ServletRequest;
import javax.servlet.ServletResponse;

import com.actility.m2m.servlet.ext.ExtProtocolServlet;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.song.internal.EndpointNode;

public final class SongRequestDispatcher implements RequestDispatcher {

    private final EndpointNode endpoint;

    public SongRequestDispatcher(EndpointNode endpoint) {
        this.endpoint = endpoint;
    }

    public void forward(ServletRequest request, ServletResponse response) throws ServletException, IOException {
        // ApplicationSession appSession = null;
        // SongSession session = null;
        ExtServletContext servletContext = (ExtServletContext) endpoint.getSongServlet().getServletContext();
        // TODO set handler
        // if (request != null) {
        // SongServletRequest songRequest = (SongServletRequest) request;
        // appSession = songRequest.getApplicationSession(false);
        // session = songRequest.getSession(false);
        // } else {
        // SongServletResponse songResponse = (SongServletResponse) response;
        // appSession = songResponse.getApplicationSession(false);
        // session = songResponse.getSession(false);
        // }
        // if (appSession != null) {
        // appSession.setHandler(protocol, songServlet);
        // }
        // if (session != null) {
        // session.setHandler(songServlet);
        // }
        ExtProtocolServlet previousRequestHandler = null;
        try {
            previousRequestHandler = servletContext.setCurrentRequestHandler(endpoint.getProtocol(), endpoint);
            endpoint.getSongServlet().service(request, response);
        } finally {
            servletContext.setCurrentRequestHandler(endpoint.getProtocol(), previousRequestHandler);
        }
    }

    public void include(ServletRequest request, ServletResponse response) throws ServletException {
        throw new ServletException("The include method has no meaning for SONG servlets.");
    }
}
