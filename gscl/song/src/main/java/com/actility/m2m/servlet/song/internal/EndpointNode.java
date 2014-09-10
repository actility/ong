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
 * id $Id: EndpointNode.java 6085 2013-10-15 14:08:29Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6085 $
 * lastrevision $Date: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song.internal;

import javax.servlet.RequestDispatcher;

import com.actility.m2m.be.BackendEndpoint;
import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.servlet.ext.ExtProtocolContainer;
import com.actility.m2m.servlet.ext.ExtProtocolServlet;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.song.SongServlet;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;

public interface EndpointNode extends InternalNode, BackendEndpoint, ExtProtocolServlet {

    SongServlet getSongServlet();

    ExtProtocolContainer getProtocol();

    ExtServletContext getServletContext();

    RequestDispatcher getRequestDispatcher(String path);

    SongContainerFacade getContainer();

    int getPriority();

    InOut buildRequestExchange(SongServletRequest request) throws MessagingException;

    InOut buildResponseExchange(InOut exchange, SongServletResponse response) throws MessagingException;

}
