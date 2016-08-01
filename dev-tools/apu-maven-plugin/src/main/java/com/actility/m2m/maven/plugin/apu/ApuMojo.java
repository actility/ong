/*******************************************************************************
 * Copyright   Actility, SA. All Rights Reserved.
 * DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License version
 * 2 only, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License version 2 for more details (a copy is
 * included at /legal/license.txt).
 * 
 * You should have received a copy of the GNU General Public License
 * version 2 along with this work; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 * 
 * Please contact Actility, SA.,  4, rue Ampere 22300 LANNION FRANCE
 * or visit www.actility.com if you need additional
 * information or have any questions.
 *******************************************************************************/
package com.actility.m2m.maven.plugin.apu;

import java.io.File;

import org.apache.maven.execution.MavenSession;
import org.apache.maven.plugin.AbstractMojo;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.project.MavenProject;
import org.apache.maven.project.MavenProjectHelper;

/**
 * @goal apu
 *
 * @phase package
 */
public class ApuMojo extends AbstractMojo {

    private static final String[] DEFAULT_EXCLUDES = new String[] { "" };

    private static final String[] DEFAULT_INCLUDES = new String[] { "**/**" };

    /**
     * List of files to include. Specified as fileset patterns which are relative to the input directory whose contents is being
     * packaged into the APU.
     *
     * @parameter
     */
    private String[] includes;

    /**
     * List of files to exclude. Specified as fileset patterns which are relative to the input directory whose contents is being
     * packaged into the APU.
     *
     * @parameter
     */
    private String[] excludes;

    /**
     * Directory containing the resource files that should be packaged into the APU.
     *
     * @parameter default-value="${project.build.outputDirectory}"
     * @required
     */
    private File resourcesDirectory;

    /**
     * Directory containing the generated APU.
     *
     * @parameter default-value="${project.build.directory}"
     * @required
     */
    private File outputDirectory;

    /**
     * Name of the generated APU.
     *
     * @parameter alias="apuName" expression="${apu.finalName}" default-value="${project.build.finalName}"
     * @required
     */
    private String finalName;

    /**
     * @component
     */
    private MavenProjectHelper projectHelper;

    /**
     * The Maven project.
     *
     * @parameter default-value="${project}"
     * @required
     * @readonly
     */
    private MavenProject project;

    /**
     * Classifier to add to the artifact generated. If given, the artifact will be attached. If this is not given,it will merely
     * be written to the output directory according to the finalName.
     *
     * @parameter
     */
    private String classifier;

    /**
     * @parameter default-value="${session}"
     * @readonly
     * @required
     */
    private MavenSession session;

    /**
     * The archive configuration to use. See <a href="http://maven.apache.org/shared/maven-archiver/index.html">Maven Archiver
     * Reference</a>.
     *
     * @parameter
     */
    private ApuConfiguration apuConfig = new ApuConfiguration();

    /**
     *
     * @parameter
     */
    private String packageName;

    /**
     *
     * @parameter
     */
    private String source;

    /**
     *
     * @parameter
     */
    private String section;

    /**
     *
     * @parameter
     */
    private String priority;

    /**
     *
     * @parameter
     * @required
     */
    private String architecture;

    /**
     *
     * @parameter
     */
    private String essential;

    /**
     *
     * @parameter
     * @required
     */
    private String maintainer;

    /**
     *
     * @parameter
     */
    private String description;

    /**
     *
     * @parameter
     */
    private String homepage;

    // /**
    // * Path to the default control file to use. It will be used if <code>useDefaultControlFile</code> is set to
    // * <code>true</code>.
    // *
    // * @parameter default-value="${project.build.outputDirectory}/control/control"
    // * @required
    // * @readonly
    // */
    // private File defaultControlFile;

    // /**
    // * Set this to <code>true</code> to enable the use of the <code>defaultControlFile</code>.
    // *
    // * @parameter expression="${apu.useDefaultControlFile}" default-value="false"
    // *
    // */
    // private boolean useDefaultControlFile;

    @Override
    public void execute() throws MojoExecutionException {
        if (!resourcesDirectory.exists()) {
            throw new MojoExecutionException("Cannot create the apu, no resources found in "
                    + resourcesDirectory.getAbsolutePath());
        }
        if (description == null && project.getDescription() == null) {
            throw new MojoExecutionException("No description provided for APU");
        }
        getLog().info("Classifier: " + classifier);
        getLog().info("Architecture: " + architecture);
        File apuFile = createApu();

        if (classifier != null) {
            projectHelper.attachArtifact(project, "apu", classifier, apuFile);
        } else {
            project.getArtifact().setFile(apuFile);
        }
    }

    /**
     * Generates the APU.
     */
    public File createApu() throws MojoExecutionException {
        File apuFile = getApuFile(outputDirectory, finalName, classifier);

        ApuArchiver archiver = new ApuArchiver(apuFile, resourcesDirectory, classifier);

        apuConfig.setForced(false);
        apuConfig.setPackageName(packageName);
        apuConfig.setSource(source);
        apuConfig.setSection(section);
        apuConfig.setPriority(priority);
        apuConfig.setArchitecture(architecture);
        apuConfig.setEssential(essential);
        apuConfig.setMaintainer(maintainer);
        apuConfig.setDescription(description);
        apuConfig.setHomepage(homepage);

        try {
            File contentDirectory = resourcesDirectory;
            if (!contentDirectory.exists()) {
                getLog().warn("APU will be empty - no content was marked for inclusion!");
            }
            // Includes and excludes not supported for now...
            // else {
            // archiver.getArchiver().addDirectory(contentDirectory, getIncludes(), getExcludes());
            // }

            // File existingControl = defaultControlFile;

            // if (useDefaultControlFile && existingControl.exists() && apuConfig.getControlFile() == null) {
            // getLog().info("Adding existing control to archive. Found under: " + existingControl.getPath());
            // apuConfig.setControlFile(existingControl);
            // }

            archiver.createArchive(session, project, apuConfig);

            return apuFile;
        } catch (Exception e) {
            throw new MojoExecutionException("Error assembling APU", e);
        }
    }

    public File getApuFile(File basedir, String finalName, String classifier) {
        if (classifier == null) {
            classifier = "";
        } else if (classifier.trim().length() > 0 && !classifier.startsWith("-")) {
            classifier = "-" + classifier;
        }

        return new File(basedir, finalName + classifier + ".apu");
    }

    private String[] getIncludes() {
        if (includes != null && includes.length > 0) {
            return includes;
        }
        return DEFAULT_INCLUDES;
    }

    private String[] getExcludes() {
        if (excludes != null && excludes.length > 0) {
            return excludes;
        }
        return DEFAULT_EXCLUDES;
    }
}
