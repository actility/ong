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
 * id $Id: XoObject.java 7483 2014-02-03 17:19:04Z JReich $
 * author $Author: JReich $
 * version $Revision: 7483 $
 * lastrevision $Date: 2014-02-03 18:19:04 +0100 (Mon, 03 Feb 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-02-03 18:19:04 +0100 (Mon, 03 Feb 2014) $
 */

package com.actility.m2m.xo;

import java.util.List;

/**
 * Xo object which is a structure to represent an XML or oBIX object.
 */
public interface XoObject {

    /**
     * Special attribute to access the value of an XML simple type extension
     */
    String ATTR_VALUE = "value$";

    /**
     * Gets the Xo type of the Xo object.
     * <p>
     * The Xo type is the one defined in Xo descriptions (.ref files)
     *
     * @return The Xo type of the Xo object
     */
    String getType();

    /**
     * Gets the name of the Xo object.
     * <p>
     * This name correspond to the tag name of the XML or oBIX document
     *
     * @return The name of the Xo object
     */
    String getName();

    /**
     * Sets the name of the Xo object.
     * <p>
     * This name correspond to the tag name of the XML or oBIX document
     *
     * @param name The new name for the Xo object. A null value unsets the name
     */
    void setName(String name);

    /**
     * Sets a namespace given its prefix to the XML element represented by this Xo object
     * <p>
     * Only relevant for Xo objects which represents XML
     *
     * @param prefix The well known prefix of the namespace to set
     */
    void setNameSpace(String prefix);

    /**
     * Unsets a namespace given its prefix to the XML element represented by this Xo object
     * <p>
     * Only relevant for Xo objects which represents XML
     *
     * @param prefix The well known prefix of the namespace to remove
     */
    void unsetNameSpace(String prefix);

    /**
     * Clears all namespaces from the XML element represented by this Xo object
     * <p>
     * Only relevant for Xo objects which represents XML
     */
    void clearNameSpaces();

    /**
     * Gets an attribute of an Xo object which is known to be an Xo object.
     *
     * @param path The path to the attribute
     * @return The attribute or null if it does not exist
     */
    XoObject getXoObjectAttribute(String path);

    /**
     * Gets an attribute of an Xo object which is known to be a String.
     *
     * @param path The path to the attribute
     * @return The attribute or null if it does not exist
     */
    String getStringAttribute(String path);

    /**
     * Gets an attribute of an Xo object which is known to be a byte[].
     *
     * @param path The path to the attribute
     * @return The attribute or null if it does not exist
     */
    byte[] getBufferAttribute(String path);

    /**
     * Gets an attribute of an Xo object which is known to be a list of Xo objects.
     *
     * @param path The path to the attribute
     * @return The attribute or null if it does not exist
     */
    List getXoObjectListAttribute(String path);

    /**
     * Gets an attribute of an Xo object which is known to be a list of Strings.
     *
     * @param path The path to the attribute
     * @return The attribute or null if it does not exist
     */
    List getStringListAttribute(String path);

    /**
     * Sets the given attribute of the Xo object which is known to be an Xo object.
     *
     * @param path The path to the attribute
     * @param value The new attribute value. A null value unsets the attribute
     */
    void setXoObjectAttribute(String path, XoObject value);

    /**
     * Sets the given attribute of the Xo object which is known to be a String.
     *
     * @param path The path to the attribute
     * @param value The new attribute value. A null value unsets the attribute
     */
    void setStringAttribute(String path, String value);

    /**
     * Sets the given attribute of the Xo object which is known to be a byte[].
     *
     * @param path The path to the attribute
     * @param value The new attribute value. A null value unsets the attribute
     */
    void setBufferAttribute(String path, byte[] value);

    /**
     * Converts an XoBuffer attribute which holds an XO binary buffer to its corresponding XML buffer.
     *
     * @param path The path of the attribute
     * @param obix Whether the XML buffer is an oBIX document
     * @throws XoException If any XO problem occurs while converting the XO buffer
     */
    void xoBufferBinaryToXml(String path, boolean obix) throws XoException;

    /**
     * Converts an XoBuffer attribute which holds an XO binary buffer to its corresponding EXI buffer.
     *
     * @param path The path of the attribute
     * @param obix Whether the EXI buffer is an oBIX document
     * @throws XoException If any XO problem occurs while converting the XO buffer
     */
    void xoBufferBinaryToExi(String path, boolean obix) throws XoException;

    /**
     * Converts an XoBuffer attribute which holds an XML buffer to its corresponding XO binary buffer.
     *
     * @param path The path of the attribute
     * @param obix Whether the XML buffer is an oBIX document
     * @throws XoException If any XO problem occurs while converting the XO buffer
     */
    void xoBufferXmlToBinary(String path, boolean obix) throws XoException;

    /**
     * Converts an XoBuffer attribute which holds an XML buffer to its corresponding EXI buffer.
     *
     * @param path The path of the attribute
     * @param obix Whether the XML buffer is an oBIX document
     * @throws XoException If any XO problem occurs while converting the XO buffer
     */
    void xoBufferXmlToExi(String path, boolean obix) throws XoException;

    /**
     * Converts an XoBuffer attribute which holds an EXI buffer to its corresponding XO binary buffer.
     *
     * @param path The path of the attribute
     * @param obix Whether the EXI buffer is an oBIX document
     * @throws XoException If any XO problem occurs while converting the XO buffer
     */
    void xoBufferExiToBinary(String path, boolean obix) throws XoException;

    /**
     * Converts an XoBuffer attribute which holds an EXI buffer to its corresponding XML buffer.
     *
     * @param path The path of the attribute
     * @param obix Whether the EXI buffer is an oBIX document
     * @throws XoException If any XO problem occurs while converting the XO buffer
     */
    void xoBufferExiToXml(String path, boolean obix) throws XoException;

    /**
     * Checks whether XO binary format is supported by this implementation
     *
     * @return Whether XO binary format is supported by this implementation
     */
    boolean isBinarySupported();

    /**
     * Builds a binary representation of the Xo object as a byte array.
     *
     * @return The binary representation as a byte array
     * @throws XoException If any problem occurs while encoding the Xo object
     */
    byte[] saveBinary() throws XoException;

    /**
     * Checks whether XML format is supported by this implementation
     *
     * @return Whether XML format is supported by this implementation
     */
    boolean isXmlSupported();

    /**
     * Builds the UTF-8 encoded XML representation of the Xo object as a byte array.
     *
     * @return The UTF-8 encoded XML representation as a byte array
     * @throws XoException If any problem occurs while encoding the Xo object
     */
    byte[] saveXml() throws XoException;

    /**
     * Builds the UTF-8 encoded XML representation of an Xo object attribute as a byte array.
     *
     * @param attributeName The attribute name which identifies the attribute to encode
     * @return The UTF-8 encoded XML representation as a byte array
     * @throws XoException If any problem occurs while encoding the Xo object attribute or if the attribute does not exist or is
     *             not set
     */
    byte[] savePartialXml(String attributeName) throws XoException;

    /**
     * Checks whether EXI format is supported by this implementation
     *
     * @return Whether EXI format is supported by this implementation
     */
    boolean isExiSupported();

    /**
     * Builds the UTF-8 encoded EXI representation of the Xo object as a byte array.
     *
     * @return The UTF-8 encoded EXI representation as a byte array
     * @throws XoException If any problem occurs while encoding the Xo object
     */
    byte[] saveExi() throws XoException;

    /**
     * Builds the UTF-8 encoded EXI representation of an Xo object attribute as a byte array.
     *
     * @param attributeName The attribute name which identifies the attribute to encode
     * @return The UTF-8 encoded EXI representation as a byte array
     * @throws XoException If any problem occurs while encoding the Xo object attribute or if the attribute does not exist or is
     *             not set
     */
    byte[] savePartialExi(String attributeName) throws XoException;

    /**
     * Copies the Xo object into a new Xo object.
     *
     * @param recurse Whether to perform a deep copy or not
     * @return The built copy of the Xo object
     */
    XoObject copy(boolean recurse);

    /**
     * Frees the Xo object.
     *
     * @param cascadeMode Whether to also delete attributes and sub-attributes of the Xo object.
     */
    void free(boolean cascadeMode);
}
