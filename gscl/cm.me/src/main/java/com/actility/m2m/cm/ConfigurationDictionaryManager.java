/*******************************************************************************
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
 *******************************************************************************/

package com.actility.m2m.cm;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Properties;
import java.util.Set;
import java.util.Vector;

import org.apache.log4j.Logger;
import org.osgi.framework.Constants;
import org.osgi.service.cm.ConfigurationAdmin;

import com.actility.m2m.cm.log.BundleLogger;
import com.actility.m2m.util.StringUtils;
import com.actility.m2m.util.log.OSGiLogger;

public final class ConfigurationDictionaryManager {
    private static final Logger LOG = OSGiLogger.getLogger(ConfigurationDictionaryManager.class, BundleLogger.getStaticLogger());

    /**
     * * Allowed object types.
     */
    private final static Map SIMPLE_TO_TYPE = new HashMap();
    static {
        SIMPLE_TO_TYPE.put(String.class, "String");
        SIMPLE_TO_TYPE.put(Integer.class, "Integer");
        SIMPLE_TO_TYPE.put(Long.class, "Long");
        SIMPLE_TO_TYPE.put(Float.class, "Float");
        SIMPLE_TO_TYPE.put(Double.class, "Double");
        SIMPLE_TO_TYPE.put(Byte.class, "Byte");
        SIMPLE_TO_TYPE.put(Short.class, "Short");
        SIMPLE_TO_TYPE.put(Character.class, "Character");
        SIMPLE_TO_TYPE.put(Boolean.class, "Boolean");
    }

    private final static Set EXCLUDED_KEYS = new HashSet();
    static {
        EXCLUDED_KEYS.add(ConfigurationAdmin.SERVICE_FACTORYPID);
        EXCLUDED_KEYS.add(Constants.SERVICE_PID);
    }

    private final File etcConfigDir;
    private final File usrConfigDir;

    public ConfigurationDictionaryManager(File etcConfigDir, File usrConfigDir) {
        this.etcConfigDir = etcConfigDir;
        this.usrConfigDir = usrConfigDir;

        etcConfigDir.mkdirs();
        usrConfigDir.mkdirs();
    }

    public Iterator listPids() {
        Set pids = new HashSet();
        File[] configurations = etcConfigDir.listFiles();
        for (int i = 0; i < configurations.length; ++i) {
            File configuration = configurations[i];
            String configurationName = configuration.getName();
            if (configurationName.endsWith(".cfg")) {
                pids.add(configurationName.substring(0, configurationName.length() - 4));
            }
        }
        configurations = usrConfigDir.listFiles();
        for (int i = 0; i < configurations.length; ++i) {
            File configuration = configurations[i];
            String configurationName = configuration.getName();
            if (configurationName.endsWith(".cfg")) {
                pids.add(configurationName.substring(0, configurationName.length() - 4));
            } else if (configurationName.endsWith(".loc")) {
                pids.add(configurationName.substring(0, configurationName.length() - 4));
            }
        }
        return pids.iterator();
    }

    public void delete(String servicePid) {
        File usrConfFile = new File(usrConfigDir, servicePid + ".cfg");
        if (usrConfFile.exists()) {
            usrConfFile.delete();
        }
        File usrLocConfFile = new File(usrConfigDir, servicePid + ".loc");
        if (usrLocConfFile.exists()) {
            usrLocConfFile.delete();
        }
    }

    public void save(String servicePid, ConfigurationDictionary configuration) throws IOException {
        int nbUserProps = configuration.getNbUserProps() - 1;
        String factoryPid = (String) configuration.get(ConfigurationAdmin.SERVICE_FACTORYPID);
        String location = configuration.getLocation();
        boolean dynamic = configuration.isDynamic();
        if (factoryPid != null) {
            --nbUserProps;
        }

        File usrConfigFile = new File(usrConfigDir, servicePid + ".cfg");
        if (nbUserProps > 0) {
            PrintWriter out = new PrintWriter(new OutputStreamWriter(new FileOutputStream(usrConfigFile)));

            try {
                Iterator it = configuration.getDictionary().entrySet().iterator();
                while (it.hasNext()) {
                    Entry entry = (Entry) it.next();
                    if (!EXCLUDED_KEYS.contains(entry.getKey())) {
                        Property value = (Property) entry.getValue();
                        if (value.isUserValue()) {
                            out.print(entry.getKey());
                            printValue(out, value.getValue());
                        }
                    }
                }
            } finally {
                out.close();
            }
        } else if (usrConfigFile.exists()) {
            usrConfigFile.delete();
        }
        File usrLocConfigFile = new File(usrConfigDir, servicePid + ".loc");
        if (location != null) {
            PrintWriter out = new PrintWriter(new OutputStreamWriter(new FileOutputStream(usrLocConfigFile)));

            try {
                out.print(ConfigurationAdmin.SERVICE_BUNDLELOCATION);
                printValue(out, location);
                if (dynamic) {
                    out.print(CMConstants.SERVICE_DYNAMICLOCATION);
                    printValue(out, new Boolean(dynamic));
                }
            } finally {
                out.close();
            }
        } else if (usrLocConfigFile.exists()) {
            usrLocConfigFile.delete();
        }
    }

