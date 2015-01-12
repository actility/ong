/*
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
 *
 * id $Id: MainInstallDialog.java 8956 2014-06-20 12:12:51Z MLouiset $
 * author $Author: MLouiset $
 * version $Revision: 8956 $
 * lastrevision $Date: 2014-06-20 14:12:51 +0200 (Fri, 20 Jun 2014) $
 * modifiedby $LastChangedBy: MLouiset $
 * lastmodified $LastChangedDate: 2014-06-20 14:12:51 +0200 (Fri, 20 Jun 2014) $
 */
package com.actility.ong.installer;

import java.awt.Canvas;
import java.awt.EventQueue;
import java.awt.Font;
import java.awt.Graphics;
import java.awt.Image;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;
import java.net.URI;
import java.net.URISyntaxException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Properties;

import javax.swing.AbstractAction;
import javax.swing.Action;
import javax.swing.DefaultComboBoxModel;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JSeparator;
import javax.swing.JTextField;
import javax.swing.SwingConstants;

import com.actility.ong.installer.config.ArchConfig;
import com.actility.ong.installer.config.ModuleConfig;
import com.actility.ong.installer.config.Modules;
import com.actility.ong.installer.config.ProductConfig;
import com.actility.ong.installer.module.config.GsclConfig;
import com.actility.ong.installer.module.config.Iec61131Config;
import com.actility.ong.installer.module.config.KnxConfig;
import com.actility.ong.installer.module.config.ModbusConfig;
import com.actility.ong.installer.module.config.OngConfig;
import com.actility.ong.installer.module.config.WMbusConfig;
import com.actility.ong.installer.module.config.WattecoConfig;
import com.actility.ong.installer.module.config.ZigBeeConfig;

/**
 * 
 * @author mlouiset
 * 
 */
public class MainInstallDialog {

    private JFrame frame;
    private JTextField text_sshUser;
    private JTextField text_sshHost;
    private JTextField text_sshPort;
    private JTextField text_maxLogSize;
    private JTextField text_maxLogSizeDefault;
    private JTextField text_nsclUri;
    private JTextField text_tpkDevUri;
    private JTextField text_gsclCoapHost;
    private JTextField text_gsclCoapPort;
    // private JTextField text_acsUrl;
    private JTextField text_proxyHost;
    private JTextField text_proxyPort;
    private JTextField text_rootAct;
    private JTextField text_ongName;
    private JTextField text_domainName;
    private JComboBox combo_boxProduct;
    // private JComboBox combo_boxZbZnpSecurityMode;
    private JButton btn_install;
    // private JButton btn_flash_zb_dongle;
    private JButton btn_cancel;
    private JCheckBox chckbx_ongName;
    private JCheckBox chckbx_dev;
    private JCheckBox chckbx_proxy;
    private JCheckBox chckbx_gscl;
    private JCheckBox chckbx_zigBeeDriver;
    private JCheckBox chckbx_wMbusDriver;
    // private JCheckBox chckbx_flash_zb_dongle;
    private JCheckBox chckbx_wattecoDriver;
    private JCheckBox chckbx_knxDriver;
    private JCheckBox chckbx_modbusDriver;
    private JCheckBox chckbx_iec61131;
    private JCheckBox chckbx_tcpdump;
    private final Action action_cancel = new SwingActionCancel();
    private final Action action_install = new SwingActionInstall();
    // private final Action action_flash_zb_dongle = new SwingActionFlashZbDongle();
    private final Action action_chk_proxy = new SwingActionChkProxy();
    private final Action action_chk_ongName = new SwingActionChkOngName();
    private MyJarFile myJarFile;
    private PrintStream log;
    private RemoteOngInstaller installer;
    private String ongVersion;
	private String targetedArch;

    /**
     * Launch the application.
     */
    public static void main(final String[] args) {
        EventQueue.invokeLater(new Runnable() {
            @Override
            public void run() {
                try {
                    boolean dev = false;
                    for (String arg : args) {
                        if ("-dev".equals(arg)) {
                            dev = true;
                        }
                    }
                    MainInstallDialog window = new MainInstallDialog(dev);
                    window.frame.setVisible(true);
                } catch (Exception e) {
                    e.printStackTrace();
                }
            }
        });
    }

    /**
     * Create the application.
     */
    public MainInstallDialog(boolean dev) {
        File logFile = null;

        String logFileName = System.getProperty("user.dir") + System.getProperty("file.separator") + "ong-install-"
                + InstallerUtil.getInstance().getDateFormatForFileName() + "-" + System.currentTimeMillis() + ".log";
        try {
            logFile = new File(logFileName);
            log = new PrintStream(logFile);
        } catch (FileNotFoundException e) {
            System.err.println("Unable to open log file " + logFileName);
            e.printStackTrace();
            System.exit(1);
        }

        String jarFileName = System.getProperty("user.dir") + System.getProperty("file.separator")
                + System.getProperty("java.class.path");
        if (new File(jarFileName).exists()) {
            myJarFile = new MyJarFile(jarFileName, log);
        } else {
            myJarFile = new MyJarFile(log);
        }
        initialize(dev);
    }

