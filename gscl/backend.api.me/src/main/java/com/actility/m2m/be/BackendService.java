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

import java.util.Iterator;
import java.util.Map;

/**
 * The backend service that allows endpoints to register themselves in order to send asynchronous messages on a bus.
 */
public interface BackendService {

    /**
     * Registers an endpoint on the backend bus.
     *
     * @param endpoint The endpoint to register
     * @param configuration Configuration parameters
     * @throws BackendException If any problem occurs while regitering the endpoint
     */
    void registerEndpoint(BackendEndpoint endpoint, Map configuration) throws BackendException;

    /**
     * Unregisters an endpoint from the backend bus.
     *
     * @param id The endpoint id to unregister
     */
    void unregisterEndpoint(int id);

    /**
     * Gets an iterator on registered endpoints.
     *
     * @return The iterator on registered endpoints
     */
    Iterator getEndpoints();

    /**
     * Gets a register endpoint from its internal id.
     *
     * @param endpointId The internal id of the requested endpoint
     * @return The backend endpoint context or null if it does not exist
     */
    BackendEndpointContext getEndpoint(int endpointId);

    /**
     * Registers a hook to the backend bus. Only one hook at a time can be registered.
     *
     * @param hook The hook to register
     * @throws BackendException If a hook is already registered
     */
    void registerHook(BackendHook hook) throws BackendException;

    /**
     * Unregisters a hook from the backend bus.
     */
    void unregisterHook();

    /**
     * Gets the currently registered hook or null if none is registered.
     *
     * @return The currently registered hook
     */
    BackendHook getHook();

    /**
     * Gets the backend bus {@link BackendExecutor executor}.
     *
     * @return The backend bus executor
     */
    BackendExecutor getExecutor();
}
