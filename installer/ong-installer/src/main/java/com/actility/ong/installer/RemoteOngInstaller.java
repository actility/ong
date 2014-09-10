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
 * id $Id: RemoteOngInstaller.java 8956 2014-06-20 12:12:51Z MLouiset $
 * author $Author: MLouiset $
 * version $Revision: 8956 $
 * lastrevision $Date: 2014-06-20 14:12:51 +0200 (Fri, 20 Jun 2014) $
 * modifiedby $LastChangedBy: MLouiset $
 * lastmodified $LastChangedDate: 2014-06-20 14:12:51 +0200 (Fri, 20 Jun 2014) $
 */
package com.actility.ong.installer;

import java.awt.HeadlessException;
import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import javax.swing.JFrame;
import javax.swing.JOptionPane;

import com.actility.ong.installer.config.ArchConfig;
import com.actility.ong.installer.config.LogConfig;
import com.actility.ong.installer.config.ModuleConfig;
import com.actility.ong.installer.module.config.GsclConfig;
import com.actility.ong.installer.module.config.Iec61131Config;
import com.actility.ong.installer.module.config.KnxConfig;
import com.actility.ong.installer.module.config.ModbusConfig;
import com.actility.ong.installer.module.config.OngConfig;
import com.actility.ong.installer.module.config.WMbusConfig;
import com.actility.ong.installer.module.config.WattecoConfig;
import com.actility.ong.installer.module.config.ZigBeeConfig;
import com.jcraft.jsch.JSchException;

/**
 * @author mlouiset
 * 
 */
public class RemoteOngInstaller implements Runnable {

    private static final String[] VALID_ROOTACT_PREFIXS = new String[] { "/home", "/mnt", "/opt", "/usr", "/media" };

    private static final int ONG_INSTALLED = 1;
    private static final int FOLDER_EXISTS = 2;
    private static final int USR_EXISTS = 4;
    private static final int PING_CONFIGURED = 8;

    public static final int REMOTE_ONG_INSTALL = 0;
    public static final int REMOTE_ONG_FLASH_ZB_DONGLE = 1;

    private static final int HTTP_PORT = 1;
    private static final int TELNET_PORT = 2;
    private static final int SHUTDOWN_PORT = 4;

    private static final int PORTS = HTTP_PORT | TELNET_PORT | SHUTDOWN_PORT;

    private SshSessionHandler sshHandler;
    private PrintStream log;
    private MyJarFile myJarFile;
    private JFrame mainFrame;
    private MyProgressMonitor myProgressMonitor;
    private List<ModuleConfig> modules;
    // private String zigBeeFirmwareSubPath;
    private boolean doFlashZigBeeDongle;
    private String backupDir;
    private int remoteOngAction = REMOTE_ONG_INSTALL;
    private final String preEnvVariables;
    private final String postEnvVariables;
    private final String ongVersion;

    private final OngConfig ongConfig;
    private final GsclConfig gsclConfig;
    private final WattecoConfig wattecoConfig;
    private final ZigBeeConfig zigBeeConfig;
    private final WMbusConfig wMbusConfig;
    private final KnxConfig knxConfig;
    private final ModbusConfig modbusConfig;
    private final Iec61131Config iec61131Config;

    /**
     * Constructor
     * 
     * @param mainFrame the main frame
     * @param myJarFile the jar file of the installer
     * @param log logger handler
     * @param iec61131Config
     * @param login user login on target
     * @param targetedHost target host name or IP address.
     * @param port SSH listening port on target
     * @param flavor installation flavor (x86 or arm)
     * @param rootActDir ONG home directory where ONG image will be copy to.
     * @param nsclUri URIs of NSC to register to.
     * @param proxyHost proxy host name.
     * @param proxyPort proxy port.
     * @param doFlashZigBeeDongle
     */
    public RemoteOngInstaller(int action, JFrame mainFrame, MyJarFile myJarFile, PrintStream log, String ongVersion,
            OngConfig ongConfig, GsclConfig gsclConfig, ZigBeeConfig zigBeeConfig, WattecoConfig wattecoConfig,
            WMbusConfig wMbusConfig, KnxConfig knxConfig, ModbusConfig modbusConfig, Iec61131Config iec61131Config,
            List<ModuleConfig> modules, boolean doFlashZigBeeDongle) {
        this.remoteOngAction = action;
        this.mainFrame = mainFrame;
        this.myJarFile = myJarFile;
        this.log = log;
        this.ongConfig = ongConfig;
        this.gsclConfig = gsclConfig;
        this.zigBeeConfig = zigBeeConfig;
        this.wattecoConfig = wattecoConfig;
        this.wMbusConfig = wMbusConfig;
        this.knxConfig = knxConfig;
        this.modbusConfig = modbusConfig;
        this.iec61131Config = iec61131Config;
        // this.zigBeeFirmwareSubPath = zigBeeFirmwareSubPath;
        this.modules = modules;
        this.doFlashZigBeeDongle = doFlashZigBeeDongle;

        this.backupDir = System.getProperty("user.dir") + File.separator + InstallerUtil.ONG_REMANANT_BACKUP_DIR + "-"
                + ongConfig.getSshHost() + File.separator;

        this.ongVersion = ongVersion;

        ArchConfig arch = ongConfig.getArch();
        this.preEnvVariables = "export SHELL=\"" + arch.getShell() + "\";export PROFILE=\"" + arch.getProfileFile()
                + "\";export NEW_ROOTACT=\"" + ongConfig.getRootAct() + "\"; export INIT=\"" + arch.getInitSystem() + "\";";
        this.postEnvVariables = "export SHELL=\"" + arch.getShell() + "\";export PROFILE=\"" + arch.getProfileFile()
                + "\";export ROOTACT=\"" + ongConfig.getRootAct() + "\"; export INIT=\"" + arch.getInitSystem() + "\";";

        log.println("PARAM - ongVersion: " + this.ongVersion);
        log.println("PARAM - remoteOngAction: " + this.remoteOngAction);
        log.println("PARAM - arch: " + arch.getName());
        log.println("PARAM - modules: " + this.modules.toString());
        log.println("PARAM - ongConfig: " + this.ongConfig.toString());
        log.println("PARAM - gsclConfig: " + this.gsclConfig.toString());
        log.println("PARAM - zigBeeConfig: " + this.zigBeeConfig.toString());
        log.println("PARAM - wattecoConfig: " + this.wattecoConfig.toString());
        log.println("PARAM - wMbusConfig: " + this.wMbusConfig.toString());
        log.println("PARAM - knxConfig: " + this.knxConfig.toString());
        log.println("PARAM - modbusConfig: " + this.modbusConfig.toString());
        log.println("PARAM - iec61131Config: " + this.iec61131Config.toString());
        log.println("BUILT PARAM - preEnvVariables: " + this.preEnvVariables);
        log.println("BUILT PARAM - postEnvVariables: " + this.postEnvVariables);
    }