    /**
     * Initialize the contents of the frame.
     */
    private void initialize(boolean dev) {
        int frameWidth = 600;
        InputStream is = getClass().getClassLoader().getResourceAsStream("META-INF/application.properties");
        Properties props = new Properties();
        try {
            props.load(is);
            ongVersion = props.getProperty("version");
            targetedArch = props.getProperty("targetedarch");
        } catch (IOException e1) {
            ongVersion = "(version unknown)";
        }
        Properties archProps = new Properties();
        try {
            archProps.load(getClass().getClassLoader().getResourceAsStream(
                    "arch/" + targetedArch + "/installer/arch.properties"));
            InstallerUtil.getInstance().setArch(targetedArch, archProps);
        } catch (IOException e1) {
            log.append("Cannot load architecture property file: arch/" + targetedArch
                    + "/installer/arch.properties");
        }

        List<MyFileEntry> products = myJarFile.getSubDirectoriesOnOneLevel("product/");
        for (MyFileEntry product : products) {
            Properties productProps = new Properties();
            try {
                log.println("Load product " + product.getFileBaseName());
                productProps.load(getClass().getClassLoader().getResourceAsStream(
                        "product/" + product.getFileBaseName() + "/product.properties"));
                InstallerUtil.getInstance().addNewProduct(product.getFileBaseName(), productProps);
            } catch (IOException e1) {
                log.append("Cannot load product property file: product/" + product.getFileBaseName() + "/product.properties");
            }
        }
        List<MyFileEntry> modules = myJarFile.getSubDirectoriesOnOneLevel("module/");
        for (MyFileEntry module : modules) {
            Properties moduleProps = new Properties();
            try {
                log.println("Load module " + module.getFileBaseName());
                moduleProps.load(getClass().getClassLoader().getResourceAsStream(
                        "module/" + module.getFileBaseName() + "/module.properties"));
                InstallerUtil.getInstance().addNewModule(module.getFileBaseName(), moduleProps);
            } catch (IOException e1) {
                log.append("Cannot load module property file: module/" + module.getFileBaseName() + "/module.properties");
            }
        }

        frame = new JFrame();
        frame.setResizable(false);
        frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        frame.setTitle("ONG Installer v" + ongVersion);
        frame.getContentPane().setLayout(null);

        byte[] rawImg = myJarFile.getFileRawContent("images/cocoon-transparent.gif");
        if (rawImg != null) {
            Toolkit toolkit = Toolkit.getDefaultToolkit();
            MyCanvas canvas = new MyCanvas(toolkit.createImage(rawImg));
            canvas.setBounds(60, 30, 138, 80); // 116
            frame.getContentPane().add(canvas);
        }

        JLabel lbl_title = new JLabel("Installation");
        lbl_title.setFont(new Font("URW Gothic L Book", Font.BOLD | Font.ITALIC, 24));
        lbl_title.setBounds(240, 60, 160, 32);
        frame.getContentPane().add(lbl_title);

        int firstColumnXOffset = 30;
        int secondColumnXOffset = 260;
        int yOffset = 120;
        int firstColumnWidth = 230;
        int secondColumnWidth = 310;

        int totalWidth = secondColumnXOffset - firstColumnXOffset + secondColumnWidth;

        int firstSubColumn3XOffset = secondColumnXOffset;
        int firstSubColumn3Width = 2 * (secondColumnWidth - 12) / 5;
        int secondSubColumn3XOffset = firstSubColumn3XOffset + firstSubColumn3Width + 6;
        int secondSubColumn3Width = 1 * (secondColumnWidth - 12) / 5;
        int thirdSubColumn3XOffset = secondSubColumn3XOffset + secondSubColumn3Width + 6;
        int thirdSubColumn3Width = 2 * (secondColumnWidth - 12) / 5;

        int firstSubColumn2XOffset = secondColumnXOffset;
        int firstSubColumn2Width = secondSubColumn3XOffset - firstSubColumn3XOffset + secondSubColumn3Width;
        int secondSubColumn2XOffset = thirdSubColumn3XOffset;
        int secondSubColumn2Width = thirdSubColumn3Width;

        int firstSubColumnBis2XOffset = secondColumnXOffset;
        int firstSubColumnBis2Width = firstSubColumn3Width;
        int secondSubColumnBis2XOffset = secondSubColumn3XOffset;
        int secondSubColumnBis2Width = thirdSubColumn3XOffset - secondSubColumn3XOffset + thirdSubColumn3Width;

        // /////////////////////////////////////////////////////////////////
        // Box configuration
        // /////////////////////////////////////////////////////////////////
        JLabel lbl_config = new JLabel("Cocoon box configuration");
        lbl_config.setFont(new Font(lbl_config.getFont().getName(), Font.PLAIN, lbl_config.getFont().getSize() + 5));
        lbl_config.setBounds(firstColumnXOffset, yOffset, totalWidth, 20);
        frame.getContentPane().add(lbl_config);

        yOffset += 35;

        JLabel lbl_ipBox = new JLabel("SSH host & port");
        lbl_ipBox.setBounds(firstColumnXOffset, yOffset, firstColumnWidth, 15);
        frame.getContentPane().add(lbl_ipBox);

        text_sshHost = new JTextField();
        text_sshHost.setBounds(firstSubColumn2XOffset, yOffset - 2, firstSubColumn2Width, 19);
        text_sshHost.setText("localhost");
        frame.getContentPane().add(text_sshHost);
        text_sshHost.setColumns(20);

        text_sshPort = new JTextField();
        text_sshPort.setBounds(secondSubColumn2XOffset, yOffset - 2, secondSubColumn2Width, 19);
        text_sshPort.setText("22");
        frame.getContentPane().add(text_sshPort);
        text_sshPort.setColumns(20);

        yOffset += 24;

        JLabel lbl_user = new JLabel("SSH login");
        lbl_user.setBounds(firstColumnXOffset, yOffset, firstColumnWidth, 15);
        frame.getContentPane().add(lbl_user);

        text_sshUser = new JTextField();
        text_sshUser.setBounds(secondColumnXOffset, yOffset - 2, secondColumnWidth, 19);
        text_sshUser.setText("root");
        frame.getContentPane().add(text_sshUser);
        text_sshUser.setColumns(20);

        yOffset += 24;

        JLabel lbl_Arch = new JLabel("Architecture");
        lbl_Arch.setBounds(firstColumnXOffset, yOffset, firstColumnWidth, 15);
        frame.getContentPane().add(lbl_Arch);

        JLabel lbl_ArchValue = new JLabel(targetedArch);
        lbl_ArchValue.setBounds(secondColumnXOffset, yOffset - 5, secondColumnWidth, 24);
        frame.getContentPane().add(lbl_ArchValue);

        yOffset += 26;

        String[] productsArray = InstallerUtil.getInstance().getProducts();
        combo_boxProduct = new JComboBox();
        combo_boxProduct.setModel(new DefaultComboBoxModel(productsArray));
        combo_boxProduct.setMaximumRowCount(4);
        if (productsArray.length > 1) {
            JLabel lbl_boxProduct = new JLabel("Product");
            lbl_boxProduct.setBounds(firstColumnXOffset, yOffset, firstColumnWidth, 15);
            frame.getContentPane().add(lbl_boxProduct);

            combo_boxProduct.setBounds(secondColumnXOffset, yOffset - 5, secondColumnWidth, 24);
            frame.getContentPane().add(combo_boxProduct);

            yOffset += 26;
        }

        JLabel lbl_maxLogSize = new JLabel("Maximum log size (K,M,G)");
        lbl_maxLogSize.setBounds(firstColumnXOffset, yOffset, firstColumnWidth, 15);
        frame.getContentPane().add(lbl_maxLogSize);

        text_maxLogSize = new JTextField();
        text_maxLogSize.setBounds(firstSubColumn3XOffset, yOffset - 2, firstSubColumn3Width, 19);
        frame.getContentPane().add(text_maxLogSize);
        text_maxLogSize.setColumns(20);

        JLabel lbl_maxLogSizeDefault = new JLabel("Default");
        lbl_maxLogSizeDefault.setBounds(secondSubColumn3XOffset, yOffset, secondSubColumn3Width, 15);
        frame.getContentPane().add(lbl_maxLogSizeDefault);

        text_maxLogSizeDefault = new JTextField();
        text_maxLogSizeDefault.setBounds(thirdSubColumn3XOffset, yOffset - 2, thirdSubColumn3Width, 19);
        text_maxLogSizeDefault.setText(InstallerUtil.getInstance().getArch().getMaxLogSize());
        text_maxLogSizeDefault.setEditable(false);
        frame.getContentPane().add(text_maxLogSizeDefault);
        text_maxLogSizeDefault.setColumns(20);

        yOffset += 24;

        JLabel lbl_rootAct = new JLabel("Actility ROOT path");
        lbl_rootAct.setBounds(firstColumnXOffset, yOffset, firstColumnWidth, 15);
        frame.getContentPane().add(lbl_rootAct);

        text_rootAct = new JTextField();
        text_rootAct.setBounds(secondColumnXOffset, yOffset - 2, secondColumnWidth, 19);
        text_rootAct.setText("/home/ong/");
        frame.getContentPane().add(text_rootAct);
        text_rootAct.setColumns(20);

        yOffset += 24;

        chckbx_ongName = new JCheckBox("Auto-detect ONG name");
        chckbx_ongName.setBounds(firstColumnXOffset - 3, yOffset, firstColumnWidth + secondColumnWidth, 23);
        chckbx_ongName.setAction(action_chk_ongName);
        chckbx_ongName.setSelected(true);
        frame.getContentPane().add(chckbx_ongName);

        yOffset += 24;

        JLabel lbl_ongName = new JLabel("ONG name & domain");
        lbl_ongName.setBounds(firstColumnXOffset, yOffset, firstColumnWidth, 15);
        frame.getContentPane().add(lbl_ongName);

        text_ongName = new JTextField();
        text_ongName.setBounds(firstSubColumnBis2XOffset, yOffset - 2, firstSubColumnBis2Width, 19);
        text_ongName.setText("<auto-detected>");
        text_ongName.setEnabled(false);
        frame.getContentPane().add(text_ongName);
        text_ongName.setColumns(20);

        JLabel lbl_rmsNameDot = new JLabel(".");
        lbl_rmsNameDot.setBounds(secondSubColumnBis2XOffset - 4, yOffset, 3, 15);
        frame.getContentPane().add(lbl_rmsNameDot);

        text_domainName = new JTextField();
        text_domainName.setBounds(secondSubColumnBis2XOffset, yOffset - 2, secondSubColumnBis2Width, 19);
        text_domainName.setText("actility.com");
        frame.getContentPane().add(text_domainName);
        text_domainName.setColumns(20);

        if (dev) {
            yOffset += 24;

            chckbx_dev = new JCheckBox("Install development version");
            chckbx_dev.setBounds(firstColumnXOffset, yOffset, totalWidth, 23);
            chckbx_dev.setSelected(true);
            frame.getContentPane().add(chckbx_dev);
        }

        yOffset += 35;

        JSeparator sep = new JSeparator(SwingConstants.HORIZONTAL);
        sep.setBounds(firstColumnXOffset, yOffset, firstColumnWidth + secondColumnWidth, 5);
        frame.getContentPane().add(sep);

        yOffset += 10;

        // /////////////////////////////////////////////////////////////////
        // M2M configuration
        // /////////////////////////////////////////////////////////////////
        JLabel lbl_m2m = new JLabel("M2M configuration");
        lbl_m2m.setFont(new Font(lbl_m2m.getFont().getName(), Font.PLAIN, lbl_m2m.getFont().getSize() + 5));
        lbl_m2m.setBounds(firstColumnXOffset, yOffset, secondColumnXOffset - firstColumnXOffset + secondColumnWidth, 20);
        frame.getContentPane().add(lbl_m2m);

        yOffset += 35;

        JLabel lbl_nscUri = new JLabel("NSCL URI");
        lbl_nscUri.setBounds(firstColumnXOffset, yOffset, firstColumnWidth, 15);
        frame.getContentPane().add(lbl_nscUri);

        text_nsclUri = new JTextField();
        text_nsclUri.setBounds(secondColumnXOffset, yOffset - 2, secondColumnWidth, 19);
        text_nsclUri.setText("http://nsc1.actility.com:8088/m2m");
        frame.getContentPane().add(text_nsclUri);
        text_nsclUri.setColumns(20);

        yOffset += 24;

        JLabel lbl_tpkDevUri = new JLabel("ThingPark Dev URI");
        lbl_tpkDevUri.setBounds(firstColumnXOffset, yOffset, firstColumnWidth, 15);
        frame.getContentPane().add(lbl_tpkDevUri);

        text_tpkDevUri = new JTextField();
        text_tpkDevUri.setBounds(secondColumnXOffset, yOffset - 2, secondColumnWidth, 19);
        frame.getContentPane().add(text_tpkDevUri);
        text_tpkDevUri.setColumns(20);

        yOffset += 24;

        JLabel lbl_gsclCoap = new JLabel("GSCL CoAP server host & port");
        lbl_gsclCoap.setBounds(firstColumnXOffset, yOffset, firstColumnWidth, 15);
        frame.getContentPane().add(lbl_gsclCoap);

        text_gsclCoapHost = new JTextField();
        text_gsclCoapHost.setEnabled(false);
        text_gsclCoapHost.setBounds(firstSubColumn2XOffset, yOffset - 2, firstSubColumn2Width, 19);
        text_gsclCoapHost.setText("::1");
        frame.getContentPane().add(text_gsclCoapHost);
        text_gsclCoapHost.setColumns(20);

        text_gsclCoapPort = new JTextField();
        text_gsclCoapPort.setEnabled(false);
        text_gsclCoapPort.setBounds(secondSubColumn2XOffset, yOffset - 2, secondSubColumn2Width, 19);
        text_gsclCoapPort.setText("5683");
        frame.getContentPane().add(text_gsclCoapPort);
        text_gsclCoapPort.setColumns(20);

        yOffset += 35;

        sep = new JSeparator(SwingConstants.HORIZONTAL);
        sep.setBounds(firstColumnXOffset, yOffset, firstColumnWidth + secondColumnWidth, 5);
        frame.getContentPane().add(sep);

        yOffset += 10;

        // /////////////////////////////////////////////////////////////////
        // GSCL configuration
        // /////////////////////////////////////////////////////////////////
        chckbx_gscl = new JCheckBox("GSCL");
        chckbx_gscl.setBounds(firstColumnXOffset, yOffset, firstColumnWidth, 23);
        chckbx_gscl.setSelected(true);
        chckbx_gscl.setAction(new SwingActionChkGscl());
        frame.getContentPane().add(chckbx_gscl);

        yOffset += 30;

        // JLabel lbl_acsUrl = new JLabel("ACS URL");
        // lbl_acsUrl.setBounds(firstColumnXOffset, yOffset, firstColumnWidth, 15);
        // frame.getContentPane().add(lbl_acsUrl);
        //
        // text_acsUrl = new JTextField();
        // text_acsUrl.setBounds(secondColumnXOffset, yOffset - 2, secondColumnWidth, 19);
        // text_acsUrl.setText("http://acs1.actility.com:8080/openacs/acs");
        // frame.getContentPane().add(text_acsUrl);
        // text_acsUrl.setColumns(20);
        //
        // yOffset += 24;

        chckbx_proxy = new JCheckBox("HTTP proxy host & port");
        chckbx_proxy.setBounds(firstColumnXOffset - 3, yOffset, firstColumnWidth, 23);
        chckbx_proxy.setAction(action_chk_proxy);
        frame.getContentPane().add(chckbx_proxy);

        text_proxyHost = new JTextField();
        text_proxyHost.setEnabled(false);
        text_proxyHost.setBounds(firstSubColumn2XOffset, yOffset + 2, firstSubColumn2Width, 19);
        text_proxyHost.setText("");
        frame.getContentPane().add(text_proxyHost);
        text_proxyHost.setColumns(20);

        text_proxyPort = new JTextField();
        text_proxyPort.setEnabled(false);
        text_proxyPort.setBounds(secondSubColumn2XOffset, yOffset + 2, secondSubColumn2Width, 19);
        text_proxyPort.setText("");
        frame.getContentPane().add(text_proxyPort);
        text_proxyPort.setColumns(20);

        yOffset += 35;

        sep = new JSeparator(SwingConstants.HORIZONTAL);
        sep.setBounds(firstColumnXOffset, yOffset, firstColumnWidth + secondColumnWidth, 5);
        frame.getContentPane().add(sep);

        yOffset += 10;

        // ********************************************************************************************
        // GIP

        int checkBoxWidth = (frameWidth - (2 * firstColumnXOffset)) / 4;
        int xCheckBox = firstColumnXOffset;

        // /////////////////////////////////////////////////////////////////
        // ZigBee configuration
        // /////////////////////////////////////////////////////////////////
        chckbx_zigBeeDriver = new JCheckBox("ZigBee");
        chckbx_zigBeeDriver.setBounds(xCheckBox, yOffset, checkBoxWidth, 23);
        chckbx_zigBeeDriver.setSelected(true);
        chckbx_zigBeeDriver.setAction(new SwingActionChkZigBee());
        frame.getContentPane().add(chckbx_zigBeeDriver);

        xCheckBox += checkBoxWidth;

        // /////////////////////////////////////////////////////////////////
        // Watteco configuration
        // /////////////////////////////////////////////////////////////////
        chckbx_wattecoDriver = new JCheckBox("Watteco");
        chckbx_wattecoDriver.setBounds(xCheckBox, yOffset, checkBoxWidth, 23);
        chckbx_wattecoDriver.setSelected(true);
        chckbx_wattecoDriver.setAction(new SwingActionChkWatteco());
        frame.getContentPane().add(chckbx_wattecoDriver);

        xCheckBox += checkBoxWidth;

        // /////////////////////////////////////////////////////////////////
        // WMbus configuration
        // /////////////////////////////////////////////////////////////////
        chckbx_wMbusDriver = new JCheckBox("WMbus");
        chckbx_wMbusDriver.setBounds(xCheckBox, yOffset, checkBoxWidth, 23);
        chckbx_wMbusDriver.setSelected(true);
        chckbx_wMbusDriver.setAction(new SwingActionChkWMbus());
        frame.getContentPane().add(chckbx_wMbusDriver);

        xCheckBox += checkBoxWidth;
        // /////////////////////////////////////////////////////////////////
        // KNX configuration
        // /////////////////////////////////////////////////////////////////
        chckbx_knxDriver = new JCheckBox("KNX");
        chckbx_knxDriver.setBounds(xCheckBox, yOffset, checkBoxWidth, 23);
        chckbx_knxDriver.setSelected(true);
        chckbx_knxDriver.setAction(new SwingActionChkKnx());
        frame.getContentPane().add(chckbx_knxDriver);

        xCheckBox = firstColumnXOffset;
        yOffset += 30;

        // /////////////////////////////////////////////////////////////////
        // Modbus configuration
        // /////////////////////////////////////////////////////////////////
        chckbx_modbusDriver = new JCheckBox("Modbus");
        chckbx_modbusDriver.setBounds(xCheckBox, yOffset, checkBoxWidth, 23);
        chckbx_modbusDriver.setSelected(true);
        chckbx_modbusDriver.setAction(new SwingActionChkModbus());
        frame.getContentPane().add(chckbx_modbusDriver);

        xCheckBox += checkBoxWidth;

        // /////////////////////////////////////////////////////////////////
        xCheckBox = firstColumnXOffset;
        yOffset += 30;

        sep = new JSeparator(SwingConstants.HORIZONTAL);
        sep.setBounds(firstColumnXOffset, yOffset, firstColumnWidth + secondColumnWidth, 5);
        frame.getContentPane().add(sep);

        yOffset += 10;

        // /////////////////////////////////////////////////////////////////
        // IEC-61131 configuration
        // /////////////////////////////////////////////////////////////////
        chckbx_iec61131 = new JCheckBox("IEC61131 engine");
        chckbx_iec61131.setBounds(firstColumnXOffset, yOffset, firstColumnWidth, 23);
        chckbx_iec61131.setSelected(true);
        chckbx_iec61131.setAction(new SwingActionChkIec61131());
        frame.getContentPane().add(chckbx_iec61131);

        yOffset += 30;

        sep = new JSeparator(SwingConstants.HORIZONTAL);
        sep.setBounds(firstColumnXOffset, yOffset, firstColumnWidth + secondColumnWidth, 5);
        frame.getContentPane().add(sep);

        yOffset += 10;

        // /////////////////////////////////////////////////////////////////
        // Tcpdump configuration
        // /////////////////////////////////////////////////////////////////
        chckbx_tcpdump = new JCheckBox("Tcpdump");
        chckbx_tcpdump.setBounds(firstColumnXOffset, yOffset, firstColumnWidth, 23);
        chckbx_tcpdump.setSelected(true);
        chckbx_tcpdump.setAction(new SwingActionChkTcpdump());
        frame.getContentPane().add(chckbx_tcpdump);

        yOffset += 40;

        // /////////////////////////////////////////////////////////////////
        // Buttons
        // /////////////////////////////////////////////////////////////////
        btn_cancel = new JButton("Cancel");
        btn_cancel.setAction(action_cancel);
        btn_cancel.setBounds(170, yOffset, 100, 25);
        frame.getContentPane().add(btn_cancel);

        btn_install = new JButton("Install");
        btn_install.setAction(action_install);
        btn_install.setBounds(frameWidth - 100 - 170, yOffset, 100, 25);
        frame.getContentPane().add(btn_install);

        yOffset += 70;

        frame.setBounds(100, 100, frameWidth, yOffset);
    }

