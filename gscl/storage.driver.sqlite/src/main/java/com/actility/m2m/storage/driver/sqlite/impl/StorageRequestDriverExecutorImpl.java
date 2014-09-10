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
 * id $Id: StorageRequestDriverExecutorImpl.java 8771 2014-05-21 15:46:29Z JReich $
 * author $Author: JReich $
 * version $Revision: 8771 $
 * lastrevision $Date: 2014-05-21 17:46:29 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:46:29 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.storage.driver.sqlite.impl;

import java.text.SimpleDateFormat;
import java.util.Collection;
import java.util.Date;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.Map;

import org.apache.log4j.Logger;

import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.ConditionBuilder;
import com.actility.m2m.storage.SearchResult;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.storage.driver.StorageRequestDriverExecutor;
import com.actility.m2m.storage.driver.Transaction;
import com.actility.m2m.storage.driver.sqlite.log.BundleLogger;
import com.actility.m2m.storage.driver.sqlite.ni.NiAttribute;
import com.actility.m2m.storage.driver.sqlite.ni.NiAttributeArray;
import com.actility.m2m.storage.driver.sqlite.ni.NiCondition;
import com.actility.m2m.storage.driver.sqlite.ni.NiSearchResult;
import com.actility.m2m.storage.driver.sqlite.ni.NiSubSearchResult;
import com.actility.m2m.storage.driver.sqlite.ni.NiSubSearchResultArray;
import com.actility.m2m.storage.driver.sqlite.ni.SqliteDriverNIService;
import com.actility.m2m.util.Pair;
import com.actility.m2m.util.log.OSGiLogger;

/**
 * Methods retrieve, search, create, delete, update are executed in a transaction and are autocommited. For executing a couple
 * of operations in a transaction should be used beginTransaction method and after that commitTransaction or rollbackTransaction
 * for closing the transaction.
 */
public class StorageRequestDriverExecutorImpl implements StorageRequestDriverExecutor {
    private static final Logger LOG = OSGiLogger.getLogger(StorageRequestDriverExecutorImpl.class,
            BundleLogger.getStaticLogger());

    /**
     * 1024 bytes = 1kb is one page 1024 pages = 1 mb; 1mb * 6 = 6mb restricted size of the database 1024 * 6 = 6144 total pages
     * for 6 mb
     **/
    // private static final int PAGE_SIZE = 1024;
    public static final int MAX_PAGE_COUNT = 6144;
    private static final float PERCENT_SPACE_TAKEN = 0.90f;

    /**
     * A lock to prevent having several transaction running simultaneously (among all instances) When sqlite is used without
     * such a lock, an error is returned on second update
     *
     * sample with two sessions: A BEGIN; A UPDATE T1 SET ... B: BEGIN; B: UPDATE T2 SET ... Error !!!
     */
    private final static Object LOCK = new Object();

    private final static SimpleDateFormat FORMATER = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS");

    private SqliteDriverNIService sqliteDriverService;

    public StorageRequestDriverExecutorImpl(SqliteDriverNIService sqliteDriverService) {
        this.sqliteDriverService = sqliteDriverService;
    }

