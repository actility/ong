package com.actility.ong.installer;

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;

import com.actility.ong.installer.config.ArchConfig;
import com.jcraft.jsch.JSchException;

public class OngTr069Manager {
    private String acsCredentialsBackupFileName;
    private String tr069DataBackupFileName;
    private SshSessionHandler sshHandler;
    private PrintStream log;
    private ArchConfig arch;
    private String ongInstallDir;
    private String backupDir;

    /**
     * Constructor
     *
     * @param log
     * @param sshHandler
     * @param flavor
     * @param ongInstallDir
     */
    public OngTr069Manager(SshSessionHandler sshHandler, PrintStream log, ArchConfig arch, String ongInstallDir,
            String backupDir) {
        this.sshHandler = sshHandler;
        this.log = log;
        this.arch = arch;
        this.ongInstallDir = ongInstallDir;
        this.backupDir = backupDir;
    }

    /**
     * Backup file located by a props.xargs entry.
     *
     * @param propsName the name of the property to look for to retrieve the location of the file to back up
     * @return the absolute file path name if back up succeeds, null otherwise.
     * @throws JSchException
     * @throws IOException
     */
    private String backupFileFromPropsXargs(String propsName) throws JSchException, IOException {
        String res = null;
        String[] filesRes = sshHandler.backupFiles(arch.getRetrieveEnvCmd() + " && RELATIVE_PATH_NAME=$(grep '" + propsName
                + "'" + " $ONG_HOME/knopflerfish/osgi/props.xargs | cut -d= -f2)"
                + " && cd $ONG_HOME/knopflerfish/osgi/$(dirname $RELATIVE_PATH_NAME)"
                + " && find $(pwd) -name $(basename $RELATIVE_PATH_NAME) | tr -s '[:space:]' ':'", backupDir, null);
        if (filesRes != null) {
            res = filesRes[0].replace('/', File.separatorChar);
        }

        return res;
    }

    /**
     * Back up credentials for ACS connection and TR-069 data, ready to be restored at end of installation.
     */
    public void backup() throws JSchException, IOException {
        log.println("Beginning remote TR-069 parameters back up procedure at "
                + InstallerUtil.getInstance().getDateFormatForLogs());

        acsCredentialsBackupFileName = backupFileFromPropsXargs("com.francetelecom.acse.modus.config.credentialsPath");
        tr069DataBackupFileName = backupFileFromPropsXargs("com.francetelecom.acse.modus.config.savePath");

        log.println("Ending ACS back up procedure at " + InstallerUtil.getInstance().getDateFormatForLogs());
    }

    /**
     * Restore file located by a props.xargs entry.
     *
     * @param sshHandler the SSH session handler for the active connection.
     * @return the absolute file path name if back up succeeds, null otherwise.
     * @throws JSchException
     * @throws IOException
     */
    private boolean restoreFileFromPropsXargs(String propsName, String restoreAbsPath) throws JSchException, IOException {
        boolean res = true;
        String relativePath = null;
        if (null != restoreAbsPath) {
            String cmdOutput = sshHandler.runRemoteCommand("grep '" + propsName + "' " + ongInstallDir
                    + "/knopflerfish/osgi/props.xargs | cut -d= -f2 | tr -s '[:space:]' ':'");
            log.println("Restoring TR-069 files: retrieving props " + propsName + "=" + cmdOutput);
            relativePath = sshHandler.getCmdOutput(cmdOutput);

            if (null != relativePath) {
                if (relativePath.endsWith(":")) {
                    relativePath = relativePath.substring(0, relativePath.length() - 1);
                }
                if (!"".equals(relativePath)) {
                    String target = ongInstallDir + "/knopflerfish/osgi/" + relativePath;
                    log.println("Restoring TR-069 files: trying to restore " + restoreAbsPath + " in " + target);
                    res = sshHandler.copyFileTo(restoreAbsPath, target);
                }
            }
        }
        return res;
    }

    /**
     * Restore storage files
     *
     * @return true if files restoration succeeds, false otherwise.
     * @throws JSchException
     * @throws IOException
     */
    public boolean restore() throws JSchException, IOException {
        return restoreFileFromPropsXargs("com.francetelecom.acse.modus.config.credentialsPath", acsCredentialsBackupFileName)
                && restoreFileFromPropsXargs("com.francetelecom.acse.modus.config.savePath", tr069DataBackupFileName);
    }
}
