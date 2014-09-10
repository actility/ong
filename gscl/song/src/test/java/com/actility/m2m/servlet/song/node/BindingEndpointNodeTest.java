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
 * id $Id: BindingEndpointNodeTest.java 6085 2013-10-15 14:08:29Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6085 $
 * lastrevision $Date: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 16:08:29 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song.node;

import java.net.InetAddress;
import java.net.UnknownHostException;

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
import com.actility.m2m.servlet.song.message.LocalForwardedRequest;
import com.actility.m2m.servlet.song.message.LocalRequest;
import com.actility.m2m.servlet.song.message.LocalWrappedRequest;
import com.actility.m2m.util.EmptyUtils;

public class BindingEndpointNodeTest extends TestCase {
    private Object[] mocks;
    private EndpointContext bindingEndpointContext;
    private EndpointNode externalEndpoint;
    private ExtServletContext servletContext;
    private InOut exchange;
    private BackendMessage inMessage;
    private SongURI uri;
    private SongURI target;
    private SongContainerFacade container;

    private BindingEndpointNode bindingEndpoint;
    private InetAddress address;

    @Override
    public void setUp() throws UnknownHostException {
        bindingEndpointContext = EasyMock.createMock(EndpointContext.class);
        externalEndpoint = EasyMock.createMock(EndpointNode.class);
        servletContext = EasyMock.createMock(ExtServletContext.class);
        exchange = EasyMock.createMock(InOut.class);
        inMessage = EasyMock.createMock(BackendMessage.class);
        uri = EasyMock.createMock(SongURI.class);
        target = EasyMock.createMock(SongURI.class);
        container = EasyMock.createMock(SongContainerFacade.class);

        address = InetAddress.getLocalHost();
        bindingEndpoint = new BindingEndpointNode(container, null, servletContext, "TestServlet", 0, null, "http",
                new String[] { "http", "https" }, true, "HTTP/1.1", address, 80);

        mocks = new Object[] { bindingEndpointContext, externalEndpoint, servletContext, exchange, inMessage, uri, target,
                container };
    }

    public void replay() {
        EasyMock.replay(mocks);
    }

    public void verify() {
        EasyMock.verify(mocks);
    }

    public void testInit() {
        replay();
        bindingEndpoint.init(bindingEndpointContext);
        verify();

        assertEquals(bindingEndpointContext, bindingEndpoint.getContext());
    }

    public void testLocalRequestUpdate() {
        try {
            bindingEndpoint.init(bindingEndpointContext);

            EasyMock.expect(externalEndpoint.getServletContext()).andReturn(servletContext);
            EasyMock.expect(servletContext.getServerInfo()).andReturn("ONG");
            EasyMock.expect(Integer.valueOf(bindingEndpointContext.getEndpointId())).andReturn(Integer.valueOf(5));
            EasyMock.expect(servletContext.getServletRequestAttributeListeners()).andReturn(EmptyUtils.EMPTY_ITERATOR).times(2);
            exchange.setDestination(5);
            EasyMock.expect(exchange.getInMessage()).andReturn(inMessage);
            inMessage.setContent(EasyMock.isA(LocalWrappedRequest.class));

            replay();
            LocalRequest concreteRequest = new LocalRequest(externalEndpoint, null, null, "GET", "HTTP/1.1", null, uri, null);

            bindingEndpoint.update(exchange, concreteRequest, null, null, null, null);
            verify();
        } catch (MessagingException e) {
            fail();
        }
    }

    public void testLocalForwardedRequestUpdate() {
        try {
            bindingEndpoint.init(bindingEndpointContext);

            EasyMock.expect(Integer.valueOf(bindingEndpointContext.getEndpointId())).andReturn(Integer.valueOf(5));
            EasyMock.expect(servletContext.getServletRequestAttributeListeners()).andReturn(EmptyUtils.EMPTY_ITERATOR).times(2);
            exchange.setDestination(5);
            EasyMock.expect(exchange.getInMessage()).andReturn(inMessage);
            inMessage.setContent(EasyMock.isA(LocalWrappedRequest.class));

            replay();
            LocalForwardedRequest concreteRequest = new LocalForwardedRequest(externalEndpoint, null, null);

            bindingEndpoint.update(exchange, concreteRequest, null, null, null, null);
            verify();
        } catch (MessagingException e) {
            fail();
        }
    }
}
