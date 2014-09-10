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
 * id $Id: ByteArrayOutputStream.java 7256 2014-01-16 15:08:52Z JReich $
 * author $Author: JReich $
 * version $Revision: 7256 $
 * lastrevision $Date: 2014-01-16 16:08:52 +0100 (Thu, 16 Jan 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-01-16 16:08:52 +0100 (Thu, 16 Jan 2014) $
 */

package com.actility.m2m.util;

import java.io.IOException;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;

/**
 * None synchronized <code>ByteArrayOutputStream</code>. This class implements an output stream in which the data is written
 * into a byte array. The buffer automatically grows as data is written to it. The data can be retrieved using
 * <code>toByteArray()</code> and <code>toString()</code>.
 * <p>
 * Closing a <tt>ByteArrayOutputStream</tt> has no effect. The methods in this class can be called after the stream has been
 * closed without generating an <tt>IOException</tt>.
 *
 * @version 1.44, 05/03/00
 * @since JDK1.0
 */

public final class ByteArrayOutputStream extends OutputStream {

    /**
     * The buffer where data is stored.
     */
    private byte[] buf;

    /**
     * The number of valid bytes in the buffer.
     */
    private int count;

    /**
     * Creates a new byte array output stream. The buffer capacity is initially 32 bytes, though its size increases if
     * necessary.
     */
    public ByteArrayOutputStream() {
        this(32);
    }

    /**
     * Creates a new byte array output stream, with a buffer capacity of the specified size, in bytes.
     *
     * @param size the initial size.
     * @exception IllegalArgumentException if size is negative.
     */
    public ByteArrayOutputStream(int size) {
        if (size < 0) {
            throw new IllegalArgumentException("Negative initial size: " + size);
        }
        buf = new byte[size];
    }

    /**
     * Writes the specified byte to this byte array output stream.
     *
     * @param b the byte to be written.
     */
    public void write(int b) {
        int newcount = count + 1;
        if (newcount > buf.length) {
            byte[] newbuf = new byte[Math.max(buf.length << 1, newcount)];
            System.arraycopy(buf, 0, newbuf, 0, count);
            buf = newbuf;
        }
        buf[count] = (byte) b;
        count = newcount;
    }

    /**
     * Writes <code>len</code> bytes from the specified byte array starting at offset <code>off</code> to this byte array output
     * stream.
     *
     * @param b the data.
     * @param off the start offset in the data.
     * @param len the number of bytes to write.
     */
    public void write(byte b[], int off, int len) {
        if ((off < 0) || (off > b.length) || (len < 0) || ((off + len) > b.length) || ((off + len) < 0)) {
            throw new IndexOutOfBoundsException("Offset and length parameters go beyond the byte buffer length");
        } else if (len == 0) {
            return;
        }
        int newcount = count + len;
        if (newcount > buf.length) {
            byte[] newbuf = new byte[Math.max(buf.length << 1, newcount)];
            System.arraycopy(buf, 0, newbuf, 0, count);
            buf = newbuf;
        }
        System.arraycopy(b, off, buf, count, len);
        count = newcount;
    }

    /**
     * Writes the complete contents of this byte array output stream to the specified output stream argument, as if by calling
     * the output stream's write method using <code>out.write(buf, 0, count)</code>.
     *
     * @param out the output stream to which to write the data.
     * @exception IOException if an I/O error occurs.
     */
    public void writeTo(OutputStream out) throws IOException {
        out.write(buf, 0, count);
    }

    /**
     * Resets the <code>count</code> field of this byte array output stream to zero, so that all currently accumulated output in
     * the ouput stream is discarded. The output stream can be used again, reusing the already allocated buffer space.
     *
     * @see com.actility.m2m.util.ByteArrayInputStream#count
     */
    public void reset() {
        count = 0;
    }

    /**
     * Creates a newly allocated byte array. Its size is the current size of this output stream and the valid contents of the
     * buffer have been copied into it.
     *
     * @return the current contents of this output stream, as a byte array.
     * @see com.actility.m2m.util.ByteArrayOutputStream#size()
     */
    public byte toByteArray()[] {
        byte[] newbuf = new byte[count];
        System.arraycopy(buf, 0, newbuf, 0, count);
        return newbuf;
    }

    /**
     * Returns the current size of the buffer.
     *
     * @return the value of the <code>count</code> field, which is the number of valid bytes in this output stream.
     * @see com.actility.m2m.util.ByteArrayOutputStream#count
     */
    public int size() {
        return count;
    }

    /**
     * Converts the buffer's contents into a string, translating bytes into characters according to the platform's default
     * character encoding.
     *
     * @return String translated from the buffer's contents.
     * @since JDK1.1
     */
    public String toString() {
        return new String(buf, 0, count);
    }

    /**
     * Converts the buffer's contents into a string, translating bytes into characters according to the specified character
     * encoding.
     *
     * @param enc a character-encoding name.
     * @return String translated from the buffer's contents.
     * @throws UnsupportedEncodingException If the named encoding is not supported.
     * @since JDK1.1
     */
    public String toString(String enc) throws UnsupportedEncodingException {
        return new String(buf, 0, count, enc);
    }

    /**
     * Creates a newly allocated string. Its size is the current size of the output stream and the valid contents of the buffer
     * have been copied into it. Each character <i>c</i> in the resulting string is constructed from the corresponding element
     * <i>b</i> in the byte array such that: <blockquote>
     *
     * <pre>
     * c == (char) (((hibyte &amp; 0xff) &lt;&lt; 8) | (b &amp; 0xff))
     * </pre>
     *
     * </blockquote>
     *
     * deprecated This method does not properly convert bytes into characters. As of JDK&nbsp;1.1, the preferred way to do this
     * is via the <code>toString(String enc)</code> method, which takes an encoding-name argument, or the
     * <code>toString()</code> method, which uses the platform's default character encoding.
     *
     * @param hibyte the high byte of each resulting Unicode character.
     * @return the current contents of the output stream, as a string.
     * @see com.actility.m2m.util.ByteArrayOutputStream#size()
     * @see com.actility.m2m.util.ByteArrayOutputStream#toString(String)
     * @see com.actility.m2m.util.ByteArrayOutputStream#toString() public String toString(int hibyte) { return new String(buf, 0, count, enc);
     *      }
     */

    /**
     * Closing a <tt>ByteArrayOutputStream</tt> has no effect. The methods in this class can be called after the stream has been
     * closed without generating an <tt>IOException</tt>.
     * <p>
     *
     */
    public void close() {
    }

}
