package com.actility.m2m.framework.resources.osgi;

import com.actility.m2m.framework.resources.BackupClassLoader;

public final class BackupClassLoaderImpl implements BackupClassLoader {

    private final ClassLoader classLoader;

    public BackupClassLoaderImpl(ClassLoader classLoader) {
        this.classLoader = classLoader;
    }

    public void restoreThreadClassLoader() {
        Thread.currentThread().setContextClassLoader(classLoader);
    }

}
