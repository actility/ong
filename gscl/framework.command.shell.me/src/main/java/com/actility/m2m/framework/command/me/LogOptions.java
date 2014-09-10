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

import java.util.StringTokenizer;

/**
 * Parse and encapsulate command line options
 *
 */
public class LogOptions
{
    private int minLevel = 4;
    private int maxNumberOfLogs = -1;

    public LogOptions(String commandLine)
    {
        StringTokenizer st = new StringTokenizer(commandLine);
        readOptions(st);
    }

    private void readOptions(StringTokenizer st)
    {
        if (st.countTokens() > 1)
        {
            st.nextToken();
            String firstOption = st.nextToken();
            checkOption(firstOption);

            if (st.hasMoreTokens())
            {
                checkOption(st.nextToken());
            }
        }
    }

    private void checkOption(String opt)
    {
        try
        {
            maxNumberOfLogs = Integer.parseInt(opt);
        }
        catch (NumberFormatException nfe)
        {
            //do nothing, it's not a number
        }
        if ("info".equalsIgnoreCase(opt))
        {
            minLevel = 3;
        }
        else if ("warn".equalsIgnoreCase(opt))
        {
            minLevel = 2;
        }
        else if ("error".equalsIgnoreCase(opt))
        {
            minLevel = 1;
        }
    }

    public int getMinLevel()
    {
        return minLevel;
    }

    public int getMaxNumberOfLogs()
    {
        return maxNumberOfLogs;
    }
}