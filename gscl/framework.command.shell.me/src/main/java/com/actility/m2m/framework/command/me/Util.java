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

import org.osgi.framework.Bundle;
import org.osgi.framework.Constants;

public class Util
{
    public static String getBundleName(Bundle bundle)
    {
        if (bundle != null)
        {
            String name = (String) bundle.getHeaders().get(Constants.BUNDLE_NAME);
            return (name == null)
                ? "Bundle " + Long.toString(bundle.getBundleId())
                : name + " (" + Long.toString(bundle.getBundleId()) + ")";
        }
        return "[STALE BUNDLE]";
    }

    private static StringBuffer m_sb = new StringBuffer();

    public static String getUnderlineString(String s)
    {
        synchronized (m_sb)
        {
            m_sb.delete(0, m_sb.length());
            for (int i = 0; i < s.length(); i++)
            {
                m_sb.append('-');
            }
            return m_sb.toString();
        }
    }

    public static String getValueString(Object obj)
    {
        synchronized (m_sb)
        {
            if (obj instanceof String)
            {
                return (String) obj;
            }
            else if (obj instanceof String[])
            {
                String[] array = (String[]) obj;
                m_sb.delete(0, m_sb.length());
                for (int i = 0; i < array.length; i++)
                {
                    if (i != 0)
                    {
                        m_sb.append(", ");
                    }
                    m_sb.append(array[i].toString());
                }
                return m_sb.toString();
            }
            else if (obj instanceof Boolean)
            {
                return ((Boolean) obj).toString();
            }
            else if (obj instanceof Long)
            {
                return ((Long) obj).toString();
            }
            else if (obj instanceof Integer)
            {
                return ((Integer) obj).toString();
            }
            else if (obj instanceof Short)
            {
                return ((Short) obj).toString();
            }
            else if (obj instanceof Double)
            {
                return ((Double) obj).toString();
            }
            else if (obj instanceof Float)
            {
                return ((Float) obj).toString();
            }

            return "<unknown value type>";
        }
    }
}