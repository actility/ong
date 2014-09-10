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
 * id $Id: BackendServiceImpl.java 8754 2014-05-21 15:23:48Z JReich $
 * author $Author: JReich $
 * version $Revision: 8754 $
 * lastrevision $Date: 2014-05-21 17:23:48 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:23:48 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.be.impl;

import java.text.ParseException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;

import com.actility.m2m.be.BackendEndpoint;
import com.actility.m2m.be.BackendEndpointContext;
import com.actility.m2m.be.BackendException;
import com.actility.m2m.be.BackendExecutor;
import com.actility.m2m.be.BackendHook;
import com.actility.m2m.be.BackendService;
import com.actility.m2m.be.EndpointContext;
import com.actility.m2m.be.executor.LocalDataExecutor;
import com.actility.m2m.be.executor.ThreadPoolExecutor;
import com.actility.m2m.be.log.BundleLogger;
import com.actility.m2m.be.messaging.MessageExchangeFactory;
import com.actility.m2m.be.messaging.impl.DeliveryChannelImpl;
import com.actility.m2m.be.messaging.impl.MessageExchangeFactoryImpl;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * The current node manager
 */
public final class BackendServiceImpl implements BackendService {
    private static final Logger LOG = OSGiLogger.getLogger(BackendServiceImpl.class, BundleLogger.getStaticLogger());

    private static final String DEFAULT_THREAD_STRATEGY = "pool";
    private static final Integer DEFAULT_QUEUE_SIZE = new Integer(32);
    private static final Integer DEFAULT_MIN_POOL_THREADS = new Integer(4);
    private static final Integer DEFAULT_MAX_POOL_THREADS = new Integer(32);
    private static final Long DEFAULT_POOL_THREAD_EXPIRATION = new Long(10000L);

    private final MessageExchangeFactory factory;
    private final BackendExecutor executor;

    // Not thread safe
    private boolean closed;
    private int nextId;
    private int endpointsControl;
    private final Map endpoints;
    private final Map defaultConfiguration;
    private BackendHook hook;

    public BackendServiceImpl(Map configuration) {
        nextId = 0;
        closed = false;
        endpointsControl = 0;
        endpoints = new HashMap();
        factory = new MessageExchangeFactoryImpl();
        defaultConfiguration = configuration;
        if (isThreadExecutor(configuration)) {
            executor = new ThreadPoolExecutor(getQueueSize(configuration), getMinPoolThreads(configuration),
                    getMaxPoolThreads(configuration), getPoolThreadExpiration(configuration));
        } else {
            executor = null;
        }
    }

    private Object checkWithDefault(Map config, String name, Class type, Object defaultValue) {
        Object result = defaultValue;
        Object value = config.get(name);
        if (value == null) {
            if (LOG.isEnabledFor(Level.WARN)) {
                LOG.warn("Configuration property " + name + " is null -> use default value instead " + defaultValue);
            }
        } else if (!type.equals(value.getClass())) {
            LOG.error("Configuration property " + name + " must be of type " + type.toString() + " while it is of type "
                    + value.getClass() + " -> use default value instead " + defaultValue);
        } else {
            result = value;
        }
        return result;
    }

    private boolean isThreadExecutor(Map config) {
        String threadStrategy = (String) checkWithDefault(config, "threadStrategy", String.class, DEFAULT_THREAD_STRATEGY);
        if (!"pool".equals(threadStrategy) && !"distinct".equals(threadStrategy)) {
            LOG.error("Configuration property threadStrategy is not in the range [pool, distinct], use the default value pool");
            threadStrategy = "pool";
        }
        return "pool".equals(threadStrategy);
    }

    private int getQueueSize(Map config) {
        return ((Integer) checkWithDefault(config, "queueSize", Integer.class, DEFAULT_QUEUE_SIZE)).intValue();
    }

    private int getMinPoolThreads(Map config) {
        return ((Integer) checkWithDefault(config, "minPoolThreads", Integer.class, DEFAULT_MIN_POOL_THREADS)).intValue();
    }

    private int getMaxPoolThreads(Map config) {
        return ((Integer) checkWithDefault(config, "maxPoolThreads", Integer.class, DEFAULT_MAX_POOL_THREADS)).intValue();
    }

