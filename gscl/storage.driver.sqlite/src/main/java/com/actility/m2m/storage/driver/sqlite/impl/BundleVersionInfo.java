package com.actility.m2m.storage.driver.sqlite.impl;

import java.text.DecimalFormat;
import java.text.NumberFormat;
import java.util.Arrays;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

import com.actility.m2m.storage.StorageException;
import com.actility.m2m.storage.driver.Transaction;
import com.actility.m2m.storage.driver.sqlite.impl.SqliteDB;
import com.actility.m2m.storage.driver.sqlite.impl.sqlitedriver;

public final class BundleVersionInfo {

    private final static VersionItem[] VERSIONS = new VersionItem[] {
            new VersionItem(2, 1, new String[] { "SELECT * FROM DOCUMENT;", "select * from ATTRIBUTE;" }),
            new VersionItem(1, 1, new String[] { "SELECT * FROM DOCUMENT;", "select * from ATTRIBUTE;" }),
            new VersionItem(1, 2, new String[] { "SELECT * FROM DOCUMENT;", "select * from ATTRIBUTE;" }),
            new VersionItem(1, 6, new String[] { "SELECT * FROM DOCUMENT;", "select * from ATTRIBUTE;" }),
            new VersionItem(2, 0, new String[] { "SELECT * FROM DOCUMENT;", "select * from ATTRIBUTE;" }) };

    /**
     * For upgrade the database when the database version change
     *
     * @param openedDB the sqliteDatabase
     * @param majorDB the major version number
     * @param minorDB the minor version number
     * @throws StorageException if any problem occurs while upgrading the database
     */
    public static void upgradeTo(SqliteDB openedDB, int majorDB, int minorDB) throws StorageException {
        List vers = Arrays.asList(VERSIONS);
        Collections.sort(vers);
        Iterator it = vers.iterator();

        while (it.hasNext()) {
            VersionItem item = (VersionItem) it.next();
            if ((majorDB == item.getMajor() && minorDB < item.getMinor()) || majorDB < item.getMajor()) {
                String[] sqlStatements = item.getSql();
                Transaction transaction = new TransactionImpl(openedDB);
                try {
                    for (int i = 0; i < sqlStatements.length; i++) {

                        int code = sqlitedriver.executeSqlStatementWithOutParameters(openedDB, sqlStatements[i]);

                        if (code != 0 && code != 101) {
                            throw new StorageException("Error during execute the sqlite statement, error code: " + code);
                        }
                        System.out.println(new StringBuffer().append(item.getMajor()).append(".").append(item.getMinor())
                                .append(" ").append(item.getSql()[i]).append(" ").append(code));

                    }
                    String updateReleaseDB = new StringBuffer().append("UPDATE VERSION set MAJOR = '")
                            .append(intToString(item.getMajor())).append("', MINOR = '").append(intToString(item.getMinor()))
                            .append("' where MAJOR = '").append(intToString(majorDB)).append("' and MINOR = '")
                            .append(intToString(minorDB)).append("'").toString();

                    int code = sqlitedriver.executeSqlStatementWithOutParameters(openedDB, updateReleaseDB);

                    if (code != 0 && code != 101) {
                        throw new StorageException("Error during execute the sqlite statement, error code: " + code);
                    }

                    majorDB = item.getMajor();
                    minorDB = item.getMinor();

                    transaction.commitTransaction();
                } catch (StorageException e) {
                    transaction.rollbackTransaction();

                    throw (new StorageException("Error occured during update database", e));
                }
            }

        }

    }

    /**
     * For convert an integer to a string
     *
     * @param number to convert
     * @return the number in a string
     */
    private static String intToString(int number) {
        NumberFormat formatter = new DecimalFormat("00");

        return formatter.format(number);
    }

    private static class VersionItem implements Comparable {

        private int major;
        private int minor;
        private String[] sql;

        VersionItem(int major, int minor, String[] sql) {
            this.major = major;
            this.minor = minor;
            this.sql = sql;

        }

        public int compareTo(Object o) {

            if (o == null) {
                throw (new NullPointerException());
            }
            if (o instanceof VersionItem) {
                VersionItem cmp = (VersionItem) o;
                if (cmp.major == this.major) {
                    return this.minor - cmp.minor;
                } else {
                    return this.major - cmp.major;
                }
            } else {
                return -1;
            }

        }

        public int getMajor() {
            return major;
        }

        public int getMinor() {
            return minor;
        }

        public String[] getSql() {
            return sql;
        }
    }

}
