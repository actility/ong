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
 * id $Id: TransactionImpl.java 8771 2014-05-21 15:46:29Z JReich $
 * author $Author: JReich $
 * version $Revision: 8771 $
 * lastrevision $Date: 2014-05-21 17:46:29 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:46:29 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.storage.driver.sqlite.impl;

import java.util.Collection;
import java.util.Map;

import org.apache.log4j.Logger;

import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.storage.driver.Transaction;
import com.actility.m2m.storage.driver.sqlite.log.BundleLogger;
import com.actility.m2m.storage.driver.sqlite.ni.SqliteDriverNIService;
import com.actility.m2m.util.log.OSGiLogger;

public class TransactionImpl implements Transaction {
    private static final Logger LOG = OSGiLogger.getLogger(TransactionImpl.class, BundleLogger.getStaticLogger());

    private final StorageRequestDriverExecutorImpl driver;
    private SqliteDriverNIService sqliteDriverService;

    public TransactionImpl(SqliteDriverNIService sqliteDriverService) throws StorageException {
        super();

        this.sqliteDriverService = sqliteDriverService;
        driver = new StorageRequestDriverExecutorImpl(sqliteDriverService);
        LOG.info("Begin transaction");
        int sqliteCode = sqliteDriverService.beginTransaction();
        if (sqliteCode != sqliteDriverService.getSqliteOk()) {
            throw new StorageException("Cannot begin the transaction (sqliteCode:" + sqliteCode + ")");
        }
    }

    /**
     * Raises an exception if transaction cannot be committed
     *
     * @throws StorageException if transaction cannot be committed
     */
    public void commitTransaction() throws StorageException {
        LOG.info("Commit transaction");
        int sqliteCode = sqliteDriverService.commitTransaction();
        if (sqliteCode != sqliteDriverService.getSqliteOk()) {
            throw new StorageException("Cannot commit the transaction (sqliteCode:" + sqliteCode + ")");
        }
    }

    /**
     * Raises an exception if transaction cannot be rollbacked
     *
     * @throws StorageException if transaction cannot be rollbacked
     */

    public void rollbackTransaction() throws StorageException {
        try {
            LOG.info("Rollback transaction");
            int sqliteCode = sqliteDriverService.rollbackTransaction();

            if (sqliteCode != sqliteDriverService.getSqliteOk()) {
                throw new StorageException("Cannot rollback the transaction (sqliteCode:" + sqliteCode + ")");
            }
        } finally {
            // nothing is possible
        }
    }

    public void create(String path, byte[] rawDocument, Map config) throws StorageException {
        driver.create(path, rawDocument, null, true);
    }

    public void create(String path, byte[] rawDocument, Collection searchAttributes, Map config) throws StorageException {
        driver.create(path, rawDocument, searchAttributes, true);
    }

    public void delete(String path, Map config) throws StorageException {
        driver.delete(path, StorageRequestExecutor.CASCADING_NONE, true);
    }

    public void delete(String path, int cascade, Map config) throws StorageException {
        driver.delete(path, cascade, true);
    }

    public void update(String path, byte[] rawDocument, Map config) throws StorageException {
        driver.update(path, rawDocument, null, true);
    }

    public void update(String path, byte[] rawDocument, Collection searchAttributes, Map config) throws StorageException {
        driver.update(path, rawDocument, searchAttributes, true);
    }
}
