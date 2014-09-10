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
 * id $Id: Parameterable.java 6081 2013-10-15 13:33:50Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6081 $
 * lastrevision $Date: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-10-15 15:33:50 +0200 (Tue, 15 Oct 2013) $
 */

package com.actility.m2m.servlet.song;

import java.util.Iterator;
import java.util.Set;

/**
 * The Parameterable interface is used to indicate a SONG header field value with optional parameters.
 * <p>
 * <code>
 * Header: headerValue;param1=val1;param2=val2
 * </code>
 */
public interface Parameterable {

    /**
     * Returns the value of the named parameter, or null if it is not set. A zero-length String indicates a flag parameter.
     *
     * @param key The name of the parameter to search
     * @return The parameter value, a zero-length string incase it is a flag parameter or null if the parameter does not exist
     */
    java.lang.String getParameter(java.lang.String key);

    /**
     * Returns an Iterator on the names of all parameters contained in this object. The order is the order of appearance of the
     * parameters in the Parameterable.
     *
     * @return An iterator on parameter names
     */
    Iterator getParameterNames();

    /**
     * Returns a Collection view of the parameter name-value mappings contained in this Parameterable. The order is the order of
     * appearance of the parameters in the Parameterable.
     *
     * @return The collection of parameter name and value
     */
    Set getParameters();

    /**
     * Returns the field value as a string.
     *
     * @return The field value of the parameter
     */
    java.lang.String getValue();

    /**
     * Removes the named parameter from this object. Nothing is done if the object did not already contain the specific
     * parameter.
     *
     * @param name The name of the parameter to remove
     */
    void removeParameter(java.lang.String name);

    /**
     * Sets the value of the named parameter. If this object previously contained a value for the given parameter name, then the
     * old value is replaced by the specified value. The setting of a flag parameter is indicated by specifying a zero-length
     * String for the parameter value. Calling this method with null value is equivalent to calling
     *
     * @param name The name of the parameter to set
     * @param value The value of the parameter to set
     */
    void setParameter(java.lang.String name, java.lang.String value);

    /**
     * Set the value of the field.
     *
     * @param value The value of the field to set
     */
    void setValue(java.lang.String value);

}
