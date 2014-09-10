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
 * id $Id: SetCommand.java 7124 2014-01-04 13:15:00Z JReich $
 * author $Author: JReich $
 * version $Revision: 7124 $
 * lastrevision $Date: 2014-01-04 14:15:00 +0100 (Sat, 04 Jan 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-01-04 14:15:00 +0100 (Sat, 04 Jan 2014) $
 */

package com.actility.m2m.cm.command;

import java.io.PrintStream;
import java.math.BigInteger;
import java.util.Dictionary;
import java.util.Hashtable;
import java.util.StringTokenizer;

import org.apache.felix.shell.Session;
import org.apache.felix.shell.SessionCommand;
import org.osgi.framework.BundleContext;
import org.osgi.service.cm.Configuration;

public class SetCommand implements SessionCommand {

    private BundleContext bundleContext;

    public SetCommand(BundleContext bundleContext) {
        this.bundleContext = bundleContext;
    }

    public String getName() {
        return "cm-set";
    }

    public String getUsage() {
        return "cm-set <property> <value> [<type>]\n" + "<property> Name of property to set in configuration\n"
                + "<value>    New value of property\n" + "<type>     Type of value\n" + "Allowed types:\n"
                + "  String|Integer|Long|Float|Double|Byte|Short|\n " + "  Character|Boolean|BigInteger";
    }

    public String getShortDescription() {
        return "Set a property in the currently open configuration.";
    }

    public void execute(String line, PrintStream out, PrintStream err) {
        // Ignore
    }

    public void execute(Session session, String line, PrintStream out, PrintStream err) {
        // Parse command line.
        StringTokenizer st = new StringTokenizer(line, " ");

        // Ignore the command name.
        st.nextToken();

        // Check for optional argument.
        String property = null;
        String value = null;
        String type = null;
        int index = 0;
        if (st.countTokens() >= 1) {
            while (st.hasMoreTokens()) {
                String token = st.nextToken().trim();
                switch (index) {
                case 0:
                    property = token;
                    break;
                case 1:
                    value = token;
                    break;
                case 2:
                    type = token;
                    break;
                }
                ++index;
            }
        }

        try {
            if (session.getAttribute(Activator.CURRENT) == null) {
                throw new Exception("No configuration open currently");
            }

            Dictionary dict = (Dictionary) session.getAttribute(Activator.EDITED);
            if (dict == null) {
                Configuration cfg = (Configuration) session.getAttribute(Activator.CURRENT);
                if (cfg != null) {
                    dict = cfg.getProperties();
                }
                if (dict == null) {
                    dict = new Hashtable();
                }
                session.setAttribute(Activator.EDITED, dict);
            }
            Object currentValue = dict.get(property);

            if (type == null) {
                if (currentValue == null) {
                    dict.put(property, value);
                } else {
                    Class currentValueClass = currentValue.getClass();
                    Object newValue = stringToObjectOfClass(value, currentValueClass);
                    if (newValue == null) {
                        throw new Exception("Unable to convert argument to the same type as old value of property");
                    }
                    dict.put(property, newValue);
                }
            } else {
                Object newValue = null;
                try {
                    newValue = createValue(type, value);
                } catch (Exception e) {
                    newValue = null;
                }
                if (newValue == null) {
                    throw new Exception("Unable to convert " + value + " to " + type);
                }
                dict.put(property, newValue);
            }
        } catch (Exception e) {
            out.println("Set failed. Details:");
            String reason = e.getMessage();
            out.println(reason == null ? "<unknown>: " + e.toString() : reason);
        }
    }

    private Object stringToObjectOfClass(String str, Class c) {
        if (str == null) {
            return null;
        }
        Object object = null;
        try {
            if (c == null || c == String.class) {
                object = str;
            } else if (str.length() == 0) {
                // None of the other Classes can handle a zero length String
                object = null;
            } else if (c == Integer.class) {
                return new Integer(Integer.parseInt(str));
            } else if (c == Long.class) {
                object = new Long(Long.parseLong(str));
            } else if (c == Float.class) {
                object = new Float(Float.parseFloat(str));
            } else if (c == Double.class) {
                object = new Double(Double.parseDouble(str));
            } else if (c == Byte.class) {
                object = new Byte(Byte.parseByte(str));
            } else if (c == Short.class) {
                object = new Short(Short.parseShort(str));
            } else if (c == BigInteger.class) {
                object = new BigInteger(str);
            } else if (c == Character.class) {
                object = new Character(str.charAt(0));
            } else if (c == Boolean.class) {
                object = new Boolean(Boolean.TRUE.toString().equalsIgnoreCase(str));
            }
        } catch (Exception ignored) {
            object = null;
        }
        return object;
    }

    private Object createValue(String type, String rawValue) {
        String def = rawValue.equals("") ? null : rawValue;
        if (type.equals("String")) {
            return def == null ? new String() : new String(def);
        } else if (type.equals("Integer")) {
            return def == null ? new Integer(0) : new Integer(Integer.parseInt(def));
        } else if (type.equals("Long")) {
            return def == null ? new Long(0) : new Long(Long.parseLong(def));
        } else if (type.equals("Float")) {
            return def == null ? new Float(0) : new Float(Float.parseFloat(def));
        } else if (type.equals("Double")) {
            return def == null ? new Double(0) : new Double(Double.parseDouble(def));
        } else if (type.equals("Byte")) {
            return def == null ? new Byte(Byte.parseByte("0")) : new Byte(Byte.parseByte(def));
        } else if (type.equals("Short")) {
            return def == null ? new Short(Short.parseShort("0")) : new Short(Short.parseShort(def));
        } else if (type.equals("BigInteger")) {
            return def == null ? new BigInteger("0") : new BigInteger(def);
        } else if (type.equals("Character")) {
            return def == null ? new Character('a') : new Character(def.charAt(0));
        } else if (type.equals("Boolean")) {
            return def == null ? new Boolean(false) : new Boolean(Boolean.TRUE.toString().equalsIgnoreCase(def));
        } else {
            // Unsupported type
            return null;
        }
    }
}
