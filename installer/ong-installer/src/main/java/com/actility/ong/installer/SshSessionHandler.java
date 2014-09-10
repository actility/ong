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
 * id $Id: SshSessionHandler.java 8089 2014-03-12 09:45:47Z JReich $
 * author $Author: JReich $
 * version $Revision: 8089 $
 * lastrevision $Date: 2014-03-12 10:45:47 +0100 (Wed, 12 Mar 2014) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2014-03-12 10:45:47 +0100 (Wed, 12 Mar 2014) $
 */
package com.actility.ong.installer;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.io.PrintStream;
import java.util.HashSet;
import java.util.Iterator;

import javax.swing.JFrame;

import com.jcraft.jsch.Channel;
import com.jcraft.jsch.ChannelExec;
import com.jcraft.jsch.JSch;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Session;
import com.jcraft.jsch.UserInfo;

public class SshSessionHandler {
    private static final String SHELL_EXIT_STATUS_REPORTING_PREFIX = "exit-status: ";
    private static final String SHELL_PROMPT_1_STRING = "__ONG_INSTALLER__SH_PROMPT_1__";
    private static final String SHELL_PROMPT_2_STRING = "__ONG_INSTALLER__SH_PROMPT_2__";
    private static final String SHELL_EXIT_STATUS_REPORTING_OK = SHELL_EXIT_STATUS_REPORTING_PREFIX + "0";

    private JSch jsch;
    private Session session;

    private boolean connected;
    private String remoteUser;
    private String remoteHost;
    private int remotePort;
    private PrintStream log;
    private MyJarFile myJarFile;
    private JFrame mainFrame;

    /**
     * Constructor
     *
     * @param mainFrame
     * @param log output stream in which messages are printed.
     * @param remoteUser user to use to connect to the targeted host
     * @param remoteHost host name to connect to (the targeted host)
     * @param remotePort port number on targeted host.
     */
    public SshSessionHandler(JFrame mainFrame, PrintStream log, String remoteUser, String remoteHost, int remotePort,
            MyJarFile myJarFile) {
        this.mainFrame = mainFrame;
        this.log = log;
        this.remoteUser = remoteUser;
        this.remoteHost = remoteHost;
        this.remotePort = remotePort;
        this.myJarFile = myJarFile;
        connected = false;
    }

    public Session getSession() {
        return session;
    }

    /**
     * Establish the connection with the remote host
     *
     * @throws JSchException
     */
    public void connect() throws JSchException {
        if (!connected) {

            jsch = new JSch();
            session = jsch.getSession(remoteUser, remoteHost, remotePort);

            // username and password will be given via UserInfo interface.
            UserInfo ui = new MyUserInfo(mainFrame);
            session.setUserInfo(ui);
            session.connect();

            connected = true;
        }
    }

    /**
     * Getter for remoteHost
     */
    public String getRemoteHost() {
        return remoteHost;
    }

    /**
     * Close the active connection
     */
    public void close() {
        if (connected) {
            connected = false;
            session.disconnect();
        }
    }

    /**
     * Copy file lfile (from jar) onto targeted host in file rfile.
     *
     * @param fe handle on file entry
     * @return true if file was copied successfully, false otherwise.
     * @throws IOException
     * @throws JSchException
     */
    public boolean copyFileFromJarTo(MyFileEntry fe) throws JSchException, IOException {
        return copyFileFromJarTo(fe, fe.getRemoteAbsolutePath());
    }

