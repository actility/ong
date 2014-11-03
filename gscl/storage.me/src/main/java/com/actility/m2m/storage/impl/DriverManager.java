package com.actility.m2m.storage.impl;

import java.util.HashMap;
import java.util.Map;

import com.actility.m2m.storage.StorageRequestExecutor;
import com.actility.m2m.storage.driver.StorageRequestDriverExecutor;

public final class DriverManager {
    private StorageRequestDriverExecutor defaultDriver;
    private final String defaultDriverName;
    private Map/* <String, StorageRequestDriverExecutor> */ drivers;

    public DriverManager(String defaultDriverName, Map/* <String, StorageRequestDriverExecutor> */ drivers) {
        this.defaultDriverName = defaultDriverName;
        this.defaultDriver = (StorageRequestDriverExecutor) drivers.get(defaultDriverName);
        this.drivers = new HashMap/* <String, StorageRequestDriverExecutor> */(drivers);
    }

    public void clear() {
        this.defaultDriver = null;
    }

    public String getDefaultDriverName() {
        return defaultDriverName;
    }

    public void addDriver(String name, StorageRequestDriverExecutor driver) {
        if (defaultDriverName.equals(name)) {
            defaultDriver = driver;
        }
        Map newDrivers = new HashMap(drivers);
        newDrivers.put(name, driver);
        this.drivers = newDrivers;
    }

    public void removeDriver(String name) {
        if (defaultDriverName.equals(name)) {
            defaultDriver = null;
        }
        Map newDrivers = new HashMap(drivers);
        newDrivers.remove(name);
        this.drivers = newDrivers;
    }

    public String getDriverName(Map config) {
        String name = (config != null) ? (String) config.get(StorageRequestExecutor.CONFIG_DRIVER_NAME) : null;
        if (name == null) {
            return defaultDriverName;
        }
        return name;
    }

    public StorageRequestDriverExecutor resolveDriver(Map config) {
        String name = (config != null) ? (String) config.get(StorageRequestExecutor.CONFIG_DRIVER_NAME) : null;
        if (name == null) {
            return defaultDriver;
        }
        return (StorageRequestDriverExecutor) drivers.get(name);
    }

    public StorageRequestDriverExecutor resolveDriver(String name) {
        return (StorageRequestDriverExecutor) drivers.get(name);
    }
}
