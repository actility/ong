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
 * id $Id: MyUserInfo.java 1698 2012-01-23 13:41:34Z MLouiset $
 * author $Author: MLouiset $
 * version $Revision: 1698 $
 * lastrevision $Date: 2012-01-23 14:41:34 +0100 (Mon, 23 Jan 2012) $
 * modifiedby $LastChangedBy: MLouiset $
 * lastmodified $LastChangedDate: 2012-01-23 14:41:34 +0100 (Mon, 23 Jan 2012) $
 */
package com.actility.ong.installer;

import java.awt.Container;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Insets;

import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPasswordField;
import javax.swing.JTextField;

import com.jcraft.jsch.UIKeyboardInteractive;
import com.jcraft.jsch.UserInfo;

/**
 * This object handle interactions with user
 * 
 */
public class MyUserInfo implements UserInfo, UIKeyboardInteractive {

    private String passwd;
    private JTextField passwordField = new JPasswordField(20);
    private final GridBagConstraints gbc = new GridBagConstraints(0, 0, 1, 1, 1, 1, GridBagConstraints.NORTHWEST,
            GridBagConstraints.NONE, new Insets(0, 0, 0, 0), 0, 0);
    private Container panel;
    private JFrame mainFrame;

    public MyUserInfo(JFrame mainFrame) {
        super();
        this.mainFrame = mainFrame;
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.jcraft.jsch.UserInfo#getPassword()
     */
    public String getPassword() {
        return passwd;
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.jcraft.jsch.UserInfo#promptYesNo(java.lang.String)
     */
    public boolean promptYesNo(String str) {
        Object[] options = { "yes", "no" };
        int foo = JOptionPane.showOptionDialog(mainFrame, str, "Warning", JOptionPane.DEFAULT_OPTION,
                JOptionPane.WARNING_MESSAGE, null, options, options[0]);
        return foo == 0;
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.jcraft.jsch.UserInfo#getPassphrase()
     */
    public String getPassphrase() {
        return null;
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.jcraft.jsch.UserInfo#promptPassphrase(java.lang.String)
     */
    public boolean promptPassphrase(String message) {
        return true;
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.jcraft.jsch.UserInfo#promptPassword(java.lang.String)
     */
    public boolean promptPassword(String message) {
        Object[] ob = { passwordField };
        int result = JOptionPane.showConfirmDialog(mainFrame, ob, message, JOptionPane.OK_CANCEL_OPTION);
        if (result == JOptionPane.OK_OPTION) {
            passwd = passwordField.getText();
            return true;
        } else {
            return false;
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.jcraft.jsch.UserInfo#showMessage(java.lang.String)
     */
    public void showMessage(String message) {
        JOptionPane.showMessageDialog(null, message);
    }

    /*
     * (non-Javadoc)
     * 
     * @see com.jcraft.jsch.UIKeyboardInteractive#promptKeyboardInteractive(java.lang.String, java.lang.String,
     * java.lang.String, java.lang.String[], boolean[])
     */
    public String[] promptKeyboardInteractive(String destination, String name, String instruction, String[] prompt,
            boolean[] echo) {
        panel = new JPanel();
        panel.setLayout(new GridBagLayout());

        gbc.weightx = 1.0;
        gbc.gridwidth = GridBagConstraints.REMAINDER;
        gbc.gridx = 0;
        panel.add(new JLabel(instruction), gbc);
        gbc.gridy++;

        gbc.gridwidth = GridBagConstraints.RELATIVE;

        JTextField[] texts = new JTextField[prompt.length];
        for (int i = 0; i < prompt.length; i++) {
            gbc.fill = GridBagConstraints.NONE;
            gbc.gridx = 0;
            gbc.weightx = 1;
            panel.add(new JLabel(prompt[i]), gbc);

            gbc.gridx = 1;
            gbc.fill = GridBagConstraints.HORIZONTAL;
            gbc.weighty = 1;
            if (echo[i]) {
                texts[i] = new JTextField(20);
            } else {
                texts[i] = new JPasswordField(20);
            }
            panel.add(texts[i], gbc);
            gbc.gridy++;
        }

        if (JOptionPane.showConfirmDialog(mainFrame, panel, destination + ": " + name, JOptionPane.OK_CANCEL_OPTION,
                JOptionPane.QUESTION_MESSAGE) == JOptionPane.OK_OPTION) {
            String[] response = new String[prompt.length];
            for (int i = 0; i < prompt.length; i++) {
                response[i] = texts[i].getText();
            }
            return response;
        } else {
            return null; // cancel
        }
    }
}
