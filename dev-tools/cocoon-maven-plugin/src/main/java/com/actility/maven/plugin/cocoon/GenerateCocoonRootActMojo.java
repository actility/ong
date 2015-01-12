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

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.jar.Attributes;
import java.util.jar.JarInputStream;

import org.apache.maven.artifact.Artifact;
import org.apache.maven.plugin.MojoExecutionException;
import org.apache.maven.shared.artifact.filter.collection.ArtifactsFilter;
import org.codehaus.plexus.archiver.tar.TarGZipUnArchiver;
import org.codehaus.plexus.util.DirectoryScanner;

/**
 * Goal which generate the init.xargs Knopflerfish init script.
 *
 * @goal generate-cocoon-rootact
 *
 * @phase prepare-package
 * @requiresDependencyResolution compile+runtime
 */
public class GenerateCocoonRootActMojo extends AbstractDependencyFilterMojo {

    /**
     * Directory containing the resource files that should be packaged into the APU.
     *
     * @parameter default-value="${project.build.outputDirectory}"
     */
    private File outputDirectory;

    /**
     * Default location used for mojo unless overridden in ArtifactItem
     *
     * @parameter default-value="${project.build.directory}"
     */
    private File buildDirectory;

    /**
     * Default module where all dependencies are copied unless a specific module is defined for them in modules
     *
     * @parameter
     * @required
     */
    private String defaultModule;

    /**
     * Mapping of a dependency to a module. If no module is specified, the defaultModule is used instead.
     *
     * @parameter
     */
    private String modules;

    /**
     * The name of the only architecture to include.
     *
     * @parameter
     */
    private String includeArchitecture;

    /**
     * Comma separated list of Products names to exclude.
     *
     * @parameter
     */
    private String excludeProducts;

    /**
     * Comma separated list of Modules names to exclude.
     *
     * @parameter
     */
    private String excludeModules;

    /**
     * The Tar.gz unarchiver.
     *
     * @component role="org.codehaus.plexus.archiver.UnArchiver" roleHint="tar.gz"
     */
    private TarGZipUnArchiver tarGzipArchiver;

    @Override
    public void execute() throws MojoExecutionException {
        try {
            buildRootAct();
        } catch (RuntimeException e) {
            e.printStackTrace();
            throw e;
        }
    }

    private void buildRootAct() throws MojoExecutionException {
        try {
            Map<String, String> modulesMapping = buildModulesMapping();
            DependencyStatusSets dss = getDependencySets(true);
            Set<Artifact> dependencies = dss.getResolvedDependencies();

            Set<Artifact> skippedArtifacts = dss.getSkippedDependencies();
            for (Artifact artifact : skippedArtifacts) {
                getLog().info(artifact.getFile().getName() + " already exists in destination.");
            }

            List<String> excludedProductsList = (excludeProducts != null) ? Arrays.asList(excludeProducts.split(","))
                    : Collections.EMPTY_LIST;
            List<String> excludedModulesList = (excludeModules != null) ? Arrays.asList(excludeModules.split(","))
                    : Collections.EMPTY_LIST;
            File tmpDir = new File(buildDirectory, "tmp");
            tmpDir.mkdir();
            File destDir = null;
            for (Artifact artifact : dependencies) {

            	if ("jar".equals(artifact.getType())) {
                    if (isBundle(artifact)) {
                        if (artifact.getClassifier() != null) {
                            destDir = new File(outputDirectory.getAbsolutePath() + "/arch/" + artifact.getClassifier() + "/"
                                    + getModule(modulesMapping, artifact) + "/data/lib/osgi/");
                        } else {
                            destDir = new File(outputDirectory.getAbsolutePath() + "/common/"
                                    + getModule(modulesMapping, artifact) + "/data/lib/osgi/");
                        }
                    } else {
                        if (artifact.getClassifier() != null) {
                            destDir = new File(outputDirectory.getAbsolutePath() + "/arch/" + artifact.getClassifier() + "/"
                                    + getModule(modulesMapping, artifact) + "/data/lib/java/");
                        } else {
                            destDir = new File(outputDirectory.getAbsolutePath() + "/common/"
                                    + getModule(modulesMapping, artifact) + "/data/lib/java/");
                        }
                    }
                    destDir.mkdirs();
                    copyFile(artifact.getFile(), new File(destDir, artifact.getFile().getName()));
                } else if ("apu".equals(artifact.getType())) {
                    removeDirectoryContent(tmpDir);

                    if (artifact.getClassifier() != null) {
                        destDir = new File(outputDirectory.getAbsolutePath() + "/arch/" + artifact.getClassifier() + "/"
                                + getModule(modulesMapping, artifact) + "/");
                    } else {
                        destDir = new File(outputDirectory.getAbsolutePath() + "/common/" + getModule(modulesMapping, artifact)
                                + "/");
                    }

                    // Untar file in tmp directory
                    tarGzipArchiver.setSourceFile(artifact.getFile());
                    tarGzipArchiver.setDestDirectory(tmpDir);
                    tarGzipArchiver.extract();

                    // Control
                    tarGzipArchiver.setSourceFile(new File(tmpDir, "control.tar.gz"));
                    tarGzipArchiver.setDestDirectory(tmpDir);
                    tarGzipArchiver.extract();

                    File preInstall = new File(tmpDir.getAbsolutePath() + "/preinst");
                    File postInstall = new File(tmpDir.getAbsolutePath() + "/postinst");

                    if (preInstall.exists()) {
                        copyFile(preInstall,
                                new File(destDir.getAbsolutePath() + "/control/preinst/" + artifact.getArtifactId()));
                    }
                    if (postInstall.exists()) {
                        copyFile(postInstall,
                                new File(destDir.getAbsolutePath() + "/control/postinst/" + artifact.getArtifactId()));
                    }

                    // Control
                    destDir = new File(destDir, "data");
                    destDir.mkdirs();
                    tarGzipArchiver.setSourceFile(new File(tmpDir, "data.tar.gz"));
                    tarGzipArchiver.setDestDirectory(destDir);
                    tarGzipArchiver.extract();
                } else {
                    throw new MojoExecutionException("Unknown artifact type (" + artifact.getType() + ") for dependency ("
                            + artifact.getGroupId() + ":" + artifact.getArtifactId() + ":" + artifact.getVersion() + ")");
                }
            }
            
            
            for (File archDir : new File(outputDirectory.getAbsolutePath() + "/arch/").listFiles()) {
            	if (! includeArchitecture.equals(archDir.getName())) {
                    removeDirectory(archDir);
                }
            }
            for (String product : excludedProductsList) {
                product = product.trim();
                if (product.length() > 1) {
                    removeDirectory(new File(outputDirectory.getAbsolutePath() + "/product/" + product + "/"));
                }
            }
            for (String module : excludedModulesList) {
                module = module.trim();
                if (module.length() > 1) {
                    removeDirectory(new File(outputDirectory.getAbsolutePath() + "/module/" + module + "/"));
                }
            }
        } catch (IOException e) {
            throw new MojoExecutionException("IOException: ", e);
        }
    }

