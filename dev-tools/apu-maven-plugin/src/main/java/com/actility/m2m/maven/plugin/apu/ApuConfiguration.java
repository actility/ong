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