    private long getPoolThreadExpiration(Map config) {
        String defaultStr = FormatUtils.formatDuration(DEFAULT_POOL_THREAD_EXPIRATION.longValue());
        String duration = (String) checkWithDefault(config, "poolThreadExpiration", String.class, defaultStr);
        try {
            return FormatUtils.parseDuration(duration);
        } catch (ParseException e) {
            LOG.error("Configuration property poolThreadExpiration cannot be decoded as a duration, use default value "
                    + defaultStr, e);
            return DEFAULT_POOL_THREAD_EXPIRATION.longValue();
        }
    }

    private BackendExecutor getExecutor(Integer id, BackendEndpoint endpoint, Map configuration) {
        BackendExecutor executor = null;
        if (configuration == null) {
            if (this.executor != null) {
                executor = this.executor;
            } else {
                executor = new LocalDataExecutor(id, endpoint, getQueueSize(defaultConfiguration));
            }
        } else if (isThreadExecutor(configuration)) {
            executor = new ThreadPoolExecutor(getQueueSize(configuration), getMinPoolThreads(configuration),
                    getMaxPoolThreads(configuration), getPoolThreadExpiration(configuration));
        } else {
            executor = new LocalDataExecutor(id, endpoint, getQueueSize(configuration));
        }
        return executor;
    }

    public void stop() {
        synchronized (this) {
            closed = true;
            endpoints.clear();
            hook = null;
        }
        executor.destroy();
    }

    public synchronized void registerEndpoint(BackendEndpoint endpoint, Map configuration) throws BackendException {
        LOG.info("registerEndpoint");

        if (!closed) {
            if (endpoint != null) {
                endpointsControl = (endpointsControl + 1) % Integer.MAX_VALUE;
                ++nextId;
                Integer id = new Integer(nextId);
                if (!endpoints.containsKey(id)) {
                    DeliveryChannelImpl deliveryChannel = new DeliveryChannelImpl(endpoint, this, factory);
                    EndpointContext context = new EndpointContextImpl(id.intValue(), deliveryChannel);
                    BackendExecutor executor = getExecutor(id, endpoint, configuration);
                    endpoints.put(id, new BackendEndpointContext(endpoint, executor));
                    if (LOG.isDebugEnabled()) {
                        LOG.debug("Endpoint is registered: " + id.intValue());
                    }
                    endpoint.init(context);
                } else if (LOG.isDebugEnabled()) {
                    LOG.debug("Endpoint id is already registered: " + id);
                }
            } else {
                LOG.debug("Unable to register null endpoint");
                throw new BackendException("Unable to register null endpoint");
            }
        } else {
            throw new BackendException("The backend service is already stopped");
        }
    }

    public synchronized void unregisterEndpoint(int ongId) {
        Integer id = new Integer(ongId);
        if (LOG.isInfoEnabled()) {
            LOG.info("unregisterEndpoint: " + id);
        }
        if (endpoints.containsKey(id)) {
            endpointsControl = (endpointsControl + 1) % Integer.MAX_VALUE;
            BackendEndpointContext context = (BackendEndpointContext) endpoints.remove(id);
            if (context != null) {
                BackendExecutor executor = context.getExecutor();
                if (executor != this.executor) {
                    executor.destroy();
                }
            }
        }
    }

    public BackendEndpointContext getEndpoint(int endpoint) {
        int endpointsControl = 0;
        BackendEndpointContext backendEndpoint = null;
        Integer endpointId = new Integer(endpoint);
        while (true) {
            endpointsControl = this.endpointsControl;
            backendEndpoint = (BackendEndpointContext) endpoints.get(endpointId);
            if (endpointsControl == this.endpointsControl) {
                return backendEndpoint;
            }
        }
    }

    public Iterator getEndpoints() {
        int endpointsControl = 0;
        Iterator it = null;
        while (true) {
            endpointsControl = this.endpointsControl;
            it = endpoints.values().iterator();
            if (endpointsControl == this.endpointsControl) {
                return it;
            }
        }
    }

    public BackendExecutor getExecutor() {
        return executor;
    }

    public synchronized void registerHook(BackendHook hook) throws BackendException {
        if (!closed) {
            LOG.info("registerHook");
            if (hook != null) {
                if (this.hook == null) {
                    this.hook = hook;
                } else {
                    LOG.debug("A hook is already registered");
                    throw new BackendException("A hook is already registered");
                }
            } else {
                LOG.debug("Unable to register null hook");
                throw new BackendException("Unable to register null hook");
            }
        } else {
            throw new BackendException("The backend service is already stopped");
        }
    }

    public synchronized void unregisterHook() {
        LOG.info("unregisterHook");
        hook = null;
    }

    public BackendHook getHook() {
        LOG.info("getHook");
        return hook;
    }
}
