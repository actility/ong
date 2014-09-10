package com.actility.m2m.maven.plugin.apu;

import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FileWriter;
import java.io.IOException;

import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveOutputStream;
import org.apache.commons.compress.compressors.gzip.GzipCompressorOutputStream;
import org.apache.commons.compress.utils.IOUtils;
import org.apache.maven.execution.MavenSession;
import org.apache.maven.project.MavenProject;

public class ApuArchiver {
    private final File apuFile;
    private final File controlFile;
    private final File dataFile;
    private final String classifier;
    private final File resourcesDirectory;

    public ApuArchiver(File apuFile, File resourcesDirectory, String classifier) {
        this.apuFile = apuFile;
        this.controlFile = new File(apuFile.getParentFile(), "control.tar.gz");
        this.dataFile = new File(apuFile.getParentFile(), "data.tar.gz");
        this.resourcesDirectory = resourcesDirectory;
        this.classifier = classifier;
    }

    /**
     * Creates a tar entry for the path specified with a name built from the base passed in and the file/directory name. If the
     * path is a directory, a recursive call is made such that the full directory is added to the tar.
     *
     * @param tOut The tar file's output stream
     * @param path The filesystem path of the file/directory being added
     * @param base The base prefix to for the name of the tar file entry
     *
     * @throws IOException If anything goes wrong
     */
    private long addFileToTarGz(TarArchiveOutputStream tOut, File f, String base) throws IOException {
        String entryName = base + f.getName();
        TarArchiveEntry tarEntry = new TarArchiveEntry(f, entryName);
        long totalSize = 0L;

        tOut.setLongFileMode(TarArchiveOutputStream.LONGFILE_GNU);
        tOut.putArchiveEntry(tarEntry);

        if (f.isFile()) {
            totalSize = f.length();
            IOUtils.copy(new FileInputStream(f), tOut);

            tOut.closeArchiveEntry();
        } else {
            tOut.closeArchiveEntry();

            totalSize = f.length();
            File[] children = f.listFiles();

            if (children != null) {
                for (File child : children) {
                    totalSize += addFileToTarGz(tOut, child, entryName + "/");
                }
            }
        }
        return totalSize;
    }

    /**
     * Creates a tar entry for the path specified with a name built from the base passed in and the file/directory name. If the
     * path is a directory, a recursive call is made such that the full directory is added to the tar.
     *
     * @param tOut The tar file's output stream
     * @param path The filesystem path of the file/directory being added
     * @param base The base prefix to for the name of the tar file entry
     *
     * @throws IOException If anything goes wrong
     */
    private long addDirSubElemsToTarGz(TarArchiveOutputStream tOut, File dir, String base) throws IOException {
        long totalSize = 0L;
        if (dir.isDirectory()) {
            File[] children = dir.listFiles();
            if (children != null) {
                for (File child : children) {
                    totalSize += addFileToTarGz(tOut, child, base);
                }
            }
        }
        return totalSize;
    }

    private Control createControl(MavenProject project, ApuConfiguration apuConfig) {
        Control control = new Control();
        if (apuConfig.getPackageName() != null) {
            control.setPackageName(apuConfig.getPackageName());
        } else {
            control.setPackageName(project.getArtifactId());
        }
        control.setSource(apuConfig.getSource());
        control.setVersion(project.getVersion());
        control.setSection(apuConfig.getSection());
        control.setPriority(apuConfig.getPriority());
        control.setArchitecture(apuConfig.getArchitecture());
        control.setEssential(apuConfig.getEssential());
        control.setMaintainer(apuConfig.getMaintainer());
        if (apuConfig.getDescription() != null) {
            control.setDescription(apuConfig.getDescription());
        } else {
            control.setDescription(project.getDescription());
        }
        if (apuConfig.getHomepage() != null) {
            control.setHomepage(apuConfig.getHomepage());
        } else {
            control.setHomepage(project.getUrl());
        }
        return control;
    }

    private void createControlTarGz(Control control) throws IOException {
        TarArchiveOutputStream tarGzFile = null;

        try {
            tarGzFile = new TarArchiveOutputStream(new GzipCompressorOutputStream(new BufferedOutputStream(
                    new FileOutputStream(controlFile))));

            FileWriter fileWriter = new FileWriter(new File(resourcesDirectory.getAbsolutePath() + "/common/control/control"));
            try {
                control.dump(fileWriter);
            } finally {
                fileWriter.close();
            }
            addDirSubElemsToTarGz(tarGzFile, new File(resourcesDirectory.getAbsolutePath() + "/common/control"), "");
            if (classifier != null) {
                addDirSubElemsToTarGz(tarGzFile, new File(resourcesDirectory.getAbsolutePath() + "/arch/" + classifier
                        + "/control"), "");
            }
        } finally {
            if (tarGzFile != null) {
                tarGzFile.finish();
                tarGzFile.close();
            }
        }
    }

    private void createDataTarGz(Control control) throws IOException {
        TarArchiveOutputStream tarGzFile = null;
        long dataSize = 0L;

        try {
            tarGzFile = new TarArchiveOutputStream(new GzipCompressorOutputStream(new BufferedOutputStream(
                    new FileOutputStream(dataFile))));

            dataSize = addDirSubElemsToTarGz(tarGzFile, new File(resourcesDirectory.getAbsolutePath() + "/common/data"), "");
            if (classifier != null) {
                dataSize += addDirSubElemsToTarGz(tarGzFile, new File(resourcesDirectory.getAbsolutePath() + "/arch/"
                        + classifier + "/data"), "");
            }
            if (dataSize % 1024L == 0) {
                control.setInstalledSize(String.valueOf(dataSize / 1024L) + "K");
            } else {
                control.setInstalledSize(String.valueOf((dataSize / 1024L) + 1L) + "K");
            }
        } finally {
            if (tarGzFile != null) {
                tarGzFile.finish();
                tarGzFile.close();
            }
        }
    }

    private void createApu() throws IOException {
        TarArchiveOutputStream tarGzFile = null;

        try {
            tarGzFile = new TarArchiveOutputStream(new GzipCompressorOutputStream(new BufferedOutputStream(
                    new FileOutputStream(apuFile))));

            addFileToTarGz(tarGzFile, controlFile, "");
            addFileToTarGz(tarGzFile, dataFile, "");
        } finally {
            if (tarGzFile != null) {
                tarGzFile.finish();
                tarGzFile.close();
            }
        }
    }

    public void createArchive(MavenSession session, MavenProject project, ApuConfiguration apuConfig) throws IOException {
        Control control = createControl(project, apuConfig);
        createDataTarGz(control);
        createControlTarGz(control);
        createApu();
    }
}
