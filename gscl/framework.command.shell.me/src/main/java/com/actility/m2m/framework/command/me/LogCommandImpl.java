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
import java.util.Enumeration;
import java.util.Iterator;
import java.util.LinkedList;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceReference;
import org.osgi.service.log.LogEntry;
import org.osgi.service.log.LogReaderService;

/**
 * Apache Felix Shell command to display recent log entries
 */
public class LogCommandImpl implements Command
{
    private final BundleContext m_bundleContext;

    public LogCommandImpl(BundleContext context)
    {
        m_bundleContext = context;
    }

    public void execute(String line, PrintStream out, PrintStream err)
    {
        LogOptions options = new LogOptions(line);

        ServiceReference ref =
            m_bundleContext.getServiceReference(LogReaderService.class.getName());
        if (ref != null)
        {
            LogReaderService service = (LogReaderService) m_bundleContext.getService(ref);
            Enumeration entries = service.getLog();

            LinkedList logs = new LinkedList();
            int index = 0;
            while (entries.hasMoreElements()
                && (options.getMaxNumberOfLogs() < 0 || index < options.getMaxNumberOfLogs()))
            {
                LogEntry entry = (LogEntry) entries.nextElement();
                if (entry.getLevel() <= options.getMinLevel())
                {
                    logs.addFirst(entry);
                    index++;
                }
            }
            Iterator it = logs.iterator();
            while (it.hasNext()) {
                display((LogEntry) it.next(), out);
            }
        }
        else
        {
            out.println("No LogReaderService available");
        }
    }

    private void display(LogEntry entry, PrintStream out)
    {
        out.println(entry.toString());
    }

    public String getName()
    {
        return "log";
    }

    public String getShortDescription()
    {
        return "list the most recent log entries.";
    }

    public String getUsage()
    {
        return "log [<max log entries>] [error | warn | info | debug]";
    }
}