    public ConfigurationDictionary loadOne(String factoryPid) throws IOException {
        String pidName = null;
        File[] configurations = etcConfigDir.listFiles();
        for (int i = 0; i < configurations.length; ++i) {
            String configurationName = configurations[i].getName();
            if (configurationName.startsWith(factoryPid + "-") && configurationName.endsWith(".cfg")) {
                pidName = configurationName.substring(0, configurationName.length() - 4);
                break;
            }
        }
        if (pidName == null) {
            configurations = usrConfigDir.listFiles();
            for (int i = 0; i < configurations.length; ++i) {
                String configurationName = configurations[i].getName();
                if (configurationName.startsWith(factoryPid + "-") && configurationName.endsWith(".cfg")) {
                    pidName = configurationName.substring(0, configurationName.length() - 4);
                    break;
                } else if (configurationName.startsWith(factoryPid + "-") && configurationName.endsWith(".loc")) {
                    pidName = configurationName.substring(0, configurationName.length() - 4);
                    break;
                }
            }
        }
        return load(pidName);
    }

    public List/* <ConfigurationDictionary> */loadAll(String factoryPid) throws IOException {
        List/* <ConfigurationDictionary> */configurations = new ArrayList/* <ConfigurationDictionary> */();
        Iterator/* <String> */servicePids = listPids();
        while (servicePids.hasNext()) {
            String servicePid = (String) servicePids.next();
            configurations.add(load(servicePid));
        }
        return configurations;
    }

