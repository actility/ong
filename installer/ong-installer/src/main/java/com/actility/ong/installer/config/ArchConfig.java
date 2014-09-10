package com.actility.ong.installer.config;

import java.util.Properties;

public final class ArchConfig {

    private final String name;
    private final int index;
    private final String label;
    private final String retrieveEnvCmd;
    private final String profileFile;
    private final String initSystem;
    private final String shell;
    private final String sshOptions;
    private final String[] extraModules;
    private final String[] excludedModules;
    private final String maxLogSize;

    public ArchConfig(String name, Properties properties) {
        this.name = name;
        this.index = Integer.parseInt((String) properties.get("index"));
        this.label = (String) properties.get("label");
        this.retrieveEnvCmd = (String) properties.get("retrieveEnvCmd");
        this.profileFile = (String) properties.get("profileFile");
        this.initSystem = (String) properties.get("initSystem");
        this.shell = (String) properties.get("shell");
        this.sshOptions = (String) properties.get("sshOptions");
        this.maxLogSize = (String) properties.get("maxLogSize");

        String extraModulesStr = properties.getProperty("extraModules");
        String[] extraModules = null;
        if (extraModulesStr != null) {
            extraModules = extraModulesStr.split(",");
            for (int i = 0; i < extraModules.length; ++i) {
                extraModules[i] = extraModules[i].trim();
            }
        }
        this.extraModules = extraModules;

        String excludedModulesStr = properties.getProperty("excludedModules");
        String[] excludedModules = null;
        if (excludedModulesStr != null) {
            excludedModules = excludedModulesStr.split(",");
            for (int i = 0; i < excludedModules.length; ++i) {
                excludedModules[i] = excludedModules[i].trim();
            }
        }
        this.excludedModules = excludedModules;
    }

    public String getName() {
        return name;
    }

    public int getIndex() {
        return index;
    }

    public String getLabel() {
        return label;
    }

    public String getRetrieveEnvCmd() {
        return retrieveEnvCmd;
    }

    public String getProfileFile() {
        return profileFile;
    }

    public String getInitSystem() {
        return initSystem;
    }

    public String getShell() {
        return shell;
    }

    public String getSshOptions() {
        return sshOptions;
    }

    public String[] getExtraModules() {
        return extraModules;
    }

    public String[] getExcludedModules() {
        return excludedModules;
    }

    public String getMaxLogSize() {
        return maxLogSize;
    }

    @Override
    public String toString() {
        return name;
    }
}
