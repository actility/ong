package com.actility.m2m.storage;

/**
 * @author ventrill
 *
 */
public final class StorageException extends Exception {

    /**
     *
     */
    private static final long serialVersionUID = 2208151934914305109L;

    /**
     *
     */
    public StorageException() {
        super();
    }

    /**
     * @param message
     */
    public StorageException(String message) {
        super(message);
    }

    /**
     * @param cause
     */
    public StorageException(Throwable cause) {
        super(cause);
    }

    /**
     * @param message
     * @param cause
     */
    public StorageException(String message, Throwable cause) {
        super(message, cause);
    }

}