    public ConfigurationDictionary load(String servicePid) throws IOException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Loading configuration for pid " + servicePid);
        }
        String fileName = servicePid + ".cfg";

        File etcConfigFile = new File(etcConfigDir, fileName);
        File usrConfigFile = new File(usrConfigDir, fileName);
        File usrLocConfigFile = new File(usrConfigDir, servicePid + ".loc");
        if (etcConfigFile.exists() || usrConfigFile.exists() || usrLocConfigFile.exists()) {
            ConfigurationDictionary config = new ConfigurationDictionary();
            if (etcConfigFile.exists()) {
                if (LOG.isDebugEnabled()) {
                    LOG.debug("Loading configuration from " + etcConfigFile.getPath());
                }
                loadConfigurationFromFile(config, etcConfigFile, false);
            }
            if (usrConfigFile.exists()) {
                if (LOG.isDebugEnabled()) {
                    LOG.debug("Loading configuration from " + usrConfigFile.getPath());
                }
                loadConfigurationFromFile(config, usrConfigFile, true);
            }
            if (usrLocConfigFile.exists()) {
                if (LOG.isDebugEnabled()) {
                    LOG.debug("Loading location configuration from " + usrLocConfigFile.getPath());
                }
                loadLocationFromFile(config, usrLocConfigFile);
            } else {
                config.setLocation(null);
                config.setDynamic(true);
            }
            String factoryPid = null;
            int dashIndex = servicePid.lastIndexOf('-');
            if (dashIndex >= 0) {
                factoryPid = servicePid.substring(0, dashIndex);
            }
            config.put(Constants.SERVICE_PID, servicePid);
            if (factoryPid != null) {
                config.put(ConfigurationAdmin.SERVICE_FACTORYPID, factoryPid);
            }
            return config;
        }

        return null;
    }

    private void loadLocationFromFile(ConfigurationDictionary config, File configFile) {
        String configurationName = configFile.getName();
        InputStream in = null;
        try {
            in = new BufferedInputStream(new FileInputStream(configFile));
            Properties props = new Properties();
            props.load(in);

            String location = (String) props.get(ConfigurationAdmin.SERVICE_BUNDLELOCATION + ":String");
            Boolean dynamic = (Boolean) props.get(CMConstants.SERVICE_DYNAMICLOCATION + ":Boolean");
            if (location != null) {
                config.setLocation(location);
            }
            if (dynamic != null) {
                config.setDynamic(dynamic.booleanValue());
            }
        } catch (IOException e) {
            LOG.error("Cannot read or create configuration file: " + configurationName, e);
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                    LOG.error("Cannot close configuration file: " + configurationName, e);
                }
            }
        }
    }

    private void loadConfigurationFromFile(ConfigurationDictionary config, File configFile, boolean user) {
        String configurationName = configFile.getName();
        InputStream in = null;
        try {
            in = new BufferedInputStream(new FileInputStream(configFile));
            Properties props = new Properties();
            props.load(in);

            Enumeration it = props.keys();
            while (it.hasMoreElements()) {
                String key = (String) it.nextElement();
                String value = props.getProperty(key);
                Object builtValue = null;

                int equalIndex = value.indexOf('=');
                if (equalIndex < 0) {
                    builtValue = value;
                } else if (equalIndex == (value.length() - 1)) {
                    String type = value.substring(0, equalIndex);
                    value = "";
                    builtValue = buildValue(value, type);
                } else {
                    String type = value.substring(0, equalIndex);
                    value = value.substring(equalIndex + 1);
                    builtValue = buildValue(value, type);
                }
                if (LOG.isDebugEnabled()) {
                    LOG.debug("add cm elt: (key:" + key + ") (value:" + builtValue + ")");
                }
                if (user) {
                    config.put(key, builtValue);
                } else {
                    config.putEtc(key, builtValue);
                }
            }
        } catch (IOException e) {
            LOG.error("Cannot read or create configuration file: " + configurationName, e);
        } finally {
            if (in != null) {
                try {
                    in.close();
                } catch (IOException e) {
                    LOG.error("Cannot close configuration file: " + configurationName, e);
                }
            }
        }
    }

    private Object buildValue(String value, String type) {
        if ("String".equals(type)) {
            return value;
        } else if ("String[]".equals(type)) {
            return StringUtils.split(value, ',', true);
        } else if ("char".equals(type) || "Character".equals(type)) {
            return new Character((value.length() > 1) ? value.charAt(0) : (char) 0);
        } else if ("char[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            char[] realValues = new char[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = (value.length() > 1) ? values[i].charAt(0) : (char) 0;
            }
            return realValues;
        } else if ("Character[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            Character[] realValues = new Character[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = new Character((value.length() > 1) ? values[i].charAt(0) : (char) 0);
            }
            return realValues;
        } else if ("boolean".equals(type) || "Boolean".equals(type)) {
            return new Boolean(Boolean.TRUE.toString().equalsIgnoreCase(value));
        } else if ("boolean[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            boolean[] realValues = new boolean[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = Boolean.TRUE.toString().equalsIgnoreCase(value);
            }
            return realValues;
        } else if ("Boolean[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            Boolean[] realValues = new Boolean[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = new Boolean(Boolean.TRUE.toString().equalsIgnoreCase(value));
            }
            return realValues;
        } else if ("byte".equals(type) || "Byte".equals(type)) {
            return new Byte(Byte.parseByte(type));
        } else if ("byte[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            byte[] realValues = new byte[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = Byte.parseByte(values[i]);
            }
            return realValues;
        } else if ("Byte[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            Byte[] realValues = new Byte[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = new Byte(Byte.parseByte(values[i]));
            }
            return realValues;
        } else if ("int".equals(type) || "Integer".equals(type)) {
            return Integer.valueOf(value);
        } else if ("int[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            int[] realValues = new int[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = Integer.parseInt(values[i]);
            }
            return realValues;
        } else if ("Integer[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            Integer[] realValues = new Integer[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = Integer.valueOf(values[i]);
            }
            return realValues;
        } else if ("short".equals(type) || "Short".equals(type)) {
            return new Short(Short.parseShort(value));
        } else if ("short[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            short[] realValues = new short[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = Short.parseShort(values[i]);
            }
            return realValues;
        } else if ("Short[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            Short[] realValues = new Short[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = new Short(Short.parseShort(values[i]));
            }
            return realValues;
        } else if ("long".equals(type) || "Long".equals(type)) {
            return new Long(Long.parseLong(value));
        } else if ("long[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            long[] realValues = new long[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = Long.parseLong(values[i]);
            }
            return realValues;
        } else if ("Long[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            Long[] realValues = new Long[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = new Long(Long.parseLong(values[i]));
            }
            return realValues;
        } else if ("float".equals(type) || "Float".equals(type)) {
            return Float.valueOf(value);
        } else if ("float[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            float[] realValues = new float[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = Float.parseFloat(values[i]);
            }
            return realValues;
        } else if ("Float[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            Float[] realValues = new Float[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = Float.valueOf(values[i]);
            }
            return realValues;
        } else if ("double".equals(type) || "Double".equals(type)) {
            return Double.valueOf(value);
        } else if ("double[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            double[] realValues = new double[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = Double.parseDouble(values[i]);
            }
            return realValues;
        } else if ("Double[]".equals(type)) {
            String[] values = StringUtils.split(value, ',', true);
            Double[] realValues = new Double[values.length];
            for (int i = 0; i < values.length; ++i) {
                realValues[i] = Double.valueOf(values[i]);
            }
            return realValues;
        } else if (type.startsWith("List<") && type.endsWith(">")) {
            String[] values = StringUtils.split(value, ',', true);
            List realValues = new ArrayList();
            String subType = type.substring(5, type.length() - 1);
            for (int i = 0; i < values.length; ++i) {
                realValues.add(buildValue(values[i], subType));
            }
            return realValues;
        } else if (type.startsWith("Vector<") && type.endsWith(">")) {
            String[] values = StringUtils.split(value, ',', true);
            Vector realValues = new Vector();
            String subType = type.substring(7, type.length() - 1);
            for (int i = 0; i < values.length; ++i) {
                realValues.addElement(buildValue(values[i], subType));
            }
            return realValues;
        } else {
            // Unknown
            LOG.error("Unknown type used in configuration use String instead: " + type);
            return value;
        }
    }

    private void printValue(PrintWriter out, Object value) {
        Class valueClass = value.getClass();
        if (valueClass.isArray()) {
            printArray(out, value);
        } else if (value instanceof List) {
            printList(out, (List) value);
        } else if (value instanceof Vector) {
            printVector(out, (Vector) value);
        } else {
            printSimple(out, value);
        }
    }

    private void printArray(PrintWriter out, Object array) {
        Class componentClass = array.getClass();
        if (String[].class == componentClass) {
            out.print("String[]=");
            String[] values = (String[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (char[].class == componentClass) {
            out.print("char[]=");
            char[] values = (char[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (Character[].class == componentClass) {
            out.print("Character[]=");
            Character[] values = (Character[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (boolean[].class == componentClass) {
            out.print("boolean[]=");
            boolean[] values = (boolean[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (Boolean[].class == componentClass) {
            out.print("Boolean[]=");
            Boolean[] values = (Boolean[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (byte[].class == componentClass) {
            out.print("byte[]=");
            byte[] values = (byte[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (Byte[].class == componentClass) {
            out.print("Byte[]=");
            Byte[] values = (Byte[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (int[].class == componentClass) {
            out.print("int[]=");
            int[] values = (int[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (Integer[].class == componentClass) {
            out.print("Integer[]=");
            Integer[] values = (Integer[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (short[].class == componentClass) {
            out.print("short[]=");
            short[] values = (short[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (Short[].class == componentClass) {
            out.print("Short[]=");
            Short[] values = (Short[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (long[].class == componentClass) {
            out.print("long[]=");
            long[] values = (long[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (Long[].class == componentClass) {
            out.print("Long[]=");
            Long[] values = (Long[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (float[].class == componentClass) {
            out.print("float[]=");
            float[] values = (float[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (Float[].class == componentClass) {
            out.print("Float[]=");
            Float[] values = (Float[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (double[].class == componentClass) {
            out.print("double[]=");
            double[] values = (double[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        } else if (Double[].class == componentClass) {
            out.print("Double[]=");
            Double[] values = (Double[]) array;
            for (int i = 0; i < values.length; ++i) {
                out.print(values[i]);
                if (i != (values.length - 1)) {
                    out.print(',');
                }
            }
        }
        out.println();
    }

    private void printSimple(PrintWriter out, Object value) {
        out.print(':');
        out.print(SIMPLE_TO_TYPE.get(value.getClass()));
        out.print('=');
        out.print(value);
        out.println();
    }

    private void printVector(PrintWriter out, Vector vector) {
        Enumeration i = vector.elements();
        out.print(":Vector<");
        if (vector.size() > 0) {
            out.print(SIMPLE_TO_TYPE.get(vector.elementAt(0).getClass()));
        } else {
            out.print("String");
        }
        out.print(">=");
        while (i.hasMoreElements()) {
            out.print(i.nextElement());
            if (i.hasMoreElements()) {
                out.print(',');
            }
        }
        out.println();
    }

    private void printList(PrintWriter out, List list) {
        Iterator i = list.iterator();
        out.print(":List<");
        if (list.size() > 0) {
            out.print(SIMPLE_TO_TYPE.get(list.get(0).getClass()));
        } else {
            out.print("String");
        }
        out.print(">=");
        while (i.hasNext()) {
            out.print(i.next());
            if (i.hasNext()) {
                out.print(',');
            }
        }
        out.println();
    }

}