    private void setAllEnabled(boolean enable) {
        text_sshUser.setEnabled(enable);
        text_sshHost.setEnabled(enable);
        text_sshPort.setEnabled(enable);
        text_maxLogSize.setEnabled(enable);
        text_maxLogSizeDefault.setEnabled(enable);
        combo_boxProduct.setEnabled(enable);
        text_rootAct.setEnabled(enable);
        text_nsclUri.setEnabled(enable);
        text_tpkDevUri.setEnabled(enable);
        if (chckbx_dev != null) {
            chckbx_dev.setEnabled(enable);
        }

        setEnableGscl(enable, true);
        setEnableOngName(enable, true);
        setEnableZigBee(enable, true);
        setEnableWatteco(enable, true);
        setEnableWMbus(enable, true);
        setEnableKnx(enable, true);
        setEnableModbus(enable, true);
        setEnableIec61131(enable, true);
        setEnableTcpdump(enable, true);

        btn_install.setEnabled(enable);
        btn_cancel.setEnabled(enable);
        // btn_flash_zb_dongle.setEnabled(enable);
    }

    private void setEnableOngName(boolean enable, boolean updateCheckbox) {
        chckbx_ongName.setEnabled(enable);
        text_domainName.setEnabled(enable);
        if (chckbx_ongName.isEnabled() && chckbx_ongName.isSelected()) {
            text_ongName.setEnabled(enable);
        } else {
            text_ongName.setEnabled(false);
        }
    }

