package org.apache.log4j;

public class LogManager {
    public static Logger getRootLogger() {
        return Logger.getRootLogger();
    }

    public static Logger getLogger(final String name) {
        return Logger.getLogger(name);
    }

    public static Logger getLogger(final Class clazz) {
        return Logger.getLogger(clazz);
    }
}
