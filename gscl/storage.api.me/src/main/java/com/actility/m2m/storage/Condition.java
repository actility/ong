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

/**
 * Represents a condition that is used while performing a search operation to narrow the result set.
 */
public interface Condition {
    /**
     * Attribute operation to test if the attribute value is equal to a given operand
     */
    public final static int ATTR_OP_EQUAL = 0;
    /**
     * Attribute operation to test if the attribute value is not equal to a given operand
     */
    public final static int ATTR_OP_NOT_EQUAL = 1;
    /**
     * Attribute operation to test if the attribute value is strictly greater than a given operand
     */
    public final static int ATTR_OP_STRICTLY_GREATER = 2;
    /**
     * Attribute operation to test if the attribute value is lower or equal to a given operand
     */
    public final static int ATTR_OP_LOWER_OR_EQUAL = 3;
    /**
     * Attribute operation to test if the attribute value is strictly lower than a given operand
     */
    public final static int ATTR_OP_STRICTLY_LOWER = 4;
    /**
     * Attribute operation to test if the attribute value is greater or equal to a given operand
     */
    public final static int ATTR_OP_GREATER_OR_EQUAL = 5;
    /**
     * Attribute operation to test if the string attribute value starts with a given operand (only valid for string attributes)
     */
    public final static int ATTR_OP_STARTS_WITH = 6;

    /**
     * Condition operation to test condition1 and condition2
     */
    public final static int COND_OP_AND = 0;
    /**
     * Condition operation to test condition1 or condition2
     */
    public final static int COND_OP_OR = 1;

    /**
     * Gets the attribute name on which the condition applies
     *
     * @return The attribute name
     */
    public String getAttributeName();

    /**
     * Gets the operation performed by the condition (either {@link #ATTR_OP_EQUAL}, {@link #ATTR_OP_NOT_EQUAL},
     * {@link #ATTR_OP_STRICTLY_GREATER}, {@link #ATTR_OP_LOWER_OR_EQUAL}, {@link #ATTR_OP_STRICTLY_LOWER},
     * {@link #ATTR_OP_GREATER_OR_EQUAL}, {@link #ATTR_OP_STARTS_WITH}, {@link #COND_OP_AND}, {@link #COND_OP_OR})
     *
     * @return The condition operation
     */
    public int getOperator();

    /**
     * Gets the operand associated with the condition
     * <p>
     * This is only relevant for attribute conditions
     *
     * @return The condition operand
     */
    public Object getOperand();

    /**
     * Checks whether the condition has an exists operator (specific to array attributes)
     *
     * @return Whether the condition has an exists operator
     */
    public boolean isExists();

    /**
     * Gets the first condition associated with the condition
     * <p>
     * This is only relevant for a condition on conditions
     *
     * @return The first condition
     */
    public List/* <Condition> */getConditions();
}
