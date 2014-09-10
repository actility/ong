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
 * id $Id: ServletEndpointNodeTest.java 6085 2013-10-15 14:08:29Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6085 $
 * lastrevision $Date: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song.node;

import java.net.InetAddress;

import junit.framework.TestCase;

import org.easymock.EasyMock;

import com.actility.m2m.be.EndpointContext;
import com.actility.m2m.be.messaging.BackendMessage;
import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.internal.EndpointNode;
import com.actility.m2m.servlet.song.internal.SongContainerFacade;
import com.actility.m2m.servlet.song.message.LocalRequest;
import com.actility.m2m.servlet.song.message.RemoteForwardedRequest;
import com.actility.m2m.servlet.song.message.RemoteRequest;
import com.actility.m2m.servlet.song.message.RemoteWrappedRequest;
import com.actility.m2m.util.EmptyUtils;

public class ServletEndpointNodeTest extends TestCase {
    private Object[] mocks;
    private EndpointContext servletEndpointContext;
    private EndpointNode externalEndpoint;
    private InOut exchange;
    private BackendMessage inMessage;
    private SongURI uri;
    private SongURI target;
    private SongContainerFacade container;
    private ExtServletContext servletContext;

    private ServletEndpointNode songServletEndpoint;
    private InetAddress address;

    @Override
    public void setUp() {
        servletEndpointContext = EasyMock.createMock(EndpointContext.class);
        externalEndpoint = EasyMock.createMock(EndpointNode.class);
        exchange = EasyMock.createMock(InOut.class);
        inMessage = EasyMock.createMock(BackendMessage.class);
        uri = EasyMock.createMock(SongURI.class);
        target = EasyMock.createMock(SongURI.class);
        container = EasyMock.createMock(SongContainerFacade.class);
        servletContext = EasyMock.createMock(ExtServletContext.class);

        songServletEndpoint = new ServletEndpointNode(container, null, servletContext, "TestServlet", 0);
        songServletEndpoint.init(servletEndpointContext);

        mocks = new Object[] { servletEndpointContext, externalEndpoint, exchange, inMessage, uri, target, container,
                servletContext };
    }

    public void replay() {
        EasyMock.replay(mocks);
    }

    public void verify() {
        EasyMock.verify(mocks);
    }

    public void testInit() {
        replay();
        songServletEndpoint.init(servletEndpointContext);
        verify();

        assertEquals(servletEndpointContext, songServletEndpoint.getContext());
    }

    public void testLocalRequestUpdate() {
        try {
            EasyMock.expect(externalEndpoint.getServletContext()).andReturn(servletContext);
            EasyMock.expect(servletContext.getServerInfo()).andReturn("ONG");
            EasyMock.expect(Integer.valueOf(servletEndpointContext.getEndpointId())).andReturn(Integer.valueOf(5));
            exchange.setDestination(5);
            EasyMock.expect(exchange.getInMessage()).andReturn(inMessage);
            inMessage.setContent(EasyMock.isA(RemoteForwardedRequest.class));
            EasyMock.expect(servletContext.getServletRequestAttributeListeners()).andReturn(EmptyUtils.EMPTY_ITERATOR);

            replay();
            LocalRequest concreteRequest = new LocalRequest(externalEndpoint, null, null, "GET", "HTTP/1.1", null, uri, null);

            songServletEndpoint.update(exchange, concreteRequest, null, "/context", "", "/mock");
            verify();
        } catch (MessagingException e) {
            fail();
        }
    }

    public void testRemoteRequestUpdate() {
        try {
            EasyMock.expect(Integer.valueOf(servletEndpointContext.getEndpointId())).andReturn(Integer.valueOf(5));
            exchange.setDestination(5);
            EasyMock.expect(exchange.getInMessage()).andReturn(inMessage);
            inMessage.setContent(EasyMock.isA(RemoteWrappedRequest.class));

            replay();
            RemoteRequest concreteRequest = new RemoteRequest(externalEndpoint, "GET", "HTTP/1.1", null, uri, address, 80,
                    address, 456, false, null);

            songServletEndpoint.update(exchange, concreteRequest, null, "/context", null, "/mock");
            verify();
        } catch (MessagingException e) {
            fail();
        }
    }
}
