package com.actility.ong.installer;

import java.io.File;
import java.io.IOException;
import java.io.PrintStream;

import com.actility.ong.installer.config.ArchConfig;
import com.jcraft.jsch.JSchException;

public class OngStorageManager {

    private String[] storageBackupFileName;
    private SshSessionHandler sshHandler;
    private PrintStream log;
    private ArchConfig arch;
    private String ongInstallDir;
    private String backupDir;

    /**
     * Constructor
     *
     * @param log
     */
    public OngStorageManager(SshSessionHandler sshHandler, PrintStream log, ArchConfig arch, String ongInstallDir,
            String backupDir) {
        this.sshHandler = sshHandler;
        this.log = log;
        this.arch = arch;
        this.ongInstallDir = ongInstallDir;
        this.backupDir = backupDir;
    }

    /**
     * Back up storage, ready to be restored at end of installation.
     */
    public void backup() throws JSchException, IOException {
        log.println("Beginning remote storage back up procedure at " + InstallerUtil.getInstance().getDateFormatForLogs());
        storageBackupFileName = sshHandler.backupFiles(arch.getRetrieveEnvCmd()
                + " && ls -1 $ONG_HOME/knopflerfish/osgi/resources/storage.db* | tr -s '[:space:]' ':'", backupDir, null);

        log.println("Ending remote storage back up procedure at " + InstallerUtil.getInstance().getDateFormatForLogs());
    }

    /**
     * Restore storage files
     *
     * @return true if files restoration succeeds, false otherwise.
     * @throws JSchException
     * @throws IOException
     */
    public boolean restore() throws JSchException, IOException {
        boolean res = true;
        if (null != storageBackupFileName) {
            for (int i = 0; (i < storageBackupFileName.length) && res; i++) {

                String targetFileName = storageBackupFileName[i].substring(storageBackupFileName[i]
                        .lastIndexOf(File.separatorChar) + 1);
                String target = ongInstallDir + "/knopflerfish/osgi/resources/" + targetFileName;

                log.println("Restoring " + storageBackupFileName[i] + " in " + target + "(target file name:" + targetFileName
                        + ")");
                res = sshHandler.copyFileTo(storageBackupFileName[i], target);
            }
        }
        return res;
    }
}
