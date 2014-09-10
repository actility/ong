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
 * id $Id: $
 * author $Author: $
 * version $Revision: $
 * lastrevision $Date: $
 * modifiedby $LastChangedBy: $
 * lastmodified $LastChangedDate: $
 */

package com.actility.m2m.storage.driver.sqlite.ni;


public interface SqliteDriverNIService {
    /*
     * Getters for constants
     */
    char getPathDelimiter();

    char getStarChar();

    int getCascadingNone();

    int getCascadingOneLevel();

    int getCascadingSubtree();

    int getTypeCreate();

    int getTypeUpdate();

    int getTypeDelete();

    int getScopeExact();

    int getScopeSubtree();

    int getOrderAsc();

    int getOrderDesc();

    int getOrderUnknown();

    int getOperatorEqual();

    int getOperatorGreater();

    int getOperatorLower();

    int getOperatorStartsWith();

    int getOperatorAnd();

    int getOperatorNotEqualTo();

    int getSqliteOk();

    int getTypeDate();

    int getTypeInteger();

    int getTypeString();

    /*
     * DB admin operation
     */
    void backupDb(String szFilename, int nbPagePerStep, int sleepDelayBetweenStep);

    NiAttribute createInternalAttribute();

    NiAttributeArray createInternalAttributeArray(int size);

    NiCondition createInternalCondition();

    /*
     * SQL operations
     */
    byte[] sqliteRetrieve(String fullPath);

    int sqliteCreate(String fullPath, byte[] content, int len, NiAttributeArray attrs, int attrs_len);

    int sqliteUpdate(String fullPath, byte[] content, int len, NiAttributeArray attrs, int attrs_len);

    int sqliteDelete(String fullPath, int cascadeLevel);

    NiSearchResult search(String basePath, int scope, NiCondition condition, int order, int limit);

    /*
     * Transactions
     */
    int beginTransaction();

    int commitTransaction();

    int rollbackTransaction();
}
