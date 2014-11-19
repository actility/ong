package com.actility.m2m.storage.driver.sqlite.impl;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.Dictionary;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.apache.log4j.Logger;
import org.osgi.framework.BundleContext;

import com.actility.m2m.storage.AttributeOperation;
import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.DefaultDeletionHandler;
import com.actility.m2m.storage.DeletionHandler;
import com.actility.m2m.storage.Document;
import com.actility.m2m.storage.SearchResult;
import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.storage.driver.Transaction;


public final class SQLiteRequestExecutor {
    private final static Logger LOG = Logger.getLogger(SQLiteRequestExecutor.class);
    private final static SimpleDateFormat FORMATER = new SimpleDateFormat("yyyy-MM-dd'T'HH:mm:ss.SSS");

    private String actualDBFileName;
    private InternalAttribute[] attributesJavaBackup;
    private InternalAttrOperation[] operationsJavaBackup;
    private List/* <InternalCondition> */conditionsJavaBackup;
    private int internalConditionLength = 0;
    private DeletionHandler deletionHandler;
    public SqliteDB openedDB;

    /**
     * Class constructor
     *
     * @param openedConnToDB the sqlite database
     */
    public SQLiteRequestExecutor(SqliteDB openedConnToDB) {
        this.openedDB = openedConnToDB;
    }

    /**
     * Class constructor
     *
     * @param bundleContext
     * @param config storage configuration to apply
     * @throws StorageException if any problem occurs while activate the driver
     */
    public SQLiteRequestExecutor(Dictionary config) throws StorageException {
        if (LOG.isInfoEnabled()) {
            LOG.info("Begin of start()");
        }
        boolean dbExists = false;
        actualDBFileName = Util.DEFAULT_DB_FILE_NAME;
        Object fileNameConfig = config.get("fileName");
        if (fileNameConfig != null && fileNameConfig instanceof String) {
            actualDBFileName = (String) fileNameConfig;
        } else {
            LOG.warn("using default db file name");
        }
        // Test if file exists
        File dbFile = new File(actualDBFileName);

        dbExists = dbFile.exists();
        LOG.info(new StringBuffer("dbFile: ").append(dbFile.getAbsolutePath()));
        if (!dbExists) {
            LOG.warn(new StringBuffer("creating database ").append(actualDBFileName));
             if(actualDBFileName.lastIndexOf('/')!=-1){
                 File folder = new File(actualDBFileName.substring(0, actualDBFileName.lastIndexOf('/')));
                 if(!folder.exists() || !folder.isDirectory()){
                     LOG.error("Directory of database file not exist or is not a directory "/*+folder.getAbsolutePath()*/);
                 throw new
                 StorageException("Directory of database file not exist or is not a directory "/*+folder.getAbsolutePath()*/);
                 }
             }
        }
        openedDB = sqlitedriver.SqliteOpen(actualDBFileName);
        if (openedDB == null) {
            LOG.error("The database can not be opened");
        }

        try {
            // read and execute script
            if (!dbExists) {
                // make a copy of the database model and execute the sql script of creation
                readSqlScript();
            }
        } catch (IOException e) {
            e.printStackTrace();
        }
        // Check database version
        try {
            checkDBVersion();
        } catch (StorageException e) {
            LOG.error(e.toString());
        }

        // turn on auto_vacuum incremental
        sqlitedriver.activate(openedDB);

        sqlitedriver.setMaxPageCount(openedDB, Util.MAX_PAGE_COUNT);

        if (deletionHandler == null) {
            deletionHandler = new DefaultDeletionHandler();
        }
        if (LOG.isInfoEnabled()) {
            LOG.info("End of start()");
        }
    }

    /**
     * Convert a list of conditions to a tab of conditions readable in C
     *
     * @param conditions a list of conditions
     * @return internal condition table
     */
    private SWIGTYPE_p_p_InternalCondition convertToInternalCondition(List conditions) {
        if (conditions == null) {
            return null;
        }
        SWIGTYPE_p_p_InternalCondition internalConditions = sqlitedriver.new_condition_array(conditions.size());
        for (int i = 0; i < conditions.size(); i++) {
            InternalCondition internalCondition = convertToInternalCondition((Condition) conditions.get(i), false);
            sqlitedriver.condition_array_setitem(internalConditions, i, internalCondition);
        }
        internalConditionLength = conditions.size();
        return internalConditions;
    }

    /**
     * Convert a condition to a condition readable in C
     *
     * @param conditions the document condition
     * @return internal condition
     */
    private InternalCondition convertToInternalCondition(Condition condition, boolean firstTime) {
        if (condition == null) {
            return null;
        }
        if (firstTime == true) {
            conditionsJavaBackup = new LinkedList();
        }
        InternalCondition internalCondition = new InternalCondition();

        String attrName = condition.getAttributeName();
        internalCondition.setAttributeName(attrName);
        internalCondition.setOper(condition.getOperator());

        Object operand = condition.getOperand();
        if (operand == null) {
            internalCondition.setConditions(convertToInternalCondition(condition.getConditions()));
            internalCondition.setLength(internalConditionLength);
        } else {
            if (operand instanceof String) {
                internalCondition.setOperand((String) operand);
                internalCondition.setType(Util.ATTR_TYPE_STRING);
            } else if (operand instanceof Integer) {
                internalCondition.setOperand(((Integer) operand).toString());
                internalCondition.setType(Util.ATTR_TYPE_INTEGER);
            } else {
                String formattedDate = FORMATER.format(((Date) operand));
                internalCondition.setOperand(formattedDate);
                internalCondition.setType(Util.ATTR_TYPE_DATE);
            }
        }
        conditionsJavaBackup.add(internalCondition);
        return internalCondition;
    }

