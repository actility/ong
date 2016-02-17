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
 * id $Id: LocalDataExecutor.java 8754 2014-05-21 15:23:48Z JReich $
 * author $Author: JReich $
 * version $Revision: 8754 $
 * lastrevision $Date: 2014-05-21 17:23:48 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:23:48 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.be.executor;

import org.apache.log4j.Logger;

import com.actility.m2m.be.BackendEndpoint;
import com.actility.m2m.be.BackendExecutor;
import com.actility.m2m.be.log.BundleLogger;
import com.actility.m2m.be.messaging.InOnly;
import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessageExchange;
import com.actility.m2m.be.messaging.MessagingException;
import com.actility.m2m.util.concurrent.CircularArray;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * Implementation of a thread for {@link BackendExecutor}. This reads message exchanges from its local queue to send them for
 * execution to the managed {@link BackendEndpoint}.
 *
 */
public final class LocalDataExecutor extends Thread implements BackendExecutor {
    private static final Logger LOG = OSGiLogger.getLogger(LocalDataExecutor.class, BundleLogger.getStaticLogger());

    private final CircularArray queue;
    private volatile boolean end;
    private final BackendEndpoint endpoint;

    /**
     * Builds a {@link LocalDataExecutor}.
     *
     * @param id The id of the endpoint that this thread is managing
     * @param endpoint The concrete endpoint that this thread is managing
     * @param queueSize Maximum queue size
     */
    public LocalDataExecutor(Integer id, BackendEndpoint endpoint, int queueSize) {
        super("BEndpoint-" + id);
        this.endpoint = endpoint;
        this.queue = new CircularArray(queueSize);
        this.end = false;
        start();
    }

    public void send(MessageExchange exchange, BackendEndpoint destination, int priority) throws MessagingException {
        if (!queue.offer(exchange)) {
            throw new MessagingException("The endpoint queue is full, cannot send to message exchange", true);
        }
        LOG.info("message queued");
    }

    public void destroy() {
        end = true;
        interrupt();
        try {
            join();
        } catch (InterruptedException e) {
            LOG.error("Thread interrupted while joining for the LocalDataExecutor thread", e);
        }
    }

    public void run() {
        MessageExchange exchange = null;
        while (!end) {
            try {
                exchange = (MessageExchange) queue.take();

                if ("IN".equals(exchange.getPattern())) {
                    endpoint.process((InOnly) exchange);
                } else if ("IN-OUT".equals(exchange.getPattern())) {
                    endpoint.process((InOut) exchange);
                } else {
                    LOG.error("Ignore message exchange. Unknown MEP: " + exchange.getPattern(), null);
                }
            } catch (InterruptedException e) {
                end = true;
            } catch (Exception e) {
                LOG.error("Error while processing the exchange", e);
            }
        }
    }
}
