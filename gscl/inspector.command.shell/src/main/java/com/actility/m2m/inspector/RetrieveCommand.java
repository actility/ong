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

import java.io.IOException;
import java.io.PrintStream;
import java.net.URISyntaxException;
import java.util.Iterator;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;

import com.actility.m2m.servlet.song.SongFactory;
import com.actility.m2m.servlet.song.SongServletRequest;
import com.actility.m2m.servlet.song.SongServletResponse;
import com.actility.m2m.servlet.song.SongURI;
import com.actility.m2m.util.concurrent.SyncCall;

public class RetrieveCommand implements Command {

    private final SongFactory factory;
    private final SongURI defaultRequestingEntity;

    public RetrieveCommand(SongFactory factory) throws URISyntaxException {
        this.factory = factory;
        this.defaultRequestingEntity = factory.createURI("/m2m");
    }

    public String getName() {
        return "m2m-retrieve";
    }

    public String getUsage() {
        return "m2m-retrieve { -re requestingEntity} targetId";
    }

    public String getShortDescription() {
        return "Retrieves an M2M resource on the given targetId";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        try {
            SyncCall syncCall = new SyncCall();
            SongURI requestingEntity = defaultRequestingEntity;
            SongURI targetID = null;

            // Parse command line.
            StringTokenizer st = new StringTokenizer(line, " ");

            // Ignore the command name.
            st.nextToken();

            // Check for optional argument.
            String tmp = st.nextToken();

            if ("-re".equals(tmp)) {
                // Requesting entity
                requestingEntity = factory.createURI(st.nextToken());
            } else {
                targetID = factory.createURI(tmp);
            }

            SongServletRequest request = factory.createRequest(SongServletRequest.MD_RETRIEVE, requestingEntity, targetID);
            request.setAttribute("syncCall", syncCall);

            synchronized (syncCall) {
                request.send();
                syncCall.suspend(32000L);
            }
            SongServletResponse response = (SongServletResponse) syncCall.getObject();
            out.println(response.getStatus() + " " + response.getReasonPhrase());
            Iterator headers = response.getHeaderNames();
            String headerName = null;
            while (headers.hasNext()) {
                headerName = (String) headers.next();
                out.println(headerName + ": " + response.getHeader(headerName));
            }
            out.println();

            out.print(new String(response.getRawContent(), response.getCharacterEncoding()));
        } catch (URISyntaxException e) {
            err.println("Cannot perform operation because of following error: " + e.toString());
            e.printStackTrace();
        } catch (IOException e) {
            err.println("Cannot perform operation because of following error: " + e.toString());
            e.printStackTrace();
        } catch (InterruptedException e) {
            err.println("Cannot perform operation because of following error: " + e.toString());
            e.printStackTrace();
        }
    }
}
