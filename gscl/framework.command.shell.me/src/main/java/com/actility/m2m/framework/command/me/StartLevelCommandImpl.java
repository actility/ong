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

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.startlevel.StartLevel;

public class StartLevelCommandImpl implements Command
{
    private BundleContext m_context = null;

    public StartLevelCommandImpl(BundleContext context)
    {
        m_context = context;
    }

    public String getName()
    {
        return "startlevel";
    }

    public String getUsage()
    {
        return "startlevel [<level>]";
    }

    public String getShortDescription()
    {
        return "get or set framework start level.";
    }

    public void execute(String s, PrintStream out, PrintStream err)
    {
        // Get start level service.
        ServiceReference ref = m_context.getServiceReference(
            org.osgi.service.startlevel.StartLevel.class.getName());
        if (ref == null)
        {
            out.println("StartLevel service is unavailable.");
            return;
        }

        StartLevel sl = (StartLevel) m_context.getService(ref);
        if (sl == null)
        {
            out.println("StartLevel service is unavailable.");
            return;
        }

        // Parse command line.
        StringTokenizer st = new StringTokenizer(s, " ");

        // Ignore the command name.
        st.nextToken();

        if (st.countTokens() == 0)
        {
            out.println("Level " + sl.getStartLevel());
        }
        else if (st.countTokens() >= 1)
        {
            String levelStr = st.nextToken().trim();

            try {
                int level = Integer.parseInt(levelStr);
                sl.setStartLevel(level);
            } catch (NumberFormatException ex) {
                err.println("Unable to parse integer '" + levelStr + "'.");
            } catch (Exception ex) {
                err.println(ex.toString());
            }
        }
    }
}