    /**
     * Convert an iterator of attributes to a tab of attributes readable in C
     *
     * @param searchAttributes the iterator of attributes
     * @return the internal table of attributes
     * @throws StorageException if any problem occurs while convert attributes
     */
    private SWIGTYPE_p_p_InternalAttribute convertToInternalAttributes(Iterator searchAttributes) throws StorageException {
        List listInternal = new LinkedList();
        int index = 0;
        while (searchAttributes.hasNext()) {
            Object searchAttribute = searchAttributes.next();
            if (searchAttribute instanceof Entry) {
                String attributeName = (String) ((Entry) searchAttribute).getKey();
                Object attributeValue = ((Entry) searchAttribute).getValue();
                InternalAttribute attribute = new InternalAttribute();
                attribute.setName(attributeName);
                if (attributeValue instanceof String) {
                    attribute.setString_value((String) attributeValue);
                    attribute.setType(Util.ATTR_TYPE_STRING);
                    listInternal.add(attribute);
                    index++;
                } else if (attributeValue instanceof Integer) {
                    attribute.setInt_value(((Integer) attributeValue).intValue());
                    attribute.setType(Util.ATTR_TYPE_INTEGER);
                    listInternal.add(attribute);
                    index++;
                } else if (attributeValue instanceof List) {
                    List attributeList = (List) attributeValue;
                    for (int i = 0; i < attributeList.size(); i++) {
                        attribute = new InternalAttribute();
                        attribute.setName(attributeName);
                        if (attributeList.get(i) instanceof String) {
                            attribute.setString_value((String) attributeList.get(i));
                            attribute.setType(Util.ATTR_TYPE_LIST_STRING);
                        } else if (attributeList.get(i) instanceof Integer) {
                            attribute.setInt_value(((Integer) attributeList.get(i)).intValue());
                            attribute.setType(Util.ATTR_TYPE_LIST_INTEGER);
                        } else if (attributeList.get(i) instanceof Date) {
                            attribute.setDate_value(FORMATER.format(((Date) attributeList.get(i))));
                            attribute.setType(Util.ATTR_TYPE_LIST_DATE);
                        } else {
                            throw new StorageException("Search attributes embed an object which is not a valid attribute: "
                                    + attributeList.get(i) + " (" + attributeList.get(i).getClass() + ")");
                        }
                        listInternal.add(attribute);
                        index++;
                    }
                } else if (attributeValue instanceof Date) {
                    attribute.setDate_value(FORMATER.format(((Date) attributeValue)));
                    attribute.setType(Util.ATTR_TYPE_DATE);
                    listInternal.add(attribute);
                    index++;
                } else {
                    if (attributeValue != null) {
                        throw new StorageException("Search attributes embed an object which is not a valid attribute: "
                                + attributeValue + " (" + attributeValue.getClass() + ")");
                    }
                }
            } else {
                throw new StorageException("Search attributes embed an object which is not a pair: " + searchAttribute + " ("
                        + searchAttribute.getClass() + ")");
            }
        }
        SWIGTYPE_p_p_InternalAttribute attributes = sqlitedriver.new_attribute_array(index);
        attributesJavaBackup = new InternalAttribute[index];
        Iterator internalIterator = listInternal.iterator();
        int counter = 0;
        while (internalIterator.hasNext()) {
            InternalAttribute internal = (InternalAttribute) internalIterator.next();
            attributesJavaBackup[counter] = internal;
            sqlitedriver.attribute_array_setitem(attributes, counter, internal);
            counter++;
        }
        return attributes;
    }

    /**
     * Convert a map of storage configuration to an object readable in C
     *
     * @param config storage configuration to apply
     * @return the object readable in C
     */
    private ConfigList internalGetConfigList(Map config) {
        if (config == null) {
            return null;
        }
        int size = config.size();
        SWIGTYPE_p_p_ConfigItem swigConfigItem = sqlitedriver.new_configItem_array(size);
        Set entrySet = config.entrySet();

        Iterator entryIterator = entrySet.iterator();
        int i = -1;
        while (entryIterator.hasNext()) {
            i++;
            Entry entry = (Entry) entryIterator.next();

            ConfigItem item = new ConfigItem();
            item.setKey((String) entry.getKey());
            item.setValue((String) entry.getValue());

            sqlitedriver.configItem_array_setitem(swigConfigItem, i, item);
        }

        ConfigList configList = new ConfigList();
        configList.setItems(swigConfigItem);
        configList.setLength(size);
        return configList;
    }

