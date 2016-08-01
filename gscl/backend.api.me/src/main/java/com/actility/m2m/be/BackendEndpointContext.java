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

/**
 * Allows to gather the BackendEndpoint with its corresponding BackendExecutor
 */
public final class BackendEndpointContext {
    private final BackendEndpoint endpoint;
    private final BackendExecutor executor;

    /**
     * Builds a BackendContext from a BackendEnpoint and BackendExecutor
     *
     * @param endpoint The BackendEndpoint for which this context is built
     * @param executor The BackendExecutor that will execute messages for this BackendEndpoint
     */
    public BackendEndpointContext(BackendEndpoint endpoint, BackendExecutor executor) {
        this.endpoint = endpoint;
        this.executor = executor;
    }

    /**
     * The BackendEndpoint of this context
     *
     * @return The BackendEndpoint of this context
     */
    public BackendEndpoint getEndpoint() {
        return endpoint;
    }

    /**
     * The BackendExecutor that will execute messages for this BackendEndpoint
     *
     * @return The BackendExecutor that will execute messages for this BackendEndpoint
     */
    public BackendExecutor getExecutor() {
        return executor;
    }
}