    @Override
    public void run() {
        connectToTarget();
        switch (remoteOngAction) {
        case REMOTE_ONG_INSTALL:
            install();
            break;
        case REMOTE_ONG_FLASH_ZB_DONGLE:
            flashZbDongle();
            break;
        default:
            log.println("*** unsupported remoteOngAction: " + remoteOngAction);
            break;
        }
        closeSession();
        System.exit(0);
    }

    private boolean checkRootAct() {
        for (String validPrefix : VALID_ROOTACT_PREFIXS) {
            if (ongConfig.getRootAct().startsWith(validPrefix)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Perform the various installation steps: <li>back up installed ONG image <li>un-install ONG if needed <li>install new ONG
     * image, phoneME and libs <li>post configuration
     */
    public void install() {
        boolean doInstall = true;
        boolean preserveUsr = false;
        boolean doBackupFolder = false;
        boolean doBackup = false;
        boolean doUninstallFolder = false;
        boolean doUninstall = false;
        // boolean flashFailed = false;
        // OngStorageManager storageMgmt = new OngStorageManager(sshHandler, log, flavor, rootActDir);
        // OngTr069Manager tr069Mgmt = new OngTr069Manager(sshHandler, log, flavor, rootActDir);

        try {
            // if (true) {
            // testSession();
            // return;
            // }
            if (!checkRootAct()) {
                int res = JOptionPane.showConfirmDialog(mainFrame, "Specified \"Actility ROOT path\" " + ongConfig.getRootAct()
                        + " is a path potentially dangerous.\n" + "Continue the installation anyway  ?",
                        "\"Actility ROOT path\" is potentially dangerous", JOptionPane.YES_NO_OPTION);

                if (res == JOptionPane.NO_OPTION) {
                    doInstall = false;
                }
            }
            int ongInstalledMask = 0;
            int portMask = 0;
            int res = 0;
            if (doInstall) {
                ongInstalledMask = ongAlreadyInstalled();
            }

            if (doInstall && (ongInstalledMask & PING_CONFIGURED) == 0) {
                res = JOptionPane.showConfirmDialog(mainFrame,
                        "The host name resolution fails on targeted host !\nPlease check LAN configuration on targeted host.\n"
                                + "Continue the installation anyway  ?", "Network configuration issue on target",
                        JOptionPane.YES_NO_OPTION);

                if (res == JOptionPane.NO_OPTION) {
                    doInstall = false;
                }
            }

            if (doInstall && (ongInstalledMask & ONG_INSTALLED) != 0) {
                res = JOptionPane.showConfirmDialog(mainFrame,
                        "ONG is already installed, ong-installer will uninstall it first.\n"
                                + "Do you want to back up current ONG image ?", "ONG is already installed",
                        JOptionPane.YES_NO_CANCEL_OPTION);

                if (res == JOptionPane.CANCEL_OPTION) {
                    // Abort installation
                    doInstall = false;
                } else if (res == JOptionPane.YES_OPTION) {
                    doBackup = true;
                    doUninstall = true;
                } else {
                    doUninstall = true;
                }
            }

            if (doInstall && (ongInstalledMask & USR_EXISTS) != 0) {
                res = JOptionPane.showConfirmDialog(mainFrame, "A usr folder exists in the installed ONG.\n"
                        + "Do you want to preserve it ?", "Usr folder detected", JOptionPane.YES_NO_CANCEL_OPTION);

                if (res == JOptionPane.CANCEL_OPTION) {
                    // Abort installation
                    doInstall = false;
                } else if (res == JOptionPane.YES_OPTION) {
                    preserveUsr = true;
                }
            }

            if (doInstall && (ongInstalledMask & FOLDER_EXISTS) != 0) {
                res = JOptionPane.showConfirmDialog(mainFrame,
                        "Specified \"Actility ROOT path\" already exists on the targeted platform, ong-installer will remove "
                                + "it first.\n" + "Do you want to back up current \"Actility ROOT path\" folder ?",
                        "\"Actility ROOT path\" already exists", JOptionPane.YES_NO_CANCEL_OPTION);

                if (res == JOptionPane.CANCEL_OPTION) {
                    // Abort installation
                    doInstall = false;
                } else if (res == JOptionPane.YES_OPTION) {
                    doBackupFolder = true;
                    doUninstallFolder = true;
                } else {
                    doUninstallFolder = true;
                }
            }

            if (doInstall && doUninstall) {
                myProgressMonitor = new MyProgressMonitor(mainFrame, "Stopping ONG");
                myProgressMonitor.setIndeterminate(true);
                stopOng();
                // storageMgmt.backup();
                // tr069Mgmt.backup();
                myProgressMonitor.setIndeterminate(false);
                myProgressMonitor.setCurrentProgress(100);
                myProgressMonitor.close();
            }

            if (doInstall) {
                portMask = checkPorts();
            }

            if (doInstall && (portMask & PORTS) != 0) {
                String servers = "";
                if ((portMask & HTTP_PORT) != 0) {
                    servers += "HTTP ";
                }
                if ((portMask & TELNET_PORT) != 0) {
                    servers += "TELNET ";
                }
                if ((portMask & SHUTDOWN_PORT) != 0) {
                    servers += "SHUTDOWN ";
                }
                res = JOptionPane.showConfirmDialog(mainFrame, "ONG ports are already bound on the targeted server: " + servers
                        + "\n" + "Do you want to continue the installation anyway ?", "Configured ports are already bound",
                        JOptionPane.YES_NO_OPTION);
                if (res == JOptionPane.NO_OPTION) {
                    doInstall = false;
                }
            }

            if (doInstall && doBackupFolder) {
                myProgressMonitor = new MyProgressMonitor(mainFrame,
                        "Backing up current \"Actility ROOT path\" folder on targeted platform");
                if (!backupRootact()) {
                    throw new InstallerException("\"Actility ROOT path\" backup failed");
                }
                myProgressMonitor.close();
            }

            if (doInstall && doBackup) {
                myProgressMonitor = new MyProgressMonitor(mainFrame, "Backing up ONG image");
                if (!backupOng()) {
                    throw new InstallerException("ONG backup failed");
                }
                myProgressMonitor.close();
            }

            if (doInstall && doUninstallFolder) {
                myProgressMonitor = new MyProgressMonitor(mainFrame, "Uninstalling \"Actility ROOT path\"");
                myProgressMonitor.setIndeterminate(true);
                if (!uninstallRootact()) {
                    throw new InstallerException("\"Actility ROOT path\" uninstallation failed");
                }
                myProgressMonitor.setIndeterminate(false);
                myProgressMonitor.setCurrentProgress(100);
                myProgressMonitor.close();
            }

            if (doInstall && doUninstall) {
                myProgressMonitor = new MyProgressMonitor(mainFrame, "Uninstalling ONG");
                myProgressMonitor.setIndeterminate(true);
                if (!uninstall(preserveUsr)) {
                    throw new InstallerException("ONG uninstallation Failed");
                }
                myProgressMonitor.setIndeterminate(false);
                myProgressMonitor.setCurrentProgress(100);
                myProgressMonitor.close();
            }

            if (doInstall) {
                // ////////////////////////////////////////////////////////////////////////////////////////////////////////////
                // do install ONG
                myProgressMonitor = new MyProgressMonitor(mainFrame, "Installing ONG image");
                if (!preInstall() || !installRootAct() || !postInstall()) {
                    throw new InstallerException("ONG Installation Failed");
                }
                myProgressMonitor.close();

                // storageMgmt.restore();
                // tr069Mgmt.restore();

                // if (doInstall && doFlashZigBeeDongle) {
                // // ////////////////////////////////////////////////////////////////////////////////////////////////////
                // // do flash the dongle
                // myProgressMonitor = new MyProgressMonitor(mainFrame, "Flashing CC2531 dongle");
                // if (!flashCC2531Dongle()) {
                // throw new InstallerException("Failed to flash the dongle");
                // }
                // myProgressMonitor.close();
                // }

                // ////////////////////////////////////////////////////////////////////////////////////////////////////////
                // Configure logs

                myProgressMonitor = new MyProgressMonitor(mainFrame, "Configuring ONG");
                myProgressMonitor.setIndeterminate(true);
                if (!configureLogs() || !configureModules()) {
                    throw new InstallerException("Configuration failed");
                }
                myProgressMonitor.setIndeterminate(false);
                myProgressMonitor.setCurrentProgress(100);
                myProgressMonitor.close();

                // ////////////////////////////////////////////////////////////////////////////////////////////////////////
                // try to restart ONG

                myProgressMonitor = new MyProgressMonitor(mainFrame, "Restarting ONG");
                myProgressMonitor.setIndeterminate(true);
                if (!restartOng(!preserveUsr)) {
                    throw new InstallerException("Installation failed");
                }
                myProgressMonitor.setIndeterminate(false);
                myProgressMonitor.setCurrentProgress(100);
                myProgressMonitor.close();

                if (ongConfig.isDev()) {
                    JOptionPane.showMessageDialog(mainFrame, "The ong-installer didn't change your environment settings; "
                            + "you will need to define the environment variable ROOTACT to " + ongConfig.getRootAct()
                            + " in order to complete your installation.", "Environment", JOptionPane.WARNING_MESSAGE);
                }
                JOptionPane.showMessageDialog(mainFrame, "Installation completed successfully !", "Installation completed",
                        JOptionPane.PLAIN_MESSAGE);
            }
        } catch (InstallerException e) {
            JOptionPane.showMessageDialog(mainFrame, e.getMessage() + ". Please check logs", "Installation failed",
                    JOptionPane.ERROR_MESSAGE);
        } catch (HeadlessException e) {
            e.printStackTrace(log);
            JOptionPane.showMessageDialog(mainFrame, "Exception occured while installing ONG (" + e.getMessage()
                    + "). Please check logs", "Installation failed", JOptionPane.ERROR_MESSAGE);
        } catch (JSchException e) {
            e.printStackTrace(log);
            JOptionPane.showMessageDialog(mainFrame, "Exception occured while installing ONG (" + e.getMessage()
                    + "). Please check logs", "Installation failed", JOptionPane.ERROR_MESSAGE);
        } catch (IOException e) {
            e.printStackTrace(log);
            JOptionPane.showMessageDialog(mainFrame, "Exception occured while installing ONG (" + e.getMessage()
                    + "). Please check logs", "Installation failed", JOptionPane.ERROR_MESSAGE);
        } catch (RuntimeException e) {
            e.printStackTrace(log);
            JOptionPane.showMessageDialog(mainFrame, "Exception occured while installing ONG (" + e.getMessage()
                    + "). Please check logs", "Installation failed", JOptionPane.ERROR_MESSAGE);
        } finally {
            localCleanUp();
        }
    }

    /**
     * Flash the TI CC2531 dongle for ZigBee GIP access
     */
    public void flashZbDongle() {

        boolean flashFailed = false;

        try {
            if (doFlashZigBeeDongle) {
                myProgressMonitor = new MyProgressMonitor(mainFrame, "Stopping ONG");
                myProgressMonitor.setIndeterminate(true);
                stopOng();
                myProgressMonitor.setIndeterminate(false);
                myProgressMonitor.setCurrentProgress(100);
                myProgressMonitor.close();

                // ////////////////////////////////////////////////////////////////////////////////////////////////////
                // do flash the dongle
                myProgressMonitor = new MyProgressMonitor(mainFrame, "Flashing CC2531 dongle");
                if (!flashCC2531Dongle()) {
                    flashFailed = true;
                }
                myProgressMonitor.close();
            }

            // ////////////////////////////////////////////////////////////////////////////////////////////////////////
            // try to restart ONG
            if (flashFailed) {
                JOptionPane.showMessageDialog(mainFrame, "Failed to flash the dongle. Please try again", "Flash failed",
                        JOptionPane.ERROR_MESSAGE);
            } else {
                myProgressMonitor = new MyProgressMonitor(mainFrame, "Restarting ONG");
                restartOng(false);
                myProgressMonitor.close();
                JOptionPane.showMessageDialog(mainFrame, "Flash completed successfully !", "Flash completed",
                        JOptionPane.PLAIN_MESSAGE);
            }

        } catch (HeadlessException e) {
            e.printStackTrace(log);
            JOptionPane.showMessageDialog(mainFrame, "Exception occured while installing ONG (" + e.getMessage()
                    + "). Please check logs", "Installation failed", JOptionPane.ERROR_MESSAGE);
        } catch (JSchException e) {
            e.printStackTrace(log);
            JOptionPane.showMessageDialog(mainFrame, "Exception occured while installing ONG (" + e.getMessage()
                    + "). Please check logs", "Installation failed", JOptionPane.ERROR_MESSAGE);
        } catch (IOException e) {
            e.printStackTrace(log);
            JOptionPane.showMessageDialog(mainFrame, "Exception occured while installing ONG (" + e.getMessage()
                    + "). Please check logs", "Installation failed", JOptionPane.ERROR_MESSAGE);
        } catch (RuntimeException e) {
            e.printStackTrace(log);
            JOptionPane.showMessageDialog(mainFrame, "Exception occured while installing ONG (" + e.getMessage()
                    + "). Please check logs", "Installation failed", JOptionPane.ERROR_MESSAGE);
        }
    }

    /**
     * Establish SSH link with target host.
     */
    protected boolean connectToTarget() {
        boolean connected = false;
        log.println("Log in to target host: " + ongConfig.getSshLogin() + "@" + ongConfig.getSshHost() + ":"
                + ongConfig.getSshPort());
        sshHandler = new SshSessionHandler(mainFrame, log, ongConfig.getSshLogin(), ongConfig.getSshHost(),
                ongConfig.getSshPort(), myJarFile);
        try {
            sshHandler.connect();
            connected = true;
        } catch (JSchException e) {
            log.println("RemoteOngInstaller - ConnectToTarget: " + e);
            e.printStackTrace();
            connected = false;
            JOptionPane.showMessageDialog(mainFrame, "Unable to connect to target !", "Connection error",
                    JOptionPane.ERROR_MESSAGE);
        }

        return connected;
    }

    /**
     * Close SSH session.
     */
    private void closeSession() {
        sshHandler.close();
    }

    /**
     * Check if ONG is already installed on the target.
     * 
     * @return true if ONG was detected on target, false otherwise
     * @throws JSchException
     * @throws IOException
     */
    protected int ongAlreadyInstalled() throws JSchException, IOException {
        int result = 0;
        log.println(">>> Check if ONG is already installed");
        String cmdOutput = sshHandler.runRemoteCommand(preEnvVariables,
                myJarFile.getFile("common/installer/scripts/check_install"));
        log.println("<<< Check installation result: " + cmdOutput);

        if (null != cmdOutput && cmdOutput.contains("PING")) {
            result |= PING_CONFIGURED;
        }
        if (null != cmdOutput && cmdOutput.contains("ONG")) {
            result |= ONG_INSTALLED;
        }
        if (null != cmdOutput && cmdOutput.contains("USR")) {
            result |= USR_EXISTS;
        }
        if (null != cmdOutput && cmdOutput.contains("FOLDER")) {
            result |= FOLDER_EXISTS;
        }

        return result;
    }

    /**
     * Back up ONG image installed on targeted box
     * 
     * @return true if operation succeeded, false otherwise
     * @throws JSchException
     * @throws IOException
     */
    protected boolean backupOng() throws JSchException, IOException {
        InstallerUtil.getInstance().makeLocalDir(log, backupDir);
        boolean res = false;
        log.println(">>> Beginning remote ONG back up procedure at " + InstallerUtil.getInstance().getDateFormatForLogs());
        String backupPath = System.getProperty("user.dir") + System.getProperty("file.separator") + "ong-backup-"
                + ongConfig.getSshHost() + "-" + InstallerUtil.getInstance().getDateFormatForFileName()
                + System.getProperty("file.separator");
        if (sshHandler.backupFiles(ongConfig.getArch().getRetrieveEnvCmd()
                + " && find $ROOTACT -type f | tr -s '[:space:]' ':'", backupPath, myProgressMonitor) != null) {
            JOptionPane.showMessageDialog(mainFrame, "Back up available in " + backupPath, "ONG back up completed",
                    JOptionPane.INFORMATION_MESSAGE);
            res = true;
        } else {
            log.println("BackupOng: failed to backup ONG files !!");
            res = false;
        }
        log.println("<<< Ending remote ONG back up procedure at " + InstallerUtil.getInstance().getDateFormatForLogs());
        myProgressMonitor.setCurrentProgress(100);
        return res;
    }

    /**
     * Check if ONG is already installed on the target.
     * 
     * @return true if ONG was detected on target, false otherwise
     * @throws JSchException
     * @throws IOException
     */
    protected int checkPorts() throws JSchException, IOException {
        int result = 0;
        log.println(">>> Check if ONG ports are already bound");
        String variables = preEnvVariables + "export HTTP_PORT=\"" + 8080 + "\"; export TELNET_PORT=\"" + 2323
                + "\"; export SHUTDOWN_PORT=\"" + 4444 + "\";";
        String cmdOutput = sshHandler.runRemoteCommand(variables, myJarFile.getFile("common/installer/scripts/check_ports"));
        log.println("<<< Result: " + cmdOutput);

        if (null != cmdOutput && cmdOutput.contains("HTTP")) {
            result |= HTTP_PORT;
        }
        if (null != cmdOutput && cmdOutput.contains("TELNET")) {
            result |= TELNET_PORT;
        }
        if (null != cmdOutput && cmdOutput.contains("SHUTDOWN")) {
            result |= SHUTDOWN_PORT;
        }

        return result;
    }

    /**
     * Back up rootact installed on targeted box
     * 
     * @return true if operation succeeded, false otherwise
     * @throws JSchException
     * @throws IOException
     */
    protected boolean backupRootact() throws JSchException, IOException {
        boolean res = false;
        log.println(">>> Beginning remote \"Actility ROOT path\" back up procedure at "
                + InstallerUtil.getInstance().getDateFormatForLogs());
        String backupPath = System.getProperty("user.dir") + System.getProperty("file.separator") + "rootact-backup-"
                + ongConfig.getSshHost() + "-" + InstallerUtil.getInstance().getDateFormatForFileName()
                + System.getProperty("file.separator");
        if (sshHandler.backupFiles("find " + ongConfig.getRootAct() + " -type f | tr -s '[:space:]' ':'", backupPath,
                myProgressMonitor) != null) {
            JOptionPane.showMessageDialog(mainFrame, "Back up available in " + backupPath,
                    "\"Actility ROOT path\" back up completed", JOptionPane.INFORMATION_MESSAGE);
            res = true;
        } else {
            log.println("BackupFolder: failed to backup \"Actility ROOT path\" files !!");
            res = false;
        }
        log.println("<<< Ending remote \"Actility ROOT path\" back up procedure at "
                + InstallerUtil.getInstance().getDateFormatForLogs());
        myProgressMonitor.setCurrentProgress(100);
        return res;
    }

    /**
     * Uninstall ONG image installed on targeted box
     * 
     * @return true if operation succeeded, false otherwise
     * @throws JSchException
     * @throws IOException
     */
    private boolean uninstall(boolean preserveUsr) throws JSchException, IOException {
        log.println(">>> Beginning remote ONG uninstall at " + InstallerUtil.getInstance().getDateFormatForLogs());
        String uninstall = sshHandler.runRemoteCommand("export PRESERVE_USR=" + ((preserveUsr) ? "1" : "0") + ";"
                + preEnvVariables, myJarFile.getFile("common/installer/scripts/uninstall"));
        log.println("<<< Ending ONG uninstallation: " + uninstall);

        if (sshHandler.getCmdResult(uninstall)) {
            return true;
        }
        return false;
    }

    /**
     * Uninstall Rootact image installed on targeted box
     * 
     * @return true if operation succeeded, false otherwise
     * @throws JSchException
     * @throws IOException
     */
    private boolean uninstallRootact() throws JSchException, IOException {
        log.println(">>> Beginning remote \"Actility ROOT path\" uninstall at "
                + InstallerUtil.getInstance().getDateFormatForLogs());
        String uninstall = sshHandler.runRemoteCommand(preEnvVariables,
                myJarFile.getFile("common/installer/scripts/uninstall_rootact"));
        log.println("<<< Ending \"Actility ROOT path\" uninstallation: " + uninstall);

        if (sshHandler.getCmdResult(uninstall)) {
            return true;
        }
        return false;
    }

    /**
     * Install Ong remotely
     * 
     * @return true if operation succeeded, false otherwise
     * @throws IOException
     * @throws JSchException
     */
    protected boolean installRootAct() throws JSchException, IOException {
        log.println(">>> Beginning remote install of ONG at " + InstallerUtil.getInstance().getDateFormatForLogs());

        boolean res = true;
        int i = 0;
        double section = 40.0 / modules.size();
        for (ModuleConfig module : modules) {
            if (res) {
                res = sshHandler.copyPathFromJar("common/" + module.getName() + "/data/", ongConfig.getRootAct(),
                        myProgressMonitor, (int) (section * i) + 10, (int) (section * (i + 1)) + 10);
                ++i;
            }
            if (res) {
                res = sshHandler.copyPathFromJar("arch/" + ongConfig.getArch().getName() + "/" + module.getName() + "/data/",
                        ongConfig.getRootAct(), myProgressMonitor, (int) (section * i) + 10, (int) (section * (i + 1)) + 10);
                ++i;
            }
        }

        log.println("<<< Ending remote install of ONG at " + InstallerUtil.getInstance().getDateFormatForLogs());
        return res;
    }

    /**
     * Pre-installation consists in running shell scripts to prepare the installation and remove a previous ONG version...
     * 
     * @throws IOException
     * @throws JSchException
     */
    protected boolean preInstall() throws JSchException, IOException {
        log.println(">>> Beginning pre installation of ONG at " + InstallerUtil.getInstance().getDateFormatForLogs());

        String cmdOutput;
        boolean res = true;
        int i = 0;
        List<MyFileEntry> preInstallScripts = myJarFile.getSubFiles("common/installer/preinst");
        preInstallScripts.addAll(myJarFile.getSubFiles("arch/" + ongConfig.getArch().getName() + "/installer/preinst"));
        Iterator<MyFileEntry> it = preInstallScripts.iterator();

        while (it.hasNext() && (res)) {
            MyFileEntry fileEntry = it.next();
            log.println("  >>> Execute pre install script: " + fileEntry.getFileBaseName());
            cmdOutput = sshHandler.runRemoteCommandInShell(preEnvVariables, fileEntry);
            log.println("  <<< Result: " + cmdOutput);
            i++;
            myProgressMonitor.setCurrentProgress(0 + ((5 * i) / preInstallScripts.size()));

            if (!sshHandler.getCmdResult(cmdOutput)) {
                res = false;
            }
        }

        res = true;
        preInstallScripts = new ArrayList<MyFileEntry>();
        for (ModuleConfig module : modules) {
            preInstallScripts.addAll(myJarFile.getSubFiles("common/" + module.getName() + "/control/preinst"));
            preInstallScripts.addAll(myJarFile.getSubFiles("arch/" + ongConfig.getArch().getName() + "/" + module.getName()
                    + "/control/preinst"));
        }
        it = preInstallScripts.iterator();
        i = 0;
        while (it.hasNext() && (res)) {
            MyFileEntry fileEntry = it.next();
            log.println("  >>> Execute pre install script: " + fileEntry.getFileBaseName());
            cmdOutput = sshHandler.runRemoteCommandInShell(preEnvVariables, fileEntry);
            log.println("  <<< Result: " + cmdOutput);
            i++;
            myProgressMonitor.setCurrentProgress(5 + ((5 * i) / preInstallScripts.size()));

            if (!sshHandler.getCmdResult(cmdOutput)) {
                res = false;
            }
        }

        log.println("<<< Ending pre installation of ONG at " + InstallerUtil.getInstance().getDateFormatForLogs());

        return res;
    }

    /**
     * Post-installation consists in running shell scripts for setting rights, setting environment variables, making symbolic
     * links...
     * 
     * @throws IOException
     * @throws JSchException
     */
    protected boolean postInstall() throws JSchException, IOException {
        // InputStream is = getClass().getClassLoader().getResourceAsStream("META-INF/application.properties");
        // Properties props = new Properties();
        // String propertyVersion = null;
        // try {
        // props.load(is);
        // propertyVersion = props.getProperty("version");
        // } catch (IOException e1) {
        // propertyVersion = "(version unknown)";
        // }

        log.println(">>> Beginning post installation of ONG at " + InstallerUtil.getInstance().getDateFormatForLogs());

        // Post install deployed apus
        String cmdOutput;
        boolean res = true;
        int i = 0;
        List<MyFileEntry> postInstallScripts = new ArrayList<MyFileEntry>();
        for (ModuleConfig module : modules) {
            postInstallScripts.addAll(myJarFile.getSubFiles("common/" + module.getName() + "/control/postinst"));
            postInstallScripts.addAll(myJarFile.getSubFiles("arch/" + ongConfig.getArch().getName() + "/" + module.getName()
                    + "/control/postinst"));
        }

        Iterator<MyFileEntry> it = postInstallScripts.iterator();

        while (it.hasNext() && (res)) {
            MyFileEntry fileEntry = it.next();
            log.println("  >>> Execute post install script: " + fileEntry.getFileBaseName());
            cmdOutput = sshHandler.runRemoteCommandInShell(postEnvVariables, fileEntry);
            log.println("  <<< Result: " + cmdOutput);
            i++;
            myProgressMonitor.setCurrentProgress(90 + ((5 * i) / postInstallScripts.size()));

            if (!sshHandler.getCmdResult(cmdOutput)) {
                res = false;
            }
        }

        postInstallScripts = myJarFile.getSubFiles("common/installer/postinst");
        // Do not register ONG to boot if dev version
        if (!ongConfig.isDev()) {
            postInstallScripts.addAll(myJarFile.getSubFiles("arch/" + ongConfig.getArch().getName() + "/installer/postinst"));
        } else {
            postInstallScripts.add(myJarFile.getFile("common/installer/scripts/fix_ong_cfg"));
        }
        it = postInstallScripts.iterator();

        while (it.hasNext() && (res)) {
            MyFileEntry fileEntry = it.next();
            log.println("  >>> Execute post install script: " + fileEntry.getFileBaseName());
            cmdOutput = sshHandler.runRemoteCommandInShell(postEnvVariables, fileEntry);
            log.println("  <<< Result: " + cmdOutput);
            i++;
            myProgressMonitor.setCurrentProgress(95 + ((5 * i) / postInstallScripts.size()));

            if (!sshHandler.getCmdResult(cmdOutput)) {
                res = false;
            }
        }

        log.println("<<< Ending post installation of ONG at " + InstallerUtil.getInstance().getDateFormatForLogs());

        return res;
    }

    /**
     * Pre-installation step, stopping ONG
     * 
     * @throws IOException
     * @throws JSchException
     */
    protected boolean stopOng() throws JSchException, IOException {
        log.println(">>> Stopping ONG");
        String cmdOutput = sshHandler.runRemoteCommand(ongConfig.getArch().getRetrieveEnvCmd()
                + " && echo $ROOTACT && export LD_LIBRARY_PATH=$ROOTACT/lib && $ROOTACT/etc/ong stop ");
        log.println("<<< Result: " + cmdOutput);

        if (sshHandler.getCmdResult(cmdOutput)) {
            return true;
        }
        return false;
    }

    /**
     * Configure logs according to the available disk space.
     * 
     * @throws IOException
     * @throws JSchException
     */
    private boolean configureLogs() throws JSchException, IOException {
        log.println(">>> Configure logs at " + InstallerUtil.getInstance().getDateFormatForLogs());

        String variables = postEnvVariables;
        // Max 50Mo of logs per file
        long maxLogSizePerDynamicGeneration = 1024L * 1024L * 50L;
        // Min 1Ko of logs per file
        long minLogSizePerDynamicGeneration = 1024L;
        long maxLogSize = ongConfig.getMaxLogSize();
        // TODO log size for storage.driver.sqlite are fixed (50Ko * 2 files) (add in module.properties)
        maxLogSize -= (100 * 1024);
        if (maxLogSize < 0) {
            maxLogSize = 0;
        }

        double totalWeight = 0.0;
        for (ModuleConfig module : modules) {
            for (LogConfig logConfig : module.getLogConfigs()) {
                totalWeight += logConfig.getWeight();
            }
        }

        for (ModuleConfig module : modules) {
            if (module.getLogConfigs().length > 0) {
                String logVariables = variables;
                for (LogConfig logConfig : module.getLogConfigs()) {
                    long logTotalSizeKb = (long) ((maxLogSize * logConfig.getWeight()) / (totalWeight * 1024.0));
                    long logTotalSize = (long) ((maxLogSize * logConfig.getWeight()) / (totalWeight));
                    if (!logConfig.isAppendOnRestart()) {
                        // Divide by 2 because all files can be copied for backup
                        logTotalSizeKb >>= 1;
                        logTotalSize >>= 1;
                    }
                    if (logConfig.isFixed()) {
                        logTotalSizeKb /= logConfig.getFixedGeneration();
                        logTotalSize /= logConfig.getFixedGeneration();
                        logVariables += " export LOG_" + logConfig.getName().toUpperCase() + "_SIZE_KB=" + logTotalSizeKb + ";";
                        logVariables += " export LOG_" + logConfig.getName().toUpperCase() + "_SIZE=" + logTotalSize + ";";
                    } else {
                        // Compute maxGeneration and minGeneration
                        long maxGeneration = logTotalSize / minLogSizePerDynamicGeneration;
                        long minGeneration = logTotalSize / maxLogSizePerDynamicGeneration;
                        long generation = 7L;
                        if (minGeneration > 7L) {
                            generation = minGeneration;
                        } else if (maxGeneration < 7L) {
                            generation = maxGeneration;
                        }
                        logTotalSizeKb /= generation;
                        logTotalSize /= generation;
                        logVariables += " export LOG_" + logConfig.getName().toUpperCase() + "_SIZE_KB=" + logTotalSizeKb + ";";
                        logVariables += " export LOG_" + logConfig.getName().toUpperCase() + "_SIZE=" + logTotalSize + ";";
                        logVariables += " export LOG_" + logConfig.getName().toUpperCase() + "_GENERATION=" + generation + ";";
                    }
                }
                log.println(">>> Configure logs for module " + module.getName());
                String cmdOutput = sshHandler.runRemoteCommandInShell(logVariables,
                        myJarFile.getFile("module/" + module.getName() + "/configure_logs"));
                log.println("<<< Result: " + cmdOutput);
                if (!sshHandler.getCmdResult(cmdOutput)) {
                    return false;
                }
            }
        }
        log.println("<<< Ending log configuration of ONG at " + InstallerUtil.getInstance().getDateFormatForLogs());

        return true;
    }

    /**
     * Configure modules.
     * 
     * @throws IOException
     * @throws JSchException
     */
    private boolean configureModules() throws JSchException, IOException {

        // Init variables
        Map<String, String> variables = new HashMap<String, String>();
        variables.put("DOMAIN_NAME", ongConfig.getDomainName());
        variables.put("GSCL_COAP_HOST", ongConfig.getGsclCoapHost());
        variables.put("GSCL_COAP_PORT", String.valueOf(ongConfig.getGsclCoapPort()));
        variables.put("NSCL_URI", ongConfig.getNsclUri());
        variables.put("ONG_NAME", ongConfig.getOngName());
        variables.put("ONG_VERSION", ongVersion);
        variables.put("PROXY_HOST", gsclConfig.getHttpProxyHost());
        variables.put("PROXY_PORT", String.valueOf(gsclConfig.getHttpProxyPort()));
        variables.put("SSH_OPTIONS", ongConfig.getArch().getSshOptions());
        variables.put("TPK_DEV_URI", ongConfig.getTpkDevUri());

        for (ModuleConfig module : modules) {
            MyFileEntry configureFile = myJarFile.getFile("module/" + module.getName() + "/configure");
            if (configureFile != null) {
                String configureVariables = postEnvVariables;
                for (String variable : module.getConfigureVariables()) {
                    configureVariables += " export " + variable + "=\"" + variables.get(variable) + "\";";
                }
                log.println(">>> Configure module " + module.getName() + " with variables " + configureVariables + " at "
                        + InstallerUtil.getInstance().getDateFormatForLogs());
                String cmdOutput = sshHandler.runRemoteCommandInShell(configureVariables, configureFile);
                log.println("<<< Result: " + cmdOutput);
                if (!sshHandler.getCmdResult(cmdOutput)) {
                    return false;
                }
            }
        }
        return true;
    }

    /**
     * Post-installation consists in running shell scripts for setting rights, setting environment variables, making symbolic
     * links...
     * 
     * @param resetOng
     * 
     * @throws IOException
     * @throws JSchException
     */
    protected boolean restartOng(boolean resetOng) throws JSchException, IOException {
        // InputStream is = getClass().getClassLoader().getResourceAsStream("META-INF/application.properties");
        // Properties props = new Properties();
        // String propertyVersion = null;
        // try {
        // props.load(is);
        // propertyVersion = props.getProperty("version");
        // } catch (IOException e1) {
        // propertyVersion = "(version unknown)";
        // }
        log.println(">>> Restart ONG at " + InstallerUtil.getInstance().getDateFormatForLogs() + " (resetOng:" + resetOng + ")");

        String cmdOutput = null;
        String cmdOptions = "export ONG_RESTART_OPTIONS=;";
        // WARNING: to be able to run over ssh with busybox, command length MUST be less than 700 characters

        if (resetOng) {
            cmdOptions = "export ONG_RESTART_OPTIONS=-i;";
        }
        cmdOutput = sshHandler.runRemoteCommand(postEnvVariables + cmdOptions,
                myJarFile.getFile("arch/" + ongConfig.getArch().getName() + "/installer/scripts/restart_ong"));
        log.println("<<< Result: " + cmdOutput);

        if (sshHandler.getCmdResult(cmdOutput)) {
            return true;
        }
        return false;
    }

    /**
     * Clean up local installation files
     * 
     * @return always true
     */
    private boolean localCleanUp() {
        // InstallerUtil.getInstance().rmLocalDir(InstallerUtil.getInstance().getOngRemanantBackupDir());
        return true;
    }

    /**
     * Do invoke remote command to flash the dongle on the target.
     * 
     * @return true if dongle was flashed successfully, false otherwise.
     */
    private boolean flashCC2531Dongle() throws JSchException, IOException {
        myProgressMonitor.setIndeterminate(true);
        // // WARNING: to be able to run over ssh with busybox, command length MUST be less than 700 characters
        // String cmdOutput = "Install failed";
        // String targetDir = rootActDir + "/firmware/";
        //
        // log.println("Beginning remote install of CC2531 flashprgm utilities at "
        // + InstallerUtil.getInstance().getDateFormatForLogs());
        //
        // log.println("Creating directories " + targetDir);
        // sshHandler.runRemoteCommand("mkdir -p " + targetDir);
        // if (InstallerUtil.getInstance().getCmdResult(sshHandler.runRemoteCommand("mkdir -p " + targetDir))
        // && sshHandler.copyFileFromJarFromJar("common/data/firmware/flashprgm.cc2531-.*.jar", "exe/"
        // + InstallerUtil.getInstance().getArchFromFlavor(flavor) + "/cc2531-flash-prgm.x", targetDir
        // + "cc2531-flash-prgm.x")
        // && sshHandler.copyFileFromJarFromJar("install/ong/.*/install/factory/actility/flashprgm.cc2531-.*.jar", "lib/"
        // + InstallerUtil.getInstance().getArchFromFlavor(flavor) + "/libcc2531-flash-prgm.so", targetDir
        // + "libcc2531-flash-prgm.so")) {
        // cmdOutput = sshHandler.runRemoteCommand("export NEW_ONG_HOME=" + rootActDir + "; ",
        // myJarFile.getFile("flashdongle/" + flavor + "/flash_cc2531"));
        // log.println("*** result for flash_cc2531: " + cmdOutput);
        // log.println("*** remove flash-util files: " + sshHandler.runRemoteCommand("rm -rf " + targetDir));
        // }

        myProgressMonitor.setIndeterminate(false);
        myProgressMonitor.setCurrentProgress(100);
        // if (InstallerUtil.getInstance().getCmdResult(cmdOutput)) {
        // return true;
        // }
        // return false;
        return true;
    }
}