    public byte[] retrieve(String fullPath) throws StorageException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Begin of retrieve (" + fullPath + ")");
        }

        byte[] xmlContent = sqliteDriverService.sqliteRetrieve(fullPath);

        if (LOG.isInfoEnabled()) {
            LOG.info("End of retrieve (" + fullPath + ")");
        }

        return xmlContent.length == 0 ? null : xmlContent;
    }

    public byte[] retrieve(Map config, String path) throws StorageException {
        // TODO use config
        return retrieve(path);
    }

    public SearchResult search(String basePath, Condition condition) throws StorageException {
        return search(basePath, StorageRequestExecutor.SCOPE_EXACT, condition, -1, -1);
    }

    public SearchResult search(String basePath, int scope, Condition condition) throws StorageException {
        return search(basePath, scope, condition, -1, -1);
    }

    public SearchResult search(String basePath, Condition condition, int order) throws StorageException {
        return search(basePath, StorageRequestExecutor.SCOPE_EXACT, condition, order, -1);
    }

    // without limit -1
    public SearchResult search(String basePath, Condition condition, int order, int limit) throws StorageException {

        return search(basePath, StorageRequestExecutor.SCOPE_EXACT, condition, order, limit);
    }

    public SearchResult search(String basePath, int scope, Condition condition, int order) throws StorageException {
        return search(basePath, scope, condition, order, -1);
    }

    public SearchResult search(String basePath, int scope, Condition condition, int order, int limit) throws StorageException {

        if (LOG.isInfoEnabled()) {
            LOG.info("Begin of search(PATH=" + basePath + ", SCOPE=" + scope + ", CONDITION="
                    + Util.conditionToString(condition) + ", ORDER=" + order + ", LIMIT=" + limit + ")");
        }

        NiCondition internalCondition = null;
        NiSearchResult returnedData = null;
        try {
            if (condition != null) {
                internalCondition = convertToInternalCondition(condition);
            }

            returnedData = sqliteDriverService.search(basePath, scope, internalCondition, order, limit);
            NiSubSearchResultArray results = returnedData.getData();

            Map searchedResults = new LinkedHashMap();
            for (int i = 0; i < returnedData.getSize(); i++) {
                NiSubSearchResult subResult = results.getItem(i);
                String fullPathOfResult = subResult.getPath() + subResult.getName();
                if (LOG.isInfoEnabled()) {
                    LOG.info("Found (PATH=" + fullPathOfResult + ", CONTENT.SIZE=" + subResult.getContent().length + ")");
                }
                searchedResults.put(fullPathOfResult, subResult.getContent());
            }

            boolean hasMore = returnedData.getHasMore() == 0 ? false : true;
            SearchResult searchResult = new SearchResult(hasMore, searchedResults);
            return searchResult;
        } finally {
            if (internalCondition != null) {
                internalCondition.delete();
            }
            if (returnedData != null) {
                returnedData.delete();
            }
        }
    }

    private NiAttributeArray convertToInternalAttributes(Collection searchAttributes) throws StorageException {
        NiAttributeArray attributes = sqliteDriverService.createInternalAttributeArray(searchAttributes.size());

        Iterator searchAttributesIterator = searchAttributes.iterator();
        int index = 0;
        while (searchAttributesIterator.hasNext()) {
            Object searchAttribute = searchAttributesIterator.next();
            if (searchAttribute instanceof Pair) {
                String attributeName = (String) ((Pair) searchAttribute).getFirst();
                Object attributeValue = ((Pair) searchAttribute).getSecond();
                NiAttribute attribute = sqliteDriverService.createInternalAttribute();
                attribute.setName(attributeName);
                if (attributeValue instanceof String) {
                    attribute.setStringValue((String) attributeValue);
                    attribute.setType(ConditionBuilder.TYPE_STRING);
                } else if (attributeValue instanceof Integer) {
                    attribute.setIntValue(((Integer) attributeValue).intValue());
                    attribute.setType(ConditionBuilder.TYPE_INTEGER);
                } else {
                    attribute.setDateValue(FORMATER.format(((Date) attributeValue)));
                    attribute.setType(ConditionBuilder.TYPE_DATE);
                }
                attributes.setItem(index, attribute);
                ++index;
            } else {
                throw new StorageException("Search attributes embed an object which is not a pair: " + searchAttribute + " ("
                        + searchAttribute.getClass() + ")");
            }
        }

        return attributes;
    }

    private NiCondition convertToInternalCondition(Condition condition) throws StorageException {
        NiCondition internalCondition = sqliteDriverService.createInternalCondition();
        if (condition.getAttributeName() == null) {
            internalCondition.setOper(condition.getOperator());
            NiCondition condition1 = convertToInternalCondition(condition.getCondition1());
            internalCondition.setCondition1(condition1);

            NiCondition condition2 = convertToInternalCondition(condition.getCondition2());
            internalCondition.setCondition2(condition2);
        } else {
            String attrName = condition.getAttributeName();
            internalCondition.setAttributeName(attrName);
            internalCondition.setOper(condition.getOperator());

            int conditionType = condition.getType();
            internalCondition.setType(conditionType);

            Object operand = condition.getOperand();
            if (operand instanceof String) {
                internalCondition.setOperand((String) operand);
            } else if (operand instanceof Integer) {
                internalCondition.setOperand(((Integer) operand).toString());
            } else if (operand instanceof Date) {
                String formattedDate = null;
                synchronized (LOCK) {
                    formattedDate = FORMATER.format(((Date) operand));
                }
                internalCondition.setOperand(formattedDate);
            } else if (operand == null) {
                throw new NullPointerException("Received a storage condition with a null operand: " + attrName);
            } else {
                throw new StorageException("Received a storage condition with an unknown class: " + attrName + " ("
                        + operand.getClass() + ")");
            }
        }

        return internalCondition;
    }

    public SearchResult search(Map config, String basePath, Condition condition) throws StorageException {
        // TODO recreate the method to use config
        return search(basePath, condition);
    }

    public SearchResult search(Map config, String basePath, Condition condition, int order) throws StorageException {
        // TODO recreate the method to use config
        return search(basePath, StorageRequestExecutor.SCOPE_EXACT, condition, order, -1);
    }

    public SearchResult search(Map config, String basePath, Condition condition, int order, int limit) throws StorageException {
        // TODO recreate the method to use config
        return search(basePath, StorageRequestExecutor.SCOPE_EXACT, condition, order, limit);
    }

    public SearchResult search(Map config, String basePath, int scope, Condition condition) throws StorageException {
        // TODO recreate the method to use config
        return search(basePath, scope, condition, -1, -1);
    }

    public SearchResult search(Map config, String basePath, int scope, Condition condition, int order) throws StorageException {
        // TODO recreate the method to use config
        return search(basePath, scope, condition, order, -1);
    }

    public SearchResult search(Map config, String basePath, int scope, Condition condition, int order, int limit)
            throws StorageException {
        // TODO recreate the method to use config
        return search(basePath, scope, condition, order, limit);
    }

    public void create(String path, byte[] rawDocument) throws StorageException {
        create(path, rawDocument, null, false);
    }

    public void create(String fullPath, byte[] rawDocument, Collection searchAttributes) throws StorageException {
        create(fullPath, rawDocument, searchAttributes, false);
    }

    public void create(Map config, String path, byte[] rawDocument) throws StorageException {
        // TODO Recreate the method to use config
        create(path, rawDocument, null, false);
    }

    public void create(Map config, String path, byte[] rawDocument, Collection searchAttributes) throws StorageException {
        // TODO Recreate the method to use config
        create(path, rawDocument, searchAttributes, false);
    }

    public void create(String fullPath, byte[] rawDocument, Collection searchAttributes, boolean trans) throws StorageException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Begin of create(PATH=" + fullPath + ", DOCUMENT.SIZE=" + (rawDocument != null ? rawDocument.length : 0)
                    + ", ATTRIBUTES=" + Util.AttributesToString(searchAttributes) + ", TRANS=" + trans + ")");
        }

        NiAttributeArray attrs = null;
        int attrsLength = 0;
        synchronized (LOCK) {
            // there must be only one transaction for some CRUD operations
            boolean error = false;
            try {
                if (!trans) {
                    sqliteDriverService.beginTransaction();
                }

                if (searchAttributes != null) {
                    attrs = convertToInternalAttributes(searchAttributes);
                    attrsLength = searchAttributes.size();
                }
                int sqlCode = sqliteDriverService.sqliteCreate(fullPath, rawDocument, rawDocument.length, attrs, attrsLength);

                if (sqlCode != 101) {
                    throw new StorageException("Sqlite code: " + sqlCode);
                }
            } catch (RuntimeException e) {
                error = true;
                throw e;
            } catch (StorageException e) {
                error = true;
                throw e;
            } finally {
                if (attrs != null) {
                    attrs.delete();
                }
                if (!trans) {
                    if (!error) {
                        sqliteDriverService.commitTransaction();
                    } else {
                        sqliteDriverService.rollbackTransaction();
                    }
                }
            }
        }/* synchronized */

        if (LOG.isInfoEnabled()) {
            LOG.info("End of create(" + fullPath + ", ...)");
        }
    }

    public void update(String fullPath, byte[] rawDocument) throws StorageException {
        update(fullPath, rawDocument, null, false);
    }

    public void update(String fullPath, byte[] rawDocument, Collection searchAttributes) throws StorageException {
        update(fullPath, rawDocument, searchAttributes, false);
    }

    public void update(Map config, String path, byte[] rawDocument) throws StorageException {
        // TODO recreate the method
        update(path, rawDocument, null, false);
    }

    public void update(Map config, String path, byte[] rawDocument, Collection searchAttributes) throws StorageException {
        // TODO recreate the method
        update(path, rawDocument, searchAttributes, false);
    }

    public void update(String fullPath, byte[] rawDocument, Collection searchAttributes, boolean trans) throws StorageException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Begin of update(PATH=" + fullPath + ", DOCUMENT.SIZE=" + (rawDocument != null ? rawDocument.length : 0)
                    + ", ATTRIBUTES=" + Util.AttributesToString(searchAttributes) + ", TRANS=" + trans + ")");
        }

        NiAttributeArray attrs = null;
        int attrsLength = 0;
        synchronized (LOCK) {
            boolean error = false;
            try {
                if (!trans) {
                    sqliteDriverService.beginTransaction();
                }
                if (searchAttributes != null) {
                    attrs = convertToInternalAttributes(searchAttributes);
                    attrsLength = searchAttributes.size();
                }
                int numberOfAffectedRows = sqliteDriverService.sqliteUpdate(fullPath, rawDocument,
                        (rawDocument != null) ? rawDocument.length : 0, attrs, attrsLength);

                if (numberOfAffectedRows == 0) {
                    throw new StorageException("There isn't such a document");
                }
            } catch (RuntimeException e) {
                error = true;
                throw e;
            } catch (StorageException e) {
                error = true;
                throw e;
            } finally {
                if (attrs != null) {
                    attrs.delete();
                }
                if (!trans) {
                    if (!error) {
                        sqliteDriverService.commitTransaction();
                    } else {
                        sqliteDriverService.rollbackTransaction();
                    }
                }
            }
        }/* synchronized */

        if (LOG.isInfoEnabled()) {
            LOG.info("End of update(" + fullPath + ", ...)");
        }
    }

    public void delete(String fullPath) throws StorageException {
        delete(fullPath, StorageRequestExecutor.CASCADING_NONE, false);
    }

    public void delete(String path, int cascade) throws StorageException {
        delete(path, cascade, false);
    }

    public void delete(Map config, String path) throws StorageException {
        // TODO recreate the method with using config
        delete(path, StorageRequestExecutor.CASCADING_NONE, false);
    }

    public void delete(Map config, String path, int cascade) throws StorageException {
        // TODO Auto-generated method stub
        delete(path, cascade, false);
    }

    public void delete(String path, int cascade, boolean trans) throws StorageException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Begin of delete(PATH=" + path + ", CASCADE=" + cascade + ", TRANS=" + trans + ")");
        }

        boolean error = false;
        try {
            if (!trans) {
                sqliteDriverService.beginTransaction();
            }
            int numberOfAffectedRows = sqliteDriverService.sqliteDelete(path, cascade);
            if (numberOfAffectedRows == 0) {
                throw new StorageException("There isn't such a document");
            }
        } catch (RuntimeException e) {
            error = true;
            throw e;
        } catch (StorageException e) {
            error = true;
            throw e;
        } finally {
            if (!trans) {
                if (!error) {
                    sqliteDriverService.commitTransaction();
                } else {
                    sqliteDriverService.rollbackTransaction();
                }
            }
        }
    }

    public String reserveSpace(Map config, String path, String docNumber, double docSize) {
        return null;
    }

    // Creates new object of type transaction and begin a transaction.
    public Transaction beginTransaction() throws StorageException {
        synchronized (LOCK) {
            // BUG CORRECTION: give the reference on the openDB instance, and not the DB file name in order not to re-open a new
            // connection for each transaction
            Transaction newTransaction = new TransactionImpl(sqliteDriverService);

            return newTransaction;
        }
    }

}
