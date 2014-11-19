package com.actility.m2m.storage.driver.sqlite.impl;

import java.util.List;
import java.util.Map;

import org.apache.log4j.Logger;

import com.actility.m2m.storage.Batch;
import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.driver.Transaction;
import com.actility.m2m.storage.driver.sqlite.impl.SqliteDB;

public final class TransactionImpl implements Transaction {
    private final static Logger LOG = Logger.getLogger(TransactionImpl.class);

    private final SqliteDB openedDB;
    private final SQLiteRequestExecutor driver;

    public TransactionImpl(SQLiteRequestExecutor driver) throws StorageException {
        super();
        if (LOG.isInfoEnabled()) {
            LOG.info("beginTransaction");
        }
        this.driver = driver;
        this.openedDB = driver.openedDB;
        driver.createTransaction();
    }

    public TransactionImpl(SqliteDB openedDb) throws StorageException {
        super();
        if (LOG.isInfoEnabled()) {
            LOG.info("beginTransaction");
        }
        this.openedDB = openedDb;
        this.driver = new SQLiteRequestExecutor(openedDB);
        // Thread.currentThread().getName();
        // sqlitedriver.beginTransaction(openedDB);
        driver.createTransaction();
    }

    /**
     * Prepares the transaction.
     * <p>
     * According to real storage implementation, operations happen here and may return an error through its return value.
     *
     * @return An integer which described the preparation result. ({@link Batch#BATCH_OK}, {@link Batch#BATCH_CREATE_FAILED},
     *         {@link Batch#BATCH_UPDATE_FAILED}, {@link Batch#BATCH_DELETE_FAILED})
     * @throws StorageException if transaction cannot be prepared
     */
    public int prepareTransaction() throws StorageException {
        return Batch.BATCH_OK;
    }

    /**
     * Raises an exception if transaction cannot be committed
     *
     * @throws StorageException if transaction cannot be committed
     */

    public void commitTransaction() throws StorageException {
        driver.commitTransaction();
    }

    /**
     * Raises an exception if transaction cannot be rollbacked
     *
     * @throws StorageException if transaction cannot be rollbacked
     */

    public void rollbackTransaction() throws StorageException {
        driver.rollbackTransaction();
    }

    public boolean update(Map config, Document document, Condition condition) throws StorageException {
        return driver.update(config, document, condition);
    }

    public boolean partialUpdate(Map config, Document document, byte[] content, List attrOps, Condition condition)
            throws StorageException {
        return driver.partialUpdate(config, document, content, attrOps, condition);
    }

    public boolean create(Map config, Document document) throws StorageException {
        if (LOG.isDebugEnabled()) {
            LOG.debug("Begin of create document(" + Util.documentToString(document) + ")");
        }
        return driver.create(config, document);
    }

    public boolean delete(Map config, Document document, int scope, Condition condition) throws StorageException {
        return driver.delete(config, document, scope, condition);
    }

    public boolean delete(Map config, Document document, Condition condition) throws StorageException {
        return driver.delete(config, document, condition);
    }
}
