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
 * id $Id: SongServlet.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song;

import java.io.IOException;

import javax.servlet.GenericServlet;

/**
 * Provides an abstract class to be subclassed to create a SONG servlet. This class receives incoming messages through the
 * service method. This method calls doRequest or doResponse for incoming requests and responses, respectively. These two
 * methods in turn dispatch on request method or status code to one of the following methods:
 * <p>
 * For requests:
 * <ul>
 * <li>doRetrieve - for SONG RETRIEVE requests</li>
 * <li>doUpdate - for SONG UPDATE requests</li>
 * <li>doCreate - for SONG CREATE requests</li>
 * <li>doDelete - for SONG DELETE requests</li>
 * </ul>
 * <p>
 * For responses:
 * <ul>
 * <li>doProvisionalResponse - for SONG 202</li>
 * <li>doSuccessResponse - for SONG 2xx - 3xx</li>
 * <li>doErrorResponse - for SONG 4xx - 5xx</li>
 * </ul>
 * <p>
 * All request handling methods reject the request with a 500 error response. Subclasses of SongServlet will usually override
 * one or more of these methods.
 */
public abstract class SongServlet extends GenericServlet {
    private static final long serialVersionUID = -4999805211681614748L;

    /**
     * The string "com.actility.servlet.song.SongFactory". This is the name of the ServletContext attribute whose value is an
     * instance of the SongFactory interface.
     */
    public static final java.lang.String SONG_FACTORY = "com.actility.servlet.song.SongFactory";

    /**
     * The string "com.actility.servlet.song.protocol.SongProtocolFactory". This is the name of the ServletContext attribute
     * whose value is an instance of the SongProtocolFactory interface.
     */
    public static final java.lang.String SONG_BINDING_FACTORY = "com.actility.servlet.song.protocol.SongProtocolFactory";

    /**
     * The string "com.actility.servlet.song.SongDynamicRouter". This is the name of the ServletContext attribute whose value is
     * an instance of the SongDynamicRouter interface.
     */
    public static final java.lang.String SONG_DYNAMIC_ROUTER = "com.actility.servlet.song.SongDynamicRouter";

    /**
     * The string "com.actility.servlet.TimerService". This is the name of the ServletContext attribute whose value is an
     * instance of the TimerService interface.
     */
    public static final java.lang.String TIMER_SERVICE = "com.actility.servlet.TimerService";

    /**
     * The string "com.actility.servlet.song.HostName". This is the default host name configured for the SONG container.
     */
    public static final java.lang.String HOST_NAME = "com.actility.servlet.song.HostName";

    /**
     * The string "com.actility.servlet.song.DomainName". This is the default domain name configured for the SONG container.
     */
    public static final java.lang.String DOMAIN_NAME = "com.actility.servlet.song.DomainName";

    /**
     * Invoked to handle incoming requests. This method dispatched requests to one of the doXxx methods where Xxx is the Song
     * method used in the request. Servlets will not usually need to override this method.
     */
    protected void doRequest(final SongServletRequest req) throws javax.servlet.ServletException, java.io.IOException {
        final String m = req.getMethod();
        if (SongServletRequest.MD_RETRIEVE.equals(m)) {
            doRetrieve(req);
        } else if (SongServletRequest.MD_UPDATE.equals(m)) {
            doUpdate(req);
        } else if (SongServletRequest.MD_CREATE.equals(m)) {
            doCreate(req);
        } else if (SongServletRequest.MD_DELETE.equals(m)) {
            doDelete(req);
        } else {
            notHandled(req);
        }
    }

    /**
     * Invoked to handle incoming responses. This method dispatched responses to one of the , , . Servlets will not usually need
     * to override this method.
     */
    protected void doResponse(final SongServletResponse resp) throws javax.servlet.ServletException, java.io.IOException {
        final int status = resp.getStatus();
        if (status < 200) {
            doProvisionalResponse(resp);
        } else if (status < 300) {
            doSuccessResponse(resp);
        } else {
            doErrorResponse(resp);
        }
    }

    /**
     * Handle SONG RETRIEVE request
     *
     * @param request The SONG RETRIEVE request
     * @throws javax.servlet.ServletException If there is a problem during the request handling
     * @throws java.io.IOException If there is an IO problem during the request handling
     */
    protected void doRetrieve(SongServletRequest request) throws javax.servlet.ServletException, java.io.IOException {
        notImplemented(request);
    }

