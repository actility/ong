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
import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.startlevel.StartLevel;

public class BundleLevelCommandImpl implements Command
{
    private static final String INITIAL_LEVEL_SWITCH = "-i";

    private BundleContext m_context = null;

    public BundleLevelCommandImpl(BundleContext context)
    {
        m_context = context;
    }

    public String getName()
    {
        return "bundlelevel";
    }

    public String getUsage()
    {
        return "bundlelevel <level> <id> ... | <id> | -i <level>";
    }

    public String getShortDescription()
    {
        return "set/get bundle start level or set initial bundle start level.";
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

        // If there is only one token, then assume it is
        // a bundle ID for which we must retrieve the bundle
        // level.
        if (st.countTokens() == 1)
        {
            // Get the bundle and display start level.
            Bundle bundle = null;
            String token = null;
            try
            {
                token = st.nextToken();
                long id = Long.parseLong(token);
                bundle = m_context.getBundle(id);
                if (bundle != null)
                {
                    out.println("Bundle " + token + " is level "
                        + sl.getBundleStartLevel(bundle));
                }
                else
                {
                    err.println("Bundle ID " + token + " is invalid.");
                }
            }
            catch (NumberFormatException ex)
            {
                err.println("Unable to parse integer '" + token + "'.");
            }
            catch (Exception ex)
            {
                err.println(ex.toString());
            }
        }
        // If there is more than one token, assume the first
        // token is the new start level and the remaining
        // tokens are the bundle IDs whose start levels should
        // be changed.
        else if (st.countTokens() > 1)
        {
            // Get the bundle.
            Bundle bundle = null;
            String token = null;
            int startLevel = -1;

            token = st.nextToken().trim();

            // If next token is the initial level switch,
            // then set it.
            if (token.equals(INITIAL_LEVEL_SWITCH))
            {
                if (st.countTokens() == 1)
                {
                    try
                    {
                        token = st.nextToken().trim();
                        startLevel = Integer.parseInt(token);
                        sl.setInitialBundleStartLevel(startLevel);
                    }
                    catch (NumberFormatException ex)
                    {
                        err.println("Unable to parse start level '" + token + "'.");
                    }
                }
                else
                {
                    err.println("Incorrect number of arguments.");
                }
            }
            // Otherwise, set the start level for the specified bundles.
            else
            {
                try
                {
                    startLevel = Integer.parseInt(token);
                }
                catch (NumberFormatException ex)
                {
                    err.println("Unable to parse start level '" + token + "'.");
                }

                // Ignore invalid start levels.
                if (startLevel > 0)
                {
                    // Set the start level for each specified bundle.
                    while (st.hasMoreTokens())
                    {
                        try
                        {
                            token = st.nextToken();
                            long id = Long.parseLong(token);
                            bundle = m_context.getBundle(id);
                            if (bundle != null)
                            {
                                sl.setBundleStartLevel(bundle, startLevel);
                            }
                            else
                            {
                                err.println("Bundle ID '" + token + "' is invalid.");
                            }
                        }
                        catch (NumberFormatException ex)
                        {
                            err.println("Unable to parse bundle ID '" + token + "'.");
                        }
                        catch (Exception ex)
                        {
                            err.println(ex.toString());
                        }
                    }
                }
                else
                {
                    err.println("Invalid start level.");
                }
            }
        }
        else
        {
            err.println("Incorrect number of arguments.");
        }
    }
}