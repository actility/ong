package com.actility.ong.installer;

public class InstallerException extends Exception {
    /**
     *
     */
    private static final long serialVersionUID = -1307695973094068418L;

    /**
    *
    */
    public InstallerException() {
        super();
    }

    /**
     * @param message
     */
    public InstallerException(String message) {
        super(message);
    }

    /**
     * @param cause
     */
    public InstallerException(Throwable cause) {
        super(cause);
    }

    /**
     * @param message
     * @param cause
     */
    public InstallerException(String message, Throwable cause) {
        super(message, cause);
    }

}
