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
 * id $Id: ConditionBuilder.java 6104 2013-10-15 15:31:47Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6104 $
 * lastrevision $Date: 2013-10-15 17:31:47 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 17:31:47 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.storage;

import java.util.Date;

public interface ConditionBuilder {
    // public final static String DEFAULT_ATTRIBUTE_NAME = "searchStrings";
    public final static int OPERATOR_EQUAL = 1;
    public final static int OPERATOR_GREATER = 2;
    public final static int OPERATOR_LOWER = 3;
    public final static int OPERATOR_STARTS_WITH = 4;
    public final static int OPERATOR_NOT_EQUAL_TO = 9;
    public final static int OPERATOR_AND = 5;
    public final static int TYPE_STRING = 6;
    public final static int TYPE_INTEGER = 7;
    public final static int TYPE_DATE = 8;

    /**
     * Builds a condition object by analyzing the expression<BR>
     * Expression must conform to an LDAP syntax<BR>
     *
     * @param expression : expression in LDAP like style <BR>
     *            sample: (&(searchString=zigbee)(searchString=light*))(searchString=zigbee)
     * @return Condition object
     */
    public Condition createCondition(String expression);

    /**
     * Creates a string condition (deprecated – now createStringCondition should be used)
     *
     * @param attributeName
     * @param operator : operator constant
     * @param operandValue
     * @return Condition object
     * @throws an exception if limitation are not fulfilled
     */
    public Condition createCondition(String attributeName, int operator, String operandValue);

    /**
     * Creates a condition implying a string attribute.
     *
     * @param attributeName
     * @param operator : operator constant
     * @param operandValue
     * @return Condition object
     * @throws an exception if limitation are not fulfilled
     */
    public Condition createStringCondition(String attributeName, int operator, String operandValue);

    /**
     * Creates a condition implying a date.
     *
     * @param attributeName
     * @param operator : operator constant
     * @param operandValue
     * @return Condition object
     * @throws an exception if limitation are not fullfilled
     */
    public Condition createDateCondition(String attributeName, int operator, Date operandValue);

    /**
     * Creates a condition implying an integer.
     *
     * @param attributeName
     * @param operator : operator constant
     * @param operandValue
     * @return Condition object
     * @throws an exception if limitation are not fullfilled
     */
    public Condition createIntegerCondition(String attributeName, int operator, Integer operandValue);

    /**
     * Builds a condition object equivalent to condition1 AND condition2
     *
     * @param condition1 : first operand of AND
     * @param condition2 : second operand of AND
     * @return Condition object
     */
    public Condition createConjunction(Condition condition1, Condition condition2);
}
