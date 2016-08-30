/*******************************************************************************
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
 *******************************************************************************/

package com.actility.m2m.util;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;

/**
 * Utils class to manipulate files.
 */
public class FileUtils {

    private FileUtils() {
        // Static class
    }

    private static final int DEFAULT_BUFFER_SIZE = 1024;

    /**
     * Normalizes a path according to the given base.
     * <p>
     * If the path is absolute, it kept unchanged.
     * If the path is relative, it is normalized according to the given base path
     *
     * @param base The base path which will be used to normalize the given path if it is relative
     * @param path The path to normalize
     * @return The normalize path as a File
     */
    public static File normalizeFile(File base, String path) {
        File pathFile = new File(path);
        if (!pathFile.isAbsolute()) {
            pathFile = new File(base.getAbsolutePath() + "/" + path);
        }
        return pathFile;
    }

    /**
     * Writes the given string to a file.
     *
     * @param filename The filename where to write the given string
     * @param encoding The encoding to use when writing the string
     * @param content The string to write in the filename
     * @throws IOException In case an IO error occurs while writing to file
     */
    public static void writeToFile(String filename, String encoding, String content) throws IOException {
        BufferedWriter out = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(filename), encoding));
        try {
            out.write(content);
        } finally {
            out.close();
        }
    }

    /**
     * Writes the given string to a file.
     *
     * @param file The file where to write the given string
     * @param encoding The encoding to use when writing the string
     * @param content The string to write in the filename
     * @throws IOException In case an IO error occurs while writing to file
     */
    public static void writeToFile(File file, String encoding, String content) throws IOException {
        BufferedWriter out = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(file), encoding));
        try {
            out.write(content);
        } finally {
            out.close();
        }
    }

    /**
     * Reads the given file and extract its content to a string.
     *
     * @param filename The filename to read
     * @param encoding The encoding of the file content
     * @return the string extracted from the given file
     * @throws IOException In case an IO error occurs while reading to file or the content cannot be decoded with the given
     *             encoding
     */
    public static String readFomFile(String filename, String encoding) throws IOException {
        StringBuffer buffer = new StringBuffer(DEFAULT_BUFFER_SIZE);
        BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(filename), encoding));
        try {
            char[] buf = new char[DEFAULT_BUFFER_SIZE];
            int numRead = 0;
            while ((numRead = reader.read(buf)) != -1) {
                buffer.append(buf, 0, numRead);
            }
        } finally {
            reader.close();
        }
        return buffer.toString();
    }

    /**
     * Reads the given file and extract its content to a string.
     *
     * @param file The file to read
     * @param encoding The encoding of the file content
     * @return the string extracted from the given file
     * @throws IOException In case an IO error occurs while reading to file or the content cannot be decoded with the given
     *             encoding
     */
    public static String readFomFile(File file, String encoding) throws IOException {
        StringBuffer buffer = new StringBuffer(DEFAULT_BUFFER_SIZE);
        BufferedReader reader = new BufferedReader(new InputStreamReader(new FileInputStream(file), encoding));
        try {
            char[] buf = new char[DEFAULT_BUFFER_SIZE];
            int numRead = 0;
            while ((numRead = reader.read(buf)) != -1) {
                buffer.append(buf, 0, numRead);
            }
        } finally {
            reader.close();
        }
        return buffer.toString();
    }

    /**
     * Copies the given file to the specified destination.
     *
     * @param toCopy The file to copy
     * @param destination The destination where to copy the file
     * @throws IOException If an I/O problem occurs while reading or writing a file.
     */
    public static void copyFile(File toCopy, File destination) throws IOException {
        InputStream in = null;
        try {
            in = new BufferedInputStream(new FileInputStream(toCopy));
            copyStream(in, destination);
        } finally {
            if (in != null) {
                in.close();
            }
        }
    }

    /**
     * Copies the given input stream to the specified destination.
     *
     * @param toCopy The input stream to copy
     * @param destination The destination where to copy the file
     * @throws IOException If an I/O problem occurs while reading or writing a file.
     */
    public static void copyStream(InputStream toCopy, File destination) throws IOException {
        InputStream in = toCopy;
        OutputStream out = null;
        try {
            out = new BufferedOutputStream(new FileOutputStream(destination));

            byte[] buf = new byte[DEFAULT_BUFFER_SIZE];
            int len;
            while ((len = in.read(buf)) > 0) {
                out.write(buf, 0, len);
            }
        } finally {
            if (out != null) {
                out.close();
            }
        }
    }

    /**
     * Deletes recursively the given directory.
     *
     * @param dir The directory to delete
     * @return Whether the deletion as succeeded or not. In case the deletion failed, the state of the directory is undefined.
     */
    public static boolean deleteDirectory(File dir) {
        if (dir.isDirectory()) {
            File[] children = dir.listFiles();
            for (int i = 0; i < children.length; i++) {
                boolean success = deleteDirectory(children[i]);
                if (!success) {
                    return false;
                }
            }
        }

        // The directory is now empty so delete it
        return dir.delete();
    }

    /**
     * Deletes all files contained in the given directory.
     * <p>
     * This function only deletes file which returns true to the method {@link File#isFile()}
     *
     * @param dir The directory whose files must be deleted
     */
    public static void deleteDirectorySubFiles(File dir) {
        if (dir.isDirectory()) {
            File[] children = dir.listFiles();
            for (int i = 0; i < children.length; i++) {
                if (children[i].isFile()) {
                    children[i].delete();
                }
            }
        }
    }
}
