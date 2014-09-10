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
 * id $Id: MyProgressMonitor.java 3387 2012-10-23 11:27:55Z JReich $
 * author $Author: JReich $
 * version $Revision: 3387 $
 * lastrevision $Date: 2012-10-23 13:27:55 +0200 (Tue, 23 Oct 2012) $
 * modifiedby $LastChangedBy: JReich $
 * lastmodified $LastChangedDate: 2012-10-23 13:27:55 +0200 (Tue, 23 Oct 2012) $
 */
package com.actility.ong.installer;

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.ProgressMonitor;
import javax.swing.SwingUtilities;
import javax.swing.Timer;

public class MyProgressMonitor extends ProgressMonitor implements ActionListener {

    private int lastCollectedProgress;
    private boolean indeterminateMode;
    private static int indeterminateLevel;
    private static final int indeterminateDeltaConst = 9;
    private static int indeterminateDeltaProgress = indeterminateDeltaConst;

    /**
     * Constructor
     *
     * @param remoteOngInstaller
     *
     * @param parentComponent
     * @param message
     * @param max
     */
    public MyProgressMonitor(Component parentComponent, String message) {
        super(parentComponent, message, "", 0, 100);
        setMillisToDecideToPopup(100);
        setMillisToPopup(100);
        setProgress(0);
        lastCollectedProgress = 0;
        indeterminateMode = false;

        // Fire a timer every once in a while to update the progress.
        Timer timer = new Timer(500, this);
        timer.start();

    }

    public void setCurrentProgress(int lastCollectedProgress) {
        this.lastCollectedProgress = lastCollectedProgress;
    }

    public void setIndeterminate(boolean indeterminate) {
        indeterminateMode = indeterminate;
        indeterminateLevel = 0;
    }

    @Override
    public void actionPerformed(ActionEvent e) {
        // Invoked by the timer every half second. Simply place
        // the progress monitor update on the event queue.
        SwingUtilities.invokeLater(new Runnable() {
            @Override
            public void run() {
                if (isCanceled()) {
                    close();
                    System.exit(1);
                } else if (!indeterminateMode) {
                    String message = String.format("Completed %d%%\n", lastCollectedProgress);
                    setNote(message);
                    setProgress(lastCollectedProgress);
                } else {
                    indeterminateLevel += indeterminateDeltaProgress;
                    if (indeterminateLevel >= 99) {
                        indeterminateDeltaProgress = -indeterminateDeltaConst;
                        indeterminateLevel = 99; // mustn't be set to 100 otherwise the progress bar will close.
                    }
                    if (indeterminateLevel <= 0) {
                        indeterminateDeltaProgress = indeterminateDeltaConst;
                        indeterminateLevel = 0;
                    }
                    // setNote("Indeterminate end\n");
                    setProgress(indeterminateLevel);
                }
            }
        });
    }
}
