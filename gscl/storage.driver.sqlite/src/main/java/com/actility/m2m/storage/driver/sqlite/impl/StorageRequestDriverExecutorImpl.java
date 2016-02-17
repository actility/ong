/*
 * Methods retrieve, search, create, delete, update are executed in a transaction and are autocommited.
 * For executing a couple of operations in a transaction should be used beginTransaction method and after that
 * commitTransaction or rollbackTransaction for closing the transaction.
 */

package com.actility.m2m.storage.driver.sqlite.impl;

import java.util.Dictionary;
import java.util.List;
import java.util.Map;

import org.apache.log4j.Logger;

import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.DeletionHandler;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.SearchResult;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.driver.StorageRequestDriverExecutor;
import com.actility.m2m.storage.driver.Transaction;

/**
 * It provides all the necessary methods to operate on data in a driver
 */
public final class StorageRequestDriverExecutorImpl implements StorageRequestDriverExecutor {
    /**
     *  Attribute to display logs in the console
     */
    private final static Logger LOG = Logger.getLogger(StorageRequestDriverExecutorImpl.class);

    /**
     * Attribute which can manage and do operations with the SQLite DB
     */
    private final SQLiteRequestExecutor sqlite;

    /**
     * Class constructor.
     *
     * @param config The bundle configuration
     * @throws StorageException If any problem occurs while initializing the class
     */
    public StorageRequestDriverExecutorImpl(Dictionary config) throws StorageException {
        sqlite = new SQLiteRequestExecutor(config);
    }

    public SQLiteRequestExecutor getSQLiteRequestExecutor() {
        return sqlite;
    }

    public String reserveSpace(Map config, String path, String docNumber, double docSize) {
        return null;
    }

    public Transaction beginTransaction() throws StorageException {
        return sqlite.beginTransaction();
    }

    /**
     * Link the deletionHandler to the delete function
     * @param deletionHandler to link
     */
    public void bindDeletionHandler(DeletionHandler deletionHandler) {
        sqlite.bindDeletionHandler(deletionHandler);
    }
    /**
     * UnLink the deletionHandler to the delete function
     * @param deletionHandler to unlink
     */
    public void unbindDeletionHandler(DeletionHandler deletionHandler) {
        sqlite.unbindDeletionHandler(deletionHandler);
    }

    public Document retrieve(Map config, String path, Condition condition) throws StorageException {
        Document doc;
        doc = sqlite.retrieve(config, path, condition);
        return doc;
    }

    public Document retrieve(Map config, Object id, Condition condition) throws StorageException {
        Document doc;
        doc = sqlite.retrieve(config,id, condition);
        return doc;
    }

    public boolean update(Map config, Document document, Condition condition) throws StorageException {
        sqlite.createTransaction();
        boolean result = false;
        try{
             result = sqlite.update(config, document, condition);
        }finally{
            if(result){
                sqlite.commitTransaction();
            }else{
                sqlite.rollbackTransaction();
            }
        }
        return result;
    }

    public boolean partialUpdate(Map config, Document document, byte[] content, List attrOps, Condition condition)
            throws StorageException {
        sqlite.createTransaction();
        boolean result = false;
        try{
             result = sqlite.partialUpdate(config, document, content, attrOps, condition);
        }finally{
            if(result){
                sqlite.commitTransaction();
            }else{
                sqlite.rollbackTransaction();
            }
        }
        return result;
    }

    public boolean create(Map config, Document document) throws StorageException {
        if(LOG.isDebugEnabled()){
            LOG.debug("Begin of create document("+Util.documentToString(document)+")");
        }
        sqlite.createTransaction();
        boolean result = false;
        try{
             result = sqlite.create(config, document);
        }finally{
            if(result){
                sqlite.commitTransaction();
            }else{
                sqlite.rollbackTransaction();
            }
        }
        return result;
    }

    public boolean delete(Map config, Document document, int scope, Condition condition) throws StorageException {
        sqlite.createTransaction();
        boolean result = false;
        try{
            result  = sqlite.delete(config, document, scope, condition);
        }finally{
            if(result){
                sqlite.commitTransaction();
            }else{
                sqlite.rollbackTransaction();
            }
        }
        return result;
     }

    public boolean delete(Map config, Document document, Condition condition) throws StorageException {
        sqlite.createTransaction();
        boolean result = false;
        try{
            result  = sqlite.delete(config, document, condition);
        }finally{
            if(result){
                sqlite.commitTransaction();
            }else{
                sqlite.rollbackTransaction();
            }
        }
        return result;
     }

    public SearchResult search(Map config, String basePath, int scope, Condition condition, int order, int limit,
            boolean withContent, List withAttributes) throws StorageException {
        return sqlite.search(config, basePath, scope, condition, order, limit, withContent, withAttributes);
    }
}
