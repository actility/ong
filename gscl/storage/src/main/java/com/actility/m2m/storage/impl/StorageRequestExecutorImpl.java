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
 * id $Id: StorageRequestExecutorImpl.java 8777 2014-05-21 15:51:53Z JReich $
 * author $Author: JReich $
 * version $Revision: 8777 $
 * lastrevision $Date: 2014-05-21 17:51:53 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:51:53 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.storage.impl;

import java.sql.SQLException;
import java.util.Collection;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Map.Entry;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleContext;
import org.osgi.framework.InvalidSyntaxException;
import org.osgi.framework.ServiceReference;

import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Modification;
import com.actility.m2m.storage.SearchResult;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.storage.driver.StorageRequestDriverExecutor;
import com.actility.m2m.storage.driver.Transaction;
import com.actility.m2m.storage.log.BundleLogger;
import com.actility.m2m.util.Profiler;
import com.actility.m2m.util.log.OSGiLogger;

public class StorageRequestExecutorImpl implements StorageRequestExecutor {
    private static final Logger LOG = OSGiLogger.getLogger(StorageRequestExecutorImpl.class, BundleLogger.getStaticLogger());

    private BundleContext bundleContext;
    private String defaultDriverName;

    public StorageRequestExecutorImpl(String defaultDriverName, BundleContext context) {
        this.defaultDriverName = defaultDriverName;
        this.bundleContext = context;
    }