    /**
     * Copy file lfile (from jar) onto targeted host in file rfile.
     *
     * @param fe handle on file entry
     * @param remoteAbsolutePath the remote absolute path of the file to copy.
     * @return true if file was copied successfully, false otherwise.
     * @throws IOException
     * @throws JSchException
     */
    public boolean copyFileFromJarTo(MyFileEntry fe, String remoteAbsolutePath) throws JSchException, IOException {

        boolean res = true;
        InputStream is = null;

        // exec 'scp -t rfile' remotely
        String command = "scp -p -t " + remoteAbsolutePath;
        ChannelExec channel = (ChannelExec) session.openChannel("exec");
        try {
            (channel).setCommand(command);

            // get I/O streams for remote scp
            OutputStream out = channel.getOutputStream();
            InputStream in = channel.getInputStream();

            channel.connect();

            if (checkAck(in) != 0) {
                return false;
            }

            long filesize = fe.getFileSize();
            log.println("transferring file " + fe.getFileName() + " (size:" + filesize + "B)");
            command = "C0644 " + filesize + " " + fe.getFileBaseName() + "\n";
            try {
                out.write(command.getBytes());
                out.flush();
                if (checkAck(in) != 0) {
                    return false;
                }

                byte[] buf = new byte[1024];
                // send a content of local zip file
                is = fe.getFileInputStream();
                while (true) {
                    int len = is.read(buf, 0, buf.length);
                    if (len <= 0) {
                        break;
                    }
                    out.write(buf, 0, len); // out.flush();
                    log.print("#");
                }
                log.println();
                is.close();
                is = null;

                // send '\0'
                buf[0] = 0;
                out.write(buf, 0, 1);
                out.flush();
                if (checkAck(in) != 0) {
                    System.exit(0);
                }
                out.close();
                in.close();
            } catch (IOException e) {
                res = false;
                log.println();
                log.println("copyFileFromJar: " + e);
                e.printStackTrace();
                try {
                    if (is != null) {
                        is.close();
                    }
                } catch (Exception ee) {
                    log.println("copyFileFromJar - failed to close is: " + ee);
                    ee.printStackTrace();
                }
            }
        } finally {
            channel.disconnect();
        }

        return res;
    }

    /**
     * Copy file lfile (from jar) on the targeted host in file rfile. <br>
     * ; stop as soon as an error is detected.
     *
     * @param lfile local file name to copy
     * @param rfile remote file name
     * @return true if file was copied successfully, false otherwise.
     * @throws IOException
     * @throws JSchException
     */
    public boolean copyFileFrom(String rfile, String lfile) throws JSchException, IOException {

        boolean res = true;
        String prefix = null;
        if (new File(lfile).isDirectory()) {
            prefix = lfile + File.separator;
        }

        // exec 'scp -f rfile' remotely
        String command = "scp -f " + rfile;
        Channel channel = session.openChannel("exec");
        try {
            ((ChannelExec) channel).setCommand(command);

            // get I/O streams for remote scp
            OutputStream out = channel.getOutputStream();
            InputStream in = channel.getInputStream();

            channel.connect();

            byte[] buf = new byte[1024];

            // send '\0'
            buf[0] = 0;
            out.write(buf, 0, 1);
            out.flush();

            while (true) {
                int c = checkAck(in);
                if (c != 'C') {
                    break;
                }

                // read '0644 '
                in.read(buf, 0, 5);

                long filesize = 0L;
                while (true) {
                    if (in.read(buf, 0, 1) < 0) {
                        // error
                        res = false;
                        break;
                    }
                    if (buf[0] == ' ') {
                        break;
                    }
                    filesize = filesize * 10L + (buf[0] - '0');
                }

                String file = null;
                for (int i = 0;; i++) {
                    in.read(buf, i, 1);
                    if (buf[i] == (byte) 0x0a) {
                        file = new String(buf, 0, i);
                        break;
                    }
                }

                // System.out.println("filesize="+filesize+", file="+file);

                // send '\0'
                buf[0] = 0;
                out.write(buf, 0, 1);
                out.flush();

                // read a content of lfile
                FileOutputStream fos = new FileOutputStream(prefix == null ? lfile : prefix + file);
                int foo;
                while (true) {
                    if (buf.length < filesize) {
                        foo = buf.length;
                    } else {
                        foo = (int) filesize;
                    }
                    foo = in.read(buf, 0, foo);
                    if (foo < 0) {
                        res = false;
                        // error
                        break;
                    }
                    fos.write(buf, 0, foo);
                    filesize -= foo;
                    if (filesize == 0L) {
                        break;
                    }
                }
                fos.close();
                fos = null;

                if (checkAck(in) != 0) {
                    res = false;
                    break;
                }

                // send '\0'
                buf[0] = 0;
                out.write(buf, 0, 1);
                out.flush();
            }
        } finally {
            channel.disconnect();
        }

        return res;
    }

