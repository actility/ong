package com.actility.m2m.storage;

import java.util.List;
import java.util.Map;

/**
 * It provides all the necessary methods to operate on data
 */
public interface StorageRequestExecutor {
    /**
     * Scopes only documents one level below the given path (this excludes the exact path)
     */
    int SCOPE_ONE_LEVEL = 0;

    /**
     * Scopes all documents reachable from the root document pointed by the given path (this includes the exact path)
     */
    int SCOPE_SUB_TREE = 1;

    /**
     * No limit special value when a search request does not define any limit
     */
    int NO_LIMIT = -1;

    /**
     * No order defined
     */
    int ORDER_UNKNOWN = 0;

    /**
     * Sorts documents in ascending order against their path
     */
    int ORDER_ASC = 1;

    /**
     * Sorts documents in descending order against their path
     */
    int ORDER_DESC = 2;

    /**
     * Storage configuration attribute to set the driver that will handle the operation
     */
    String CONFIG_DRIVER_NAME = "com.actility.m2m.storage.config.driverName";

    /**
     * Storage configuration attribute to tell whether the current document must support transactional updates
     */
    String CONFIG_TRANSACTIONAL_UPDATE = "transacUpdate";

    /**
     * Storage configuration attribute to log on debug instead of info for the concerned operation
     */
    String CONFIG_DEBUG = "debug";

    /**
     * Retrieves a document from its path
     * 
     * @param config storage configuration to apply
     * @param path the full (absolute) path of the document (it must not end with a slash and must be normalized)
     * @param condition condition that must be fulfilled in order to perform the operation
     * @return The retrieved document or <code>null</code> if the document does not exist or the condition is not fulfilled
     * @throws StorageException if any problem occurs while retrieving the document
     */
    Document retrieve(Map/* <String, String> */config, String path, Condition condition) throws StorageException;

    /**
     * Retrieves a document from its internal ID
     *
     * @param config storage configuration to apply
     * @param id Internal id of the document to retrieve
     * @param condition condition that must be fulfilled in order to perform the operation
     * @return The retrieved document or <code>null</code> if the document does not exist or the condition is not fulfilled
     * @throws StorageException if any problem occurs while retrieving the document
     */
    Document retrieveFromId(Map/* <String, String> */config, String id, Condition condition) throws StorageException;

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
    boolean update(Map/* <String, String> */config, Document document, Condition condition) throws StorageException;

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
    boolean partialUpdate(Map/* <String, String> */config, Document document, byte[] content,
            List/* <AttributeOperation> */attrOps, Condition condition) throws StorageException;

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
    boolean create(Map/* <String, String> */config, Document document) throws StorageException;

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
    boolean delete(Map/* <String, String> */config, Document document, int scope, Condition condition) throws StorageException;

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
    boolean delete(Map/* <String, String> */config, Document document, Condition condition) throws StorageException;

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
    SearchResult search(Map/* <String, String> */config, String path, int scope, Condition condition, int order, int limit,
            boolean withContent, List/* <String> */withAttributes) throws StorageException;

    /**
     * Reserves space for the creation of future documents
     * 
     * @param config StorageConfiguration to apply
     * @param path path under which documents are going to be created
     * @param docNumber maximum number of documents that can be created in reserved space
     * @param docSize maximum size of documents that can be created in reserved space
     * @return A reservation code
     * @throws StorageException if any problem occurs while reserving space
     */
    String reserveSpace(Map/* <String, String> */config, String path, String docNumber, double docSize) throws StorageException;

    /**
     * Gets the storage factory that allows applications to build storage objects.
     * 
     * @return The storage factory
     */
    StorageFactory getStorageFactory();
}
