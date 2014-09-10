package com.actility.ong.installer.config;

import java.util.HashMap;
import java.util.Map;
import java.util.Properties;

public final class ModuleConfig {

    private final String name;
    private final String[] configureVariables;
    private final LogConfig[] logConfigs;

    public ModuleConfig(String name, Properties properties) {
        this.name = name;

        String configureVariablesStr = properties.getProperty("configureVariables");
        String[] configureVariables = null;
        if (configureVariablesStr != null) {
            configureVariables = configureVariablesStr.split(",");
            for (int i = 0; i < configureVariables.length; ++i) {
                configureVariables[i] = configureVariables[i].trim();
            }
        }
        this.configureVariables = configureVariables;

        Map<String, LogConfig> logConfigs = new HashMap<String, LogConfig>();
        for (Object keyObj : properties.keySet()) {
            String key = (String) keyObj;
            if (key.startsWith("log.")) {
                int dotIndex = key.indexOf('.', 4);
                if (dotIndex != -1) {
                    String logName = key.substring(4, dotIndex);
                    if (!logConfigs.containsKey(logName)) {
                        logConfigs.put(logName, new LogConfig(logName, properties));
                    }
                }
            }
        }
        this.logConfigs = new LogConfig[logConfigs.size()];
        int index = 0;
        for (LogConfig logConfig : logConfigs.values()) {
            this.logConfigs[index] = logConfig;
            ++index;
        }
    }

    public String getName() {
        return name;
    }

    public String[] getConfigureVariables() {
        return configureVariables;
    }

    public LogConfig[] getLogConfigs() {
        return logConfigs;
    }

    @Override
    public String toString() {
        return name;
    }
}
