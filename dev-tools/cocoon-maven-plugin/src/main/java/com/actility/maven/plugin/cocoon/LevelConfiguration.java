package com.actility.maven.plugin.cocoon;

import java.util.ArrayList;
import java.util.List;

import org.apache.maven.plugin.MojoExecutionException;
import org.codehaus.plexus.util.StringUtils;

public class LevelConfiguration {
    private int level;
    private String description;
    private List<BundleConfiguration> bundlesConfiguration;

    public LevelConfiguration(String[] args) throws MojoExecutionException {
        if (args.length != 2) {
            throw new MojoExecutionException("Level configuration has a wrong number of arguments: "
                    + StringUtils.join(args, ":"));
        }
        level = Integer.parseInt(args[0]);
        if (level < 1) {
            throw new MojoExecutionException("Level configuration value is stricly lower than 1: "
                    + StringUtils.join(args, ":"));
        }
        description = args[1];
        bundlesConfiguration = new ArrayList<BundleConfiguration>();
    }

    public LevelConfiguration(int level) {
        this.level = level;
        bundlesConfiguration = new ArrayList<BundleConfiguration>();
    }

    public int getLevel() {
        return level;
    }

    public String getDescription() {
        return description;
    }

    public List<BundleConfiguration> getBundlesConfiguration() {
        return bundlesConfiguration;
    }

    public void addBundleConfiguration(BundleConfiguration bundleConfig) {
        bundlesConfiguration.add(bundleConfig);
    }
}