    /**
     * Convert a list of operations to a table of operations readable in C
     *
     * @param operations the list operations to convert
     * @return the table of operations readable in C
     */
    private SWIGTYPE_p_p_InternalAttrOperation convertAttributeOperationToInternal(List operations) {
        if (operations == null) {
            return null;
        }
        int counter = 0;
        for (int i = 0; i < operations.size(); i++) {
            if (((AttributeOperation) operations.get(i)).getValue() == null) {
                counter++;
            } else if (((AttributeOperation) operations.get(i)).getValue() instanceof List) {
                counter += ((List) ((AttributeOperation) operations.get(i)).getValue()).size();
            } else {
                counter++;
            }
        }
        SWIGTYPE_p_p_InternalAttrOperation list = sqlitedriver.new_operation_array(counter);
        operationsJavaBackup = new InternalAttrOperation[counter];
        counter = 0;
        for (int i = 0; i < operations.size(); i++) {
            InternalAttrOperation internalOps = new InternalAttrOperation();

            InternalAttribute attribute = new InternalAttribute();
            AttributeOperation attrOps = (AttributeOperation) operations.get(i);
            internalOps.setType(attrOps.getType());
            attribute.setName(attrOps.getName());
            Object value = attrOps.getValue();
            if (value != null) {
                if (value instanceof String) {
                    attribute.setString_value((String) value);
                    attribute.setType(Util.ATTR_TYPE_STRING);
                } else if (value instanceof Integer) {
                    attribute.setInt_value(((Integer) value).intValue());
                    attribute.setType(Util.ATTR_TYPE_INTEGER);
                } else if (value instanceof List) {
                    Iterator attrIterator = ((List) value).iterator();
                    while (attrIterator.hasNext()) {
                        Object listObject = attrIterator.next();
                        InternalAttrOperation myInternalOps = new InternalAttrOperation();
                        InternalAttribute myAttribute = new InternalAttribute();
                        myInternalOps.setType(attrOps.getType());
                        myAttribute.setName(attrOps.getName());
                        if (listObject instanceof String) {
                            myAttribute.setString_value((String) listObject);
                            myAttribute.setType(Util.ATTR_TYPE_LIST_STRING);
                        } else if (listObject instanceof Integer) {
                            myAttribute.setInt_value(((Integer) listObject).intValue());
                            myAttribute.setType(Util.ATTR_TYPE_LIST_INTEGER);
                        } else {
                            myAttribute.setDate_value(FORMATER.format(((Date) listObject)));
                            myAttribute.setType(Util.ATTR_TYPE_LIST_DATE);
                        }
                        myInternalOps.setAttribute(myAttribute);
                        operationsJavaBackup[counter] = myInternalOps;
                        sqlitedriver.operation_array_setitem(list, counter, myInternalOps);
                        ++counter;
                    }
                } else {
                    attribute.setDate_value(FORMATER.format(((Date) value)));
                    attribute.setType(Util.ATTR_TYPE_DATE);
                }
            }
            if (value == null || !(value instanceof List)) {
                internalOps.setAttribute(attribute);
                operationsJavaBackup[counter] = internalOps;
                sqlitedriver.operation_array_setitem(list, counter, internalOps);
                counter++;
            }
        }
        return list;
    }

    /**
     * Convert a result of a search to document
     *
     * @param subResult the result of a search
     * @return The document
     * @throws ParseException if any problem occurs while convert attributes
     */
    private Document convertSubSearchResultToDocument(subSearchResult subResult) throws ParseException {
        Document doc = new DocumentImpl(new Long(subResult.getId()), new StringBuffer(subResult.getPath()).append(
                subResult.getName()).toString());
        doc.setContent(subResult.getContent().length == 0 ? null : subResult.getContent());
        SWIGTYPE_p_p_InternalAttribute attributes = subResult.getAttributes();
        if (subResult.getAttr_count() == 0) {
        }
        for (int i = 0; i < subResult.getAttr_count(); i++) {
            InternalAttribute attr = sqlitedriver.attribute_array_getitem(attributes, i);
            switch (attr.getType()) {
            case Util.ATTR_TYPE_STRING:
                doc.setAttribute(attr.getName(), attr.getString_value());
                break;
            case Util.ATTR_TYPE_INTEGER:
                doc.setAttribute(attr.getName(), new Integer(attr.getInt_value()));
                break;
            case Util.ATTR_TYPE_DATE:
                Date date = FORMATER.parse(attr.getDate_value());
                doc.setAttribute(attr.getName(), date);
                break;
            case Util.ATTR_TYPE_LIST_STRING:
                List list = doc.getListAttribute(attr.getName());
                if (list != null) {
                    list.add(attr.getString_value());
                } else {
                    list = new LinkedList();
                    list.add(attr.getString_value());
                    doc.setAttribute(attr.getName(), list);
                }
                break;
            case Util.ATTR_TYPE_LIST_INTEGER:
                List list2 = doc.getListAttribute(attr.getName());
                if (list2 != null) {
                    list2.add(new Integer(attr.getInt_value()));
                } else {
                    list2 = new LinkedList();
                    list2.add(new Integer(attr.getInt_value()));
                    doc.setAttribute(attr.getName(), list2);
                }
                break;
            case Util.ATTR_TYPE_LIST_DATE:
                Date date2 = FORMATER.parse(attr.getDate_value());
                List list3 = doc.getListAttribute(attr.getName());
                if (list3 != null) {
                    list3.add(date2);
                } else {
                    list3 = new LinkedList();
                    list3.add(date2);
                    doc.setAttribute(attr.getName(), list3);
                }
                break;
            }
        }
        return doc;
    }

    /**
     * Reserves space for the creation of future documents
     *
     * @param config StorageConfiguration to apply
     * @param path path under which documents are going to be created
     * @param docNumber maximum number of documents that can be created in reserved space
     * @param docSize maximum size of documents that can be created in reserved space
     * @return A reservation code
     */
    public String reserveSpace(Map config, String path, String docNumber, double docSize) {
        return null;
    }

