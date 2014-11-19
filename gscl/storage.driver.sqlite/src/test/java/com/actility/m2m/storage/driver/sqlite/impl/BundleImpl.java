package com.actility.m2m.storage.driver.sqlite.impl;

import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.Dictionary;
import java.util.Enumeration;
import java.util.Map;

import org.osgi.framework.Bundle;
import org.osgi.framework.BundleContext;
import org.osgi.framework.BundleException;
import org.osgi.framework.ServiceReference;
import org.osgi.framework.Version;

public final class BundleImpl implements Bundle {

    public int getState() {
        // TODO Auto-generated method stub
        return 0;
    }

    public void start(int options) throws BundleException {
        // TODO Auto-generated method stub

    }

    public void start() throws BundleException {
        // TODO Auto-generated method stub

    }

    public void stop(int options) throws BundleException {
        // TODO Auto-generated method stub

    }

    public void stop() throws BundleException {
        // TODO Auto-generated method stub

    }

    public void update(InputStream input) throws BundleException {
        // TODO Auto-generated method stub

    }

    public void update() throws BundleException {
        // TODO Auto-generated method stub

    }

    public void uninstall() throws BundleException {
        // TODO Auto-generated method stub

    }

    public Dictionary getHeaders() {
        // TODO Auto-generated method stub
        return null;
    }

    public long getBundleId() {
        // TODO Auto-generated method stub
        return 0;
    }

    public String getLocation() {
        // TODO Auto-generated method stub
        return null;
    }

    public ServiceReference[] getRegisteredServices() {
        // TODO Auto-generated method stub
        return null;
    }

    public ServiceReference[] getServicesInUse() {
        // TODO Auto-generated method stub
        return null;
    }

    public boolean hasPermission(Object permission) {
        // TODO Auto-generated method stub
        return false;
    }

    public URL getResource(String name) {
        // TODO Auto-generated method stub
        return null;
    }

    public Dictionary getHeaders(String locale) {
        // TODO Auto-generated method stub
        return null;
    }

    public String getSymbolicName() {
        // TODO Auto-generated method stub
        return null;
    }

    public Class loadClass(String name) throws ClassNotFoundException {
        // TODO Auto-generated method stub
        return null;
    }

    public Enumeration getResources(String name) throws IOException {
        // TODO Auto-generated method stub
        return null;
    }

    public Enumeration getEntryPaths(String path) {
        // TODO Auto-generated method stub
        return null;
    }

    public URL getEntry(String path) {
        // TODO Auto-generated method stub
        return null;
    }

    public long getLastModified() {
        // TODO Auto-generated method stub
        return 0;
    }

    public Enumeration findEntries(String path, String filePattern, boolean recurse) {
        // TODO Auto-generated method stub
        return null;
    }

    public BundleContext getBundleContext() {
        // TODO Auto-generated method stub
        return null;
    }

    public Map getSignerCertificates(int signersType) {
        // TODO Auto-generated method stub
        return null;
    }

    public Version getVersion() {
        // TODO Auto-generated method stub
        return null;
    }

}
