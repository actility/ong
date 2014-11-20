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
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.TreeMap;

import org.apache.maven.artifact.Artifact;
import org.apache.maven.artifact.versioning.OverConstrainedVersionException;
import org.apache.maven.plugin.AbstractMojo;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.project.MavenProject;
import org.codehaus.plexus.archiver.Archiver;
import org.codehaus.plexus.archiver.ArchiverException;
import org.codehaus.plexus.archiver.manager.ArchiverManager;
import org.codehaus.plexus.archiver.manager.NoSuchArchiverException;
import org.codehaus.plexus.archiver.util.DefaultFileSet;
import org.codehaus.plexus.util.FileUtils;
import org.codehaus.plexus.util.StringUtils;

/**
 * Goal which generate the init.xargs Knopflerfish init script.
 *
 * @goal generate-kf-cocoon
 *
 * @phase prepare-package
 */
public class GenerateKfCocoonMojo extends AbstractMojo {
    /**
     * POM
     *
     * @parameter expression="${project}"
     * @readonly
     * @required
     */
    private MavenProject project;

    /**
     * Firmware qualifier.
     *
     * @parameter alias="firmwareQualifier"
     */
    private String firmwareQualifier;

    /**
     * Firmware entries configuration.
     *
     * @parameter alias="firmwareEntriesConfiguration"
     */
    private String firmwareEntriesConfiguration;

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
     * Default location used for mojo unless overridden in ArtifactItem
     *
     * @parameter expression="${project.build.directory}"
     * @required
     */
    protected File buildDirectory;

    /**
     * Location of base KF directory.
     *
     * @parameter alias="kfBaseDirectory"
     * @required
     */
    private File kfBaseDirectory;

    /**
     * Location of Factory directory.
     *
     * @parameter alias="factoryDirectory"
     * @required
     */
    private File factoryDirectory;

    /**
     * Initial start level configuration.
     *
     * @parameter alias="initialStartLevel" default-value="7"
     */
    private int initialStartLevel;

    /**
     * Version prefix of the firmware file.
     *
     * @parameter alias="versionPrefix"
     * @required
     */
    private String versionPrefix;

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
    private Map<String, Artifact> nameVersionToDependency;

    @Override
    public void execute() throws MojoExecutionException {
        try {
            if (!kfBaseDirectory.isDirectory()) {
                throw new MojoExecutionException("KF base directory does not exist: " + kfBaseDirectory);
            }
            if (!factoryDirectory.isDirectory()) {
                throw new MojoExecutionException("Factory directory does not eists: " + factoryDirectory);
            }
            getLog().debug("Read bundlesConfiguration: " + bundlesConfiguration);
            getLog().debug("Read kfBaseDirectory: " + kfBaseDirectory);
            getLog().debug("Read factoryDirectory: " + factoryDirectory);

            Set<Artifact> dependencies = project.getDependencyArtifacts();
            nameToDependency = new HashMap<String, Artifact>();
            for (Artifact artifact : dependencies) {
                nameToDependency.put(artifact.getGroupId() + "~" + artifact.getArtifactId(), artifact);
            }

            nameVersionToDependency = new HashMap<String, Artifact>();
            for (Artifact artifact : dependencies) {
                nameVersionToDependency.put(
                        artifact.getGroupId() + "~" + artifact.getArtifactId() + "~" + artifact.getVersion(), artifact);
            }

            // Build levels configuration
            Map<Integer, LevelConfiguration> levelsConfigurations = buildLevelsConfiguration();
            // Build bundles configuration
            buildBundlesConfiguration(levelsConfigurations);
            // Build firmware entries configuration
            List<FirmwareEntryConfiguration> firmwareEntries = buildFirmwareEntriesConfiguration(levelsConfigurations);
            // Build init.xargs
            buildInitXargs(levelsConfigurations);
            // Build firmware-manifest.properties
            buildFirmwareManifest(firmwareEntries, factoryDirectory, firmwareQualifier);
            // Copy cocoon bundles
            copyFirmwareEntries(firmwareEntries, factoryDirectory);
            // Build firmware file
            buildFirmwareFile(firmwareEntries, factoryDirectory, firmwareQualifier);
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
            BundleConfiguration bundleConfig = new BundleConfiguration(getLog(), nameToDependency, classifier,
                    StringUtils.split(realConfig, ":"));
            if (bundleConfig.getArtifact() != null) {
                LevelConfiguration levelConfig = levelsConfiguration.get(bundleConfig.getStartLevel());
                if (levelConfig == null) {
                    levelConfig = new LevelConfiguration(bundleConfig.getStartLevel());
                    levelsConfiguration.put(bundleConfig.getStartLevel(), levelConfig);
                }
                levelConfig.addBundleConfiguration(bundleConfig);
            }
            getLog().debug("Read bundle configuration OK: " + realConfig);
        }
    }