    /**
     * Begin a transaction. Raises an exception if transaction cannot be opened
     *
     * @return an instance of class Transaction
     * @throws StorageException if transaction cannot be opened
     */
    public Transaction beginTransaction() throws StorageException {
        if (LOG.isInfoEnabled()) {
            LOG.info("begin transaction");
        }
        if (isFullDB()) {
            throw new StorageException(new StringBuffer().append(Util.PERCENT_SPACE_TAKEN)
                    .append("% of the database pages are exceeded").toString());
        }
        return new TransactionImpl(this);
    }

    /**
     * Create an SQL transaction. Manage the writable access of the SQLite database
     *
     * @throws StorageException if the transaction cannot be created
     */
    public void createTransaction() throws StorageException {
        if (LOG.isInfoEnabled()) {
            LOG.info("createTransaction");
        }
        SQLiteSemaphore.getInstance().getSemaphore();
        int sqliteCode = sqlitedriver.beginTransaction(openedDB);
        if (sqliteCode != sqlitedriverConstants.SQLITE_OK) {
            SQLiteSemaphore.getInstance().releaseSemaphore();
            throw new StorageException("Cannot create the transaction, response code:(" + sqliteCode + ")");
        }
    }

    /**
     * Commit the SQL transaction.
     *
     * @throws StorageException if the transaction cannot be commit
     */
    public void commitTransaction() throws StorageException {
        if (LOG.isInfoEnabled()) {
            LOG.info("commitTransaction");
        }
        int sqliteCode = sqlitedriver.commitTransaction(openedDB);
        if (sqliteCode != sqlitedriverConstants.SQLITE_OK) {
            throw new StorageException("Cannot commit the transaction, response code:(" + sqliteCode + ")");
        } else {
            SQLiteSemaphore.getInstance().releaseSemaphore();
        }
    }

    /**
     * Rollback the SQL transaction.
     *
     * @throws StorageException if the transaction cannot be rollback.
     */
    public void rollbackTransaction() throws StorageException {
        if (LOG.isInfoEnabled()) {
            LOG.info("rollbackTransaction");
        }
        int sqliteCode = sqlitedriver.rollbackTransaction(openedDB);
        if (sqliteCode != sqlitedriverConstants.SQLITE_OK) {
            throw new StorageException("Cannot rollback the transaction, response code:(" + sqliteCode + ")");
        } else {
            SQLiteSemaphore.getInstance().releaseSemaphore();
        }
    }

    /**
     * Check if the database is full
     *
     * @return Return true if the database if full else return false;
     */
    public boolean isFullDB() {
        int[] pageCount = { 0 };
        sqlitedriver.getPageCount(openedDB, pageCount);

        int[] freeListCount = { 0 };
        sqlitedriver.getFreeListCount(openedDB, freeListCount);

        return (pageCount[0] + freeListCount[0]) > Util.MAX_PAGE_COUNT * Util.PERCENT_SPACE_TAKEN;
    }

    /**
     * Link the deletionHandler to the delete function
     *
     * @param deletionHandler to link
     */
    public void bindDeletionHandler(DeletionHandler deletionHandler) {
        this.deletionHandler = deletionHandler;
    }

    /**
     * UnLink the deletionHandler to the delete function
     *
     * @param deletionHandler to unlink
     */
    public void unbindDeletionHandler(DeletionHandler deletionHandler) {
        if (this.deletionHandler == deletionHandler) {
            this.deletionHandler = new DefaultDeletionHandler();
        }
    }

    /**
     * Read the SQL file which can generate the database
     *
     * @throws IOException if any problem occurs while read the SQL script
     */
    private void readSqlScript() throws IOException {
        LOG.debug("Executing db creation script");
        InputStream in = this.getClass().getResourceAsStream("/sql/createGsc.sql");
        Reader otherReader = new InputStreamReader(in);
        BufferedReader br = new BufferedReader(otherReader);
        StringBuffer statement = new StringBuffer();
        String line = br.readLine();
        while (line != null) {
            LOG.debug(line);
            if (!line.startsWith("--")) {
                statement.append(line);
                if (line.endsWith(";")) {
                    sqlitedriver.executeSqlStatementWithOutParameters(openedDB, statement.toString());
                    statement = new StringBuffer();
                }
            }
            line = br.readLine();
        }
    }

    /**
     * Check if the database has the last version number and if not, upgrade it
     *
     * @throws StorageException if any problem occurs
     */
    private void checkDBVersion() throws StorageException {
        int[] major = { 0 };
        int[] minor = { 0 };
        int code = sqlitedriver.checkDBVersion(openedDB, major, minor);
        if (code == 101) {
            int majorDB = major[0];
            int minorDB = minor[0];
            if (Util.MAJOR_VERSION > majorDB || (Util.MAJOR_VERSION == majorDB && Util.MINOR_VERSION > minorDB)) {
                BundleVersionInfo.upgradeTo(openedDB, majorDB, minorDB);
            } else if (Util.MAJOR_VERSION == majorDB && Util.MINOR_VERSION < minorDB) {
                LOG.error("The DB's minor version can not be bigger than internal minor version!");
                close();
                throw new RuntimeException();
            } else if (Util.MAJOR_VERSION < majorDB) {
                LOG.error("The DB's major version can not be bigger than internal major version!");
                close();
                throw new RuntimeException();
            }
        }
    }