    private void setEnableGscl(boolean enable, boolean updateCheckbox) {
        if (updateCheckbox) {
            chckbx_gscl.setEnabled(enable);
        }
        // text_acsUrl.setEnabled(enable);
        chckbx_proxy.setEnabled(enable);
        if (chckbx_proxy.isEnabled() && chckbx_proxy.isSelected()) {
            text_proxyHost.setEnabled(enable);
            text_proxyPort.setEnabled(enable);
        } else {
            text_proxyHost.setEnabled(false);
            text_proxyPort.setEnabled(false);
        }
    }

    private void setEnableZigBee(boolean enable, boolean updateCheckbox) {
        if (updateCheckbox) {
            chckbx_zigBeeDriver.setEnabled(enable);
        }
        // combo_boxZbZnpSecurityMode.setEnabled(enable);
        // chckbx_flash_zb_dongle.setEnabled(enable);
    }

    private void setEnableWatteco(boolean enable, boolean updateCheckbox) {
        if (updateCheckbox) {
            chckbx_wattecoDriver.setEnabled(enable);
        }
    }

    private void setEnableWMbus(boolean enable, boolean updateCheckbox) {
        if (updateCheckbox) {
            chckbx_wMbusDriver.setEnabled(enable);
        }
    }

    private void setEnableKnx(boolean enable, boolean updateCheckbox) {
        if (updateCheckbox) {
            chckbx_knxDriver.setEnabled(enable);
        }
    }