    /**
     * Copy file lfile (from jar) onto targeted host in file rfile; stop as soon as an error is detected.
     *
     * @param lfile local file name to copy
     * @param rfile remote file name
     * @return true if file was copied successfully, false otherwise.
     * @throws IOException
     * @throws JSchException
     */
    public boolean copyFileTo(String lfile, String rfile) throws JSchException, IOException {

        boolean res = true;
        FileInputStream fis = null;

        // exec 'scp -t rfile' remotely
        String command = "scp -p -t " + rfile;
        Channel channel = session.openChannel("exec");
        try {
            ((ChannelExec) channel).setCommand(command);

            // get I/O streams for remote scp
            OutputStream out = channel.getOutputStream();
            InputStream in = channel.getInputStream();

            channel.connect();

            // send "C0644 filesize filename", where filename should not include '/'
            long filesize = (new File(lfile)).length();
            command = "C0644 " + filesize + " ";
            if (lfile.lastIndexOf('/') > 0) {
                command += lfile.substring(lfile.lastIndexOf('/') + 1);
            } else {
                command += lfile;
            }
            command += "\n";
            out.write(command.getBytes());
            out.flush();
            if (checkAck(in) != 0) {
                System.exit(0);
            }

            // send a content of lfile
            fis = new FileInputStream(lfile);
            byte[] buf = new byte[1024];
            while (true) {
                int len = fis.read(buf, 0, buf.length);
                if (len <= 0) {
                    break;
                }
                out.write(buf, 0, len); // out.flush();
            }
            fis.close();
            fis = null;
            // send '\0'
            buf[0] = 0;
            out.write(buf, 0, 1);
            out.flush();
            if (checkAck(in) != 0) {
                res = false;
            }
        } finally {
            channel.disconnect();
        }

        return res;
    }

    /**
     * Copy files recursively from jar path on the targeted host in a targeted path. <br>
     * The function returns as soon as an error occurs.
     *
     * @param lpath local path to copy (recursively)
     * @param rpath remote targeted path
     * @param myProgressMonitor progress monitor
     * @param minIntProgress starting progression rank
     * @param maxIntProgress ending progression rank
     * @return true if all files in directory were copied successfully, false otherwise
     * @throws IOException
     * @throws JSchException
     */
    public boolean copyPathFromJar(String lpath, String rpath, MyProgressMonitor myProgressMonitor, int minIntProgress,
            int maxIntProgress) throws JSchException, IOException {

        boolean res = true;
        String dirs = myJarFile.getDirectories(lpath, rpath);

        // need to split string, because busybox on ARM target don't support commands length greater than about 700/800 bytes
        while (dirs.length() > 1) {
            int splitAt = dirs.lastIndexOf(" ", Math.min(dirs.length(), 700));
            String subDirs = dirs.substring(0, splitAt);
            log.println("Creating directories " + subDirs);
            runRemoteCommand("mkdir -p " + subDirs);
            // last iteration, dirs should equals " "
            dirs = dirs.substring(splitAt);
        }

        HashSet<MyFileEntry> files = myJarFile.getFiles(lpath, rpath);
        int i = 0;
        for (Iterator<MyFileEntry> iter = files.iterator(); iter.hasNext();) {
            MyFileEntry fe = iter.next();
            if (!copyFileFromJarTo(fe)) {
                res = false;
                break;
            }

            // update progression monitor status
            i++;
            int progress = ((i * (maxIntProgress - minIntProgress)) / files.size()) + minIntProgress;
            myProgressMonitor.setCurrentProgress(progress);
        }

        return res;

    }

    /**
     * Copy a file from a jar file, itself included in the installer which is also a jar file.
     *
     * @param ljar string regular expression for the local jar file name to copy from
     * @param lfile local file embedded in ljar to do the copy from
     * @param rpath remote targeted path
     * @return true if file was copied successfully, false otherwise
     * @throws IOException
     * @throws JSchException
     */
    public boolean copyFileFromJarFromJar(String ljar, String lfile, String rpath) throws JSchException, IOException {

        boolean res = false;

        log.println("Copying " + lfile + " file from " + ljar + " into " + rpath);

        File jarFileFromJar = myJarFile.extractFile(ljar);
        MyFileEntry mfe = null;

        if (jarFileFromJar != null) {
            MyJarFile jarFromJar = new MyJarFile(jarFileFromJar.getAbsolutePath(), log);
            mfe = jarFromJar.getFile(lfile);
            res = copyFileFromJarTo(mfe, rpath);
        }

        return res;
    }

    /**
     * Read connection input waiting for the command prompt.
     *
     * @param in the input stream on the SSH active connection
     * @param output the collected output stream for the command result
     * @return true if the awaited command prompt was read, false otherwise.
     * @throws IOException
     */
    private boolean waitForInstallerPrompt(InputStream in, StringBuffer output) throws IOException {
        byte[] tmp = new byte[1024];
        boolean readPrompt = false;
        short maxAttempts = 50;
        while ((!readPrompt) && (maxAttempts > 0)) {
            try {
                Thread.sleep(100);
            } catch (Exception e) {
            }
            while (in.available() > 0) {
                int i = in.read(tmp, 0, 1024);
                if (i < 0) {
                    break;
                }
                String readStr = new String(tmp, 0, i);
                output.append(readStr);
                log.println("read: " + readStr);
                if (readStr.endsWith(SHELL_PROMPT_1_STRING)) {
                    readPrompt = true;
                } else if (readStr.endsWith(SHELL_PROMPT_2_STRING)) {
                    readPrompt = true;
                }
            }
            maxAttempts--;
        }

        return readPrompt;
    }

