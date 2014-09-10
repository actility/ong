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
import java.util.ArrayList;
import java.util.List;
import java.util.StringTokenizer;

import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.framework.Constants;
import org.osgi.framework.ServiceReference;
import org.osgi.service.startlevel.StartLevel;

/**
 * Shell command to display a list of bundles whose
 * Bundle-Name or Bundle-Symbolic-Name contains
 * a specified string
 *
 */
public class FindCommandImpl extends PsCommandImpl
{
    public FindCommandImpl(BundleContext context)
    {
        super(context);
    }

    public void execute(String line, PrintStream out, PrintStream err)
    {
        StringTokenizer st = new StringTokenizer(line);
        if (st.countTokens() < 2)
        {
            out.println("Please specify a bundle name");
            return;
        }

        // Get start level service.
        ServiceReference ref = m_context.getServiceReference(
            org.osgi.service.startlevel.StartLevel.class.getName());
        StartLevel sl = null;
        if (ref != null)
        {
            sl = (StartLevel) m_context.getService(ref);
        }

        if (sl == null)
        {
            out.println("StartLevel service is unavailable.");
        }

        st.nextToken();
        String pattern = st.nextToken();

        Bundle[] bundles = m_context.getBundles();

        List found = new ArrayList();

        for (int i = 0; i < bundles.length; i++)
        {
            Bundle bundle = bundles[i];
            String name = (String) bundle.getHeaders().get(Constants.BUNDLE_NAME);
            if (match(bundle.getSymbolicName(), pattern) || match(name, pattern))
            {
                found.add(bundle);
            }
        }

        if (found.size() > 0)
        {
            printBundleList((Bundle[]) found.toArray(new Bundle[found.size()]), sl, out, false, false, false);
        }
        else
        {
            out.println("No matching bundles found");
        }

    }

    private boolean match(String name, String pattern)
    {
        return name != null && (name.toLowerCase().indexOf(pattern.toLowerCase()) != -1);
    }

    public String getName()
    {
        return "find";
    }

    public String getShortDescription()
    {
        return "find bundles by name.";
    }

    public String getUsage()
    {
        return "find <bundle name>";
    }
}