    private void setEnableModbus(boolean enable, boolean updateCheckbox) {
        if (updateCheckbox) {
            chckbx_modbusDriver.setEnabled(enable);
        }
    }

    private void setEnableIec61131(boolean enable, boolean updateCheckbox) {
        if (updateCheckbox) {
            chckbx_iec61131.setEnabled(enable);
        }
    }

    private void setEnableTcpdump(boolean enable, boolean updateCheckbox) {
        if (updateCheckbox) {
            chckbx_tcpdump.setEnabled(enable);
        }
    }

    private boolean isGsclSelected() {
        return chckbx_gscl.isEnabled() && chckbx_gscl.isSelected();
    }

    private boolean isZigBeeSelected() {
        return chckbx_zigBeeDriver.isEnabled() && chckbx_zigBeeDriver.isSelected();
    }

    private boolean isWMbusSelected() {
        return chckbx_wMbusDriver.isEnabled() && chckbx_wMbusDriver.isSelected();
    }

    private boolean isWattecoSelected() {
        return chckbx_wattecoDriver.isEnabled() && chckbx_wattecoDriver.isSelected();
    }

    private boolean isKnxSelected() {
        return chckbx_knxDriver.isEnabled() && chckbx_knxDriver.isSelected();
    }

    private boolean isModbusSelected() {
        return chckbx_modbusDriver.isEnabled() && chckbx_modbusDriver.isSelected();
    }

    private boolean isIec61131Selected() {
        return chckbx_iec61131.isEnabled() && chckbx_iec61131.isSelected();
    }

    private boolean isTcpdumpSelected() {
        return chckbx_tcpdump.isEnabled() && chckbx_tcpdump.isSelected();
    }

    private boolean checkModules() {
        return isGsclSelected() || isZigBeeSelected() || isWattecoSelected() || isWMbusSelected() || isKnxSelected()
                || isModbusSelected() || isIec61131Selected();
    }

