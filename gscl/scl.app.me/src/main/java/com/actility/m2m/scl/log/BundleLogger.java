package com.actility.m2m.scl.log;

import com.actility.m2m.util.log.BundleStaticLogger;

public final class BundleLogger {
    // TODO remove this patch. Start patch
    private static BundleStaticLogger LOG;

    public synchronized static BundleStaticLogger getStaticLogger() {
        if (LOG == null) {
            LOG = new BundleStaticLogger();
        }
        return LOG;
    }
    // End patch
}
