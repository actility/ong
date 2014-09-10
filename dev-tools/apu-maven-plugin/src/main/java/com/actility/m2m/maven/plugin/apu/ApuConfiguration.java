package com.actility.m2m.maven.plugin.apu;

/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

import java.io.File;

/**
 * Capture common archive configuration.
 *
 * @author <a href="mailto:brett@apache.org">Brett Porter</a>
 * @version $Id: MavenArchiveConfiguration.java 1233956 2012-01-20 15:27:43Z olamy $
 * @todo is this general enough to be in Plexus Archiver?
 */
public class ApuConfiguration {

    private File controlFile;
    /**
     * @since 2.2
     */
    private boolean forced = true;
    private String packageName;
    private String source;
    private String section;
    private String priority;
    private String architecture;
    private String essential;
    private String maintainer;
    private String description;
    private String homepage;

    /**
     * <p>
     * Returns, whether recreating the archive is forced (default). Setting this option to false means, that the archiver should
     * compare the timestamps of included files with the timestamp of the target archive and rebuild the archive only, if the
     * latter timestamp precedes the former timestamps. Checking for timestamps will typically offer a performance gain (in
     * particular, if the following steps in a build can be suppressed, if an archive isn't recrated) on the cost that you get
     * inaccurate results from time to time. In particular, removal of source files won't be detected.
     * </p>
     * <p>
     * An archiver doesn't necessarily support checks for uptodate. If so, setting this option to true will simply be ignored.
     * </p>
     *
     * @return True, if the target archive should always be created; false otherwise
     * @see #setForced(boolean)
     */
    public boolean isForced() {
        return forced;
    }

    /**
     * <p>
     * Sets, whether recreating the archive is forced (default). Setting this option to false means, that the archiver should
     * compare the timestamps of included files with the timestamp of the target archive and rebuild the archive only, if the
     * latter timestamp precedes the former timestamps. Checking for timestamps will typically offer a performance gain (in
     * particular, if the following steps in a build can be suppressed, if an archive isn't recrated) on the cost that you get
     * inaccurate results from time to time. In particular, removal of source files won't be detected.
     * </p>
     * <p>
     * An archiver doesn't necessarily support checks for uptodate. If so, setting this option to true will simply be ignored.
     * </p>
     *
     * @param forced True, if the target archive should always be created; false otherwise
     * @see #isForced()
     */
    public void setForced(boolean forced) {
        this.forced = forced;
    }

    public String getPackageName() {
        return packageName;
    }

    public void setPackageName(String packageName) {
        this.packageName = packageName;
    }

    public String getSource() {
        return source;
    }

    public void setSource(String source) {
        this.source = source;
    }

    public String getSection() {
        return section;
    }

    public void setSection(String section) {
        this.section = section;
    }

    public String getPriority() {
        return priority;
    }

    public void setPriority(String priority) {
        this.priority = priority;
    }

    public String getArchitecture() {
        return architecture;
    }

    public void setArchitecture(String architecture) {
        this.architecture = architecture;
    }

    public String getEssential() {
        return essential;
    }

    public void setEssential(String essential) {
        this.essential = essential;
    }

    public String getMaintainer() {
        return maintainer;
    }

    public void setMaintainer(String maintainer) {
        this.maintainer = maintainer;
    }

    public String getDescription() {
        return description;
    }

    public void setDescription(String description) {
        this.description = description;
    }

    public String getHomepage() {
        return homepage;
    }

    public void setHomepage(String homepage) {
        this.homepage = homepage;
    }
}