    private String getModule(Map<String, String> modulesMapping, Artifact artifact) {
        String artifactName = artifact.getGroupId() + ":" + artifact.getArtifactId();
        if (modulesMapping.containsKey(artifactName)) {
            return modulesMapping.get(artifactName);
        }
        return defaultModule;
    }

    private Map<String, String> buildModulesMapping() {
        Map<String, String> result = new HashMap<String, String>();
        if (modules != null) {
            String[] modulesArray = modules.split(",");
            for (String rawModule : modulesArray) {
                String[] moduleTokens = rawModule.trim().split(":");
                if (moduleTokens.length == 3) {
                    result.put(moduleTokens[0] + ":" + moduleTokens[1], moduleTokens[2]);
                }
            }
        }
        return result;
    }

    private boolean isBundle(Artifact artifact) throws IOException {
        JarInputStream jarStream = new JarInputStream(new FileInputStream(artifact.getFile()));
        Attributes attributes = jarStream.getManifest().getMainAttributes();
        return attributes.getValue("Bundle-ManifestVersion") != null;
    }

    public boolean removeDirectoryContent(File directory) {

        if (directory == null) {
            return false;
        }
        if (!directory.exists()) {
            return true;
        }
        if (!directory.isDirectory()) {
            return false;
        }

        String[] list = directory.list();

        if (list != null) {
            for (int i = 0; i < list.length; i++) {
                File entry = new File(directory, list[i]);

                if (entry.isDirectory()) {
                    if (!removeDirectory(entry)) {
                        return false;
                    }
                } else {
                    if (!entry.delete()) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    public boolean removeDirectory(File directory) {

        if (directory == null) {
            return false;
        }
        if (!directory.exists()) {
            return true;
        }
        if (!directory.isDirectory()) {
            return false;
        }

        String[] list = directory.list();

        if (list != null) {
            for (int i = 0; i < list.length; i++) {
                File entry = new File(directory, list[i]);

                if (entry.isDirectory()) {
                    if (!removeDirectory(entry)) {
                        return false;
                    }
                } else {
                    if (!entry.delete()) {
                        return false;
                    }
                }
            }
        }

        return directory.delete();
    }

    @Override
    protected ArtifactsFilter getMarkedArtifactFilter() {
        return new DestFileFilter(false, false, true, false, false, false, false, false, this.outputDirectory);
    }
}
