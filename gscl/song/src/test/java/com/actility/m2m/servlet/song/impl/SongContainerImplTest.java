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
 * id $Id: SongContainerImplTest.java 9309 2014-08-21 10:13:58Z JReich $
 * author $Author: JReich $
 * version $Revision: 9309 $
 * lastrevision $Date: 2014-08-21 12:13:58 +0200 (Thu, 21 Aug 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-08-21 12:13:58 +0200 (Thu, 21 Aug 2014) $
 */

package com.actility.m2m.servlet.song.impl;

import java.io.IOException;
import java.net.InetAddress;
import java.net.URI;
import java.net.UnknownHostException;
import java.util.Iterator;
import java.util.Map;
import java.util.Timer;

import javax.servlet.ServletException;

import junit.framework.TestCase;

import org.easymock.EasyMock;
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;

import com.actility.m2m.be.BackendException;
import com.actility.m2m.be.BackendService;
import com.actility.m2m.be.EndpointContext;
import com.actility.m2m.be.messaging.BackendMessage;
import com.actility.m2m.be.messaging.DeliveryChannel;
import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessageExchangeFactory;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.framework.resources.ResourcesAccessorService;
import com.actility.m2m.log.LogService;
import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.StatusCode;
import com.actility.m2m.servlet.NamespaceException;
import com.actility.m2m.servlet.ext.ExtProtocolServlet;
import com.actility.m2m.servlet.ext.ExtServletContext;
import com.actility.m2m.servlet.service.ext.ExtServletService;
import com.actility.m2m.servlet.song.SongDynamicRouter;
import com.actility.m2m.servlet.song.SongFactory;
import com.actility.m2m.servlet.song.SongServlet;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.servlet.song.internal.BindingNode;
import com.actility.m2m.servlet.song.internal.EndpointNode;
import com.actility.m2m.servlet.song.message.LocalRequest;
import com.actility.m2m.servlet.song.message.RemoteForwardedRequest;
import com.actility.m2m.servlet.song.message.RemoteResponse;
import com.actility.m2m.servlet.song.node.ServletEndpointNode;
import com.actility.m2m.transport.logger.TransportLoggerService;
import com.actility.m2m.util.EmptyUtils;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;
import com.actility.m2m.xo.XoService;

public class SongContainerImplTest extends TestCase {
    private Object[] mocks;
    private ExtServletService servletService;
    private BackendService backendService;
    private XoService xoService;
    private XoObject xoObject;
    private ResourcesAccessorService resourcesAccessorService;
    private TransportLoggerService transportLoggerService;
    private EndpointContext containerEndpointContext;
    private EndpointContext servletEndpointContext;
    private EndpointNode endpointNode;
    private DeliveryChannel deliveryChannel;
    private MessageExchangeFactory messageExchangeFactory;
    private InOut exchange;
    private BackendMessage inMessage;
    private BackendMessage outMessage;
    private SongServletRequest request;
    private ExtServletContext servletContext;
    private SongServlet songServlet;
    private SongURI uri;
    private BindingNode bindingNode;
    // private SongContainerFacade facade;
    private BundleContext bundleContext;
    private Bundle bundle;

    private SongContainer songContainer;
    private InetAddress address;

    @Override
    public void setUp() throws UnknownHostException, BackendException {
        servletService = EasyMock.createMock(ExtServletService.class);
        backendService = EasyMock.createMock(BackendService.class);
        xoService = EasyMock.createMock(XoService.class);
        resourcesAccessorService = EasyMock.createMock(ResourcesAccessorService.class);
        xoObject = EasyMock.createMock(XoObject.class);
        transportLoggerService = EasyMock.createMock(TransportLoggerService.class);
        containerEndpointContext = EasyMock.createMock(EndpointContext.class);
        servletEndpointContext = EasyMock.createMock(EndpointContext.class);
        endpointNode = EasyMock.createMock(EndpointNode.class);
        deliveryChannel = EasyMock.createMock(DeliveryChannel.class);
        messageExchangeFactory = EasyMock.createMock(MessageExchangeFactory.class);
        exchange = EasyMock.createMock(InOut.class);
        inMessage = EasyMock.createMock(BackendMessage.class);
        outMessage = EasyMock.createMock(BackendMessage.class);
        request = EasyMock.createMock(SongServletRequest.class);
        uri = EasyMock.createMock(SongURI.class);
        servletContext = EasyMock.createMock(ExtServletContext.class);
        songServlet = EasyMock.createMock(SongServlet.class);
        bindingNode = EasyMock.createMock(BindingNode.class);
        bundleContext = EasyMock.createMock(BundleContext.class);
        bundle = EasyMock.createMock(Bundle.class);

        address = InetAddress.getLocalHost();
        // facade = new SongContainerFacade(songContainer, containerEndpointContext, servletEndpointContext, servletContext);

        mocks = new Object[] { servletService, backendService, xoService, resourcesAccessorService, xoObject,
                transportLoggerService, containerEndpointContext, servletEndpointContext, endpointNode, deliveryChannel,
                messageExchangeFactory, exchange, inMessage, outMessage, request, uri, servletContext, songServlet,
                bindingNode, bundleContext, bundle };
    }

    private void replay() {
        EasyMock.replay(mocks);
    }

    private void verify() {
        EasyMock.verify(mocks);
    }

    private void reset() {
        EasyMock.reset(mocks);
    }

    public void testPathsGeneration() {
        // To test

        // Register with context path "/gsc" servlet "/"
        // http://localhost:8080/gsc/apps => contextPath: /gsc, servletPath: , pathInfo: /apps

        // Register with context path "/gsc" servlet "/"
        // http://localhost:8080/gsc/apps/ => contextPath: /gsc, servletPath: , pathInfo: /apps/

        // Register with context path "/gsc" servlet "/"
        // http://localhost:8080/gsc/ => contextPath: /gsc, servletPath: , pathInfo: /

        // Register with context path "/gsc" servlet "/"
        // http://localhost:8080/gsc => contextPath: /gsc, servletPath: , pathInfo: null

        // Register with context path "/gsc" servlet "/apps"
        // http://localhost:8080/gsc/apps => contextPath: /gsc, servletPath: /apps, pathInfo: null

        // Register with context path "/gsc" servlet "/apps"
        // http://localhost:8080/gsc/apps/ => contextPath: /gsc, servletPath: /apps, pathInfo: /

        // Register with context path "/gsc" servlet "/apps"
        // http://localhost:8080/gsc/apps/app1 => contextPath: /gsc, servletPath: /apps, pathInfo: /app1

        // Register with context path "/gsc" servlet "/apps"
        // http://localhost:8080/gsc/apps/app1/ => contextPath: /gsc, servletPath: /apps, pathInfo: /app1/
    }

    public void testExternalAlias() {
        URI receivedURI = URI
                .create("http://r35538.ovh.net:8082//http//contact//d91ea51174a6f0fa90db64dd35f3710245b5a541/apps/toto");
        String bestPrefix = "http://r35538.ovh.net:8082/http/contact/d91ea51174a6f0fa90db64dd35f3710245b5a541/";
        String internalContextPath = "/gsc";

        String contextPath = "/http/contact/d91ea51174a6f0fa90db64dd35f3710245b5a541";
        String target = "/gsc/apps/toto";

        // Search for a song servlet
        URI realURI = receivedURI.normalize();
        String externalAlias = realURI.toString();
        String externalContextPath = null;

        int slashOffset = 0;
        if (!externalAlias.endsWith("/")) {
            slashOffset = 1;
            externalAlias = externalAlias + "//";
        } else {
            externalAlias = externalAlias + "/";
        }

        String remainingPath = "";
        if ((externalAlias.length() - slashOffset) >= (bestPrefix.length() + 1)) {
            remainingPath = externalAlias.substring(bestPrefix.length() - 1, externalAlias.length() - 1 - slashOffset);
        }
        externalContextPath = realURI.getPath().substring(0, realURI.getPath().length() - remainingPath.length());
        String newTarget = internalContextPath + remainingPath;

        assertEquals(contextPath, externalContextPath);
        assertEquals(target, newTarget);
    }

    private void start() {
        EasyMock.expect(Boolean.valueOf(xoService.isXmlSupported())).andReturn(Boolean.TRUE).anyTimes();
        EasyMock.expect(Boolean.valueOf(xoService.isExiSupported())).andReturn(Boolean.FALSE).anyTimes();
        try {
            backendService.registerEndpoint(EasyMock.isA(SongContainer.class), (Map) EasyMock.isNull());
        } catch (BackendException e) {
            fail();
        }
        // EasyMock.expect(bundleContext.getBundle()).andReturn(bundle);
        // EasyMock.expect(Long.valueOf(bundle.getBundleId())).andReturn(Long.valueOf(1L));
        // try {
        // bundleContext.addServiceListener((ServiceListener) EasyMock.anyObject(), (String) EasyMock.anyObject());
        // } catch (InvalidSyntaxException e) {
        // fail();
        // }
        // EasyMock.expect(bundleContext.getServiceReference(LogService.class.getName())).andReturn(null).anyTimes();
        // EasyMock.expect(bundleContext.getServiceReference(org.osgi.service.log.LogService.class.getName())).andReturn(null)
        // .anyTimes();

        replay();
        try {
            songContainer = new SongContainer(servletService, backendService, xoService, resourcesAccessorService,
                    transportLoggerService, new RouteConfiguration[] { new RouteConfiguration("0.0.0.0", 0, false) }, "test",
                    "localdomain", 100L);
            verify();
            reset();
        } catch (UnknownHostException e) {
            fail();
        } catch (BackendException e) {
            fail();
        }
    }

    private void init() {
        EasyMock.expect(bundleContext.getServiceReference(LogService.class.getName())).andReturn(null).anyTimes();
        EasyMock.expect(bundleContext.getServiceReference(org.osgi.service.log.LogService.class.getName())).andReturn(null)
                .anyTimes();
        replay();
        songContainer.init(containerEndpointContext);
        verify();
        assertEquals(containerEndpointContext, songContainer.getContext());
        reset();
    }

    private void registerServletWithPath() {
        try {
            EasyMock.expect(servletContext.getServletContextName()).andReturn("MockApplication").anyTimes();
            backendService.registerEndpoint(EasyMock.isA(ServletEndpointNode.class), (Map) EasyMock.isNull());
            EasyMock.expect(servletContext.getAttribute("com.actility.servlet.song.SongFactory")).andReturn(null);
            EasyMock.expect(servletContext.getContextPath()).andReturn("/context");
            servletContext.setAttribute(EasyMock.eq("com.actility.servlet.song.SongFactory"), EasyMock.isA(SongFactory.class));
            servletContext.setAttribute(EasyMock.eq("com.actility.servlet.song.SongDynamicRouter"),
                    EasyMock.isA(SongDynamicRouter.class));
            servletContext.setAttribute(EasyMock.eq("com.actility.servlet.song.HostName"), EasyMock.isA(String.class));
            servletContext.setAttribute(EasyMock.eq("com.actility.servlet.song.DomainName"), EasyMock.isA(String.class));
            servletContext.addServlet(EasyMock.eq("MockServlet"), EasyMock.isA(ExtProtocolServlet.class),
                    EasyMock.isNull(Map.class));
            EasyMock.expect(bundleContext.getServiceReference(LogService.class.getName())).andReturn(null).anyTimes();
            EasyMock.expect(bundleContext.getServiceReference(org.osgi.service.log.LogService.class.getName())).andReturn(null)
                    .anyTimes();

            replay();
            songContainer.registerServiceServlet(servletContext, "/mock", "MockServlet", songServlet, null, null);
            verify();
            reset();
        } catch (ServletException e) {
            fail();
        } catch (BackendException e) {
            fail();
        } catch (NamespaceException e) {
            fail();
        }
    }

    private void registerServlet() {
        try {
            EasyMock.expect(servletContext.getServletContextName()).andReturn("MockApplication");
            backendService.registerEndpoint(EasyMock.isA(ServletEndpointNode.class), (Map) EasyMock.isNull());
            EasyMock.expect(servletContext.getAttribute("com.actility.servlet.song.SongFactory")).andReturn(null);
            servletContext.setAttribute(EasyMock.eq("com.actility.servlet.song.SongFactory"), EasyMock.isA(SongFactory.class));
            servletContext.setAttribute(EasyMock.eq("com.actility.servlet.song.SongDynamicRouter"),
                    EasyMock.isA(SongDynamicRouter.class));
            servletContext.setAttribute(EasyMock.eq("com.actility.servlet.song.HostName"), EasyMock.isA(String.class));
            servletContext.setAttribute(EasyMock.eq("com.actility.servlet.song.DomainName"), EasyMock.isA(String.class));
            servletContext.addServlet(EasyMock.eq("MockServlet"), EasyMock.isA(ExtProtocolServlet.class),
                    EasyMock.isNull(Map.class));
            EasyMock.expect(bundleContext.getServiceReference(LogService.class.getName())).andReturn(null).anyTimes();
            EasyMock.expect(bundleContext.getServiceReference(org.osgi.service.log.LogService.class.getName())).andReturn(null)
                    .anyTimes();

            replay();
            songContainer.registerServiceServlet(servletContext, "MockServlet", songServlet, null, null);
            verify();
            reset();
        } catch (ServletException e) {
            fail();
        } catch (BackendException e) {
            fail();
        }
    }

    public void testStart() {
        start();
    }

    public void testInit() {
        start();
        init();
    }

    public void testRegisterServletWithPath() {
        start();
        init();
        registerServletWithPath();
    }

    public void testRegisterServletWithoutPath() {
        start();
        init();
        registerServlet();
    }

    public void testProcessWithOutMessage() {
        try {
            start();
            init();

            EasyMock.expect(exchange.getExchangeId()).andReturn("123456").times(2);
            EasyMock.expect(exchange.getOutMessage()).andReturn(outMessage);
            // EasyMock.expect(bundleContext.getServiceReference(LogService.class.getName())).andReturn(null).anyTimes();
            // EasyMock.expect(bundleContext.getServiceReference(org.osgi.service.log.LogService.class.getName())).andReturn(null)
            // .anyTimes();

            replay();
            songContainer.process(exchange);
            verify();
            reset();
        } catch (Exception e) {
            fail();
        }
    }

    public void testProcessNotARequest() {
        try {
            start();
            init();

            EasyMock.expect(exchange.getExchangeId()).andReturn("123456");
            EasyMock.expect(exchange.getOutMessage()).andReturn(null);
            EasyMock.expect(exchange.getInMessage()).andReturn(inMessage);
            EasyMock.expect(inMessage.getContent()).andReturn(uri);
            // EasyMock.expect(bundleContext.getServiceReference(LogService.class.getName())).andReturn(null).anyTimes();
            // EasyMock.expect(bundleContext.getServiceReference(org.osgi.service.log.LogService.class.getName())).andReturn(null)
            // .anyTimes();

            replay();
            songContainer.process(exchange);
            fail();
        } catch (IllegalArgumentException e) {
            // OK
            verify();
            reset();
        } catch (Exception e) {
            fail();
        }
    }

    public void testProcessNoServletFound() {
        final Timer timer = new Timer();
        try {
            start();
            init();

            EasyMock.expect(endpointNode.getServletContext()).andReturn(servletContext);
            EasyMock.expect(servletContext.getServerInfo()).andReturn("ONG");
            // EasyMock.expect(bundleContext.getServiceReference(LogService.class.getName())).andReturn(null).anyTimes();
            // EasyMock.expect(bundleContext.getServiceReference(org.osgi.service.log.LogService.class.getName())).andReturn(null)
            // .anyTimes();

            replay();
            LocalRequest concreteRequest = new LocalRequest(endpointNode, bindingNode, null, "GET", "HTTP/1.1", uri, uri, null);
            verify();
            reset();

            // concreteRequest.send()
            // servletService.scheduleTimer(EasyMock.anyObject(TimerTask.class), EasyMock.eq(32000L));
            // EasyMock.expectLastCall().andAnswer(new IAnswer<Object>() {
            // public Object answer() throws Throwable {
            // TimerTask timerTask = (TimerTask) EasyMock.getCurrentArguments()[0];
            // timer.schedule(timerTask, 32000L);
            // return null;
            // }
            // });
            EasyMock.expect(endpointNode.buildRequestExchange(concreteRequest)).andReturn(exchange);
            // EasyMock.expect(endpointNode.getContainer()).andReturn(songContainer);
            EasyMock.expect(endpointNode.getContext()).andReturn(servletEndpointContext).anyTimes();
            EasyMock.expect(Integer.valueOf(endpointNode.getPriority())).andReturn(Integer.valueOf(0));
            EasyMock.expect(servletEndpointContext.getDeliveryChannel()).andReturn(deliveryChannel);
            deliveryChannel.send(exchange, 0);

            EasyMock.expect(exchange.getExchangeId()).andReturn("123456").anyTimes();
            EasyMock.expect(uri.absoluteURI()).andReturn("http://localhost:8080/context/mock").anyTimes();
            EasyMock.expect(uri.getHost()).andReturn("localhost").anyTimes();
            EasyMock.expect(uri.getPath()).andReturn("/mock").anyTimes();

            EasyMock.expect(uri.toURI()).andReturn(URI.create("http://localhost:8080/context/mock"));
            EasyMock.expect(bindingNode.getAddress()).andReturn(address);
            EasyMock.expect(Integer.valueOf(bindingNode.getPort())).andReturn(Integer.valueOf(80));
            EasyMock.expect(exchange.getOutMessage()).andReturn(null);
            EasyMock.expect(exchange.getInMessage()).andReturn(inMessage);
            EasyMock.expect(inMessage.getContent()).andReturn(concreteRequest);
            EasyMock.expect(exchange.createMessage()).andReturn(outMessage);
            EasyMock.expect(endpointNode.getTarget()).andReturn("Mock.Endpoint").anyTimes();
            EasyMock.expect(endpointNode.getContainer()).andReturn(songContainer).anyTimes();
            EasyMock.expect(xoService.newXmlXoObject("m2m:errorInfo")).andReturn(xoObject);
            xoObject.setNameSpace(M2MConstants.PREFIX_M2M);
            xoObject.setStringAttribute(M2MConstants.TAG_M2M_STATUS_CODE, StatusCode.STATUS_NOT_FOUND.name());
            xoObject.setStringAttribute(M2MConstants.TAG_M2M_ADDITIONAL_INFO, "Unknown path: /mock");
            EasyMock.expect(xoObject.saveXml()).andReturn(EmptyUtils.EMPTY_BYTES_ARRAY);
            xoObject.free(true);
            songContainer.sendLocalRequest("GET");
            outMessage.setContent(EasyMock.isA(RemoteResponse.class));
            EasyMock.expect(containerEndpointContext.getDeliveryChannel()).andReturn(deliveryChannel);
            exchange.setOutMessage(outMessage);
            EasyMock.expect(Boolean.valueOf(deliveryChannel.sendSync(exchange))).andReturn(Boolean.TRUE);
            // EasyMock.expect(bundleContext.getServiceReference(LogService.class.getName())).andReturn(null).anyTimes();
            // EasyMock.expect(bundleContext.getServiceReference(org.osgi.service.log.LogService.class.getName())).andReturn(null)
            // .anyTimes();

            replay();
            concreteRequest.send();

            songContainer.process(exchange);
            verify();
            reset();
        } catch (MessagingException e) {
            fail();
        } catch (IOException e) {
            fail();
        } catch (XoException e) {
            fail();
        }
        timer.cancel();
    }

    public void testProcessServletFound() {
        try {
            start();
            init();
            registerServletWithPath();

            ServletEndpointNode endpoint = null;
            Iterator it = songContainer.getPathRoutes();
            while (it.hasNext()) {
                Map.Entry entry = (Map.Entry) it.next();
                String path = (String) entry.getKey();
                endpoint = (ServletEndpointNode) entry.getValue();
                if (path.equals("/context/mock/")) {
                    endpoint.init(servletEndpointContext);
                    break;
                }
            }

            EasyMock.expect(endpointNode.getServletContext()).andReturn(servletContext);
            EasyMock.expect(servletContext.getServerInfo()).andReturn("ONG");
            // EasyMock.expect(bundleContext.getServiceReference(LogService.class.getName())).andReturn(null).anyTimes();
            // EasyMock.expect(bundleContext.getServiceReference(org.osgi.service.log.LogService.class.getName())).andReturn(null)
            // .anyTimes();

            replay();
            LocalRequest concreteRequest = new LocalRequest(endpointNode, bindingNode, null, "GET", "HTTP/1.1", uri, uri, null);
            verify();
            reset();

            // concreteRequest.send()
            // servletService.scheduleTimer(EasyMock.anyObject(TimerTask.class), EasyMock.eq(32000L));
            // EasyMock.expectLastCall().andAnswer(new IAnswer<Object>() {
            // public Object answer() throws Throwable {
            // TimerTask timerTask = (TimerTask) EasyMock.getCurrentArguments()[0];
            // timer.schedule(timerTask, 32000L);
            // return null;
            // }
            // });
            EasyMock.expect(endpointNode.buildRequestExchange(concreteRequest)).andReturn(exchange);
            // EasyMock.expect(endpointNode.getContainer()).andReturn(songContainer);
            EasyMock.expect(endpointNode.getContext()).andReturn(servletEndpointContext);
            EasyMock.expect(Integer.valueOf(endpointNode.getPriority())).andReturn(Integer.valueOf(0));
            EasyMock.expect(servletEndpointContext.getDeliveryChannel()).andReturn(deliveryChannel);
            EasyMock.expect(endpointNode.getTarget()).andReturn("Mock.Endpoint");
            EasyMock.expect(endpointNode.getContainer()).andReturn(songContainer);
            songContainer.sendLocalRequest("GET");
            deliveryChannel.send(exchange, 0);

            EasyMock.expect(servletContext.getServletContextName()).andReturn("MockApplication").anyTimes();
            EasyMock.expect(songServlet.getServletName()).andReturn("MockServlet").anyTimes();
            EasyMock.expect(exchange.getExchangeId()).andReturn("123456").anyTimes();
            EasyMock.expect(uri.getHost()).andReturn("localhost").anyTimes();
            EasyMock.expect(uri.getPath()).andReturn("/context/mock").anyTimes();
            EasyMock.expect(uri.absoluteURI()).andReturn("http://localhost:8080/context/mock").anyTimes();
            EasyMock.expect(Integer.valueOf(bindingNode.getPort())).andReturn(Integer.valueOf(80)).anyTimes();
            EasyMock.expect(servletContext.getContextPath()).andReturn("/context").anyTimes();

            EasyMock.expect(exchange.getOutMessage()).andReturn(null);
            EasyMock.expect(exchange.getInMessage()).andReturn(inMessage);
            EasyMock.expect(inMessage.getContent()).andReturn(concreteRequest);
            EasyMock.expect(uri.toURI()).andReturn(URI.create("song://localhost/context/mock"));
            // EasyMock.expect(bindingNode.getAddress()).andReturn(address);
            EasyMock.expect(Integer.valueOf(servletEndpointContext.getEndpointId())).andReturn(Integer.valueOf(5));
            exchange.setDestination(5);
            EasyMock.expect(exchange.getInMessage()).andReturn(inMessage);
            EasyMock.expect(servletContext.getServletRequestAttributeListeners()).andReturn(EmptyUtils.EMPTY_ITERATOR);
            inMessage.setContent(EasyMock.isA(RemoteForwardedRequest.class));
            EasyMock.expect(containerEndpointContext.getDeliveryChannel()).andReturn(deliveryChannel);
            deliveryChannel.send(exchange, 1);
            // EasyMock.expect(bundleContext.getServiceReference(LogService.class.getName())).andReturn(null).anyTimes();
            // EasyMock.expect(bundleContext.getServiceReference(org.osgi.service.log.LogService.class.getName())).andReturn(null)
            // .anyTimes();

            replay();
            concreteRequest.send();

            songContainer.process(exchange);
            verify();
            reset();
        } catch (Exception e) {
            fail();
        }
    }
}
