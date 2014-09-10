#!/bin/bash

#
# Copyright   Actility, SA. All Rights Reserved.
# DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License version
# 2 only, as published by the Free Software Foundation.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License version 2 for more details (a copy is
# included at /legal/license.txt).
# 
# You should have received a copy of the GNU General Public License
# version 2 along with this work; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
# 02110-1301 USA
# 
# Please contact Actility, SA.,  4, rue Ampere 22300 LANNION FRANCE
# or visit www.actility.com if you need additional
# information or have any questions.
# 
# id $Id$
# author $Author$
# version $Revision$
# lastrevision $Date$
# modifiedby $LastChangedBy$
# lastmodified $LastChangedDate$
#

# This file is intended to be invoked from your project source (e.g. from $ROOTACT/zigbee).
# 
# It brings utility functions to retrieve dependencies, built APUs 
# and other such deployment stuffs.
# freely inspired from this article:
# http://www.sonatype.com/people/2011/01/downloading-artifacts-from-nexus-with-bash/
#
# Here are the few requirements to make it work:
# 1/ ROOTACT must be defined
# 2/ all APU dependencies must be depicted in ./apu/dependencies/*.cfg file
#    with one .cfg file per dependency. It supports two type of dependencies (gave with
#    DEPENDENCY_TYPE variable):
#    - "repository": indicates the dependency is to be retrieved from Nexus server, and
#      than the *.cfg file MUST define the :
#      o GROUP_ID (e.g. com.actility)
#      o ARTIFACT_ID (e.g. rtbase-devel)
#      o VERSION (e.g. 1.0.15-0)
#      o REPO (e.g. releases)
#      o PACKAGING (e.g. apu)
#      o WITH_ARCH (can be 0: architecture not specififed, 1: target architecture is 
#                   required or 2: the architecture is forced to the one of the machine 
#                   used for compilation)
#      o INCLUDE_ARCH (It indicates the targeted architectures for which the dependency is
#                      needed. If not present and by default, apu-maker assumes that all
#                      architectures are in the included archs. If set, is a
#                      space-separated list of the concerned architectures 
#                      (e.g.: "INCLUDE_ARCH=centos6-x86 cov2" indicates that the entry is 
#                      a dependency for centos6-x86 and cov2, but this is not true for the
#                      other supported arch such as cov1...))
#    - "source": indicates the dependency is made of source; they are then copied into the
#      build path, and compilation is attempted.
#    Both also needs variable "TARGET_DIR", with is the targeted folder name to use in the 
#    build directory.
# 3/ one can build several APU from the same project. For instance for xo project:
#    - the APU so called "xo" provides the installation for the ONG runtime,
#    - the APU so called "xo-devel" provides the installation for development purpose
#    (headers and library cross-compiled for the target),
#    - the APU so called "xo-build" provides the installation for generation purpose
#    (utils for development and compilation to be ran on the machine used for compilation.
#    - the APU so called "xo-src" provides the source code.
#    Each package is defined in ./apu/package_* directory by providing the following files:
#    - "control" for the APU generation. This file has the look and feel of a manifest file.
#    - "postinst" file for APU post installation phase. This file contains the Linux 
#      commands to be run after APU installation (e.g. create symbolic link, give execution
#      rights to files, create folders, do text replacement, ...)
#      This file is optional.
#    - "apu.sh" file that must provide the shell implementation for the following functions:
#      o buildProject (and $1 is then the targeted plateform (e.g. cov2))
#      o buildApuImage (and $1 is then the APU package sub-path currently processed)
#      o projectGroupId
#      o projectName
#      o projectVersion
#      o projectApuRev

apu-make $*