    public String runRemoteCommandInShell(String command) throws JSchException, IOException {

        log.println("Execute command in shell: " + command);

        StringBuffer output = new StringBuffer();
        Channel channel = session.openChannel("shell");
        try {
            InputStream in = channel.getInputStream();
            PrintStream toServer = new PrintStream(channel.getOutputStream());
            channel.connect();

            try {
                Thread.sleep(100);
            } catch (Exception e) {
            }

            // first change the command prompt
            toServer.println("export PS1=\"" + SHELL_PROMPT_1_STRING + "\" && export PS2=\"" + SHELL_PROMPT_2_STRING + "\"");
            toServer.flush();
            if (!waitForInstallerPrompt(in, output) || channel.isClosed()) {
                log.println("Error: command unexpectedly closed while setting cmd prompt #1 and #2");
                throw new IOException("Error: command unexpectedly closed while setting cmd prompt #1 and #2");
            }
            toServer.println("export INSTALL_PS1=\"" + SHELL_PROMPT_1_STRING + "\" && export INSTALL_PS2=\""
                    + SHELL_PROMPT_2_STRING + "\"");
            toServer.flush();
            if (!waitForInstallerPrompt(in, output) || channel.isClosed()) {
                log.println("Error: command unexpectedly closed while setting cmd prompt #1 and #2 for installation");
                throw new IOException("Error: command unexpectedly closed while setting cmd prompt #1 and #2 for installation");
            }

            String[] lines = command.split("\n");
            for (String line : lines) {
                if (line.length() > 700) {
                    log.println("Warning: Command line length is long. It should be less than 700 characters: " + line);
                }
                toServer.println(line);
                toServer.flush();
                if (!waitForInstallerPrompt(in, output) || channel.isClosed()) {
                    log.println("Error: Unresponded command line: " + line + " (channelClosed:" + channel.isClosed() + ")");
                    throw new IOException("Error: Unresponded command line: " + line + " (channelClosed:" + channel.isClosed()
                            + ")");
                }
            }

            if (!channel.isClosed()) {
                toServer.println("exit $?");
                toServer.flush();
            }

            byte[] tmp = new byte[1024];
            while (true) {
                while (in.available() > 0) {
                    int i = in.read(tmp, 0, 1024);
                    if (i < 0) {
                        break;
                    }
                    output.append(new String(tmp, 0, i));
                }
                if (channel.isClosed()) {
                    output.append(SHELL_EXIT_STATUS_REPORTING_PREFIX).append(channel.getExitStatus());
                    break;
                }
                try {
                    Thread.sleep(1000);
                } catch (Exception ee) {
                }
            }
        } finally {
            channel.disconnect();
        }

        return output.toString();
    }

    /**
     * Run shell commands on remote host from file.
     *
     * @param fe file entry of a file that contains shell commands to run
     * @throws JSchException
     * @throws IOException
     */
    public String runRemoteCommandInShell(String preCmd, MyFileEntry fe) throws JSchException, IOException {

        InputStream is = fe.getFileInputStream();
        byte[] buf = new byte[1024];
        StringBuffer command = new StringBuffer(preCmd);
        command.append('\n');
        while (true) {
            int len = is.read(buf, 0, buf.length);
            if (len <= 0) {
                break;
            }
            String cmd = new String(buf, 0, len);
            command.append(cmd, 0, len);
        }
        is.close();

        return runRemoteCommandInShell(command.toString());
    }

    /**
     * Run a command on remote host.
     *
     * @param command
     * @return command output
     * @throws JSchException
     * @throws IOException
     */
    public String runRemoteCommand(String command) throws JSchException, IOException {

        StringBuffer output = new StringBuffer();
        Channel channel = session.openChannel("exec");
        try {
            if (command.length() > 700) {
                log.println("Warning: Command length is long. It should be less than 700 characters:");
            }
            log.println("Execute command: " + command);
            ((ChannelExec) channel).setCommand(command);
            channel.setInputStream(null);
            ((ChannelExec) channel).setErrStream(System.err);
            InputStream in = channel.getInputStream();
            channel.connect();

            byte[] tmp = new byte[1024];
            while (true) {
                while (in.available() > 0) {
                    int i = in.read(tmp, 0, 1024);
                    if (i < 0) {
                        break;
                    }
                    output.append(new String(tmp, 0, i));
                }
                if (channel.isClosed()) {
                    output.append(SHELL_EXIT_STATUS_REPORTING_PREFIX).append(channel.getExitStatus());
                    break;
                }
                try {
                    Thread.sleep(1000);
                } catch (Exception ee) {
                }
            }
        } finally {
            channel.disconnect();
        }

        return output.toString();
    }

