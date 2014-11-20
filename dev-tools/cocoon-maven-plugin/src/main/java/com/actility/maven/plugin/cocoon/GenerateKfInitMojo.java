package com.actility.maven.plugin.cocoon;

/*
 * Copyright 2001-2005 The Apache Software Foundation.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

import org.apache.maven.artifact.Artifact;
import org.apache.maven.plugin.AbstractMojo;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.project.MavenProject;
import org.codehaus.plexus.archiver.manager.ArchiverManager;
import org.codehaus.plexus.util.StringUtils;

/**
 * Goal which generate the init.xargs Knopflerfish init script.
 *
 * @goal generate-kf-init
 *
 * @phase prepare-package
 */
public class GenerateKfInitMojo extends AbstractMojo {
    /**
     * POM
     *
     * @parameter expression="${project}"
     * @readonly
     * @required
     */
    private MavenProject project;

    /**
     * Bundles configuration.
     *
     * @parameter alias="bundlesConfiguration"
     * @required
     */
    private String bundlesConfiguration;

    /**
     * Levels configuration.
     *
     * @parameter alias="levelsConfiguration"
     */
    private String levelsConfiguration;

    /**
     * The destination file for the generated init script
     *
     * @parameter
     * @required
     */
    private File kfConfigDirectory;

    /**
     * Initial start level configuration.
     *
     * @parameter alias="initialStartLevel" default-value="7"
     */
    private int initialStartLevel;

    /**
     * To look up Archiver/UnArchiver implementations
     *
     * @component
     */
    private ArchiverManager archiverManager;

    /**
     *
     * @parameter
     */
    private String classifier;

    private Map<String, Artifact> nameToDependency;

    @Override
    public void execute() throws MojoExecutionException {
        try {
            getLog().debug("Read levelsConfiguration: " + levelsConfiguration);
            getLog().debug("Read bundlesConfiguration: " + bundlesConfiguration);
            getLog().debug("Read kfConfigDirectory: " + kfConfigDirectory);
            Set<Artifact> dependencies = project.getDependencyArtifacts();
            nameToDependency = new HashMap<String, Artifact>();
            for (Artifact artifact : dependencies) {
                if (artifact.getClassifier() != null) {
                    getLog().info(
                            "Put artifact with classifier: " + artifact.getGroupId() + ":" + artifact.getArtifactId() + ":"
                                    + artifact.getClassifier());
                    nameToDependency.put(
                            artifact.getGroupId() + ":" + artifact.getArtifactId() + ":" + artifact.getClassifier(), artifact);
                } else {
                    getLog().info("Put artifact: " + artifact.getGroupId() + ":" + artifact.getArtifactId());
                    nameToDependency.put(artifact.getGroupId() + ":" + artifact.getArtifactId(), artifact);
                }
                getLog().info("" + artifact.getFile());
            }

            // Build levels configuration
            Map<Integer, LevelConfiguration> levelsConfigurations = buildLevelsConfiguration();
            // Build bundles configuration
            buildBundlesConfiguration(levelsConfigurations);
            // Build init.xargs
            buildInitXargs(levelsConfigurations);
        } catch (RuntimeException e) {
            e.printStackTrace();
            throw e;
        }
    }

    private Map<Integer, LevelConfiguration> buildLevelsConfiguration() throws MojoExecutionException {
        Map<Integer, LevelConfiguration> levelsConfiguration = new TreeMap<Integer, LevelConfiguration>();
        String[] configs = StringUtils.split(this.levelsConfiguration, ",");
        for (String config : configs) {
            String realConfig = config.trim();
            LevelConfiguration levelConfig = new LevelConfiguration(StringUtils.split(realConfig, ":"));
            levelsConfiguration.put(levelConfig.getLevel(), levelConfig);
            getLog().debug("Read level configuration OK: " + realConfig);
        }

        return levelsConfiguration;
    }

