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
 * id $Id: XoService.java 7355 2014-01-23 22:42:06Z JReich $
 * author $Author: JReich $
 * version $Revision: 7355 $
 * lastrevision $Date: 2014-01-23 23:42:06 +0100 (Thu, 23 Jan 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-01-23 23:42:06 +0100 (Thu, 23 Jan 2014) $
 */

package com.actility.m2m.xo;

/**
 * Xo service use to parse and build Xo objects
 */
public interface XoService {
    /**
     * Checks whether XO binary format is supported by this implementation
     *
     * @return Whether XO binary format is supported by this implementation
     */
    boolean isBinarySupported();

    /**
     * Reads an Xo object from its binary representation knowing that it is an XML representation.
     *
     * @param rawObject The binary representation of the Xo object
     * @return The built Xo object
     * @throws XoException If any problem occurs while decoding the binary representation of the Xo object
     */
    XoObject readBinaryXmlXoObject(byte[] rawObject) throws XoException;

    /**
     * Reads an Xo object from its binary representation knowing that it is an oBIX representation.
     *
     * @param rawObject The binary representation of the Xo object
     * @return The built Xo object
     * @throws XoException If any problem occurs while decoding the binary representation of the Xo object
     */
    XoObject readBinaryObixXoObject(byte[] rawObject) throws XoException;

    /**
     * Checks whether XML format is supported by this implementation
     *
     * @return Whether XML format is supported by this implementation
     */
    boolean isXmlSupported();

    /**
     * Reads an Xo object from its XML representation.
     *
     * @param rawObject The XML representation of the Xo object
     * @param encoding The XML representation encoding
     * @return The built Xo object
     * @throws XoException If any problem occurs while decoding the XML representation of the Xo object
     */
    XoObject readXmlXmlXoObject(byte[] rawObject, String encoding) throws XoException;

    /**
     * Reads an Xo object from its oBIX representation.
     *
     * @param rawObject The XML representation of the Xo object
     * @param encoding The XML representation encoding
     * @return The built Xo object
     * @throws XoException If any problem occurs while decoding the XML representation of the Xo object
     */
    XoObject readXmlObixXoObject(byte[] rawObject, String encoding) throws XoException;

    /**
     * Reads an Xo object from its partial XML representation.
     * <p>
     * A partial XML representation is an XML document which only encodes one attribute of an Xo object
     *
     * @param rawObject The partial XML representation of the Xo object
     * @param encoding The XML representation encoding
     * @param rootType The Xo type of the Xo object which is partially encoded
     * @return The built Xo object which only embeds the attribute defined in the partial XML representation
     * @throws XoException If any problem occurs while decoding the partial XML representation of the Xo object
     */
    XoObject readPartialXmlXmlXoObject(byte[] rawObject, String encoding, String rootType) throws XoException;

    /**
     * Checks whether EXI format is supported by this implementation
     *
     * @return Whether EXI format is supported by this implementation
     */
    boolean isExiSupported();

    /**
     * Reads an Xo object from its EXI representation.
     *
     * @param rawObject The EXI representation of the Xo object
     * @param encoding The EXI representation encoding
     * @return The built Xo object
     * @throws XoException If any problem occurs while decoding the EXI representation of the Xo object
     */
    XoObject readExiXmlXoObject(byte[] rawObject, String encoding) throws XoException;

    /**
     * Reads an Xo object from its EXI representation.
     *
     * @param rawObject The EXI representation of the Xo object
     * @param encoding The EXI representation encoding
     * @return The built Xo object
     * @throws XoException If any problem occurs while decoding the EXI representation of the Xo object
     */
    XoObject readExiObixXoObject(byte[] rawObject, String encoding) throws XoException;

    /**
     * Reads an Xo object from its partial EXI representation.
     * <p>
     * A partial EXI representation is an XML document which only encodes one attribute of an Xo object
     *
     * @param rawObject The partial EXI representation of the Xo object
     * @param encoding The EXI representation encoding
     * @param rootType The Xo type of the Xo object which is partially encoded
     * @return The built Xo object which only embeds the attribute defined in the partial EXI representation
     * @throws XoException If any problem occurs while decoding the partial XML representation of the Xo object
     */
    XoObject readPartialExiXmlXoObject(byte[] rawObject, String encoding, String rootType) throws XoException;

    /**
     * Builds a new Xo object which is known to be an XML representation given its Xo type.
     *
     * @param type The Xo type of the Xo object
     * @return The built Xo object
     * @throws XoException If any problem occurs while creating the Xo object or if the given type is unknown
     */
    XoObject newXmlXoObject(String type) throws XoException;

    /**
     * Builds a new Xo object which is known to be an oBIX representation given its Xo type.
     *
     * @param type The Xo type of the Xo object
     * @return The built Xo object
     * @throws XoException If any problem occurs while creating the Xo object or if the given type is unknown
     */
    XoObject newObixXoObject(String type) throws XoException;

    /**
     * Dumps statistics about XO object allocations on stdout
     */
    void dumptStats();
}
