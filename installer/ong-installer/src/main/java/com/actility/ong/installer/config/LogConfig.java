package com.actility.ong.installer.config;

import java.util.Properties;

public final class LogConfig {
    private final String name;
    private final boolean fixed;
    private final long fixedGeneration;
    private final boolean appendOnRestart;
    private final double weight;

    public LogConfig(String name, Properties properties) {
        this.name = name;
        this.fixed = ((String) properties.get("log." + name + ".mode")).equals("fixed-generation");
        long fixedGeneration = -1L;
        if (fixed) {
            fixedGeneration = Long.parseLong(((String) properties.get("log." + name + ".fixedGeneration.size")));
        }
        this.fixedGeneration = fixedGeneration;
        this.appendOnRestart = ((String) properties.get("log." + name + ".restart")).equals("append");
        this.weight = Double.parseDouble(((String) properties.get("log." + name + ".weight")));
    }

    public String getName() {
        return name;
    }

    public boolean isFixed() {
        return fixed;
    }

    public long getFixedGeneration() {
        return fixedGeneration;
    }

    public boolean isAppendOnRestart() {
        return appendOnRestart;
    }

    public double getWeight() {
        return weight;
    }

    @Override
    public String toString() {
        return name;
    }
}
