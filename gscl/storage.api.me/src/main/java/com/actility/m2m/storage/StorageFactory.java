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

import java.util.Date;
import java.util.List;

/**
 * Factory class that allows applications to build storage objects
 */
public interface StorageFactory {
    /**
     * Builds a condition that is used while performing a search operation to narrow the result set
     *
     * @param attributeName The attribute name on which the condition applies
     * @param operator The attribute operator (either EQUAL, NOT_EQUAL, STRICTLY_GREATER, LOWER_OR_EQUAL, STRICTLY_LOWER,
     *            GREATER_OR_EQUAL, STARTS_WITH)
     * @param operand The String operand associated with the condition
     * @return the created condition
     */
    public Condition createStringCondition(String attributeName, int operator, String operand);

    /**
     * Builds a condition that is used while performing a search operation to narrow the result set
     *
     * @param attributeName The attribute name on which the condition applies
     * @param operator The attribute operator (either EQUAL, NOT_EQUAL, STRICTLY_GREATER, LOWER_OR_EQUAL, STRICTLY_LOWER,
     *            GREATER_OR_EQUAL)
     * @param operand The Date operand associated with the condition
     * @return the created condition
     */
    public Condition createDateCondition(String attributeName, int operator, Date operand);

    /**
     * Builds a condition that is used while performing a search operation to narrow the result set
     *
     * @param attributeName The attribute name on which the condition applies
     * @param operator The attribute operator (either EQUAL, NOT_EQUAL, STRICTLY_GREATER, LOWER_OR_EQUAL, STRICTLY_LOWER,
     *            GREATER_OR_EQUAL)
     * @param operand The Integer operand associated with the condition
     * @return the created condition
     */
    public Condition createIntegerCondition(String attributeName, int operator, Integer operand);

    /**
     * Builds a condition that is used while performing a search operation to narrow the result set
     *
     * @param attributeName The attribute name on which the condition applies
     * @param operator The attribute operator (either EQUAL, NOT_EQUAL, STRICTLY_GREATER, LOWER_OR_EQUAL, STRICTLY_LOWER,
     *            GREATER_OR_EQUAL, STARTS_WITH)
     * @param operand The String operand associated with the condition
     * @return the created condition
     */
    public Condition createStringExistsCondition(String attributeName, int operator, String operand);

    /**
     * Builds a condition that is used while performing a search operation to narrow the result set
     *
     * @param attributeName The attribute name on which the condition applies
     * @param operator The attribute operator (either EQUAL, NOT_EQUAL, STRICTLY_GREATER, LOWER_OR_EQUAL, STRICTLY_LOWER,
     *            GREATER_OR_EQUAL)
     * @param operand The Date operand associated with the condition
     * @return the created condition
     */
    public Condition createDateExistsCondition(String attributeName, int operator, Date operand);

    /**
     * Builds a condition that is used while performing a search operation to narrow the result set
     *
     * @param attributeName The attribute name on which the condition applies
     * @param operator The attribute operator (either EQUAL, NOT_EQUAL, STRICTLY_GREATER, LOWER_OR_EQUAL, STRICTLY_LOWER,
     *            GREATER_OR_EQUAL)
     * @param operand The Integer operand associated with the condition
     * @return the created condition
     */
    public Condition createIntegerExistsCondition(String attributeName, int operator, Integer operand);

    /**
     * Builds a condition that is used while performing a search operation to narrow the result set.
     * <p>
     * This condition is a conjunction of 2 others conditions
     *
     * @param operator The condition operator (AND)
     * @param conditions The list of conditions that must be compound
     * @return the created condition
     */
    public Condition createConjunction(int operator, List/* <Condition> */conditions);

    /**
     * Creates a document to interact with the storage API.
     *
     * @param path The document path
     * @return The created document
     */
    public Document createDocument(String path);

    /**
     * Creates a document to interact with the storage API.
     *
     * @param id The internal id for this document
     * @return The created document
     */
    public Document createDocumentFromId(String id);

    /**
     * Builds an attribute operation to modify a single attribute in a document
     *
     * @param type The operation type (either SET or UNSET)
     * @param name The attribute name on which the operation will be performed
     * @param value The value associated with the operation (could be <code>null</code>)
     * @return The created attribute operation
     */
    public AttributeOperation createAttributeOperation(int type, String name, Object value);

    /**
     * Creates a batch to execute several modifications on the storage
     *
     * @return The created batch
     */
    public Batch createBatch();
}
