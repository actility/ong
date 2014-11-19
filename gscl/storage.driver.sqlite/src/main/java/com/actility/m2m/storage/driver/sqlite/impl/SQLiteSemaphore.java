package com.actility.m2m.storage.driver.sqlite.impl;

public final class SQLiteSemaphore {
    private static SQLiteSemaphore INSTANCE = null;
    private boolean sempahore = true;

    /**
     * Class constructor.
     */
    protected SQLiteSemaphore() {
    }

    public static SQLiteSemaphore getInstance() {
        if (INSTANCE == null) {
            INSTANCE = new SQLiteSemaphore();
        }
        return INSTANCE;
    }

    /**
     * Get the semaphore, and block the others threads if they try to get the semaphore too.
     */
    public synchronized void getSemaphore() {
        while (sempahore == false) {
            try {
                this.wait(/* timeout */);
            } catch (InterruptedException e) {

            }
        }
        sempahore = false;
    }

    /**
     * Release the semaphore, the others thread can now get the semaphore
     */
    public synchronized void releaseSemaphore() {
        sempahore = true;
        this.notify();
    }
}
