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

package com.actility.m2m.storage.driver.sqlite.jni.impl;

import com.actility.m2m.storage.driver.sqlite.ni.NiAttribute;
import com.actility.m2m.storage.driver.sqlite.ni.NiAttributeArray;
import com.actility.m2m.storage.driver.sqlite.ni.NiCondition;
import com.actility.m2m.storage.driver.sqlite.ni.NiSearchResult;
import com.actility.m2m.storage.driver.sqlite.ni.SqliteDriverNIService;

public class SqliteDriverJNIService implements SqliteDriverNIService {

    private SqliteDB openedDB;

    // for now on, this primitive is
    // with the next storage.api, the command is to be integrated within the whole swig process.
    private final native void backupDb(long jarg1, SqliteDB obj, String zFilename, int nbPagePerStep, int sleepDelayBetweenStep);

    public char getPathDelimiter() {
        return sqlitedriverConstants.PATH_DELIMITER;
    }

    public char getStarChar() {
        return sqlitedriverConstants.STAR_CHAR;
    }

    public int getCascadingNone() {
        return sqlitedriverConstants.CASCADING_NONE;
    }

    public int getCascadingOneLevel() {
        return sqlitedriverConstants.CASCADING_ONE_LEVEL;
    }

    public int getCascadingSubtree() {
        return sqlitedriverConstants.CASCADING_SUBTREE;
    }

    public int getTypeCreate() {
        return sqlitedriverConstants.TYPE_CREATE;
    }

    public int getTypeUpdate() {
        return sqlitedriverConstants.TYPE_UPDATE;
    }

    public int getTypeDelete() {
        return sqlitedriverConstants.TYPE_DELETE;
    }

    public int getScopeExact() {
        return sqlitedriverConstants.SCOPE_EXACT;
    }

    public int getScopeSubtree() {
        return sqlitedriverConstants.SCOPE_SUBTREE;
    }

    public int getOrderAsc() {
        return sqlitedriverConstants.ORDER_ASC;
    }

    public int getOrderDesc() {
        return sqlitedriverConstants.ORDER_DESC;
    }

    public int getOrderUnknown() {
        return sqlitedriverConstants.ORDER_UNKNOWN;
    }

    public int getOperatorEqual() {
        return sqlitedriverConstants.OPERATOR_EQUAL;
    }

    public int getOperatorGreater() {
        return sqlitedriverConstants.OPERATOR_GREATER;
    }

    public int getOperatorLower() {
        return sqlitedriverConstants.OPERATOR_LOWER;
    }

    public int getOperatorStartsWith() {
        return sqlitedriverConstants.OPERATOR_STARTS_WITH;
    }

    public int getOperatorAnd() {
        return sqlitedriverConstants.OPERATOR_AND;
    }

    public int getOperatorNotEqualTo() {
        return sqlitedriverConstants.OPERATOR_NOT_EQUAL_TO;
    }

    public int getSqliteOk() {
        return sqlitedriverConstants.SQLITE_OK;
    }

    public int getTypeDate() {
        return sqlitedriverConstants.TYPE_DATE;
    }

    public int getTypeInteger() {
        return sqlitedriverConstants.TYPE_INTEGER;
    }

    public int getTypeString() {
        return sqlitedriverConstants.TYPE_STRING;
    }

    public void sqliteOpen(String fname) {
        openedDB = sqlitedriver.SqliteOpen(fname);
    }

    public void sqliteClose() {
        sqlitedriver.SqliteClose(openedDB);
    }

    public int activate() {
        return sqlitedriver.activate(openedDB);
    }

    public int deactivate() {
        return sqlitedriver.deactivate(openedDB);
    }

    public int checkDBVersion(int major, int minor) {
        return sqlitedriver.checkDBVersion(openedDB, major, minor);
    }

    public void backupDb(String szFilename, int nbPagePerStep, int sleepDelayBetweenStep) {
        backupDb(SqliteDB.getCPtr(openedDB), openedDB, szFilename, nbPagePerStep, sleepDelayBetweenStep);
    }

    public void setLogLevel(int level) {
        sqlitedriver.setLogLevel(level, openedDB);
    }

    public NiAttribute createInternalAttribute() {
        return new JNiAttributeImpl(new InternalAttribute());
    }

    public NiAttributeArray createInternalAttributeArray(int size) {
        return new JNiAttributeArrayImpl(sqlitedriver.new_attributeArray(size), size);
    }

    public NiCondition createInternalCondition() {
        return new JNiConditionImpl(new InternalCondition());
    }

    public int setMaxDatabaseSizeKb(int maxDatabaseSizeKb) {
        return sqlitedriver.setMaxDatabaseSizeKb(openedDB, maxDatabaseSizeKb);
    }

    public void setMinDurationBetweenVacuums(int value) {
        sqlitedriver.setMinDurationBetweenVacuums(value);
    }

    public void setMaxOpsBeforeVacuum(int value) {
        sqlitedriver.setMaxOpsBeforeVacuum(value);
    }

    public void setMaxDurationBeforeVacuum(int value) {
        sqlitedriver.setMaxDurationBeforeVacuum(value);
    }

    public void setMaxDBUsagePourcentageBeforeVacuum(int value) {
        sqlitedriver.setMaxDBUsagePourcentageBeforeVacuum(value);
    }

    public void setMinDurationBetweenDBUsageComputation(int value) {
        sqlitedriver.setMinDurationBetweenDBUsageComputation(value);
    }

    public byte[] sqliteRetrieve(String fullPath) {
        return sqlitedriver.sqliteRetrieve(openedDB, fullPath);
    }

    public int sqliteCreate(String fullPath, byte[] content, int len, NiAttributeArray attrs, int attrs_len) {
        SWIGTYPE_p_p_InternalAttribute attrsArray = null;
        if ((attrs != null) && (attrs_len != 0)) {
            attrsArray = ((JNiAttributeArrayImpl) attrs).getSWIGTYPE_p_p_InternalArray();
        }
        return sqlitedriver.sqliteCreate(openedDB, fullPath, content, len, attrsArray, attrs_len);
    }

    public int sqliteUpdate(String fullPath, byte[] content, int len, NiAttributeArray attrs, int attrs_len) {
        SWIGTYPE_p_p_InternalAttribute attrsArray = null;
        if ((attrs != null) && (attrs_len != 0)) {
            attrsArray = ((JNiAttributeArrayImpl) attrs).getSWIGTYPE_p_p_InternalArray();
        }
        return sqlitedriver.sqliteUpdate(openedDB, fullPath, content, len, attrsArray, attrs_len);
    }

    public int sqliteDelete(String fullPath, int cascadeLevel) {
        return sqlitedriver.sqliteDelete(openedDB, fullPath, cascadeLevel);
    }

    public NiSearchResult search(String basePath, int scope, NiCondition condition, int order, int limit) {
        InternalCondition intCond = null;
        if (condition != null) {
            intCond = ((JNiConditionImpl) condition).getInternalCondition();
        }
        return new JNiSearchReturnImpl(sqlitedriver.search(openedDB, basePath, scope, intCond, order, limit));
    }

    public int executeSqlStatementWithOutParameters(String sql) {
        return sqlitedriver.executeSqlStatementWithOutParameters(openedDB, sql);
    }

    public int beginTransaction() {
        return sqlitedriver.beginTransaction(openedDB);
    }

    public int commitTransaction() {
        return sqlitedriver.commitTransaction(openedDB);
    }

    public int rollbackTransaction() {
        return sqlitedriver.rollbackTransaction(openedDB);
    }

}
