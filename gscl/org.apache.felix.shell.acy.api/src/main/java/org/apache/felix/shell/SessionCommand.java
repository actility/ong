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
package org.apache.felix.shell;

import java.io.PrintStream;

public interface SessionCommand extends Command {
    /**
     * Executes the command using the supplied command line, output
     * print stream, and error print stream.
     * @param line the complete command line, including the command name.
     * @param out the print stream to use for standard output.
     * @param err the print stream to use for standard error.
    **/
    public void execute(Session session, String line, PrintStream out, PrintStream err);
}