    private void buildBundlesConfiguration(Map<Integer, LevelConfiguration> levelsConfiguration) throws MojoExecutionException {
        String[] configs = StringUtils.split(this.bundlesConfiguration, ",");
        for (String config : configs) {
            String realConfig = config.trim();
            try {
                BundleConfiguration bundleConfig = new BundleConfiguration(getLog(), nameToDependency, classifier,
                        StringUtils.split(realConfig, ":"));
                getLog().debug("Read bundle configuration OK: " + realConfig);
                if (bundleConfig.getArtifact() != null) {
                    LevelConfiguration levelConfig = levelsConfiguration.get(bundleConfig.getStartLevel());
                    if (levelConfig == null) {
                        levelConfig = new LevelConfiguration(bundleConfig.getStartLevel());
                        levelsConfiguration.put(bundleConfig.getStartLevel(), levelConfig);
                    }
                    levelConfig.addBundleConfiguration(bundleConfig);
                } else {
                    getLog().info("Do not add bundle as it is optional and dependency is not found: " + realConfig);
                }
            } catch (RuntimeException e) {
                getLog().info("Exception on line: " + realConfig);
                throw e;
            }
        }
    }

    private void buildInitXargs(Map<Integer, LevelConfiguration> levelsConfiguration) throws MojoExecutionException {
        File initXargsFile = new File(kfConfigDirectory, "init.xargs");
        BufferedWriter out = null;
        try {
            out = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(initXargsFile), "ASCII"));

            // Headers
            out.write("#\n");
            out.write("# Generated cocoon init file\n");
            out.write("#\n");
            out.write("\n");
            out.write("# load common properties\n");
            out.write("-xargs props.xargs\n");
            out.write("\n");
            out.write("# Semicolon seprated list of base URLs for searching (completing)\n");
            out.write("# bundle URLs in \"-install URL\" command line options and in the console.\n");
            out.write("-Dorg.knopflerfish.gosg.jars=file:../../../lib/osgi/\n");
            out.write("\n");
            out.write("-init\n");
            out.write("\n");

            // Body
            for (LevelConfiguration levelConfig : levelsConfiguration.values()) {
                if (levelConfig.getDescription() != null) {
                    String[] descriptions = levelConfig.getDescription().split("\n");
                    for (String description : descriptions) {
                        out.write("# ");
                        out.write(description);
                        out.write("\n");
                    }
                }
                out.write("-initlevel ");
                out.write(String.valueOf(levelConfig.getLevel()));
                out.write("\n");

                for (BundleConfiguration bundleConfig : levelConfig.getBundlesConfiguration()) {
                    if (bundleConfig.isComment()) {
                        out.write("#");
                    }
                    out.write("-install ");
                    out.write(bundleConfig.getDestFile());
                    out.write("\n");
                }
                out.write("\n");
            }
            out.write("\n");

            out.write("-startlevel ");
            out.write(String.valueOf(initialStartLevel));
            out.write("\n");
            out.write("\n");
            out.write("\n");

            out.write("# Start of these bundles are delayed since this makes start\n");
            out.write("# order dependencies much easier\n");
            out.write("\n");

            for (LevelConfiguration levelConfig : levelsConfiguration.values()) {
                for (BundleConfiguration bundleConfig : levelConfig.getBundlesConfiguration()) {
                    if (bundleConfig.toStart()) {
                        if (bundleConfig.isComment()) {
                            out.write("#");
                        }
                        out.write("-start ");
                        out.write(bundleConfig.getDestFile());
                        out.write("\n");
                    }
                }
                out.write("\n");
            }
        } catch (IOException e) {
            throw new MojoExecutionException("IO problem while generating init.xargs file", e);
        } finally {
            try {
                if (out != null) {
                    out.close();
                }
            } catch (IOException e) {
                throw new MojoExecutionException("IO problem while closing init.xargs file", e);
            }
        }
    }
}