    /**
     * Run shell commands on remote host from file.
     *
     * @param fe file entry of a file that contains shell commands to run
     * @throws JSchException
     * @throws IOException
     */
    public String runRemoteCommand(String preCmd, MyFileEntry fe) throws JSchException, IOException {

        InputStream is = fe.getFileInputStream();
        byte[] buf = new byte[1024];
        StringBuffer command = new StringBuffer(preCmd);
        command.append('\n');
        while (true) {
            int len = is.read(buf, 0, buf.length);
            if (len <= 0) {
                break;
            }
            String cmd = new String(buf, 0, len);
            command.append(cmd, 0, len);
        }
        is.close();

        return runRemoteCommand(command.toString());
    }

    /**
     *
     * @param in
     * @return
     * @throws IOException
     */
    private int checkAck(InputStream in) throws IOException {
        int b = in.read();
        // b may be 0 for success,
        // 1 for error,
        // 2 for fatal error,
        // -1
        if (b == 0) {
            return b;
        }
        if (b == -1) {
            return b;
        }

        if (b == 1 || b == 2) {
            StringBuffer sb = new StringBuffer();
            int c;
            do {
                c = in.read();
                sb.append((char) c);
            } while (c != '\n');
            if (b == 1) { // error
                System.out.print(sb.toString());
            }
            if (b == 2) { // fatal error
                System.out.print(sb.toString());
            }
        }
        return b;
    }

    /**
     * Do a local backup of files, first located thanks to a shell command.
     *
     * @param command the command to locate the file to backup
     * @param backupDir Directory in which backup will be performed
     * @return the local path where the file was backed up
     */
    public String[] backupFiles(String command, String backupDir, MyProgressMonitor progressMonitor) throws JSchException,
            IOException {
        String[] result = null;
        String cmdOutput = runRemoteCommand(command);
        // remove trailing ":exit-status: 0"
        if (getCmdResult(cmdOutput)) {
            InstallerUtil.getInstance().makeLocalDir(log, backupDir);

            String filesToSave = getCmdOutput(cmdOutput);
            if (filesToSave.endsWith(":")) {
                filesToSave = filesToSave.substring(0, filesToSave.length() - 1);
            }

            if (!"".equals(filesToSave)) {
                String[] files = filesToSave.split(":");
                result = new String[files.length];
                log.println("Backing up files: (nb of files:" + files.length + ")");
                for (int i = 0; i < files.length; i++) {
                    String file = files[i];
                    String localFile = backupDir + file;
                    localFile = localFile.replace('/', File.separatorChar);
                    String localDir = localFile.substring(0, localFile.lastIndexOf(File.separatorChar));
                    log.println("Backing up " + file + " file into " + localDir);
                    InstallerUtil.getInstance().makeLocalDir(log, localDir);
                    result[i] = localFile;
                    copyFileFrom(file, localFile);
                    if (null != progressMonitor) {
                        progressMonitor.setCurrentProgress((i * 100) / files.length);
                    }
                }
            }
        }

        return result;
    }

    /**
     * Retrieve the remote ran command exit status.
     *
     * @param cmdOutput the output result on remote SSH command run.
     * @return true if exit status indicates the command succeeded, false otherwise.
     */
    public boolean getCmdResult(String cmdOutput) {
        return ((cmdOutput != null) && (cmdOutput.endsWith(SHELL_EXIT_STATUS_REPORTING_OK)));
    }

    /**
     * Retrieve the command output result if cmd exit status is OK.
     *
     * @param cmdOutput the output of the previously executed command.
     * @return the command output without status or <code>null</code> if the command failed
     */
    public String getCmdOutput(String cmdOutput) {
        String output = null;
        if (getCmdResult(cmdOutput)) {
            // remove trailing command status
            output = cmdOutput.substring(0, cmdOutput.length() - SHELL_EXIT_STATUS_REPORTING_OK.length());
        }
        return output;
    }

}