    private List<ModuleConfig> buildModules(ArchConfig arch, ProductConfig product) {
        List<ModuleConfig> modules = new ArrayList<ModuleConfig>();
        modules.add(InstallerUtil.getInstance().getModuleFromName(product.getInternalModule(Modules.CORE)));
        if (isZigBeeSelected() || isWattecoSelected() || isWMbusSelected()) {
            modules.add(InstallerUtil.getInstance().getModuleFromName(product.getInternalModule(Modules.TTY_MAPPER)));
        }
        if (isGsclSelected()) {
            modules.add(InstallerUtil.getInstance().getModuleFromName(product.getInternalModule(Modules.GSCL)));
        }
        if (isZigBeeSelected()) {
            modules.add(InstallerUtil.getInstance().getModuleFromName(product.getInternalModule(Modules.ZIGBEE)));
        }
        if (isWattecoSelected()) {
            modules.add(InstallerUtil.getInstance().getModuleFromName(product.getInternalModule(Modules.WATTECO)));
        }
        if (isWMbusSelected()) {
            modules.add(InstallerUtil.getInstance().getModuleFromName(product.getInternalModule(Modules.WMBUS)));
        }
        if (isKnxSelected()) {
            modules.add(InstallerUtil.getInstance().getModuleFromName(product.getInternalModule(Modules.KNX)));
        }
        if (isModbusSelected()) {
            modules.add(InstallerUtil.getInstance().getModuleFromName(product.getInternalModule(Modules.MODBUS)));
        }
        if (isIec61131Selected()) {
            modules.add(InstallerUtil.getInstance().getModuleFromName(product.getInternalModule(Modules.IEC61131)));
        }
        if (isTcpdumpSelected()) {
            modules.add(InstallerUtil.getInstance().getModuleFromName(product.getInternalModule(Modules.TCPDUMP)));
        }
        String[] extraModules = arch.getExtraModules();
        if (extraModules != null) {
            for (String extraModule : extraModules) {
                modules.add(InstallerUtil.getInstance().getModuleFromName(product.getInternalModule(extraModule)));
            }
        }
        return modules;
    }

    private boolean checkMaxLogSize() {
        String maxLogSizeStr = text_maxLogSize.getText();
        if (maxLogSizeStr == null || "".equals(maxLogSizeStr)) {
            maxLogSizeStr = text_maxLogSizeDefault.getText();
        }
        return maxLogSizeStr.matches("[0-9]+[MKG]?");
    }

    private boolean checkPort(JTextField text) {
        if (text != null) {
            if ("".equals(text.getText())) {
                return true;
            }
            if (!text.getText().matches("[0-9]+")) {
                return false;
            }
            int port = Integer.parseInt(text.getText());
            return port > 0 && port < 65535;
        }
        return true;
    }

    private boolean checkUri(JTextField text) {
        try {
            new URI(text.getText());
            return true;
        } catch (URISyntaxException e) {
            // Ignore
        } catch (RuntimeException e) {
            // Ignore
        }
        return false;
    }

    private long buildLogMaxSize() {
        String maxLogSizeStr = text_maxLogSize.getText();
        if (maxLogSizeStr == null || "".equals(maxLogSizeStr)) {
            maxLogSizeStr = text_maxLogSizeDefault.getText();
        }
        long result = Long.parseLong(maxLogSizeStr.substring(0, maxLogSizeStr.length() - 1));
        if (maxLogSizeStr.endsWith("K")) {
            result *= 1024;
        } else if (maxLogSizeStr.endsWith("M")) {
            result *= 1024 * 1024;
        } else if (maxLogSizeStr.endsWith("G")) {
            result *= 1024 * 1024 * 1024;
        }
        return result;
    }

    private int buildPort(JTextField text, int defaultValue) {
        if (text != null && !"".equals(text.getText())) {
            return Integer.parseInt(text.getText());
        }
        return defaultValue;
    }

    private boolean buildBoolean(JCheckBox chck, boolean defaultValue) {
        if (chck != null) {
            return chck.isSelected();
        }
        return defaultValue;
    }

    private boolean checkConfiguration() {
        StringBuffer errors = new StringBuffer();
        if (!checkPort(text_sshPort)) {
            errors.append("'SSH port' is not a valid port number\n");
        }
        if (!checkMaxLogSize()) {
            errors.append("Log max size value is invalid\n");
        }
        if ("/".equals(URI.create(text_rootAct.getText()).normalize().toString())) {
            errors.append("'Actility ROOT Path' cannot be '/'. Retry the installation with another path\n");
        }
        if (!checkUri(text_nsclUri)) {
            errors.append("'NSCL URI' is not a valid URI\n");
        }
        if (!checkUri(text_tpkDevUri)) {
            errors.append("'ThingPark Dev URI' is not a valid URI\n");
        }
        if (!checkPort(text_gsclCoapPort)) {
            errors.append("'GSCL CoAP port' is not a valid port number\n");
        }
        if (isGsclSelected()) {
            // if (!checkUri(text_acsUrl)) {
            // errors.append("'ACS URL' is not a valid URI\n");
            // }
            if (chckbx_proxy.isSelected() && !checkPort(text_proxyPort)) {
                errors.append("'HTTP proxy port' is not a valid port number\n");
            }
        }
        if (!checkModules()) {
            errors.append("You must install at least one module (GSCL or a GIP driver (ZigBee, Watteco, ...)\n");
        }

        if (errors.length() > 0) {
            JOptionPane.showMessageDialog(frame, errors.toString(), "Bad configuration", JOptionPane.ERROR_MESSAGE);
            return false;
        }
        return true;
    }

    private class SwingActionCancel extends AbstractAction {
        /**
         *
         */
        private static final long serialVersionUID = -71933448885154671L;

        public SwingActionCancel() {
            putValue(NAME, "Cancel");
            putValue(SHORT_DESCRIPTION, "Cancel the installation");
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            System.exit(0);
        }
    }

    private class SwingActionInstall extends AbstractAction {
        /**
         *
         */
        private static final long serialVersionUID = -2331539567658873737L;