    /**
     * Close the database connection.
     */
    private void close() {
        if (LOG.isInfoEnabled()) {
            LOG.info("Begin of stop()");
        }
        if (openedDB != null) {
            sqlitedriver.SqliteClose(openedDB);
            openedDB = null;
        }
        if (LOG.isInfoEnabled()) {
            LOG.info("End of stop()");
        }
    }

    public void close(BundleContext context) {
        close();
    }

    /**
     * Retrieves a document
     *
     * @param config storage configuration to apply
     * @param path the full (absolute) path of the document (it must not end with a slash and must be normalized)
     * @param id Internal id of the document to retrieve
     * @param condition condition that must be fulfilled in order to perform the operation
     * @return The retrieved document or <code>null</code> if the document does not exist or the condition is not fulfilled
     * @throws StorageException if any problem occurs while retrieving the document
     */
    private Document retrieve(Map config, String path, long id, Condition condition) throws StorageException {
        if (LOG.isDebugEnabled()) {
            LOG.debug("retrieve: in (SQLiteRequestExecutor)");
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug("test of Path");
        }
        Document doc = null;
        if (path == null && id <= 0) {
            throw new StorageException("The Document path can not be null");
        }
        if (path != null) {
            if (!path.equals("")) {
                boolean testSlash = false;
                for (int i = 0; i < path.length(); i++) {
                    if (path.charAt(i) == '/') {
                        testSlash = true;
                        break;
                    }
                }
                if (testSlash == false) {
                    throw new StorageException("The Document path not contains slash");
                }
                if (path.charAt(path.length() - 1) == '/') {
                    throw new StorageException("The last character of Document path can not contain slash");
                }
            }
        }
        if (LOG.isInfoEnabled()) {
            LOG.info("retrieve (path:" + path + ") (condition:" + Util.conditionToString(condition) + ")");
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug("Convert condition to internal");
        }
        InternalCondition cond = convertToInternalCondition(condition, true);
        if (LOG.isDebugEnabled()) {
            LOG.debug("Convert configList to internal");
        }
        ConfigList configList = internalGetConfigList(config);
        if (LOG.isDebugEnabled()) {
            LOG.debug("Call C method");
        }
        SearchReturn returnedData = sqlitedriver.sqliteRetrieve(openedDB, configList, path, id, cond);
        if (LOG.isDebugEnabled()) {
            LOG.debug("End of call. Test result");
        }
        if (returnedData != null) {
            if (LOG.isDebugEnabled()) {
                LOG.debug("Get subresult");
            }
            SWIGTYPE_p_p_subSearchResult results = returnedData.getData();
            if (returnedData.getSize() > 0) {
                subSearchResult subResult = sqlitedriver.search_array_getitem(results, 0);
                try {
                    if (LOG.isDebugEnabled()) {
                        LOG.debug("Convert result to external");
                    }
                    doc = convertSubSearchResultToDocument(subResult);
                } catch (ParseException e) {
                    throw new StorageException(e.getMessage());
                }
            }
        }
        if (LOG.isDebugEnabled()) {
            LOG.debug("retrieve: end");
        }
        return doc;
    }

    /**
     * Retrieves a document
     *
     * @param config storage configuration to apply
     * @param path the full (absolute) path of the document (it must not end with a slash and must be normalized)
     * @param condition condition that must be fulfilled in order to perform the operation
     * @return The retrieved document or <code>null</code> if the document does not exist or the condition is not fulfilled
     * @throws StorageException if any problem occurs while retrieving the document
     */
    public Document retrieve(Map config, String path, Condition condition) throws StorageException {
        return retrieve(config, path, 0, condition);
    }

    /**
     * Retrieves a document
     *
     * @param config storage configuration to apply
     * @param id Internal id of the document to retrieve
     * @param condition condition that must be fulfilled in order to perform the operation
     * @return The retrieved document or <code>null</code> if the document does not exist or the condition is not fulfilled
     * @throws StorageException if any problem occurs while retrieving the document
     */
    public Document retrieve(Map config, Object id, Condition condition) throws StorageException {
        try {
            long _id = ((Long) id).longValue();
            return retrieve(config, null, _id, condition);
        } catch (ClassCastException e) {
            throw new StorageException(e.getMessage());
        }

    }

