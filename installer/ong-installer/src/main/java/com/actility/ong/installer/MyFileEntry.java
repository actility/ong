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
 * id $Id: MyFileEntry.java 3577 2012-11-22 10:54:58Z JReich $
 * author $Author: JReich $
 * version $Revision: 3577 $
 * lastrevision $Date: 2012-11-22 11:54:58 +0100 (Thu, 22 Nov 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-11-22 11:54:58 +0100 (Thu, 22 Nov 2012) $
 */
package com.actility.ong.installer;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.jar.JarFile;
import java.util.zip.ZipEntry;

/**
 * @author mlouiset
 *
 */
public class MyFileEntry {

    private JarFile jarFile;
    private ZipEntry zipEntry;
    private String remoteAbsolutePath;

    /**
     * Constructor
     *
     * @param rAbsPath the remote absolute file name
     * @param ze the zip entry handler
     */
    public MyFileEntry(JarFile jf, String rAbsPath, ZipEntry ze) {
        this.jarFile = jf;
        this.zipEntry = ze;
        this.remoteAbsolutePath = rAbsPath;
    }

    /**
     * Get the remoteAbsolutePath
     *
     * @return the remoteAbsolutePath
     */
    public String getRemoteAbsolutePath() {
        return remoteAbsolutePath;
    }

    /**
     * Get the uncompressed size of the file
     *
     * @return size in number of bytes
     */
    public long getFileSize() {
        return zipEntry.getSize();
    }

    /**
     * Get the file base name
     *
     * @return the file base name
     */
    public String getFileBaseName() {
        String name = zipEntry.getName();
        int lastIndexOfSlash = -1;
        int endIndex = -1;
        if (name.endsWith("/")) {
            endIndex = name.length() - 1;
        } else {
            endIndex = name.length();
        }
        lastIndexOfSlash = name.lastIndexOf('/', endIndex - 1);
        if (lastIndexOfSlash > 0) {
            return name.substring(lastIndexOfSlash + 1, endIndex);
        }

        return name;
    }

    /**
     * Get the file name
     *
     * @return the file name
     */
    public String getFileName() {
        return zipEntry.getName();

    }

    /**
     * Get an input stream on this file
     *
     * @return an input stream on this file
     * @throws IOException
     */
    public InputStream getFileInputStream() throws IOException {
        return jarFile.getInputStream(zipEntry);
    }

    /**
     * Extract a file from a jar, and create a representation on the local file system.
     *
     * @return the File object of extracted from the jar file.
     * @throws FileNotFoundException if created new file cannot be found
     * @throws IOException if any error occurs while reading from the jar or writing into the created extracted file.
     */
    public File extractToLocalFile() throws FileNotFoundException, IOException {

        File extractedFile = File.createTempFile("jar", null);
        extractedFile.deleteOnExit();
        InputStream in = new BufferedInputStream(getFileInputStream());
        OutputStream out = new BufferedOutputStream(new FileOutputStream(extractedFile));
        byte[] buffer = new byte[2048];
        for (;;) {
            int nBytes = in.read(buffer);
            if (nBytes <= 0) {
                break;
            }
            out.write(buffer, 0, nBytes);
        }
        out.flush();
        out.close();
        in.close();

        return extractedFile;
    }
}