    /**
     * Handle SONG UPDATE request
     *
     * @param request The SONG UPDATE request
     * @throws javax.servlet.ServletException If there is a problem during the request handling
     * @throws java.io.IOException If there is an IO problem during the request handling
     */
    protected void doUpdate(SongServletRequest request) throws javax.servlet.ServletException, java.io.IOException {
        notImplemented(request);
    }

    /**
     * Handle SONG CREATE request
     *
     * @param request The SONG CREATE request
     * @throws javax.servlet.ServletException If there is a problem during the request handling
     * @throws java.io.IOException If there is an IO problem during the request handling
     */
    protected void doCreate(SongServletRequest request) throws javax.servlet.ServletException, java.io.IOException {
        notImplemented(request);
    }

    /**
     * Handle SONG DELETE request
     *
     * @param request The SONG DELETE request
     * @throws javax.servlet.ServletException If there is a problem during the request handling
     * @throws java.io.IOException If there is an IO problem during the request handling
     */
    protected void doDelete(SongServletRequest request) throws javax.servlet.ServletException, java.io.IOException {
        notImplemented(request);
    }

    /**
     * Invoked by the server (via the doResponse method) to handle incoming 202 responses. The default implementation is
     * empty and must be overridden by subclasses to do something useful.
     *
     * @throws javax.servlet.ServletException If there is a problem during the response handling
     * @throws java.io.IOException If there is an IO problem during the response handling
     */
    protected void doProvisionalResponse(final SongServletResponse resp) throws javax.servlet.ServletException,
            java.io.IOException {
        // Ignore
    }

    /**
     * Invoked by the server (via the doResponse method) to handle incoming 2xx class responses. The default implementation is
     * empty and must be overridden by subclasses to do something useful.
     *
     * @throws javax.servlet.ServletException If there is a problem during the response handling
     * @throws java.io.IOException If there is an IO problem during the response handling
     */
    protected void doSuccessResponse(final SongServletResponse resp) throws javax.servlet.ServletException, java.io.IOException {
        // Ignore
    }

    /**
     * Invoked by the server (via the doResponse method) to handle incoming 3xx - 6xx class responses. The default
     * implementation is empty and must be overridden by subclasses to do something useful.
     *
     * @throws javax.servlet.ServletException If there is a problem during the response handling
     * @throws java.io.IOException If there is an IO problem during the response handling
     */
    protected void doErrorResponse(final SongServletResponse resp) throws javax.servlet.ServletException, java.io.IOException {
        // Ignore
    }

    /**
     * Writes the specified message to a servlet log file. See {link ServletContext#log(String)}.
     */
    public void log(final java.lang.String message) {
        getServletContext().log(message);
    }

    /**
     * Writes an explanatory message and a stack trace for a given Throwable exception to the servlet log file. See
     * ServletContext.log(String, Throwable).
     */
    public void log(final java.lang.String message, final java.lang.Throwable t) {
        getServletContext().log(message, t);
    }

    /**
     * Invoked to handle incoming Song messages: requests or responses. Exactly one of the arguments is null: if the event is a
     * request the response argument is null, and vice versa, if the event is a response the request argument is null. This
     * method dispatched to doRequest() or doResponse() as appropriate. Servlets will not usually need to override this method.
     */
    public void service(final javax.servlet.ServletRequest req, final javax.servlet.ServletResponse resp)
            throws javax.servlet.ServletException, java.io.IOException {
        if (req != null) {
            doRequest((SongServletRequest) req);
        } else {
            doResponse((SongServletResponse) resp);
        }
    }

    /**
     * Responds a 500 to a given request.
     *
     * @param req The given request
     * @throws IOException If an IO problem occurs
     */
    private void notHandled(final SongServletRequest req) throws IOException {
        final SongServletResponse resp = req.createResponse(500, "Request not handled by app");
        resp.send();
    }

    /**
     * Responds a 501 to a given request
     *
     * @param req The given request
     * @throws IOException If an IO problem occurs
     */
    private void notImplemented(SongServletRequest req) throws IOException {
        final SongServletResponse resp = req.createResponse(501, "Request not implemented");
        resp.send();
    }
}