    /**
     * Updates an existing document.
     * <p>
     * Raises an exception if the document does not exist.
     *
     * @param config storage configuration to apply
     * @param document the new representation of the document
     * @param condition condition that must be fulfilled in order to perform the operation
     * @return Whether the update has succeeded. If the update fails, it means the document does not exist in the storage or the
     *         condition is not fulfilled
     * @throws StorageException if any problem occurs while retrieving the document
     */
    public boolean update(Map config, Document document, Condition condition) throws StorageException {
        if (document == null) {
            throw new StorageException("Document can not be null");
        }
        if (document.getPath() == null) {
            throw new StorageException("The Document path can not be null");
        }
        if (document.getAttributes().hasNext() == false) {
            throw new StorageException("Document must have attributes");
        }
        String path = document.getPath();
        if (!path.equals("")) {
            boolean testSlash = false;
            for (int i = 0; i < path.length(); i++) {
                if (path.charAt(i) == '/') {
                    testSlash = true;
                    break;
                }
            }
            if (testSlash == false) {
                throw new StorageException("The Document path not contains slash");
            }
            if (path.charAt(path.length() - 1) == '/') {
                throw new StorageException("The last character of Document path can not contain slash");
            }
        }
        // if(condition==null){
        // throw new StorageException("Condition is null");
        // }
        String fullPath = document.getPath();
        long id = 0;
        boolean res = false;

        try {

            id = (document.getId() == null ? 0 : ((Long) document.getId()).longValue());
        } catch (ClassCastException e) {
            throw new StorageException(e.getMessage());
        }
        if (LOG.isInfoEnabled()) {
            LOG.info("Update (document:" + Util.documentToString(document) + ") (condition:"
                    + Util.conditionToString(condition) + ")");
        }

        if (isFullDB()) {
            throw new StorageException(new StringBuffer().append(Util.PERCENT_SPACE_TAKEN)
                    .append("% of the database pages are exceeded").toString());
        }

        SWIGTYPE_p_p_InternalAttribute attrs = null;

        attrs = convertToInternalAttributes(document.getAttributes());
        int attrsLength = attributesJavaBackup.length;
        ConfigList configList = internalGetConfigList(config);
        InternalCondition cond = convertToInternalCondition(condition, true);
        int numberOfAffectedRows = sqlitedriver.sqliteUpdate(openedDB, configList, fullPath, id,
                (document.getContent() == null ? new byte[] {} : document.getContent()), (document.getContent() == null ? -1
                        : document.getContent().length), attrs, attrsLength, cond);

        if (numberOfAffectedRows < 0) {
            throw new StorageException("There isn't such a document");
        }
        res = numberOfAffectedRows > 0;

        if (LOG.isInfoEnabled()) {
            LOG.info(new StringBuffer().append("End of update(").append(fullPath).append(", ...)").toString());
        }
        return res;
    }

    /**
     * Updates partially an existing document.
     * <p>
     * Raises an exception if the document does not exist.
     *
     * @param config storage configuration to apply
     * @param document the document to partially update. Only id or path are used
     * @param content the new content for the document (if <code>null</code>, do not update the content)
     * @param attrOps operations to perform on the document attributes (if <code>null</code> or empty, do not update attributes)
     * @param condition condition that must be fulfilled in order to perform the operation
     * @return Whether the partial update has succeeded. If the partial update fails, it means the document does not exist or
     *         the condition is not fulfilled
     * @throws StorageException if any problem occurs while retrieving the document
     */
    public boolean partialUpdate(Map config, Document document, byte[] content, List attrOps, Condition condition)
            throws StorageException {
        if (document == null) {
            throw new StorageException("Document can not be null");
        }
        if (document.getPath() == null) {
            throw new StorageException("The Document path can not be null");
        }
        String path = document.getPath();
        if (!path.equals("")) {
            boolean testSlash = false;
            for (int i = 0; i < path.length(); i++) {
                if (path.charAt(i) == '/') {
                    testSlash = true;
                    break;
                }
            }
            if (testSlash == false) {
                throw new StorageException("The Document path not contains slash");
            }
            if (path.charAt(path.length() - 1) == '/') {
                throw new StorageException("The last character of Document path can not contain slash");
            }
        }
        if (attrOps == null && content == null) {
            return false;
        }
        String fullPath = document.getPath();
        long id = 0;
        boolean res = false;

        if (LOG.isInfoEnabled()) {
            LOG.info("partialUpdate: old:(document:" + Util.documentToString(document) + "),new:(content length:"
                    + ((content != null) ? content.length : -1) + ") (attrOps:" + Util.attrOpsToString(attrOps)
                    + ") (condition:" + Util.conditionToString(condition) + ")");
        }

        try {
            id = (document.getId() == null ? 0 : ((Long) document.getId()).longValue());
        } catch (ClassCastException e) {
            throw new StorageException(e.getMessage());
        }

        if (isFullDB()) {
            throw new StorageException(new StringBuffer().append(Util.PERCENT_SPACE_TAKEN)
                    .append("% of the database pages are exceeded").toString());
        }

        ConfigList configList = internalGetConfigList(config);
        InternalCondition cond = convertToInternalCondition(condition, true);
        SWIGTYPE_p_p_InternalAttrOperation internalAttrOps = null;
        try {
            internalAttrOps = convertAttributeOperationToInternal(attrOps);
        } catch (ClassCastException e) {
            throw new StorageException("Fail into convert" + e.getMessage());
        }

        byte[] internalContent = (content == null ? new byte[] {} : content);
        int numberOfAffectedRows = sqlitedriver.sqlitePartialUpdate(openedDB, configList, fullPath, id, internalContent,
                (content == null ? -1 : content.length), internalAttrOps, (attrOps == null ? 0 : operationsJavaBackup.length),
                cond);

        if (numberOfAffectedRows < 0) {
            throw new StorageException("There isn't such a document");
        }
        res = numberOfAffectedRows > 0;

        if (LOG.isInfoEnabled()) {
            LOG.info(new StringBuffer().append("End of partialUpdate(").append(fullPath).append(", ...)").toString());
        }
        return res;
    }

