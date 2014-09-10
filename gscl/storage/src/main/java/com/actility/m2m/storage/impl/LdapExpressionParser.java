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
 * id $Id: LdapExpressionParser.java 6110 2013-10-15 15:44:55Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6110 $
 * lastrevision $Date: 2013-10-15 17:44:55 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 17:44:55 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.storage.impl;

import com.actility.m2m.storage.Condition;
import com.actility.m2m.storage.ConditionBuilder;

public class LdapExpressionParser {

    private static final int STATE_INITIAL = 0;
    private static final int STATE_PAROPENED = 1;
    private static final int STATE_VALUE_READING = 2;
    private static final int STATE_VALUE_ESCAPED = 3;
    private static final int STATE_PARCLOSED = 4;

    private static final int STATE_AND_READ = 11;
    private static final int STATE_CONDITION1_READ = 12;
    private static final int STATE_CONDITION2_READ = 13;

    private static final int STATE_ERROR = 99;

    // casting functions to determine type of condition object
    private static final String CAST_TO_STRING = "castToString(";
    private static final String CAST_TO_DATE = "castToDate(";
    private static final String CAST_TO_INT = "castToInteger(";
    private static final String BEGIN_WITH_EXPRESSION = "beginWith";

    private final String expression;
    private final boolean parseToEnd;

    private int currentIndex;
    private char currentChar;

    private int currentState;
    private StringBuffer sValue;

    public LdapExpressionParser(String expression, int startIndex) {
        this(expression, startIndex, true);
    }

    public LdapExpressionParser(String expression, int startIndex, boolean toEnd) {
        this.expression = removeSpaces(expression);
        this.parseToEnd = toEnd;
        this.currentIndex = startIndex;
        this.currentState = STATE_INITIAL;
    }

    private static boolean isTerminal(int state) {
        return (state == STATE_ERROR || state == STATE_PARCLOSED);
    }

