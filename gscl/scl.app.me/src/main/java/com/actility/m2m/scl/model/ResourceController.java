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
 * id $Id: ResourceController.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.model;

import java.io.IOException;
import java.io.Serializable;
import java.net.URI;
import java.text.ParseException;
import java.util.List;

import com.actility.m2m.m2m.Indication;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.m2m.M2MSession;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public interface ResourceController {
    void doCreateIndication(SclManager manager, String path, XoObject resource, Indication indication) throws ParseException,
            IOException, StorageException, XoException, M2MException;

    void doRetrieveIndication(SclManager manager, String path, XoObject resource, Indication indication) throws ParseException,
            IOException, StorageException, XoException, M2MException;

    void doUpdateIndication(SclManager manager, String path, XoObject resource, Indication indication) throws ParseException,
            IOException, StorageException, XoException, M2MException;

    void doDeleteIndication(SclManager manager, String path, XoObject resource, Indication indication) throws ParseException,
            IOException, StorageException, XoException, M2MException;

    void doCreateIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException;

    void doRetrieveIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException;

    void doUpdateIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException;

    void doDeleteIndication(SclManager manager, String path, XoObject resource, Indication indication,
            PartialAccessor partialAccessor) throws ParseException, IOException, StorageException, XoException, M2MException;

    void timeout(SclManager manager, String path, String timerId, M2MSession session, Serializable info) throws ParseException,
            IOException, StorageException, XoException, M2MException;

    void sessionExpired(SclManager manager, String path, M2MSession session) throws ParseException, IOException,
            StorageException, XoException, M2MException;

    /**
     * Appends discovery URIs to the list of discovery URIs until limit is reached. In case, the limit is reached the counting
     * process continue to learn how many URIs cannot be added
     *
     * @param logId An ID that will be used to log messages
     * @param resource The resource targeted by the current operation
     * @param requestingEntity The requesting entity for which DISCOVER right must be granted
     * @param targetID The target ID of the discovery request
     * @param appPath The song application root path
     * @param searchStrings The search strings to match in order to add a sub-URI
     * @param discoveryURIs The current list of discovery URIs
     * @param remainingURIs The remaining number of URIs which can be added (it can be negative in which case no URI must be
     *            added)
     * @return The remaining number of URIs minus the number of matching URIs (URIs must be counted even if there are not
     *         added). So, if the number is negative it means that this negative number of URIs were not added
     */
    int appendDiscoveryURIs(String logId, SclManager manager, String path, XoObject resource, URI requestingEntity,
            URI targetId, String appPath, String[] searchStrings, List discoveryURIs, int remainingURIs)
            throws IOException, StorageException, XoException;
}