    /**
     * Creates a new document.
     * <p>
     * Raises an exception if the document already exists.
     *
     * @param config storage configuration to apply
     * @param document the representation of the document
     * @return Whether the create has succeeded. If the create fails, it means the document already exists
     * @throws StorageException if any problem occurs while retrieving the document
     */
    public boolean create(Map config, Document document) throws StorageException {
        if (document == null) {
            throw new StorageException("Document can not be null");
        }
        if (document.getPath() == null) {
            throw new StorageException("The Document path can not be null");
        }
        String path = document.getPath();
        if (!path.equals("")) {
            boolean testSlash = false;
            for (int i = 0; i < path.length(); i++) {
                if (path.charAt(i) == '/') {
                    testSlash = true;
                    break;
                }
            }
            if (testSlash == false) {
                throw new StorageException("The Document path not contains slash");
            }
            if (path.charAt(path.length() - 1) == '/') {
                throw new StorageException("The last character of Document path can not contain slash");
            }
        }

        if (LOG.isInfoEnabled()) {
            LOG.info("create (document:" + Util.documentToString(document) + ")");
        }
        if (isFullDB()) {
            throw new StorageException(new StringBuffer().append(Util.PERCENT_SPACE_TAKEN)
                    .append("% of the database pages are exceeded").toString());
        }

        int sqlCode = 0;
        SWIGTYPE_p_p_InternalAttribute attrs = null;
        Iterator attributes = document.getAttributes();
        if (attributes != null) {
            attrs = convertToInternalAttributes(attributes);
            attributes = document.getAttributes();
        }
        int attrsLength = attributesJavaBackup.length;
        ConfigList configList = internalGetConfigList(config);
        long[] doc_id = { 0 };
        sqlCode = sqlitedriver.sqliteCreate(openedDB, configList, document.getPath(),
                (document.getContent() == null ? new byte[] {} : document.getContent()), (document.getContent() == null ? -1
                        : document.getContent().length), attrs, attrsLength, doc_id);

        if (sqlCode != 101 && sqlCode != 0) {
            throw new StorageException(new StringBuffer().append("Sqlite code: ").append(sqlCode).toString());
        }

        if (LOG.isInfoEnabled()) {
            LOG.info(new StringBuffer().append("End of create(").append(document.getPath()).append(", ...)").toString());
        }
        return (sqlCode == 101);
    }

    /**
     * Deletes a document.
     * <p>
     * If scope is specified to:
     * <ul>
     * <li>{@link #SCOPE_ONE_LEVEL}: deletes documents one level below the given path (this excludes the exact path). Delete do
     * not manage concurrency and any created document created ONE_LEVEL below the given document during the deletion process
     * will not be deleted</li>
     * <li>{@link #SCOPE_SUB_TREE}: deletes documents reachable from the root document pointed by the given path (this includes
     * the exact path). Delete SUB_TREE deletes the root document at the end to ensure it is not possible to re-create a
     * sub-tree while deleting it. So after the delete sub-tree operation you are sure there is no documents in that path
     * anymore</li>
     * </ul>
     * <p>
     * Raises an exception if the document does not exist.
     *
     * @param config storage configuration to apply
     * @param document The document which serves as a root for the deletion. Only id or path are used
     * @param scope a constant ({@link #SCOPE_ONE_LEVEL} or {@link #SCOPE_SUB_TREE})
     * @param condition condition that must be fulfilled in order to perform the operation
     * @return Whether the delete has succeeded. If the delete fails, it means the document does not exist or the condition is
     *         not fulfilled
     * @throws StorageException if any problem occurs while retrieving the document
     */
    public boolean delete(Map config, Document document, int scope, Condition condition) throws StorageException {
        if (document == null) {
            throw new StorageException("Document can not be null");
        }
        if (document.getPath() == null) {
            throw new StorageException("The Document path can not be null");
        }
        String path = document.getPath();
        if (!path.equals("")) {
            boolean testSlash = false;
            for (int i = 0; i < path.length(); i++) {
                if (path.charAt(i) == '/') {
                    testSlash = true;
                    break;
                }
            }
            if (testSlash == false) {
                throw new StorageException("The Document path not contains slash");
            }
            if (path.charAt(path.length() - 1) == '/') {
                throw new StorageException("The last character of Document path can not contain slash");
            }
        }
        long id = 0;

        try {
            id = (document.getId() == null ? 0 : ((Long) document.getId()).longValue());
        } catch (ClassCastException e) {
            throw new StorageException(e.getMessage());
        }
        if (LOG.isInfoEnabled()) {
            LOG.info("delete (document:" + Util.documentToString(document) + ")");
        }
        int numberOfAffectedRows = 0;

        ConfigList configList = internalGetConfigList(config);
        InternalCondition cond = convertToInternalCondition(condition, true);
        boolean verif = false;
        if (condition != null) {
            Document tempDocument = this.retrieve(config, document.getPath(), condition);
            if (tempDocument != null) {
                verif = true;
            }
        } else {
            verif = true;
        }
        if (verif == true) {
            SearchResult deleteResult = this.search(config, document.getPath(), scope, null, StorageRequestExecutor.ORDER_ASC,
                    StorageRequestExecutor.NO_LIMIT, true, null);
            Iterator deleteIterator = deleteResult.getResults();
            while (deleteIterator.hasNext()) {
                Document returnDoc = (Document) deleteIterator.next();
                if (!returnDoc.getPath().equals(path)) {
                    Object notify = returnDoc.getAttribute(Document.ATTR_NOTIFY_WHEN_DELETED);
                    if (notify != null && notify instanceof Integer) {
                        int notifyValue = ((Integer) notify).intValue();
                        if (notifyValue == 1) {
                            deletionHandler.deleted(returnDoc);
                        }
                    }
                }
            }
            deleteResult.close();
        }
        numberOfAffectedRows = sqlitedriver.sqliteDeleteCascade(openedDB, configList, document.getPath(), id, scope, cond);

        if (numberOfAffectedRows < 0) {
            throw new StorageException(new StringBuffer("Error in delete ").append(numberOfAffectedRows).toString());
        }
        return numberOfAffectedRows > 0;
    }

