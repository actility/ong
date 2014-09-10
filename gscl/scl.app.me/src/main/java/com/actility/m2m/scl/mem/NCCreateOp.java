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
 * id $Id: NCCreateOp.java 3302 2012-10-10 14:19:16Z JReich $
 * author $Author: JReich $
 * version $Revision: 3302 $
 * lastrevision $Date: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-10 16:19:16 +0200 (Wed, 10 Oct 2012) $
 */

package com.actility.m2m.scl.mem;

import java.net.URI;
import java.util.Collection;

import com.actility.m2m.m2m.M2MConstants;
import com.actility.m2m.m2m.M2MException;
import com.actility.m2m.scl.model.SclManager;
import com.actility.m2m.scl.model.SclTransaction;
import com.actility.m2m.scl.model.TransientOp;
import com.actility.m2m.xo.XoException;
import com.actility.m2m.xo.XoObject;

public final class NCCreateOp implements TransientOp {

    private final SclManager manager;
    private final String path;
    private final XoObject resource;
    private final URI targetID;
    private final Collection searchAttributes;
    private final SclTransaction transaction;
    private URI contactUri;
    private URI longPollUri;

    public NCCreateOp(SclManager manager, String path, XoObject resource, URI targetID, Collection searchAttributes,
            SclTransaction transaction) {
        this.manager = manager;
        this.path = path;
        this.resource = resource;
        this.targetID = targetID;
        this.searchAttributes = searchAttributes;
        this.transaction = transaction;
    }

    public void prepare() throws XoException, M2MException {
        // Create long poll server connection
        URI[] uris = manager.getM2MContext().createServerLongPoll(targetID);
        contactUri = uris[0];
        longPollUri = uris[1];

        resource.setStringAttribute(M2MConstants.TAG_M2M_CONTACT_U_R_I, contactUri.toString());
        XoObject channelData = manager.getXoService().newXmlXoObject(M2MConstants.TYPE_M2M_LONG_POLLING_CHANNEL_DATA);
        channelData.setName(M2MConstants.TAG_M2M_CHANNEL_DATA);
        resource.setXoObjectAttribute(M2MConstants.TAG_M2M_CHANNEL_DATA, channelData);
        channelData.setStringAttribute(M2MConstants.ATTR_XSI_TYPE, M2MConstants.TYPE_M2M_LONG_POLLING_CHANNEL_DATA);
        channelData.setStringAttribute(M2MConstants.TAG_M2M_LONG_POLLING_U_R_I, longPollUri.toString());

        // Save resource
        transaction.createResource(path, resource, searchAttributes);
    }

    public void rollback() {
        if (contactUri != null && longPollUri != null) {
            manager.getM2MContext().deleteServerLongPoll(contactUri, longPollUri);
        }
    }

    public void postCommit() {
        // Nothing to do
    }

}
