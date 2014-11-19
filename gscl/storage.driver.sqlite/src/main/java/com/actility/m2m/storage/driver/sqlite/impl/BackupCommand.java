package com.actility.m2m.storage.driver.sqlite.impl;

import java.io.PrintStream;
import java.util.StringTokenizer;

import org.apache.felix.shell.Command;
import org.apache.log4j.Logger;

import com.actility.m2m.storage.driver.sqlite.impl.SqliteDB;

public final class BackupCommand implements Command {
    private final static Logger LOG = Logger.getLogger(BackupCommand.class);

    private SQLiteRequestExecutor driver = null;
    private PrintStream out = null;

    private final native void backupDb(long jarg1, SqliteDB obj, String zFilename, int nbPagePerStep, int sleepDelayBetweenStep);

    /**
     * Class constructor
     *
     * @param context
     * @param driver the sqlite manager
     */
    public BackupCommand(SQLiteRequestExecutor driver) {
        this.driver = driver;
    }

    public String getName() {
        return "backup";
    }

    public String getUsage() {
        return "backup <path_filename_to_backup> [<pages per step (default 5)> <sleep interval between step (default 250 ms)>";
    }

    public String getShortDescription() {
        return "backup sqlite database.";
    }

    public void execute(String s, PrintStream out, PrintStream err) {
        StringTokenizer st = new StringTokenizer(s, " ");
        // Ignore the command name.
        st.nextToken();
        this.out = out;
        // There should be at least file name .
        if (st.countTokens() >= 1) {
            if (st.hasMoreTokens()) {
                String filePath = st.nextToken().trim();
                int pages = 0;
                int interval = 0;
                if (st.hasMoreTokens()) {
                    try {
                        int p = Integer.valueOf(st.nextToken().trim()).intValue();
                        if (p > 0) {
                            pages = p;
                        }
                    } catch (NumberFormatException e) {
                        LOG.error("Incorrect pages per step" + e.getMessage());
                        err.println("Incorrect parameter 'pages per step'");
                    }
                }
                if (st.hasMoreTokens()) {
                    try {
                        int p = Integer.valueOf(st.nextToken().trim()).intValue();
                        if (p > 0) {
                            interval = p;
                        }
                    } catch (NumberFormatException e) {
                        LOG.error("Incorrect sleep interval between step" + e.getMessage());
                        err.println("Incorrect parameter 'sleep interval'");
                    }
                }

                // try {
                if (LOG.isInfoEnabled()) {
                    LOG.info("Start backup...please wait");
                }
                out.println("Backup started..");
                if (pages == 0 || interval == 0) {
                    backupDb(SqliteDB.getCPtr(driver.openedDB), driver.openedDB, filePath, 5, 250);
                } else {
                    backupDb(SqliteDB.getCPtr(driver.openedDB), driver.openedDB, filePath, pages, interval);
                }

                if (LOG.isInfoEnabled()) {
                    LOG.info("Backup is completed");
                }
                out.println("Backup is completed.");

            }
        } else {
            err.println("Incorrect number of arguments");
        }
        this.out = null;
    }

    public void callback(String log) {

        if (this.out != null) {
            this.out.print(log);

        }
    }
}