        public SwingActionInstall() {
            putValue(NAME, "Install");
            putValue(SHORT_DESCRIPTION, "Start the installation");
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            if (checkConfiguration()) {
                try {
                    String ongName = "";
                    if (!chckbx_ongName.isSelected()) {
                        ongName = text_ongName.getText();
                    }
                    ArchConfig arch = InstallerUtil.getInstance().getArch();
                    ProductConfig product = InstallerUtil.getInstance().getProductFromLabel(
                            combo_boxProduct.getModel().getSelectedItem().toString());
                    List<ModuleConfig> modules = buildModules(arch, product);
                    OngConfig ongConfig = new OngConfig(text_sshHost.getText(), buildPort(text_sshPort, 22),
                            text_sshUser.getText(), arch, buildLogMaxSize(), text_rootAct.getText(), text_nsclUri.getText(),
                            text_tpkDevUri.getText(), text_gsclCoapHost.getText(), buildPort(text_gsclCoapPort, 5683),
                            buildBoolean(chckbx_dev, false), ongName, text_domainName.getText());
                    GsclConfig gsclConfig = new GsclConfig(/* text_acsUrl.getText(), */chckbx_proxy.isSelected(),
                            text_proxyHost.getText(), buildPort(text_proxyPort, 80));
                    ZigBeeConfig zigBeeConfig = new ZigBeeConfig();
                    WMbusConfig wMbusConfig = new WMbusConfig();
                    WattecoConfig wattecoConfig = new WattecoConfig();
                    KnxConfig knxConfig = new KnxConfig();
                    ModbusConfig modbusConfig = new ModbusConfig();
                    Iec61131Config iec61131Config = new Iec61131Config();
                    setAllEnabled(false);
                    installer = new RemoteOngInstaller(RemoteOngInstaller.REMOTE_ONG_INSTALL, frame, myJarFile, log,
                            ongVersion, ongConfig, gsclConfig, zigBeeConfig, wattecoConfig, wMbusConfig, knxConfig,
                            modbusConfig, iec61131Config, modules, false);
                    Thread installThread = new Thread(installer);
                    installThread.start();
                } catch (Exception ee) {
                    System.err.println("main: " + ee);
                    ee.printStackTrace(System.err);
                }
            }
        }
    }

    private class SwingActionFlashZbDongle extends AbstractAction {

        /**
         *
         */
        private static final long serialVersionUID = -1248910383826836288L;

        public SwingActionFlashZbDongle() {
            putValue(NAME, "Flash CC2531 dongle");
            putValue(SHORT_DESCRIPTION, "Flash TI CC2531 dongle");
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            if (checkConfiguration()) {
                try {
                    String ongName = "";
                    if (!chckbx_ongName.isSelected()) {
                        ongName = text_ongName.getText();
                    }
                    ArchConfig arch = InstallerUtil.getInstance().getArch();
                    ProductConfig product = InstallerUtil.getInstance().getProductFromLabel(
                            combo_boxProduct.getModel().getSelectedItem().toString());
                    List<ModuleConfig> modules = buildModules(arch, product);
                    OngConfig ongConfig = new OngConfig(text_sshHost.getText(), buildPort(text_sshPort, 22),
                            text_sshUser.getText(), arch, buildLogMaxSize(), text_rootAct.getText(), text_nsclUri.getText(),
                            text_tpkDevUri.getText(), text_gsclCoapHost.getText(), buildPort(text_gsclCoapPort, 5683),
                            buildBoolean(chckbx_dev, false), ongName, text_domainName.getText());
                    GsclConfig gsclConfig = new GsclConfig(/* text_acsUrl.getText(), */chckbx_proxy.isSelected(),
                            text_proxyHost.getText(), buildPort(text_proxyPort, 80));
                    ZigBeeConfig zigBeeConfig = new ZigBeeConfig();
                    WMbusConfig wMbusConfig = new WMbusConfig();
                    WattecoConfig wattecoConfig = new WattecoConfig();
                    KnxConfig knxConfig = new KnxConfig();
                    ModbusConfig modbusConfig = new ModbusConfig();
                    Iec61131Config iec61131Config = new Iec61131Config();
                    setAllEnabled(false);
                    installer = new RemoteOngInstaller(RemoteOngInstaller.REMOTE_ONG_FLASH_ZB_DONGLE, frame, myJarFile, log,
                            ongVersion, ongConfig, gsclConfig, zigBeeConfig, wattecoConfig, wMbusConfig, knxConfig,
                            modbusConfig, iec61131Config, modules, false);
                    Thread installThread = new Thread(installer);
                    installThread.start();
                } catch (Exception ee) {
                    System.err.println("main: " + ee);
                    ee.printStackTrace(System.err);
                }
            }
        }
    }

    private class SwingActionComboBoxArch extends AbstractAction {

        /**
         *
         */
        private static final long serialVersionUID = -7052228329885493497L;

        public SwingActionComboBoxArch() {
            putValue(NAME, "Architecture");
            putValue(SHORT_DESCRIPTION, "Targeted architecture");
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            // Perform action
            JComboBox cb = (JComboBox) e.getSource();

            // Determine status
            String flavor = cb.getModel().getSelectedItem().toString();
            ArchConfig arch = InstallerUtil.getInstance().getArch();
            String[] excludedModulesArray = arch.getExcludedModules();
            List<String> excludedModules = (excludedModulesArray != null) ? Arrays.asList(excludedModulesArray) : null;
            if (excludedModules != null) {
                if (excludedModules.contains(Modules.GSCL)) {
                    setEnableGscl(false, true);
                } else {
                    setEnableGscl(true, true);
                }
                if (excludedModules.contains(Modules.WATTECO)) {
                    setEnableWatteco(false, true);
                } else {
                    setEnableWatteco(true, true);
                }
                if (excludedModules.contains(Modules.ZIGBEE)) {
                    setEnableZigBee(false, true);
                } else {
                    setEnableZigBee(true, true);
                }
                if (excludedModules.contains(Modules.WMBUS)) {
                    setEnableWMbus(false, true);
                } else {
                    setEnableWMbus(true, true);
                }
                if (excludedModules.contains(Modules.KNX)) {
                    setEnableKnx(false, true);
                } else {
                    setEnableKnx(true, true);
                }
                if (excludedModules.contains(Modules.MODBUS)) {
                    setEnableModbus(false, true);
                } else {
                    setEnableModbus(true, true);
                }
                if (excludedModules.contains(Modules.IEC61131)) {
                    setEnableIec61131(false, true);
                } else {
                    setEnableIec61131(true, true);
                }
                if (excludedModules.contains(Modules.TCPDUMP)) {
                    setEnableTcpdump(false, true);
                } else {
                    setEnableTcpdump(true, true);
                }
            } else {
                setEnableGscl(true, true);
                setEnableWatteco(true, true);
                setEnableZigBee(true, true);
                setEnableWMbus(true, true);
                setEnableKnx(true, true);
                setEnableModbus(true, true);
                setEnableIec61131(true, true);
                setEnableTcpdump(true, true);
            }
            text_maxLogSizeDefault.setText(arch.getMaxLogSize());
        }
    }

