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
 * id $Id: MyJarFile.java 3577 2012-11-22 10:54:58Z JReich $
 * author $Author: JReich $
 * version $Revision: 3577 $
 * lastrevision $Date: 2012-11-22 11:54:58 +0100 (Thu, 22 Nov 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-11-22 11:54:58 +0100 (Thu, 22 Nov 2012) $
 */
package com.actility.ong.installer;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.List;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.util.zip.ZipEntry;

public class MyJarFile {

    private JarFile jarFile;
    private PrintStream log;

    /**
     * Constructor
     *
     * @param log the log handler
     */
    public MyJarFile(PrintStream log) {
        this.log = log;
        String jarFileName = this.getClass().getProtectionDomain().getCodeSource().getLocation().getFile();
        this.log.println("Opening file " + jarFileName);
        try {
            jarFile = new JarFile(jarFileName);
        } catch (IOException e) {
            this.log.println("Unable to open jar file "
                    + this.getClass().getProtectionDomain().getCodeSource().getLocation().getFile() + " : " + e);
            e.printStackTrace();
        }
    }

    /**
     * Constructor
     *
     * @param filename jar file name, provided if not the installer itself.
     * @param log the log handler
     */
    public MyJarFile(String filename, PrintStream log) {
        this.log = log;
        this.log.println("Opening file " + filename);
        try {
            jarFile = new JarFile(filename);
        } catch (IOException e) {
            this.log.println("Unable to open jar file "
                    + this.getClass().getProtectionDomain().getCodeSource().getLocation().getFile() + " : " + e);
            e.printStackTrace();
        }
    }

    /**
     * Close the jar file
     *
     * @throws IOException
     */
    public void close() throws IOException {
        jarFile.close();
    }

    /**
     * Get a list (space-separated) of sub-directories relatively to a path
     *
     * @param relativePath
     * @param remotePath
     * @return the list (space-separated) of sub-directories detected within the jar file.
     */
    public String getDirectories(String relativePath, String remotePath) {
        StringBuffer directories = new StringBuffer();
        for (Enumeration<? extends JarEntry> e = jarFile.entries(); e.hasMoreElements();) {
            ZipEntry ze = e.nextElement();
            String name = ze.getName();
            // log.println("Examining " + name);
            if (name.equals(relativePath)) {
                // nothing to do
            } else if (name.startsWith(relativePath)) {
                // log.println(name + " is matching");
                String rAbsPath = remotePath + name.substring(relativePath.length());
                if (ze.isDirectory()) {
                    // WARNING: must end with a trailing space in order to be able to split it later on
                    directories.append(rAbsPath).append(" ");
                }
            }
        }
        return directories.toString();
    }

    /**
     * Get the set of files that are matching provided relativePath
     *
     * @param relativePath filter path.
     * @param remotePath path on target where files are to be installed.
     * @return the list (space-separated) of sub-directories detected within the jar file.
     */
    public HashSet<MyFileEntry> getFiles(String relativePath, String remotePath) {
        HashSet<MyFileEntry> res = new HashSet<MyFileEntry>();

        for (Enumeration<? extends JarEntry> e = jarFile.entries(); e.hasMoreElements();) {
            ZipEntry ze = e.nextElement();
            String name = ze.getName();
            // log.println("Examining " + name);
            if (name.equals(relativePath)) {
                // nothing to do
            } else if (name.startsWith(relativePath)) {
                // log.println(name + " is matching");
                String rAbsPath = remotePath + name.substring(relativePath.length());
                if (!ze.isDirectory()) {
                    MyFileEntry file = new MyFileEntry(jarFile, rAbsPath, ze);
                    res.add(file);
                }
            }
        }
        return res;
    }

    /**
     * Get a JAR file entry knowing its relative path in the JAR
     *
     * @param relativeFileName
     * @return the JAR file entry
     */
    public MyFileEntry getFile(String relativeFileName) {
        MyFileEntry file = null;

        JarEntry je = jarFile.getJarEntry(relativeFileName);
        if (null != je) {
            file = new MyFileEntry(jarFile, null, je);
        }

        return file;
    }

    /**
     * Get a JAR file entry knowing its relative path in the JAR
     *
     * @param relativeFileName
     * @return the JAR file entry
     */
    public List<MyFileEntry> getSubFiles(String relativeFileName) {
        List<MyFileEntry> files = new ArrayList<MyFileEntry>();

        Enumeration<JarEntry> jarEntries = jarFile.entries();
        while (jarEntries.hasMoreElements()) {
            JarEntry jarEntry = jarEntries.nextElement();
            if (jarEntry.getName().startsWith(relativeFileName) && !jarEntry.isDirectory()) {
                files.add(new MyFileEntry(jarFile, null, jarEntry));
            }
        }

        return files;
    }

    /**
     * Get a list (space-separated) of sub-directories relatively to a path
     *
     * @param relativePath Must ends with a '/'
     * @param remotePath
     * @return the list (space-separated) of sub-directories detected within the jar file.
     */
    public List<MyFileEntry> getSubDirectoriesOnOneLevel(String relativePath) {
        List<MyFileEntry> files = new ArrayList<MyFileEntry>();

        Enumeration<JarEntry> jarEntries = jarFile.entries();
        while (jarEntries.hasMoreElements()) {
            JarEntry jarEntry = jarEntries.nextElement();
            if (!jarEntry.getName().equals(relativePath) && jarEntry.getName().startsWith(relativePath)
                    && jarEntry.isDirectory()) {
                int slashIndex = jarEntry.getName().indexOf('/', relativePath.length());
                if (slashIndex == -1 || slashIndex == (jarEntry.getName().length() - 1)) {
                    files.add(new MyFileEntry(jarFile, null, jarEntry));
                }
            }
        }

        return files;
    }

    /**
     * Extract a JAR file entry knowing its relative path in the JAR
     *
     * @param regex the string regular expression to match for the file entry name.
     * @throws FileNotFoundException if created new file cannot be found
     * @throws IOException if any error occurs while reading from the jar or writing into the created extracted file.
     * @return the JAR file entry
     */
    public File extractFile(String regex) throws FileNotFoundException, IOException {
        File file = null;
        MyFileEntry fileEntry = null;
        String relativeFileName = null;

        for (Enumeration<? extends JarEntry> e = jarFile.entries(); e.hasMoreElements() && (relativeFileName == null);) {
            ZipEntry ze = e.nextElement();
            // log.println("extractFile: examining " + ze.getName());
            if (ze.getName().matches(regex)) {
                relativeFileName = ze.getName();
                // log.println("extractFile: " + relativeFileName + " matches");
            }
        }

        if (relativeFileName != null) {
            JarEntry je = jarFile.getJarEntry(relativeFileName);
            if (null != je) {
                fileEntry = new MyFileEntry(jarFile, null, je);
                file = fileEntry.extractToLocalFile();
            }
        } else {
            log.println("extractFile: no entry matched " + regex);
        }

        return file;
    }

    /**
     * Get JAR file raw content, knowing its relative path in the JAR
     *
     * @param relativeFileName
     * @return the JAR file raw content
     */
    public byte[] getFileRawContent(String relativeFileName) {
        byte[] content = null;

        JarEntry je = jarFile.getJarEntry(relativeFileName);
        if (null != je) {
            content = new byte[(int) je.getSize()];
            this.getClass().getClassLoader().getResource(je.getName());
            InputStream is = this.getClass().getClassLoader().getResourceAsStream(je.getName());
            int totalRead = 0;
            while (true) {
                byte[] buf = new byte[1024];
                int len = 0;
                try {
                    len = is.read(buf, 0, buf.length);
                } catch (IOException e) {
                    log.println("Unable to read file " + relativeFileName + ": " + e);
                    e.printStackTrace(log);
                    totalRead = 0;
                    content = null;
                    break;
                }
                if (len <= 0) {
                    break;
                }
                System.arraycopy(buf, 0, content, totalRead, len);
                totalRead += len;
            }

            if (totalRead != je.getSize()) {
                log.println("error occured while reading file " + relativeFileName + " (read:" + totalRead + ") (file size:"
                        + je.getSize() + ")");
            }
        }

        return content;
    }
}
