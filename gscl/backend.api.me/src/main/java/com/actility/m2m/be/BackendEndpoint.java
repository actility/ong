/*******************************************************************************
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
 *******************************************************************************/

package com.actility.m2m.be;

import com.actility.m2m.be.messaging.InOnly;
import com.actility.m2m.be.messaging.InOut;

/**
 * Represents an Endpoint on the backend bus.
 * <p>
 * Only registered Endpoint are allowed to send and received exchanges through the bus
 * 
 */
public interface BackendEndpoint {

    /**
     * Gets the {@link EndpointContext} associated to the Endpoint.
     * 
     * @return The endpoint context
     */
    EndpointContext getContext();

    /**
     * Callback used by the backend service to initialize a succesfully registered Endpoint.
     * 
     * @param context The endpoint context that must be returned afterwards by {@link #getContext()}
     */
    void init(EndpointContext context);

    /**
     * Callback used by the backend service to notify an Endpoint of an {@link InOut} exchange
     * 
     * @param exchange The InOut exchange for the current Endpoint
     */
    void process(InOut exchange);

    /**
     * Callback used by the backend service to notify an Endpoint of an {@link InOnly} exchange
     * 
     * @param exchange The InOnly exchange for the current Endpoint
     */
    void process(InOnly exchange);
}