    private static boolean isIdentifierChar(char c) {
        return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '_');
    }

    private boolean isToBeEscapedInValue(char c) {
        return (c == '&' || c == '|' || c == '*' || c == '(');
    }

    public Condition parse() throws IllegalArgumentException {
        ConditionImpl condition = new ConditionImpl();
        // controls escaping of parenthesis
        boolean parenthesisEscape = false;
        // we parse until we reach a terminal state OR until input is exhausted
        while (!isTerminal(currentState) && currentIndex < expression.length()) {
            currentChar = expression.charAt(currentIndex);
            int newState;
            switch (currentState) {
            case STATE_INITIAL:
                if (currentChar == '(') {
                    sValue = new StringBuffer();
                    int conditionType = extractCondtitionType();
                    if (conditionType != 0) {
                        condition.setType(conditionType);
                        parenthesisEscape = true; // we have to ignore an escaping parenthesis
                    } else {
                        condition.setType(ConditionBuilder.TYPE_STRING);
                    }

                    newState = STATE_PAROPENED;
                } else {
                    newState = STATE_ERROR;
                }
                break;
            case STATE_PAROPENED:
                if (currentChar == 'b') {
                    if (checkOperatorBeginsWith()) {
                        if (parenthesisEscape) {
                            newState = STATE_ERROR;
                        } else {
                            condition.setAttributeName(sValue.toString());
                            condition.setOperator(ConditionBuilder.OPERATOR_STARTS_WITH);
                            newState = STATE_VALUE_READING;
                            sValue = new StringBuffer();
                            break;
                        }
                    }
                }
                if (currentChar == '&') {
                    // Launch a nested parse for nested expression
                    LdapExpressionParser nestedParser = new LdapExpressionParser(expression, currentIndex + 1, false);
                    Condition condition1 = nestedParser.parse();
                    condition.setCondition1(condition1);
                    condition.setOperator(ConditionBuilder.OPERATOR_AND);
                    currentIndex = nestedParser.getCurrentIndex() - 1;
                    newState = STATE_CONDITION1_READ;
                } else if (isIdentifierChar(currentChar)) {
                    sValue.append(currentChar);
                    newState = STATE_PAROPENED;
                } else if (currentChar == '=') {

                    if (parenthesisEscape) {
                        newState = STATE_ERROR;
                    } else {
                        condition.setAttributeName(sValue.toString());
                        condition.setOperator(ConditionBuilder.OPERATOR_EQUAL);
                        newState = STATE_VALUE_READING;
                        sValue = new StringBuffer();
                    }
                } else if (currentChar == '>' && expression.charAt(currentIndex + 1) == '=') {
                    if (parenthesisEscape) {
                        newState = STATE_ERROR;
                    } else {
                        currentIndex++;
                        condition.setAttributeName(sValue.toString());
                        condition.setOperator(ConditionBuilder.OPERATOR_GREATER);
                        newState = STATE_VALUE_READING;
                        sValue = new StringBuffer();
                    }
                } else if (currentChar == '<' && expression.charAt(currentIndex + 1) == '=') {
                    if (parenthesisEscape) {
                        newState = STATE_ERROR;
                    } else {
                        currentIndex++;
                        condition.setAttributeName(sValue.toString());
                        condition.setOperator(ConditionBuilder.OPERATOR_LOWER);
                        newState = STATE_VALUE_READING;
                        sValue = new StringBuffer();
                    }
                } else if (currentChar == '!' && expression.charAt(currentIndex + 1) == '=') {
                    if (parenthesisEscape) {
                        newState = STATE_ERROR;
                    } else {
                        currentIndex++;
                        condition.setAttributeName(sValue.toString());
                        condition.setOperator(ConditionBuilder.OPERATOR_NOT_EQUAL_TO);
                        newState = STATE_VALUE_READING;
                        sValue = new StringBuffer();
                    }
                } else if (currentChar == ')') {
                    if (parenthesisEscape) {
                        parenthesisEscape = false;
                        newState = STATE_PAROPENED;
                    } else {
                        newState = STATE_ERROR;
                    }
                } else {
                    newState = STATE_ERROR;
                }
                break;

            case STATE_VALUE_READING:
                if (currentChar == '\\') {
                    newState = STATE_VALUE_ESCAPED;
                } else if (currentChar == ')') {
                    condition.setOperand(sValue.toString());
                    newState = STATE_PARCLOSED;
                } else if (isToBeEscapedInValue(currentChar)) {
                    newState = STATE_ERROR;
                } else {
                    sValue.append(currentChar);
                    newState = STATE_VALUE_READING;
                }
                break;
            case STATE_VALUE_ESCAPED:
                newState = STATE_VALUE_READING;
                sValue.append(currentChar);
                break;
            case STATE_CONDITION1_READ:
                // Launch a nested parse for nested expression
                LdapExpressionParser nestedParser = new LdapExpressionParser(expression, currentIndex, false);
                Condition condition2 = nestedParser.parse();
                condition.setCondition2(condition2);
                currentIndex = nestedParser.getCurrentIndex() - 1;
                newState = STATE_CONDITION2_READ;
                break;
            case STATE_CONDITION2_READ:
                if (currentChar == ')') {
                    newState = STATE_PARCLOSED;
                } else {
                    newState = STATE_ERROR;
                }
                break;
            default:
                throw (new RuntimeException("InternalError: Unexprected state value " + currentState));
            }
            currentState = newState;
            currentIndex++;
        }
        if (currentState == STATE_ERROR) {
            throw new IllegalArgumentException("Unexpected character " + currentChar + " in expression at index "
                    + currentIndex);
        } else if (!isTerminal(currentState)) {
            throw (new IllegalArgumentException("Unexprected end of input"));
        }
        if (isParseToEnd() && (currentIndex < expression.length())) {
            throw (new IllegalArgumentException("Unexpected trailing characters at " + currentIndex));
        }

        return (condition);
    }

    private boolean isParseToEnd() {
        return (parseToEnd);
    }

    public int getCurrentIndex() {
        return (currentIndex);
    }

    private int extractCondtitionType() {
        currentIndex++;
        if (expression.startsWith(CAST_TO_STRING, currentIndex)) {
            currentIndex += CAST_TO_STRING.length() - 1;
            return ConditionBuilder.TYPE_STRING;
        } else if (expression.startsWith(CAST_TO_DATE, currentIndex)) {
            currentIndex += CAST_TO_DATE.length() - 1;
            return ConditionBuilder.TYPE_DATE;
        } else if (expression.startsWith(CAST_TO_INT, currentIndex)) {
            currentIndex += CAST_TO_INT.length() - 1;
            return ConditionBuilder.TYPE_INTEGER;
        }
        currentIndex--;
        return 0;
    }

    private boolean checkOperatorBeginsWith() {
        if (expression.startsWith(BEGIN_WITH_EXPRESSION, currentIndex)) {
            currentIndex = currentIndex + BEGIN_WITH_EXPRESSION.length() - 1;
            currentChar = expression.charAt(currentIndex);
            return true;
        } else {
            return false;
        }
    }

    private String removeSpaces(String expr) {

        StringBuffer newExpr = new StringBuffer();
        char temp;

        for (int i = 0; i < expr.length(); i++) {

            temp = expr.charAt(i);
            if (temp == ' ') {
                continue;
            }
            newExpr.append(temp);

        }

        return newExpr.toString();
    }

    // public static void main(String args[]){
    //
    // String expression1 = "Expression with spaces";
    // expression1 = expression1.replaceAll(" ", "");
    // System.out.println(expression1);
    //
    // }

}