    /**
     * Deletes a document.
     * <p>
     * Raises an exception if the document does not exist.
     *
     * @param config storage configuration to apply
     * @param document The document to delete. Only id or path are used
     * @param condition condition that must be fulfilled in order to perform the operation
     * @return Whether the delete has succeeded. If the delete fails, it means the document does not exist or the condition is
     *         not fulfilled
     * @throws StorageException if any problem occurs while retrieving the document
     */
    public boolean delete(Map config, Document document, Condition condition) throws StorageException {
        long id = 0;
        if (document == null) {
            throw new StorageException("Document can not be null");
        }
        if (document.getPath() == null) {
            throw new StorageException("The Document path can not be null");
        }
        String path = document.getPath();
        if (!path.equals("")) {
            boolean testSlash = false;
            for (int i = 0; i < path.length(); i++) {
                if (path.charAt(i) == '/') {
                    testSlash = true;
                    break;
                }
            }
            if (testSlash == false) {
                throw new StorageException("The Document path not contains slash");
            }
            if (path.charAt(path.length() - 1) == '/') {
                throw new StorageException("The last character of Document path can not contain slash");
            }
        }
        try {
            id = (document.getId() == null ? 0 : ((Long) document.getId()).longValue());
        } catch (ClassCastException e) {
            throw new StorageException(e.getMessage());
        }
        if (LOG.isInfoEnabled()) {
            LOG.info("delete (document:" + Util.documentToString(document) + ") (condition:"
                    + Util.conditionToString(condition) + ")");
        }
        int numberOfAffectedRows = 0;
        ConfigList configList = internalGetConfigList(config);
        InternalCondition cond = convertToInternalCondition(condition, true);
        numberOfAffectedRows = sqlitedriver.sqliteDeleteExact(openedDB, configList, document.getPath(), id, cond);
        if (numberOfAffectedRows < 0) {
            throw new StorageException(new StringBuffer("Error in delete ").append(numberOfAffectedRows).toString());
        }
        return numberOfAffectedRows > 0;
    }

    /**
     * Searches for a document in a specified path.
     * <p>
     * If scope is specified to:
     * <ul>
     * <li>{@link #SCOPE_ONE_LEVEL}: searches documents one level below the given path (this excludes the exact path)</li>
     * <li>{@link #SCOPE_SUB_TREE}: searches documents reachable from the root document pointed by the given path (this includes
     * the exact path)</li>
     * </ul>
     *
     * @param config storage configuration to apply
     * @param path document path from which search is performed (it must not end with a slash and must be normalized)
     * @param scope a constant ({@link #SCOPE_ONE_LEVEL} or {@link #SCOPE_SUB_TREE})
     * @param condition condition that filters documents
     * @param order ordering of the results ({@link #ORDER_UNKNOWN}, {@link #ORDER_ASC}, {@link #ORDER_DESC})
     * @param limit maximum number of results returned ({@link #NO_LIMIT} means any number of values)
     * @param withContent whether to return the document content in result set
     * @param withAttributes List of attributes names. If null, return all document attributes in result set. Else if empty, do
     *            not return attributes in result set. Else, return only specified attributes in result set
     * @return a list containing the matching documents
     * @throws StorageException if any problem occurs while retrieving the document
     */
    public SearchResult search(Map config, String basePath, int scope, Condition condition, int order, int limit,
            boolean withContent, List withAttributes) throws StorageException {
        if (basePath == null) {
            throw new StorageException("The Document path is null");
        }
        if (!basePath.equals("")) {
            boolean testSlash = false;
            for (int i = 0; i < basePath.length(); i++) {
                if (basePath.charAt(i) == '/') {
                    testSlash = true;
                    break;
                }
            }
            if (testSlash == false) {
                throw new StorageException("The Document path not contains slash");
            }
            if (basePath.charAt(basePath.length() - 1) == '/') {
                throw new StorageException("The last character of Document path can not contain slash");
            }
        }

        if (LOG.isInfoEnabled()) {
            LOG.info("search (path:" + basePath + ") (scope:" + scope + ") (condition:" + Util.conditionToString(condition)
                    + ") (order:" + order + ") (limit:" + limit + ") (withContent:" + withContent + ")");
        }

        InternalCondition internalCondition = convertToInternalCondition(condition, true);
        ConfigList configList = internalGetConfigList(config);
        int attr_length = 0;
        String[] attr = new String[] {};
        if (withAttributes != null) {
            attr_length = (withAttributes.size() > 0 ? withAttributes.size() : -1);
            attr = (String[]) withAttributes.toArray(new String[withAttributes.size()]);
        }

        SearchReturn returnedData = sqlitedriver.search(openedDB, configList, basePath, scope, internalCondition, order, limit,
                (withContent ? 1 : 0), attr, attr_length);
        if (returnedData == null) {
            throw new StorageException("search return is null");
        }
        SWIGTYPE_p_p_subSearchResult results = returnedData.getData();

        List searchedResults = new ArrayList();
        for (int i = 0; i < returnedData.getSize(); i++) {
            subSearchResult subResult = sqlitedriver.search_array_getitem(results, i);
            try {
                Document doc = convertSubSearchResultToDocument(subResult);
                searchedResults.add(doc);
            } catch (ParseException e) {
                throw new StorageException(e.getMessage());
            }
        }
        return new SearchResultImpl(searchedResults.iterator());
    }
}
