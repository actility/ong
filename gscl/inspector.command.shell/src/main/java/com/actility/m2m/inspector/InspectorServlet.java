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
 * id $Id: $
 * author $Author: $
 * version $Revision: $
 * lastrevision $Date: $
 * modifiedby $LastChangedBy: $
 * lastmodified $LastChangedDate: $
 */

package com.actility.m2m.inspector;

import com.actility.m2m.servlet.song.SongFactory;
import com.actility.m2m.servlet.song.SongServlet;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.util.concurrent.SyncCall;

public class InspectorServlet extends SongServlet {

    private SongFactory factory;

    public void init() {
        this.factory = (SongFactory) getServletContext().getAttribute(SONG_FACTORY);
    }

    public SongFactory getSongFactory() {
        return factory;
    }

    public void doSuccessResponse(SongServletResponse response) {
        SyncCall syncCall = (SyncCall) response.getRequest().getAttribute("syncCall");

        if (syncCall != null) {
            syncCall.setObject(response);
            syncCall.resume();
        }
    }

    public void doErrorResponse(SongServletResponse response) {
        SyncCall syncCall = (SyncCall) response.getRequest().getAttribute("syncCall");

        if (syncCall != null) {
            syncCall.setObject(response);
            syncCall.resume();
        }
    }
}
