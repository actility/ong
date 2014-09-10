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
 * id $Id: DeliveryChannel.java 6058 2013-10-14 12:01:59Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6058 $
 * lastrevision $Date: 2013-10-14 14:01:59 +0200 (Mon, 14 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-14 14:01:59 +0200 (Mon, 14 Oct 2013) $
 */

package com.actility.m2m.be.messaging;

/**
 * Bi-directional communication channel used by endpoints to interact with the Backend. Each BackendEndpoint has one
 * DeliveryChannel associated with it. Using the delivery channel, an endpoint may:
 * <ul>
 * <li>Send a message asynchronously (that is, without waiting for a response or status change acknowledging that the exchange
 * was received).</li>
 * <li>Send a message synchronously, blocking the thread of execution until the result is received. Optionally, this can be done
 * with a time-out.</li>
 * </ul>
 */
public interface DeliveryChannel {
    /**
     * Critical (highest) priority for an exchange.
     */
    int CRITICAL = 0;
    /**
     * Important (medium) priority for an exchange.
     */
    int IMPORTANT = 1;
    /**
     * Normal (lowest) priority for an exchange.
     */
    int NORMAL = 2;

    /**
     * Close the delivery channel, halting all message traffic
     */
    void close();

    /**
     * Creates a message exchange factory. This factory will create exchange instances with all appropriate properties set to
     * <code>null</code>
     *
     * @return a message exchange factory
     */
    MessageExchangeFactory createExchangeFactory();

    /**
     * Creates a message exchange factory. This factory will create exchange instances with all appropriate properties set to
     * <code>null</code>
     *
     * @param endpointId identifier of the endpoint for which all exchanges created by the returned factory will be set for
     * @return an exchange factory that will create exchanges for the given endpoint identifier
     */
    MessageExchangeFactory createExchangeFactory(int endpointId);

    /**
     * Routes a MessageExchange instance through the Backend to the appropriate endpoint. This method supports concurrent
     * invocation for multi-threaded environments. This is used not only to send the initial message in an exchange, but also
     * for the service to "return" the exchange with the appropriate response (response, fault, or <code>ExchangeStatus</code>).
     * In more complex message exchange patterns, a single MessageExchange can be sent back-and-forth via <code>send()</code>
     * several times, but always terminating with the MessageExchange instance being sent with a terminal
     * <code>ExchangeStatus</code>.
     *
     * @param exchange message exchange to send
     * @throws MessagingException unable to send exchange
     */
    void send(MessageExchange exchange) throws MessagingException;

    /**
     * Routes a MessageExchange instance through the Backend to the appropriate endpoint. This method supports concurrent
     * invocation for multi-threaded environments. This is used not only to send the initial message in an exchange, but also
     * for the service to "return" the exchange with the appropriate response (response, fault, or <code>ExchangeStatus</code>).
     * In more complex message exchange patterns, a single MessageExchange can be sent back-and-forth via <code>send()</code>
     * several times, but always terminating with the MessageExchange instance being sent with a terminal
     * <code>ExchangeStatus</code>.
     *
     * @param exchange message exchange to send
     * @param priority the exchange priority
     * @throws MessagingException unable to send exchange
     */
    void send(MessageExchange exchange, int priority) throws MessagingException;

    /**
     * Routes a MessageExchange instance through the Backend to the appropriate endpoint, and blocks until the exchange is
     * returned (that is, the other participant sends the message exchange back). This method supports concurrent invocation for
     * multi-threaded environments. If the thread making this call is interrupted, the message exchange is treated in the same
     * fashion as a timed- out <code>sendSync</code> call. Note that the "returned" message exchange (the instance returned by
     * the other participant in the exchange) is the same instance referred to by the parameter <code>exchange</code>.
     *
     * @param exchange message exchange to send
     * @throws MessagingException unable to send exchange, or no response is expected from the <code>send()</code>
     * @throws MessagingException when the blocked thread is interrupted
     * @return <code>true</code> if the exchange has been returned
     */
    boolean sendSync(MessageExchange exchange) throws MessagingException;

    /**
     * Routes a MessageExchange instance through the Backend to the appropriate servicing component, and blocks until the
     * exchange is returned (that is, the other participant sends the message exchange back), or the specified timeout interval
     * elapses. This method supports concurrent invocation for multi-threaded environments. If the thread making this call is
     * interrupted while this method is blocked, the message exchange is treated as if the time out occurred. If this method
     * returns false (indicating that timeout occurred). Attempts by the service provider to send a MessageExchange in such an
     * ERROR state must result in the send (or sendSync) method called ending abruptly with an appropriate MessagingException.
     * Note that the “returned” message exchange (the instance returned by the other participant in the exchange) is the same
     * instance referred to by the parameter <code>exchange</code>.
     *
     * @param exchange message exchange to send
     * @param timeoutMS timeout period, in milliseconds (0 means no timeout)
     * @return <code>true</code> if the exchange has been returned, or <code>false</code> if the method timed out while waiting
     * @throws IllegalArgumentException if <code>timeoutMS</code> < 0
     * @throws MessagingException unable to send exchange, or no response is expected from the <code>send()</code> operation
     *             (i.e., the <code>MessageExchange</code> is being used to convey an <code>ExchangeStatus</code>)
     * @throws MessagingException when the blocked thread is interrupted
     */
    boolean sendSync(MessageExchange exchange, long timeoutMS) throws MessagingException;
}