    private List<FirmwareEntryConfiguration> buildFirmwareEntriesConfiguration(
            Map<Integer, LevelConfiguration> levelsConfiguration) throws MojoExecutionException {
        List<FirmwareEntryConfiguration> firmwareEntriesConfiguration = new ArrayList<FirmwareEntryConfiguration>();
        for (LevelConfiguration levelConfig : levelsConfiguration.values()) {
            for (BundleConfiguration bundleConfig : levelConfig.getBundlesConfiguration()) {
                firmwareEntriesConfiguration.add(bundleConfig);
            }
        }

        if (this.firmwareEntriesConfiguration != null) {
            String[] configs = StringUtils.split(this.firmwareEntriesConfiguration, ",");
            for (String config : configs) {
                String realConfig = config.trim();
                firmwareEntriesConfiguration.add(new FirmwareEntryConfiguration(getLog(), nameVersionToDependency, StringUtils
                        .split(realConfig, ":")));
                getLog().debug("Read firmware entry configuration OK: " + realConfig);
            }
        }

        return firmwareEntriesConfiguration;
    }

    private void buildInitXargs(Map<Integer, LevelConfiguration> levelsConfiguration) throws MojoExecutionException {
        File initXargsFile = new File(kfBaseDirectory, "init.xargs");
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
            out.write("-Dorg.knopflerfish.gosg.jars=file:");
            out.write(getRelativePath(factoryDirectory, kfBaseDirectory));
            out.write("\n");
            out.write("\n");
            out.write("-init\n");
            out.write("\n");

            // Body
            for (LevelConfiguration levelConfig : levelsConfiguration.values()) {
                if (levelConfig.getDescription() != null) {
                    out.write("# ");
                    out.write(levelConfig.getDescription());
                    out.write("\n");
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
                    if (bundleConfig.isComment()) {
                        out.write("#");
                    }
                    out.write("-start ");
                    out.write(bundleConfig.getDestFile());
                    out.write("\n");
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

    private void buildFirmwareManifest(List<FirmwareEntryConfiguration> firmwareEntries, File destDir, String firmwareQualifier)
            throws MojoExecutionException {
        File firmwareManifestFile = new File(destDir, "firmware-manifest.properties");
        BufferedWriter out = null;
        int index = 0;
        try {

            out = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(firmwareManifestFile), "ASCII"));

            out.write("firmware.versionPrefix=");
            out.write(versionPrefix);
            out.write("\n");
            out.write("firmware.versionNumber=");
            Artifact artifact = project.getArtifact();
            out.write(MavenUtils.getOSGiVersion(artifact));
            // Add firmware extra qualifier to firmware.versionNumber
            if (firmwareQualifier != null) {
                if (StringUtils.isNotEmpty(artifact.getSelectedVersion().getQualifier())) {
                    out.write("_");
                } else {
                    out.write(".");
                }
                out.write(firmwareQualifier);
            }

            out.write("\n");
            out.write("\n");

            // part.{index}.id=
            // part.{index}.namespace=
            // part.{index}.name=
            // part.{index}.version=
            // part.{index}.fullname=
            // part.{index}.installationType=
            for (FirmwareEntryConfiguration firmwareEntry : firmwareEntries) {
                if (!firmwareEntry.isOneTime()) {
                    ++index;
                    String prefix = "";
                    if (firmwareEntry.isComment()) {
                        prefix += "#";
                    }
                    prefix += "part." + String.valueOf(index) + ".";

                    out.write(prefix + "id=");
                    out.write(firmwareEntry.getId());
                    out.write("\n");
                    out.write(prefix + "namespace=");
                    out.write(firmwareEntry.getNamespace());
                    out.write("\n");
                    out.write(prefix + "name=");
                    out.write(firmwareEntry.getName());
                    out.write("\n");
                    out.write(prefix + "version=");
                    out.write(firmwareEntry.getVersion());
                    out.write("\n");
                    out.write(prefix + "fullname=");
                    out.write(firmwareEntry.getDestFile());
                    out.write("\n");
                    out.write(prefix + "installationType=");
                    out.write(firmwareEntry.getInstallationType());
                    out.write("\n");
                    out.write("\n");
                }
            }
            out.write("\n");
        } catch (IOException e) {
            throw new MojoExecutionException("IO problem while generating firmware-manifest.properties file", e);
        } catch (OverConstrainedVersionException e) {
            throw new MojoExecutionException("Failed to retrieve project artifact version", e);
        } finally {
            try {
                if (out != null) {
                    out.close();
                }
            } catch (IOException e) {
                throw new MojoExecutionException("IO problem while closing firmware-manifest.properties file", e);
            }
        }
    }

    private void copyFirmwareEntries(List<FirmwareEntryConfiguration> firmwareEntries, File destDir)
            throws MojoExecutionException {
        for (FirmwareEntryConfiguration firmwareEntry : firmwareEntries) {
            copyFile(firmwareEntry.getArtifact().getFile(), new File(destDir, firmwareEntry.getDestFile()));
        }
    }

    private void copyFile(File artifact, File destFile) throws MojoExecutionException {
        try {
            getLog().info("Copying " + artifact.getAbsolutePath() + " to " + destFile);
            FileUtils.copyFile(artifact, destFile);
        } catch (Exception e) {
            throw new MojoExecutionException("Error copying artifact from " + artifact + " to " + destFile, e);
        }
    }

    private void buildFirmwareFile(List<FirmwareEntryConfiguration> firmwareEntries, File destDir, String firmwareQualifier)
            throws MojoExecutionException {
        StringBuffer destFileName = new StringBuffer();

        try {
            destFileName.append(project.getArtifactId()).append("-").append(project.getVersion());
            if (firmwareQualifier != null) {
                if (StringUtils.isNotEmpty(project.getArtifact().getSelectedVersion().getQualifier())) {
                    destFileName.append("_").append(firmwareQualifier);
                } else {
                    destFileName.append("-").append(firmwareQualifier);
                }
            }
            destFileName.append("-firmware.zip");
            File destFile = new File(buildDirectory, destFileName.toString());
            Archiver archiver = archiverManager.getArchiver(destFile);
            archiver.setDestFile(destFile);

            List<String> excludes = new ArrayList<String>();
            excludes.add("cfg/");
            for (FirmwareEntryConfiguration firmwareEntry : firmwareEntries) {
                if (firmwareEntry.isOneTime()) {
                    excludes.add(firmwareEntry.getDestFile());
                }
            }

            DefaultFileSet fileSet = new DefaultFileSet();
            fileSet.setDirectory(destDir);
            fileSet.setPrefix("");
            fileSet.setIncludes(null);
            fileSet.setExcludes(excludes.toArray(new String[excludes.size()]));
            fileSet.setIncludingEmptyDirectories(true);
            archiver.addFileSet(fileSet);

            archiver.createArchive();
        } catch (OverConstrainedVersionException e) {
            throw new MojoExecutionException("Cannot build firmware file", e);
        } catch (NoSuchArchiverException e) {
            throw new MojoExecutionException("Cannot build firmware file", e);
        } catch (ArchiverException e) {
            throw new MojoExecutionException("Cannot build firmware file", e);
        } catch (IOException e) {
            throw new MojoExecutionException("Cannot build firmware file", e);
        }
    }

    private String getRelativePath(File targetPath, File basePath) {
        String target = targetPath.toURI().normalize().getPath();
        String base = basePath.toURI().normalize().getPath();
        if (!target.endsWith("/")) {
            target = target + "/";
        }
        if (!base.endsWith("/")) {
            base = base + "/";
        }

        int maxIndex = target.length();
        if (base.length() < maxIndex) {
            maxIndex = base.length();
        }

        // Detect common parts
        int index = 0;
        while (index < maxIndex && base.charAt(index) == target.charAt(index)) {
            ++index;
        }

        if (index == target.length()) {
            StringBuffer result = new StringBuffer();
            index = base.indexOf('/');
            while (index != -1) {
                index = base.indexOf('/', index + 1);
                result.append("../");
            }
            return result.toString();
        } else if (index == base.length()) {
            return target.substring(index);
        } else if (index > 0) {
            target = target.substring(index);
            base = base.substring(index);
        }

        StringBuffer result = new StringBuffer();
        index = base.indexOf('/');
        while (index != -1) {
            index = base.indexOf('/', index + 1);
            result.append("../");
        }
        result.append(target);

        return result.toString();
    }
}
