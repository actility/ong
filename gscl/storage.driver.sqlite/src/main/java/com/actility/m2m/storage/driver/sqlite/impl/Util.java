package com.actility.m2m.storage.driver.sqlite.impl;

import java.util.Date;
import java.util.Iterator;
import java.util.List;
import java.util.Map.Entry;

import com.actility.m2m.storage.AttributeOperation;
import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.Document;
import com.actility.m2m.util.FormatUtils;
import com.actility.m2m.util.UtilConstants;

public final class Util {

    /**
     * 1024 bytes = 1kb is one page 1024 pages = 1 mb; 1mb * 6 = 6mb restricted size of the database 1024 * 6 = 6144 total pages
     * for 6 mb
     **/
    public static final int MAX_PAGE_COUNT = 6144;
    public static final float PERCENT_SPACE_TAKEN = 0.90f;
    public static final String DEFAULT_DB_FILE_NAME = "resources/storage.db";

    public final static int MAJOR_VERSION = 3;
    public final static int MINOR_VERSION = 0;

    public final static int DEFAULT_PAGE_NUMBER_PER_STEP = 5;
    public final static int DEFAULT_SLEEP_DELAY_BETWEEN_STEP = 250;

    public final static int ORDER_UNKNOWN = -1;
    public final static int LIMIT_NONE = -1;

    public final static int ATTR_TYPE_INTEGER = 0;
    public final static int ATTR_TYPE_STRING = 1;
    public final static int ATTR_TYPE_DATE = 2;
    public final static int ATTR_TYPE_LIST_INTEGER = 3;
    public final static int ATTR_TYPE_LIST_STRING = 4;
    public final static int ATTR_TYPE_LIST_DATE = 5;

    /**
     * Build a string representation of a Document.
     *
     * @param doc the document to represent
     * @return the resulting string representation.
     */
    public static String documentToString(Document doc) {

        if (null == doc) {
            return "<unset>";
        }

        StringBuffer res = new StringBuffer();

        res.append("{");
        res.append("(path:");
        if (null != doc.getPath()) {
            res.append(doc.getPath());
        } else {
            res.append("unk");
        }
        res.append(") (id:");
        if (null != doc.getId()) {
            res.append(doc.getId());
        } else {
            res.append("unk");
        }
        res.append(") (content length:");
        if (null != doc.getContent()) {
            res.append(doc.getContent().length);
        } else {
            res.append("0");
        }
        res.append(") (attributes: ");
        Iterator it = doc.getAttributes();
        if (it != null) {
            while (it.hasNext()) {
                Entry entry = (Entry) it.next();
                String attrName = (String) entry.getKey();
                res.append("<").append(attrName).append(":");

                Object value = entry.getValue();
                if (value instanceof List) {
                    List list = (List) entry.getValue();
                    for (int i = 0; i < list.size(); i++) {
                        Object subValue = list.get(i);
                        res.append(subValue).append(",");
                    }
                } else if (value instanceof Date) {
                    res.append(FormatUtils.formatDateTime((Date) value, UtilConstants.LOCAL_TIMEZONE));
                } else {
                    res.append(value);
                }
                res.append("> ");
                // else attribute is ignored
            }
        }
        res.append(")}");

        return res.toString();
    }

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
            case Condition.ATTR_OP_EQUAL:
                where.append("=");
                break;
            case Condition.ATTR_OP_GREATER_OR_EQUAL:
                where.append(">=");
                break;
            case Condition.ATTR_OP_LOWER_OR_EQUAL:
                where.append("<=");
                break;
            case Condition.ATTR_OP_NOT_EQUAL:
                where.append("!=");
                break;
            case Condition.ATTR_OP_STARTS_WITH:
                where.append("LIKE");
                break;
            case Condition.ATTR_OP_STRICTLY_GREATER:
                where.append(">");
                break;
            case Condition.ATTR_OP_STRICTLY_LOWER:
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
            where.append("list:" + listConditionsToString(cond.getConditions(), cond.getOperator()));
        } else {
            if (operand instanceof String) {
                where.append("value:" + (String) operand);
            } else if (operand instanceof Integer) {
                where.append("value:" + ((Integer) operand).toString());
            } else {
                where.append("value:" + FormatUtils.formatDateTime((Date) operand, UtilConstants.LOCAL_TIMEZONE));
            }
        }
        where.append(")}");

        return where.toString();
    }

    /**
     * Build a string representation of a list of conditions.
     *
     * @param conditions the list of conditions to represent
     * @param operator The operator id to use between conditions. Can be either {@link Condition#COND_OP_AND} or {@link Condition#COND_OP_OR}
     * @return the resulting string representation.
     */
    public static String listConditionsToString(List conditions, int operator) {
        if (conditions == null) {
            return "<unset>";
        }
        StringBuffer listConditions = new StringBuffer();
        listConditions.append("{");
        for (int i = 0; i < conditions.size(); i++) {
            listConditions.append(conditionToString((Condition) conditions.get(i)));
            if (i + 1 < conditions.size()) {
                listConditions.append(operator == Condition.COND_OP_AND ? " && " : " || ");
            }
        }
        listConditions.append("}");
        return listConditions.toString();
    }

    /**
     * Build a string representation of the list of the attribute operations to process.
     *
     * @param attrsOps the list of attribute operation to represent
     * @return the resulting string representation.
     */
    public static String attrOpsToString(List attrsOps) {
        if (null == attrsOps) {
            return "<unset>";
        }

        StringBuffer res = new StringBuffer();

        res.append("{");
        for (int i = 0; i < attrsOps.size(); i++) {
            AttributeOperation operation = (AttributeOperation) attrsOps.get(i);
            switch (operation.getType()) {
            case AttributeOperation.TYPE_SET:
                res.append("<SET:").append(operation.getName()).append(":");
                if (operation.getValue() instanceof List) {
                    for (int j = 0; j < ((List) operation.getValue()).size(); j++) {
                        res.append(((List) operation.getValue()).get(j)).append(",");
                    }
                } else if (operation.getValue() instanceof Date) {
                    res.append(FormatUtils.formatDateTime((Date) operation.getValue(), UtilConstants.LOCAL_TIMEZONE));
                } else {
                    res.append(operation.getValue());
                }
                res.append(">");
                break;

            case AttributeOperation.TYPE_UNSET:
                res.append("<UNSET:").append(operation.getName()).append(">");
                break;
            }
        }

        res.append("}");

        return res.toString();
    }

}
