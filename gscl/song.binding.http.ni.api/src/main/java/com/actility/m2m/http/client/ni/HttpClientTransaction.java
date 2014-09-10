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
 * id $Id: HttpClientTransaction.java 5992 2013-10-07 14:32:35Z JReich $
 * author $Author: JReich $
 * version $Revision: 5992 $
 * lastrevision $Date: 2013-10-07 16:32:35 +0200 (Mon, 07 Oct 2013) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2013-10-07 16:32:35 +0200 (Mon, 07 Oct 2013) $
 */
package com.actility.m2m.http.client.ni;

import java.util.Iterator;

/**
 * Represents an HTTP client transaction.
 * <p>
 * It allows to set HTTP client requests parameters and to send it.
 * <p>
 * It contains HTTP client response parameters when it is received.
 */
public interface HttpClientTransaction {

	/**
	 * Release the transaction and the associated resources.
	 */
	void release();

	/**
	 * Adds an HTTP client request header
	 * 
	 * @param name
	 *            The header name
	 * @param value
	 *            The header value
	 * @throws HttpClientException
	 *             If any problem occurs while setting the header
	 */
	void addRequestHeader(String name, String value) throws HttpClientException;

	/**
	 * Sets the HTTP client request body as a whole
	 * 
	 * @param content
	 *            The HTTP client request body
	 * @throws HttpClientException
	 *             If any problem occurs while setting the body
	 */
	void setRequestBody(byte[] content) throws HttpClientException;

	/**
	 * Sets the HTTP client request proxy if any
	 * 
	 * @param host
	 *            The proxy host
	 * @param port
	 *            The proxy port
	 * @throws HttpClientException
	 *             If any problem occurs while setting the proxy
	 */
	void setRequestProxy(String host, int port) throws HttpClientException;

	/**
	 * Sends the HTTP client request
	 * <p>
	 * After that, it is not allowed to add/change the HTTP client headers,
	 * content or proxy
	 * 
	 * @throws HttpClientException
	 *             If any problem occurs while sending the HTTP client request
	 */
	void sendRequest() throws HttpClientException;

	/**
	 * Gets the HTTP response status code when received.
	 * 
	 * @return The HTTP response status code
	 */
	int getResponseStatusCode();

	/**
	 * Gets the HTTP response status text when received.
	 * 
	 * @return The HTTP response status text
	 */
	String getResponseStatusText();

	/**
	 * Gets the HTTP response header value when received given its name
	 * <p>
	 * This returns the first header in case this is a multi-value header
	 * 
	 * @param name
	 *            The header name to retrieve
	 * @return The HTTP response header value
	 */
	String getResponseHeader(String name);

	/**
	 * Gets an iterator on all headers (header name + header value)
	 * 
	 * @return The iterator on all headers
	 */
	Iterator/* <Entry<String, String>> */getResponseHeaders();

	/**
	 * Gets the HTTP response content length
	 * 
	 * @return The HTTP response content length
	 */
	int getResponseContentLength();

	/**
	 * Gets the HTTP response body as a whole
	 * 
	 * @return The HTTP response body
	 */
	byte[] getResponseBody();

	/**
	 * Gets the remote connection IP address of this transaction
	 * 
	 * @return The remote connection IP address of this transaction
	 */
	String getRemoteAddress();

	/**
	 * Gets the remote connection port of this transaction
	 * 
	 * @return The remote connection port of this transaction
	 */
	int getRemotePort();

	/**
	 * Gets the local connection IP address of this transaction
	 * 
	 * @return The local connection IP address of this transaction
	 */
	String getLocalAddress();

	/**
	 * Gets the local connection port of this transaction
	 * 
	 * @return The local connection port of this transaction
	 */
	int getLocalPort();

	/**
	 * Gets the HTTP transaction protocol
	 * <p>
	 * For example "HTTP/1.0" or "HTTP/1.1"
	 * 
	 * @return The HTTP transaction protocol
	 */
	String getProtocol();

	/**
	 * Gets the HTTP transaction ID if any was given at the transaction creation
	 * 
	 * @return The HTTP transaction ID or null
	 */
	String getId();

}