    public byte[] retrieve(String path) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Retrieve path: " + path);
        }
        StorageRequestDriverExecutor driverExecutor = getService(defaultDriverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }
        byte[] result = driverExecutor.retrieve(path);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Retrieve path: " + path);
        }
        return result;
    }

    public byte[] retrieve(Map config, String path) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Retrieve path: " + path);
        }
        String driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        StorageRequestDriverExecutor driverExecutor = getService(driverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }
        byte[] result = driverExecutor.retrieve(config, path);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Retrieve path: " + path);
        }
        return result;
    }

    public SearchResult search(String basePath, Condition condition) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Search path: " + basePath);
        }
        SearchResult result = search(basePath, StorageRequestExecutor.SCOPE_EXACT, condition);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Search path: " + basePath);
        }
        return result;
    }

    public SearchResult search(String basePath, Condition condition, int order) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Search path: " + basePath);
        }
        StorageRequestDriverExecutor driverExecutor = getService(defaultDriverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        SearchResult result = driverExecutor.search(basePath, condition, order);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Search path: " + basePath);
        }
        return result;
    }

    public SearchResult search(String basePath, Condition condition, int order, int limit) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Search path: " + basePath);
        }
        StorageRequestDriverExecutor driverExecutor = getService(defaultDriverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        SearchResult result = driverExecutor.search(basePath, condition, order, limit);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Search path: " + basePath);
        }
        return result;
    }

    public SearchResult search(String basePath, int scope, Condition condition) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Search path: " + basePath);
        }
        StorageRequestDriverExecutor driverExecutor = getService(defaultDriverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        SearchResult result = driverExecutor.search(basePath, scope, condition);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Search path: " + basePath);
        }
        return result;
    }

    public SearchResult search(String basePath, int scope, Condition condition, int order) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Search path: " + basePath);
        }
        StorageRequestDriverExecutor driverExecutor = getService(defaultDriverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        SearchResult result = driverExecutor.search(basePath, scope, condition, order);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Search path: " + basePath);
        }
        return result;
    }

    public SearchResult search(String basePath, int scope, Condition condition, int order, int limit) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Search path: " + basePath);
        }
        StorageRequestDriverExecutor driverExecutor = getService(defaultDriverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        SearchResult result = driverExecutor.search(basePath, scope, condition, order, limit);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Search path: " + basePath);
        }
        return result;
    }

    public SearchResult search(Map config, String basePath, Condition condition) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Search path: " + basePath);
        }
        String driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        StorageRequestDriverExecutor driverExecutor = getService(driverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        SearchResult result = driverExecutor.search(config, basePath, condition);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Search path: " + basePath);
        }
        return result;
    }

    public SearchResult search(Map config, String basePath, Condition condition, int order) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Search path: " + basePath);
        }
        String driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        StorageRequestDriverExecutor driverExecutor = getService(driverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        SearchResult result = driverExecutor.search(config, basePath, condition, order);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Search path: " + basePath);
        }
        return result;
    }

    public SearchResult search(Map config, String basePath, Condition condition, int order, int limit) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Search path: " + basePath);
        }
        String driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        StorageRequestDriverExecutor driverExecutor = getService(driverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        SearchResult result = driverExecutor.search(config, basePath, condition, order, limit);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Search path: " + basePath);
        }
        return result;
    }

    public SearchResult search(Map config, String basePath, int scope, Condition condition) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Search path: " + basePath);
        }
        String driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        StorageRequestDriverExecutor driverExecutor = getService(driverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        SearchResult result = driverExecutor.search(config, basePath, scope, condition);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Search path: " + basePath);
        }
        return result;
    }

    public SearchResult search(Map config, String basePath, int scope, Condition condition, int order) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Search path: " + basePath);
        }
        String driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        StorageRequestDriverExecutor driverExecutor = getService(driverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        SearchResult result = driverExecutor.search(config, basePath, scope, condition, order);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Search path: " + basePath);
        }
        return result;
    }

    public SearchResult search(Map config, String basePath, int scope, Condition condition, int order, int limit)
            throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Search path: " + basePath);
        }
        String driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        StorageRequestDriverExecutor driverExecutor = getService(driverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        SearchResult result = driverExecutor.search(config, basePath, scope, condition, order, limit);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Search path: " + basePath);
        }
        return result;
    }

    public void update(String path, byte[] rawDocument) throws StorageException {
        update(path, rawDocument, null);
    }

    public void update(String path, byte[] rawDocument, Collection searchAttributes) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Update path: " + path);
        }
        StorageRequestDriverExecutor driverExecutor = getService(defaultDriverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        driverExecutor.update(path, rawDocument, searchAttributes);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Update path: " + path);
        }
    }

    public void update(Map config, String path, byte[] rawDocument) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Update path: " + path);
        }
        String driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        StorageRequestDriverExecutor driverExecutor = getService(driverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        driverExecutor.update(config, path, rawDocument);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Update path: " + path);
        }
    }

    public void update(Map config, String path, byte[] rawDocument, Collection searchAttributes) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Update path: " + path);
        }
        String driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        StorageRequestDriverExecutor driverExecutor = getService(driverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        driverExecutor.update(config, path, rawDocument, searchAttributes);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Update path: " + path);
        }
    }

    public void create(String path, byte[] rawDocument) throws StorageException {
        create(path, rawDocument, (Collection) null);
    }

    public void create(String path, byte[] rawDocument, Collection searchAttributes) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Create path: " + path);
        }
        StorageRequestDriverExecutor driverExecutor = getService(defaultDriverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        driverExecutor.create(path, rawDocument, searchAttributes);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Create path: " + path);
        }
    }

    public void create(Map config, String path, byte[] rawDocument) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Create path: " + path);
        }
        String driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        StorageRequestDriverExecutor driverExecutor = getService(driverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        driverExecutor.create(config, path, rawDocument);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Create path: " + path);
        }
    }

    public void create(Map config, String path, byte[] rawDocument, Collection searchAttributes) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Create path: " + path);
        }
        String driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        StorageRequestDriverExecutor driverExecutor = getService(driverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        driverExecutor.create(config, path, rawDocument, searchAttributes);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Create path: " + path);
        }
    }

    public void delete(String path) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Delete path: " + path);
        }
        StorageRequestDriverExecutor driverExecutor = getService(defaultDriverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        driverExecutor.delete(path);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Delete path: " + path);
        }
    }

    public void delete(String path, int cascade) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Delete path: " + path);
        }
        StorageRequestDriverExecutor driverExecutor = getService(defaultDriverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        driverExecutor.delete(path, cascade);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Delete path: " + path);
        }
    }

    public void delete(Map config, String path) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Delete path: " + path);
        }
        String driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        StorageRequestDriverExecutor driverExecutor = getService(driverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        driverExecutor.delete(config, path);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Delete path: " + path);
        }
    }

    public void delete(Map config, String path, int cascade) throws StorageException {
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace(">>> Delete path: " + path);
        }
        String driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        StorageRequestDriverExecutor driverExecutor = getService(driverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        driverExecutor.delete(config, path, cascade);
        if (Profiler.getInstance().isTraceEnabled()) {
            Profiler.getInstance().trace("<<< Delete path: " + path);
        }
    }

    public void batchModify(Modification[] tabModif) throws StorageException {
        Profiler.getInstance().trace(">>> Batch modify");
        if (LOG.isInfoEnabled()) {
            LOG.info("Begin of batchModify(...)");
        }

        int nbExecutedTransactions = 0;
        Map allTransactions = new HashMap();

        try {

            for (int i = 0; i < tabModif.length; i++) {
                int modificationType = tabModif[i].getType();
                String fullPath = tabModif[i].getFullPath();
                // int indexLastSlash = fullPath.lastIndexOf('/');
                byte[] rawDocument = tabModif[i].getRawDocument();
                Collection searchAttributes = tabModif[i].getSearchAttributes();
                Map config = tabModif[i].getConfiguration();

                String driverName = findDriverName(config);

                Transaction transaction = (Transaction) allTransactions.get(driverName);

                if (transaction == null) {
                    StorageRequestDriverExecutor driver = findDriver(config);
                    transaction = driver.beginTransaction();
                    allTransactions.put(driverName, transaction);
                }

                switch (modificationType) {
                case Modification.TYPE_CREATE:
                    if (Profiler.getInstance().isTraceEnabled()) {
                        Profiler.getInstance().trace("Batch modify Create path: " + fullPath);
                    }
                    transaction.create(fullPath, rawDocument, searchAttributes, config);
                    break;
                case Modification.TYPE_UPDATE:
                    if (Profiler.getInstance().isTraceEnabled()) {
                        Profiler.getInstance().trace("Batch modify Update path: " + fullPath);
                    }
                    transaction.update(fullPath, rawDocument, searchAttributes, config);
                    break;
                case Modification.TYPE_DELETE:
                    if (Profiler.getInstance().isTraceEnabled()) {
                        Profiler.getInstance().trace("Batch modify Delete path: " + fullPath);
                    }
                    transaction.delete(fullPath, config);
                    break;
                default:
                    throw (new SQLException("Unexpected value in Modification.type: " + modificationType));
                }
            }

            for (Iterator it = allTransactions.keySet().iterator(); it.hasNext();) {
                String driverName = (String) it.next();
                Transaction transaction = (Transaction) allTransactions.get(driverName);

                if (transaction != null) {
                    transaction.commitTransaction();
                    ++nbExecutedTransactions;
                }
            }

        } catch (SQLException e) {
            throw (new StorageException("Error occured during batch modification", e));
        } finally {
            if (nbExecutedTransactions != allTransactions.size()) {
                Iterator it = allTransactions.entrySet().iterator();
                while (it.hasNext()) {
                    Entry entry = (Entry) it.next();
                    if (nbExecutedTransactions == 0) {
                        Transaction transaction = (Transaction) entry.getValue();
                        try {
                            transaction.rollbackTransaction();
                        } catch (Throwable e) {
                            LOG.error("Cannot rollback transaction for driver: " + entry.getKey(), e);
                        }
                    } else {
                        --nbExecutedTransactions;
                    }
                }
            }
        }

        if (LOG.isInfoEnabled()) {
            LOG.info("End of batchModify(...)");
        }
        Profiler.getInstance().trace("<<< Batch modify");
    }

    public Modification createModification(int type, String fullPath, byte[] rawDocument, Collection searchAttributes) {

        return new ModificationImpl(type, fullPath, rawDocument, searchAttributes);
    }

    public Modification createModification(Map config, int type, String fullPath, byte[] rawDocument,
            Collection searchAttributes) {

        return new ModificationImpl(type, fullPath, rawDocument, searchAttributes, config);
    }

    public String reserveSpace(Map config, String path, String docNumber, double docSize) {
        String driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        StorageRequestDriverExecutor driverExecutor = getService(driverName);

        if (driverExecutor == null) {
            throw new NullPointerException("There isn't such driver!");
        }

        return driverExecutor.reserveSpace(config, path, docNumber, docSize);
    }

    private StorageRequestDriverExecutor findDriver(Map config) {
        String driverName = null;

        if (config != null) {
            driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        }

        if (driverName == null) {
            driverName = defaultDriverName;
        }

        return getService(driverName);
    }

    private String findDriverName(Map config) {
        String driverName = null;

        if (config != null) {
            driverName = (String) config.get(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
        }

        if (driverName == null) {
            driverName = defaultDriverName;
        }

        return driverName;
    }

    private StorageRequestDriverExecutor getService(String driverName) {

        StringBuffer filter = new StringBuffer();
        if (driverName != null) {
            filter.append("(&(objectClass=");
            filter.append(StorageRequestDriverExecutor.class.getName());
            filter.append(")(");
            filter.append(StorageRequestDriverExecutor.PARAM_DRIVER_NAME);
            filter.append("=");
            filter.append(driverName);
            filter.append("))");
        }

        try {
            ServiceReference[] serviceReferences = bundleContext.getServiceReferences(
                    StorageRequestDriverExecutor.class.getName(), filter.toString());
            if (serviceReferences != null && serviceReferences.length > 0) {
                StorageRequestDriverExecutor service = (StorageRequestDriverExecutor) bundleContext
                        .getService(serviceReferences[0]);
                return service;
            }

        } catch (InvalidSyntaxException e) {
            e.printStackTrace();
        }

        return null;
    }
}
