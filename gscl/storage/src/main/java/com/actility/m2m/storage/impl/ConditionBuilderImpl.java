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
 * id $Id: ConditionBuilderImpl.java 8777 2014-05-21 15:51:53Z JReich $
 * author $Author: JReich $
 * version $Revision: 8777 $
 * lastrevision $Date: 2014-05-21 17:51:53 +0200 (Wed, 21 May 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-05-21 17:51:53 +0200 (Wed, 21 May 2014) $
 */

package com.actility.m2m.storage.impl;

import java.util.Date;

import org.apache.log4j.Logger;

import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.ConditionBuilder;
import com.actility.m2m.storage.log.BundleLogger;
import com.actility.m2m.util.log.OSGiLogger;

public class ConditionBuilderImpl implements ConditionBuilder {

    private static final Logger LOG = OSGiLogger.getLogger(ConditionBuilder.class, BundleLogger.getStaticLogger());

    /**
     * @see ConditionBuilder
     * @param attributeName
     * @param operator
     * @param operandValue
     * @return condition object
     * @throws an exception if limitation are not fulfilled
     */

    public Condition createCondition(String attributeName, int operator, String operandValue) {
        LOG.info("Begin of createCondition()");
        Condition valret = new ConditionImpl(attributeName, operator, operandValue);
        LOG.info("End of createCondition()");
        return (valret);
    }

    /**
     * @see ConditionBuilder
     * @param attributeName
     * @param operator : operator constant
     * @param operandValue
     * @return Condition object
     * @throws an exception if limitation are not fulfilled
     */

    public Condition createStringCondition(String attributeName, int operator, String operandValue) {
        LOG.info("Begin of createCondition()");
        Condition valret = new ConditionImpl(attributeName, ConditionBuilder.TYPE_STRING, operator, operandValue);
        LOG.info("End of createCondition()");
        return (valret);
    }

    /**
     * @see ConditionBuilder
     * @param attributeName
     * @param operator : operator constant
     * @param operandValue
     * @return Condition object
     * @throws an exception if limitation are not fulfilled
     */

    public Condition createDateCondition(String attributeName, int operator, Date operandValue) {
        LOG.info("Begin of createCondition()");
        Condition valret = new ConditionImpl(attributeName, ConditionBuilder.TYPE_DATE, operator, operandValue);
        LOG.info("End of createCondition()");
        return (valret);
    }

    /**
     * @see ConditionBuilder
     * @param attributeName
     * @param operator : operator constant
     * @param operandValue
     * @return Condition object
     * @throws an exception if limitation are not fulfilled
     */

    public Condition createIntegerCondition(String attributeName, int operator, Integer operandValue) {
        LOG.info("Begin of createCondition()");
        Condition valret = new ConditionImpl(attributeName, ConditionBuilder.TYPE_INTEGER, operator, operandValue);
        LOG.info("End of createCondition()");
        return (valret);
    }

    /**
     * @see ConditionBuilder
     * @param condition1
     * @param condition2
     * @return condition object
     *
     */
    public Condition createConjunction(Condition condition1, Condition condition2) {
        LOG.info("Begin of createConjunction()");
        Condition valret = new ConditionImpl(condition1, ConditionBuilder.OPERATOR_AND, condition2);
        LOG.info("End of createConjunction()");
        return (valret);
    }

    /**
     * @see ConditionBuilder
     * @param expression
     * @return Condition object
     *
     */

    public Condition createCondition(String expression) throws IllegalArgumentException {
        if (LOG.isInfoEnabled()) {
            LOG.info(new StringBuffer().append("Begin of createCondition(").append(expression).append(")"));
        }
        Condition valret = parseCondition(expression);
        if (LOG.isInfoEnabled()) {
            LOG.info(new StringBuffer().append("End of createCondition(").append(expression).append(")"));
        }
        return (valret);
    }

    /**
     * Accepted input: & -> AND = -> equal operator ( ) a-zA-Z0-9_- -> characters allowed in identifiers ( ) * & | \ must be
     * escaped by a \ in values samples of valid expression: <BR>
     * (&(searchString=zigbee)(searchString=light*)) (searchString=zigbee)
     *
     * @param expression
     * @return
     * @throws IllegalArgumentException
     */

    private Condition parseCondition(String expression) throws IllegalArgumentException {
        //
        LdapExpressionParser parser = new LdapExpressionParser(expression, 0);
        Condition condition = parser.parse();
        return condition;
    }

}
