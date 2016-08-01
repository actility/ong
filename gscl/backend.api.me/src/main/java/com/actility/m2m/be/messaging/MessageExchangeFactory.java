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

package com.actility.m2m.be.messaging;

/**
 * A message exchange factory is used used to create new instances of MessageExchange. Service consumers use these factories to
 * create message exchanges when initiating a new service request. There are a variety of ways to creating such factories, each
 * of which creates a context that is used to provide some of the default properties of MessageExchange instances created by the
 * factory instances. For example, a factory can be created for a particular endpoint, ensuring that all exchanges created by
 * the factory have that endpoint set as the default endpoint property of the exchange. This allows components to retain
 * factories as a way of aligning internal processing context with the context contained in the factory, ensuring that the
 * exchanges created consistently reflect that context.
 */
public interface MessageExchangeFactory {

    /**
     * Creates a new MessageExchange instance used to initiate a service invocation. JBI defines a set of eight fundamental
     * message exchange types which are created using binding and engine delivery channels. This base method is provided for
     * extensibility, to satisfy the need for vendor-specific message exchange patterns. The registration and administration of
     * these patterns is not addressed by JBI.
     *
     * @param pattern message exchange pattern
     * @return new message exchange
     * @throws MessagingException specified pattern is not registered to a message exchange type
     */
    MessageExchange createExchange(String pattern) throws MessagingException;

    /**
     * Creates a new MessageExchange instance used to initiate a service invocation. JBI defines a set of eight fundamental
     * message exchange types which are created using binding and engine delivery channels. This base method is provided for
     * extensibility, to satisfy the need for vendor-specific message exchange patterns. The registration and administration of
     * these patterns is not addressed by JBI.
     *
     * @param id The ID associated to the exchange
     * @param pattern message exchange pattern
     * @return new message exchange
     * @throws MessagingException specified pattern is not registered to a message exchange type
     */
    MessageExchange createExchange(String id, String pattern) throws MessagingException;

    /**
     * Convenience method that creates an In-Only message exchange.
     *
     * @return new In-Only message exchange
     * @throws MessagingException failed to create exchange
     */
    InOnly createInOnlyExchange() throws MessagingException;

    /**
     * Convenience method that creates an In-Only message exchange.
     *
     * @param id The ID associated to the exchange
     * @return new In-Only message exchange
     * @throws MessagingException failed to create exchange
     */
    InOnly createInOnlyExchange(String id) throws MessagingException;

    /**
     * Convenience method that creates an In-Out message exchange.
     *
     * @return new In-Out message exchange
     * @throws MessagingException failed to create exchange
     */
    InOut createInOutExchange() throws MessagingException;

    /**
     * Convenience method that creates an In-Out message exchange.
     *
     * @param id The ID associated to the exchange
     * @return new In-Out message exchange
     * @throws MessagingException failed to create exchange
     */
    InOut createInOutExchange(String id) throws MessagingException;
}
