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

package com.actility.m2m.framework.command.me;

import java.io.PrintStream;
import java.util.StringTokenizer;

import org.apache.felix.shell.CdCommand;
import org.osgi.framework.BundleContext;

import com.actility.m2m.framework.resources.ResourcesAccessorService;

public class CdCommandImpl implements CdCommand {
    private BundleContext m_context = null;
    private String m_baseURL = "";

    public CdCommandImpl(ResourcesAccessorService resourcesAccessorService, BundleContext context) {
        m_context = context;

        // See if the initial base URL is specified.
        String baseURL = resourcesAccessorService.getProperty(context, BASE_URL_PROPERTY);
        setBaseURL(baseURL);
    }

    public String getName() {
        return "cd";
    }

    public String getUsage() {
        return "cd [<base-URL>]";
    }

    public String getShortDescription() {
        return "change or display base URL.";
    }

    public void execute(String s, PrintStream out, PrintStream err) {
        StringTokenizer st = new StringTokenizer(s, " ");

        // Ignore the command name.
        st.nextToken();

        // No more tokens means to display the base URL,
        // otherwise set the base URL.
        if (st.countTokens() == 0) {
            out.println(m_baseURL);
        } else if (st.countTokens() == 1) {
            setBaseURL(st.nextToken());
        } else {
            err.println("Incorrect number of arguments");
        }
    }

    public String getBaseURL() {
        return m_baseURL;
    }

    public void setBaseURL(String s) {
        m_baseURL = (s == null) ? "" : s;
    }
}