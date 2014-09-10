/*
 * Copyright   Actility, SA. All Rights Reserved.
 import java.util.Calendar;
import java.util.Collection;
import java.util.Date;
import java.util.Iterator;

import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.ConditionBuilder;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.Pair;
 is distributed in the hope that it will be useful, but
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

package com.actility.m2m.storage.driver.sqlite.impl;

import java.util.Calendar;
import java.util.Collection;
import java.util.Date;
import java.util.Iterator;

import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.ConditionBuilder;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.Pair;

public class Util {

    /**
     * Build a string representation of a Condition.
     *
     * @param cond the condition to represent
     * @return the resulting string representation.
     */
    public static String conditionToString(Condition cond) {
        if (null == cond) {
            return "<unset>";
        }

        StringBuffer where = new StringBuffer();
        where.append("{");
        if (cond.getOperand() != null) {
            where.append("(attrName:" + cond.getAttributeName() + "),");
            where.append("(operator:");
            switch (cond.getOperator()) {
            case ConditionBuilder.OPERATOR_EQUAL:
                where.append("=");
                break;
            case ConditionBuilder.OPERATOR_NOT_EQUAL_TO:
                where.append("!=");
                break;
            case ConditionBuilder.OPERATOR_STARTS_WITH:
                where.append("LIKE");
                break;
            case ConditionBuilder.OPERATOR_GREATER:
                where.append(">");
                break;
            case ConditionBuilder.OPERATOR_LOWER:
                where.append("<");
                break;
            default:
                break;
            }
            where.append("),");
        }
        where.append("(");
        Object operand = cond.getOperand();
        if (operand == null) {
            where.append(listConditionsToString(cond.getCondition1(), cond.getCondition2(), cond.getOperator()));
        } else {
            if (operand instanceof String) {
                where.append("value:" + (String) operand);
            } else if (operand instanceof Integer) {
                where.append("value:" + ((Integer) operand).toString());
            } else {
                where.append("value:" + FormatUtils.formatDateTime((Date) operand, Calendar.getInstance().getTimeZone()));
            }
        }
        where.append(")}");

        return where.toString();
    }

    /**
     * Build a string representation of a list of conditions.
     *
     * @param conditions the list of conditions to represent
     * @return the resulting string representation.
     */
    public static String listConditionsToString(Condition condition1, Condition condition2, int operator) {
        StringBuffer listConditions = new StringBuffer();
        listConditions.append("{");
        listConditions.append(conditionToString(condition1));
        listConditions.append(operator == ConditionBuilder.OPERATOR_AND ? " && " : " ?? ");
        listConditions.append(conditionToString(condition2));
        listConditions.append("}");
        return listConditions.toString();
    }

    public static String AttributesToString(Collection searchAttributes) {
        if (null == searchAttributes) {
            return "<unset>";
        }
        StringBuffer listConditions = new StringBuffer();
        listConditions.append("{");
        Iterator searchAttributesIterator = searchAttributes.iterator();
        while (searchAttributesIterator.hasNext()) {
            Object searchAttribute = searchAttributesIterator.next();
            if (searchAttribute instanceof Pair) {
                String attributeName = (String) ((Pair) searchAttribute).getFirst();
                Object attributeValue = ((Pair) searchAttribute).getSecond();
                listConditions.append("(" + attributeName + ":" + attributeValue.toString() + ")");
            } else {
                listConditions.append("(<" + searchAttribute.getClass() + ">)");
            }
        }
        listConditions.append("}");
        return listConditions.toString();
    }

}
