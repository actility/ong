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
package com.actility.m2m.storage;

import java.util.List;
import java.util.Map;

/**
 * Represents a batch operation on the storage. It is used to execute several modifications as a whole in the storage
 */
public interface Batch {
    /**
     * Batch operation has succeeded
     */
    int BATCH_OK = 0;
    /**
     * Batch operation failed because of a CREATE failure
     */
    int BATCH_CREATE_FAILED = 1;
    /**
     * Batch operation failed because of an UPDATE failure
     */
    int BATCH_UPDATE_FAILED = 2;
    /**
     * Batch operation failed because of a DELETE failure
     */
    int BATCH_DELETE_FAILED = 3;

    /**
     * Updates an existing document.
     * <p>
     * Raises an exception if the document does not exist.
     *
     * @param config storage configuration to apply
     * @param document the new representation of the document
     * @param condition condition that must be fulfilled in order to perform the operation
     */
    public void update(Map/* <String, String> */config, Document document, Condition condition);

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
     */
    public void partialUpdate(Map/* <String, String> */config, Document document, byte[] content,
            List/* <AttributeOperation> */attrOps, Condition condition);

    /**
     * Creates a new document.
     * <p>
     * Raises an exception if the document already exists.
     *
     * @param config storage configuration to apply
     * @param document the representation of the document
     */
    public void create(Map/* <String, String> */config, Document document);

    /**
     * Deletes a document.
     * <p>
     * Raises an exception if the document does not exist.
     *
     * @param config storage configuration to apply
     * @param document The document to delete. Only id or path are used
     * @param condition condition that must be fulfilled in order to perform the operation
     */
    public void delete(Map/* <String, String> */config, Document document, Condition condition);

    /**
     * Executes the batch operation.
     *
     * @return An integer which described the batch result. ({@link #BATCH_OK}, {@link #BATCH_CREATE_FAILED},
     *         {@link #BATCH_UPDATE_FAILED}, {@link #BATCH_DELETE_FAILED})
     * @throws StorageException if any problem occurs while retrieving the document
     */
    public int execute() throws StorageException;
}
