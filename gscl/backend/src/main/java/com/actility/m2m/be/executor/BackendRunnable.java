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
 * id $Id: BackendRunnable.java 8754 2014-05-21 15:23:48Z JReich $
 * author $Author: JReich $
 * version $Revision: 8754 $
 * lastrevision $Date: 2014-05-21 17:23:48 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:23:48 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.be.executor;

import org.apache.log4j.Logger;

import com.actility.m2m.be.BackendEndpoint;
import com.actility.m2m.be.log.BundleLogger;
import com.actility.m2m.be.messaging.InOnly;
import com.actility.m2m.be.messaging.InOut;
import com.actility.m2m.be.messaging.MessageExchange;
import com.actility.m2m.util.log.OSGiLogger;

public class BackendRunnable implements Runnable {
    private static final Logger LOG = OSGiLogger.getLogger(BackendRunnable.class, BundleLogger.getStaticLogger());

    private MessageExchange exchange;
    private BackendEndpoint destination;

    public void init(MessageExchange exchange, BackendEndpoint destination) {
        this.exchange = exchange;
        this.destination = destination;
    }

    public void clear() {
        exchange = null;
        destination = null;
    }

    public void run() {
        try {
            if ("IN".equals(exchange.getPattern())) {
                if (LOG.isDebugEnabled()) {
                    LOG.debug("Send IN exchange to " + exchange.getDestination());
                }
                destination.process((InOnly) exchange);
            } else if ("IN-OUT".equals(exchange.getPattern())) {
                if (LOG.isDebugEnabled()) {
                    LOG.debug("Send IN-OUT exchange to " + exchange.getDestination());
                }
                destination.process((InOut) exchange);
            } else {
                LOG.error("Ignore message exchange. Unknown MEP: " + exchange.getPattern(), null);
            }
        } catch (RuntimeException e) {
            LOG.error("Error while processing the exchange", e);
        }
    }

}
