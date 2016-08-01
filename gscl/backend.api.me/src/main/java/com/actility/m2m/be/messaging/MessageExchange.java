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
 * MessageExchange represents a container for Backend messages which is described by an exchange pattern. The exchange pattern
 * defines the names, sequence, and cardinality of messages in an exchange. When a consumer initiates a message exchange (that
 * is, it creates a new exchange instance and initializes it), it has a choice of how to address the exchange, to indicate the
 * service desired. The initiating consumer must address the new exchange by its identifier. The Backend will always use the
 * most specific address type given, ignoring other, less specific ones, if any. Note that whenever the Backend chooses the
 * provider endpoint, it must set the endpoint property of the message exchange instance to the endpoint it selected. Note also
 * that setting the different address properties of an exchange instance does not cause side effects. For example, setting the
 * service endpoint will not affect the previously set interface name or service name.
 */
public interface MessageExchange {

    /**
     * Returns the URI of the pattern for this exchange.
     * 
     * @return pattern URI for this exchange
     */
    String getPattern();

    /**
     * Returns the unique identifier assigned by the NMS for this exchange.
     * 
     * @return unique id for this exchange
     */
    String getExchangeId();

    /**
     * Retrieves a ONG message based on the specified message reference.
     * 
     * @param name message reference
     * @return message with the specified reference name
     */
    BackendMessage getMessage(String name);

    /**
     * Sets a normalized message with the specified message reference. The pattern governing this exchange must contain a
     * definition for the reference name supplied.
     * 
     * @param message ONG message
     * @param name message reference
     * @throws MessagingException operation not permitted in the current exchange state
     */
    void setMessage(BackendMessage message, String name) throws MessagingException;

    /**
     * Creates a normalized message based on the specified message reference. The pattern governing this exchange must contain a
     * definition for the reference name supplied.
     * 
     * @return a new normalized message
     * @throws MessagingException failed to create message
     */
    BackendMessage createMessage() throws MessagingException;

    /**
     * Retrieves the specified property from the exchange.
     * 
     * @param name property name
     * @return property value
     */
    Object getProperty(String name);

    /**
     * Specifies a property for the exchange.
     * 
     * @param name property name
     * @param obj property value
     */
    void setProperty(String name, Object obj);

    /**
     * Specifies the endpoint used by this exchange.
     * 
     * @param endpoint endpoint address
     */
    void setSource(int endpoint);

    /**
     * Retrieves the endpoint used by this exchange.
     * 
     * @return endpoint address for this message exchange
     */
    int getSource();

    /**
     * Specifies the endpoint used by this exchange.
     * 
     * @param endpoint endpoint address
     */
    void setDestination(int endpoint);

    /**
     * Retrieves the endpoint used by this exchange.
     * 
     * @return endpoint address for this message exchange
     */
    int getDestination();

    /**
     * Returns the name of all properties for this exchange.
     * 
     * @return a set of all the property names, as Strings.
     */
    java.util.Set getPropertyNames();
}
