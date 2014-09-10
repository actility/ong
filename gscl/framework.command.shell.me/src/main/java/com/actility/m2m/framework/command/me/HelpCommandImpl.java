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
import java.util.Arrays;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;
import org.apache.felix.shell.ShellService;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;

public class HelpCommandImpl implements Command
{
    private BundleContext m_context = null;

    public HelpCommandImpl(BundleContext context)
    {
        m_context = context;
    }

    public String getName()
    {
        return "help";
    }

    public String getUsage()
    {
        return "help [<command> ...]";
    }

    public String getShortDescription()
    {
        return "display available command usage and description.";
    }

    public void execute(String s, PrintStream out, PrintStream err)
    {
        try {
            // Get a reference to the impl service.
            ServiceReference ref = m_context.getServiceReference(
                org.apache.felix.shell.ShellService.class.getName());

            if (ref != null)
            {
                ShellService ss = (ShellService) m_context.getService(ref);

                // Parse command line.
                StringTokenizer st = new StringTokenizer(s, " ");

                // Ignore the command name.
                st.nextToken();

                if (!st.hasMoreTokens())
                {
                    String[] cmds = ss.getCommands();
                    Arrays.sort(cmds);
                    for (int i = 0; i < cmds.length; i++)
                    {
                        out.println(cmds[i]);
                    }
                    out.println("\nUse 'help <command-name>' for more information.");
                }
                else
                {
                    String[] cmds = ss.getCommands();
                    String[] targets = new String[st.countTokens()];
                    for (int i = 0; i < targets.length; i++)
                    {
                        targets[i] = st.nextToken().trim();
                    }
                    boolean found = false;
                    for (int cmdIdx = 0; (cmdIdx < cmds.length); cmdIdx++)
                    {
                        for (int targetIdx = 0; targetIdx < targets.length; targetIdx++)
                        {
                            if (cmds[cmdIdx].equals(targets[targetIdx]))
                            {
                                if (found)
                                {
                                    out.println("---");
                                }
                                found = true;
                                out.println("Command     : "
                                    + cmds[cmdIdx]);
                                out.println("Usage       : "
                                    + ss.getCommandUsage(cmds[cmdIdx]));
                                out.println("Description : "
                                    + ss.getCommandDescription(cmds[cmdIdx]));
                            }
                        }
                    }
                }
            }
            else
            {
                err.println("No ShellService is unavailable.");
            }
        } catch (Exception ex) {
            err.println(ex.toString());
        }
    }
}