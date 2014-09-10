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
 * id $Id: M2MContext.java 7982 2014-03-07 14:49:04Z JReich $
 * author $Author: JReich $
 * version $Revision: 7982 $
 * lastrevision $Date: 2014-03-07 15:49:04 +0100 (Fri, 07 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-07 15:49:04 +0100 (Fri, 07 Mar 2014) $
 */

package com.actility.m2m.m2m;

import java.io.Serializable;
import java.net.URI;

public interface M2MContext {
    String M2M_CREATE = "CREATE";
    String M2M_RETRIEVE = "RETRIEVE";
    String M2M_UPDATE = "UPDATE";
    String M2M_DELETE = "DELETE";

    String getApplicationName();

    String getApplicationPath();

    Object getAttribute(String name);

    void setAttribute(String name, Object value);

    void removeAttribute(String name);

    String startTimer(long timeout, Serializable info);

    void cancelTimer(String timerId);

    boolean canBeServer(URI reference) throws M2MException;

    URI[] createServerLongPoll(URI reference) throws M2MException;

    void createServerLongPoll(URI contactUri, URI longPollUri) throws M2MException;

    void deleteServerLongPoll(URI contactUri, URI longpollUri);

    void createClientLongPoll(URI contactUri, URI longPollUri) throws M2MException;

    void deleteClientLongPoll(URI contactUri, URI longpollUri);

    M2MSession createSession(int timeout);

    URI createLocalUri(URI reference, String path) throws M2MException;

    Request createRequest(String method, URI requestingEntity, URI targetID);

    int[] getBacktrackableErrorCodes();

    void setBacktrackableErrorCodes(int[] backtrackableErrors);

    M2MUtils getM2MUtils();
}
