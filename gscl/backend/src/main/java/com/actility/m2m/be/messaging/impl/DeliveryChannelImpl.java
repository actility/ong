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
 * id $Id: DeliveryChannelImpl.java 8754 2014-05-21 15:23:48Z JReich $
 * author $Author: JReich $
 * version $Revision: 8754 $
 * lastrevision $Date: 2014-05-21 17:23:48 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:23:48 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.be.messaging.impl;

import org.apache.log4j.Logger;

import com.actility.m2m.be.BackendEndpoint;
import com.actility.m2m.be.BackendEndpointContext;
import com.actility.m2m.be.BackendHook;
import com.actility.m2m.be.BackendService;
import com.actility.m2m.be.log.BundleLogger;
import com.actility.m2m.be.messaging.DeliveryChannel;
import com.actility.m2m.be.messaging.InOnly;
import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessageExchange;
import com.actility.m2m.be.messaging.MessageExchangeFactory;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.util.log.OSGiLogger;

public final class DeliveryChannelImpl implements DeliveryChannel {

    private static final Logger LOG = OSGiLogger.getLogger(DeliveryChannelImpl.class, BundleLogger.getStaticLogger());
    private final BackendEndpoint endpoint;
    private final BackendService container;
    private final MessageExchangeFactory factory;

    public DeliveryChannelImpl(BackendEndpoint endpoint, BackendService container, MessageExchangeFactory factory) {
        this.endpoint = endpoint;
        this.container = container;
        this.factory = factory;
    }

    public void close() {
        // TODO
    }

    public MessageExchangeFactory createExchangeFactory() {
        return factory;
    }

    public MessageExchangeFactory createExchangeFactory(int endpointId) {
        return null;
    }

    public void send(MessageExchange exchange) throws MessagingException {
        send(exchange, NORMAL);
    }

    public boolean sendSync(MessageExchange exchange) throws MessagingException {
        // long send = -System.currentTimeMillis();
        if ("IN".equals(exchange.getPattern())) {
            if (((InOnly) exchange).getInMessage() == null) {
                throw new MessagingException("No IN message in InOnly MEP");
            }
            if (LOG.isInfoEnabled()) {
                LOG.info("Send exchange from " + exchange.getSource() + " to " + exchange.getDestination());
            }
            sendSync(exchange, exchange.getDestination());
        } else if ("IN-OUT".equals(exchange.getPattern())) {
            if (((InOut) exchange).getInMessage() == null) {
                throw new MessagingException("No IN message in InOut MEP");
            }
            if (((InOut) exchange).getOutMessage() == null) {
                if (LOG.isInfoEnabled()) {
                    LOG.info("Send exchange from " + exchange.getSource() + " to " + exchange.getDestination());
                }
                sendSync(exchange, exchange.getDestination());
            } else {
                if (LOG.isInfoEnabled()) {
                    LOG.info("Send exchange from " + exchange.getDestination() + " to " + exchange.getSource());
                }
                sendSync(exchange, exchange.getSource());
            }
        } else {
            throw new MessagingException("Unknown MEP: " + exchange.getPattern());
        }
        // send += System.currentTimeMillis();
        // LOG.error("SendSync: " + send);
        return true;
    }

    public boolean sendSync(MessageExchange exchange, long timeoutMS) {
        // TODO
        return false;
    }

    public void send(MessageExchange exchange, int priority) throws MessagingException {
        // long send = -System.currentTimeMillis();
        int realPriority = priority;
        if (priority < 0 || priority > 2) {
            realPriority = NORMAL;
        }
        if ("IN".equals(exchange.getPattern())) {
            if (((InOnly) exchange).getInMessage() == null) {
                throw new MessagingException("No IN message in InOnly MEP");
            }
            if (LOG.isInfoEnabled()) {
                LOG.info("Send exchange from " + exchange.getSource() + " to " + exchange.getDestination());
            }
            send(exchange, exchange.getDestination(), realPriority);
        } else if ("IN-OUT".equals(exchange.getPattern())) {
            if (((InOut) exchange).getInMessage() == null) {
                throw new MessagingException("No IN message in InOut MEP");
            }
            if (((InOut) exchange).getOutMessage() == null) {
                if (LOG.isInfoEnabled()) {
                    LOG.info("Send exchange from " + exchange.getSource() + " to " + exchange.getDestination());
                }
                send(exchange, exchange.getDestination(), realPriority);
            } else {
                if (LOG.isInfoEnabled()) {
                    LOG.info("Send exchange from " + exchange.getDestination() + " to " + exchange.getSource());
                }
                send(exchange, exchange.getSource(), realPriority);
            }
        } else {
            throw new MessagingException("Unknown MEP: " + exchange.getPattern());
        }
        // send += System.currentTimeMillis();
        // LOG.error("Send: " + send);
    }

    private void sendSync(MessageExchange exchange, int endpointId) throws MessagingException {
        BackendEndpointContext destEndpointContext = container.getEndpoint(endpointId);
        BackendHook hook = container.getHook();
        if (destEndpointContext != null) {
            BackendEndpoint destEndpoint = destEndpointContext.getEndpoint();
            if (hook != null) {
                hook.resolvedExchange(exchange, endpoint, destEndpoint);
            }
            try {
                if ("IN".equals(exchange.getPattern())) {
                    destEndpoint.process((InOnly) exchange);
                } else if ("IN-OUT".equals(exchange.getPattern())) {
                    destEndpoint.process((InOut) exchange);
                } else {
                    LOG.error("Ignore message exchange. Unknown MEP: " + exchange.getPattern(), null);
                }
            } catch (Exception e) {
                LOG.error("Error while processing the exchange", e);
            }
        } else {
            if (hook != null) {
                hook.unresolvedExchange(exchange, endpoint);
            }
            LOG.error("Unknwon target endpoint: " + endpointId + ", ignore it");
        }
    }

    private void send(MessageExchange exchange, int endpointId, int priority) throws MessagingException {
        BackendEndpointContext destEndpointContext = container.getEndpoint(endpointId);
        BackendHook hook = container.getHook();
        if (destEndpointContext != null) {
            BackendEndpoint destEndpoint = destEndpointContext.getEndpoint();
            if (hook != null) {
                hook.resolvedExchange(exchange, endpoint, destEndpoint);
            }
            destEndpointContext.getExecutor().send(exchange, destEndpoint, priority);
        } else {
            if (hook != null) {
                hook.unresolvedExchange(exchange, endpoint);
            }
            LOG.error("Unknwon target endpoint: " + endpointId + ", ignore it");
        }
    }
}