    private class SwingActionChkZigBee extends AbstractAction {

        /**
         *
         */
        private static final long serialVersionUID = 8586483880758858308L;

        public SwingActionChkZigBee() {
            putValue(NAME, "ZigBee");
            putValue(SHORT_DESCRIPTION, "Install ZigBee driver");
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            // Perform action
            JCheckBox cb = (JCheckBox) e.getSource();

            // Determine status
            setEnableZigBee(cb.isSelected(), false);
        }
    }

    private class SwingActionChkWMbus extends AbstractAction {

        /**
         *
         */
        private static final long serialVersionUID = 8621217588682876934L;

        public SwingActionChkWMbus() {
            putValue(NAME, "WMbus");
            putValue(SHORT_DESCRIPTION, "Install WMbus driver");
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            // Perform action
            JCheckBox cb = (JCheckBox) e.getSource();

            // Determine status
            setEnableWMbus(cb.isSelected(), false);
        }
    }

    private class SwingActionChkKnx extends AbstractAction {

        /**
         *
         */
        private static final long serialVersionUID = -7680945654358026968L;

        public SwingActionChkKnx() {
            putValue(NAME, "KNX");
            putValue(SHORT_DESCRIPTION, "Install KNX driver");
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            // Perform action
            JCheckBox cb = (JCheckBox) e.getSource();

            // Determine status
            setEnableKnx(cb.isSelected(), false);
        }
    }

    private class SwingActionChkModbus extends AbstractAction {

        /**
         * 
         */
        private static final long serialVersionUID = 8974079216896960313L;

        public SwingActionChkModbus() {
            putValue(NAME, "Modbus");
            putValue(SHORT_DESCRIPTION, "Install Modbus driver");
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            // Perform action
            JCheckBox cb = (JCheckBox) e.getSource();

            // Determine status
            setEnableModbus(cb.isSelected(), false);
        }
    }

    private class SwingActionChkIec61131 extends AbstractAction {

        /**
         *
         */
        private static final long serialVersionUID = 1L;

        /**
         *
         */

        public SwingActionChkIec61131() {
            putValue(NAME, "IEC 61131");
            putValue(SHORT_DESCRIPTION, "Install IEC 61131 engine");
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            // Perform action
            JCheckBox cb = (JCheckBox) e.getSource();

            // Determine status
            setEnableIec61131(cb.isSelected(), false);
        }
    }

    private class SwingActionChkTcpdump extends AbstractAction {

        /**
         *
         */
        private static final long serialVersionUID = 8586483880758858308L;

        public SwingActionChkTcpdump() {
            putValue(NAME, "Tcpdump");
            putValue(SHORT_DESCRIPTION, "Install Tcpdump");
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            // Perform action
            JCheckBox cb = (JCheckBox) e.getSource();

            // Determine status
            setEnableTcpdump(cb.isSelected(), false);
        }
    }

    private class SwingActionChkWatteco extends AbstractAction {

        /**
         *
         */
        private static final long serialVersionUID = 5627374498238627481L;

        public SwingActionChkWatteco() {
            putValue(NAME, "Watteco");
            putValue(SHORT_DESCRIPTION, "Install Watteco driver");
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            // Perform action
            JCheckBox cb = (JCheckBox) e.getSource();

            // Determine status
            setEnableWatteco(cb.isSelected(), false);
        }
    }

    private class SwingActionChkGscl extends AbstractAction {

        /**
         *
         */
        private static final long serialVersionUID = 5187401671717865512L;

        public SwingActionChkGscl() {
            putValue(NAME, "GSCL");
            putValue(SHORT_DESCRIPTION, "Install ETSI GSCL framework");
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            // Perform action
            JCheckBox cb = (JCheckBox) e.getSource();

            // Determine status
            boolean selected = cb.isSelected();
            setEnableGscl(selected, false);
            text_gsclCoapHost.setEnabled(!selected);
            text_gsclCoapPort.setEnabled(!selected);
            if (selected) {
                text_gsclCoapHost.setText("::1");
                text_gsclCoapPort.setText("5683");
            }
        }
    }

    private class SwingActionChkOngName extends AbstractAction {

        /**
         *
         */
        private static final long serialVersionUID = -6451608445099549406L;

        public SwingActionChkOngName() {
            putValue(NAME, "Auto-detect ONG name");
            putValue(SHORT_DESCRIPTION, "Change ONG Name");
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            // Perform action
            JCheckBox cb = (JCheckBox) e.getSource();

            // Determine status
            boolean isSel = cb.isSelected();
            if (isSel) {
                // The checkbox is now selected
                text_ongName.setText("<auto-detected>");
                text_ongName.setEnabled(false);
            } else {
                // The checkbox is now deselected
                text_ongName.setEnabled(true);
                text_ongName.setText("");
            }
        }
    }

    private class SwingActionChkProxy extends AbstractAction {

        /**
         *
         */
        private static final long serialVersionUID = 7395521841617013967L;

        public SwingActionChkProxy() {
            putValue(NAME, "HTTP proxy host & port");
            putValue(SHORT_DESCRIPTION, "Use a HTTP proxy");
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            // Perform action
            JCheckBox cb = (JCheckBox) e.getSource();

            // Determine status
            boolean isSel = cb.isSelected();
            if (isSel) {
                // The checkbox is now selected
                text_proxyHost.setEnabled(true);
                text_proxyPort.setEnabled(true);
            } else {
                // The checkbox is now deselected
                text_proxyHost.setText("");
                text_proxyPort.setText("");
                text_proxyHost.setEnabled(false);
                text_proxyPort.setEnabled(false);
            }
        }
    }

    private class MyCanvas extends Canvas {
        /**
         *
         */
        private static final long serialVersionUID = 3037720514750623419L;
        Image img;

        public MyCanvas(Image img) {
            super();
            this.img = img;
        }

        @Override
        public void paint(Graphics g) {
            if (img != null) {
                g.drawImage(img, 0, 0, this.getWidth(), this.getHeight(), this);
            }
        }
    }
}
