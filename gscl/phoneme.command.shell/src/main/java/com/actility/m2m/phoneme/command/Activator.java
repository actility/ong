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
 * id $Id: Activator.java 6307 2013-11-05 12:59:55Z mlouiset $
 * author $Author: mlouiset $
 * version $Revision: 6307 $
 * lastrevision $Date: 2013-11-05 13:59:55 +0100 (Tue, 05 Nov 2013) $
 * modifiedby $LastChangedBy: mlouiset $
 * lastmodified $LastChangedDate: 2013-11-05 13:59:55 +0100 (Tue, 05 Nov 2013) $
 */

package com.actility.m2m.phoneme.command;

import org.apache.felix.shell.Command;
import org.osgi.framework.BundleActivator;
import org.osgi.framework.BundleContext;
import org.osgi.framework.ServiceRegistration;

public class Activator implements BundleActivator {
    private ServiceRegistration dumpAllThreadsRegistration;
    private ServiceRegistration dumpHeapSimpleRegistration;
    private ServiceRegistration dumpHeapStatsRegistration;
    private ServiceRegistration dumpHeapVerboseRegistration;
    private ServiceRegistration dumpStackRegistration;
    private ServiceRegistration dumpSysInfoRegistration;
    private ServiceRegistration listAllThreadsRegistration;

    public void start(BundleContext context) throws Exception {
        dumpAllThreadsRegistration = context.registerService(Command.class.getName(), new DumpAllThreadsCommand(),
                null);
        dumpHeapSimpleRegistration = context.registerService(Command.class.getName(), new DumpHeapSimpleCommand(),
                null);
        dumpHeapStatsRegistration = context.registerService(Command.class.getName(), new DumpHeapStatsCommand(),
                null);
        dumpHeapVerboseRegistration = context.registerService(Command.class.getName(), new DumpHeapVerboseCommand(), null);
        dumpStackRegistration = context.registerService(Command.class.getName(), new DumpStackCommand(), null);
        dumpSysInfoRegistration = context.registerService(Command.class.getName(), new DumpSysInfoCommand(), null);
        listAllThreadsRegistration = context.registerService(Command.class.getName(), new ListAllThreadsCommand(), null);
    }

    public void stop(BundleContext context) throws Exception {
        dumpAllThreadsRegistration.unregister();
        dumpHeapSimpleRegistration.unregister();
        dumpHeapStatsRegistration.unregister();
        dumpHeapVerboseRegistration.unregister();
        dumpStackRegistration.unregister();
        dumpSysInfoRegistration.unregister();
        listAllThreadsRegistration.unregister();
    }

}
