
PHONEME_DEBUG=0

applyPatch()
{
  echo -n "Apply $1..."
  patch -p1 -i $1 || (cd - && echo "FAILED." && exit 1)
  echo "OK"
}

buildProject()
{
  ROOTACT=$(pwd)

  BUILD_DIR=$ROOTACT/phoneme-advanced-mr2/build/
  TARGET_DIR=$BUILD_DIR/target

  rm -rf $BUILD_DIR
  rm -rf $TARGET_DIR
  mkdir -p $BUILD_DIR
  mkdir -p $TARGET_DIR
 
  unset AS
  unset AR
  unset M4
  unset NM
  unset GDB
  unset CPP
  unset LD
  unset STRIP
  unset CXX
  unset OBJCOPY
  unset RANLIB
  unset OBJDUMP
  unset CC
 
  cd phoneme-advanced-mr2/
  if [ ! -d build/phoneme-advanced-mr2 ]; then
    tar zxvf phoneme_advanced_mr2_b167.tar.gz -C build/
    tar xzvf jdbc.tar.gz -C build/phoneme_advanced_mr2_b167
  fi
  cd build/
  applyPatch $ROOTACT/phoneme-advanced-mr2/patches/usr2-thread-dump.patch
  applyPatch $ROOTACT/phoneme-advanced-mr2/patches/replace-sigstop-by-backtrace.patch
  applyPatch $ROOTACT/phoneme-advanced-mr2/patches/europe-zone.patch
  applyPatch $ROOTACT/phoneme-advanced-mr2/patches/dns-cache-ttl.patch
  applyPatch $ROOTACT/phoneme-advanced-mr2/patches/fix-URL-handling.patch
  applyPatch $ROOTACT/phoneme-advanced-mr2/patches/remove-debug-loadlibrary.patch
  case "$1" in 
    centos6-x86|centos6-x86_64)
      COMPILE_DIR=$BUILD_DIR/phoneme_advanced_mr2_b167/cdc/build/linux-x86-generic
      PHONEME_DEBUG=1
      echo -n "Apply patch to linux-x86-generic GNUmakefile..."
      sed -i "s|^CC_ARCH_FLAGS[ \t]*=\(.*\)$|CC_ARCH_FLAGS = -m32 \1|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^ASM_ARCH_FLAGS[ \t]*=\(.*\)$|ASM_ARCH_FLAGS = -m32 \1|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^LINK_ARCH_FLAGS[ \t]*=\(.*\)$|LINK_ARCH_FLAGS = -m32 \1|" $COMPILE_DIR/GNUmakefile
      echo "OK"
      ;;
    cov1)
      COMPILE_DIR=$BUILD_DIR/phoneme_advanced_mr2_b167/cdc/build/linux-arm-generic
      echo -n "Apply patch to linux-arm-generic GNUmakefile..."
      sed -i "s|^CC_ARCH_FLAGS[ \t]*=.*$|CC_ARCH_FLAGS = -mcpu=arm920t|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^CVM_TARGET_TOOLS_PREFIX[ \t]*=.*$|CVM_TARGET_TOOLS_PREFIX = arm-linux-|" $COMPILE_DIR/GNUmakefile
      echo "OK"
      ;;
    cov2)
      COMPILE_DIR=$BUILD_DIR/phoneme_advanced_mr2_b167/cdc/build/linux-arm-generic
      echo -n "Apply patch to linux-arm-generic GNUmakefile..."
      sed -i "s|^CC_ARCH_FLAGS[ \t]*=.*$|CC_ARCH_FLAGS = -mcpu=arm920t|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^ASM_ARCH_FLAGS[ \t]*=.*$|ASM_ARCH_FLAGS = -mcpu=arm920t|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^LINK_ARCH_FLAGS[ \t]*=.*$|LINK_ARCH_FLAGS = -mcpu=arm920t|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^USE_AAPCS[ \t]*?=.*$|USE_AAPCS ?= true|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^CVM_TARGET_TOOLS_PREFIX[ \t]*=.*$|CVM_TARGET_TOOLS_PREFIX = arm-none-linux-gnueabi-|" $COMPILE_DIR/GNUmakefile
      echo "OK"
      ;;
    lpv3)
      applyPatch $ROOTACT/phoneme-advanced-mr2/patches/remove-backtrace.patch
      applyPatch $ROOTACT/phoneme-advanced-mr2/patches/uclibc-sigignore.patch

      COMPILE_DIR=$BUILD_DIR/phoneme_advanced_mr2_b167/cdc/build/linux-mips-openwrt
      echo -n "Apply patch to linux-mips-openwrt GNUmakefile..."
      sed -i "s|^\t/opt/OpenWRT/toolchain-mipsel_gcc4.1.2/bin/mipsel-linux-uclibc-.*$|mips-openwrt-linux-uclibc-|" $COMPILE_DIR/GNUmakefile
      echo "OK"
      ;;
    angstrom_panrouter)
      COMPILE_DIR=$BUILD_DIR/phoneme_advanced_mr2_b167/cdc/build/linux-arm-generic
      echo -n "Apply patch to linux-arm-generic GNUmakefile..."
      sed -i "s|^CC_ARCH_FLAGS[ \t]*=.*$|CC_ARCH_FLAGS = -mcpu=cortex-a8|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^ASM_ARCH_FLAGS[ \t]*=.*$|ASM_ARCH_FLAGS = -mcpu=cortex-a8|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^LINK_ARCH_FLAGS[ \t]*=.*$|LINK_ARCH_FLAGS = -mcpu=cortex-a8|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^USE_AAPCS[ \t]*?=.*$|USE_AAPCS ?= true|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^CVM_TARGET_TOOLS_PREFIX[ \t]*=.*$|CVM_TARGET_TOOLS_PREFIX = arm-none-linux-gnueabi-|" $COMPILE_DIR/GNUmakefile
      echo "OK"
      ;;
    android_panrouter)
      applyPatch $ROOTACT/phoneme-advanced-mr2/patches/remove-backtrace.patch
      applyPatch $ROOTACT/phoneme-advanced-mr2/patches/android.patch
      #applyPatch $ROOTACT/phoneme-advanced-mr2/patches/uclibc-sigignore.patch
      #applyPatch $ROOTACT/phoneme-advanced-mr2/patches/remove-langinfo.patch
      #applyPatch $ROOTACT/phoneme-advanced-mr2/patches/fix-destroy-zip.patch
      #applyPatch $ROOTACT/phoneme-advanced-mr2/patches/fix-gethostbyaddr.patch
      #applyPatch $ROOTACT/phoneme-advanced-mr2/patches/fix-ioctls.patch
      #applyPatch $ROOTACT/phoneme-advanced-mr2/patches/fix-ipv6-flags.patch
      #applyPatch $ROOTACT/phoneme-advanced-mr2/patches/fix-sigcontext.patch
      #applyPatch $ROOTACT/phoneme-advanced-mr2/patches/fix-stat.patch
      #applyPatch $ROOTACT/phoneme-advanced-mr2/patches/fix-wait.patch
      #applyPatch $ROOTACT/phoneme-advanced-mr2/patches/replace-pthread-with-z.patch

      COMPILE_DIR=$BUILD_DIR/phoneme_advanced_mr2_b167/cdc/build/linux-arm-generic
      echo -n "Apply patch to linux-arm-generic GNUmakefile..."
      sed -i "s|^CC_ARCH_FLAGS[ \t]*=.*$|CC_ARCH_FLAGS = |" $COMPILE_DIR/GNUmakefile
      sed -i "s|^ASM_ARCH_FLAGS[ \t]*=.*$|ASM_ARCH_FLAGS = |" $COMPILE_DIR/GNUmakefile
      sed -i "s|^LINK_ARCH_FLAGS[ \t]*=.*$|LINK_ARCH_FLAGS = |" $COMPILE_DIR/GNUmakefile
      sed -i "s|^USE_AAPCS[ \t]*?=.*$|USE_AAPCS ?= true|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^CVM_TARGET_TOOLS_PREFIX[ \t]*=.*$|CVM_TARGET_TOOLS_PREFIX = arm-android-gnueabi-|" $COMPILE_DIR/GNUmakefile
      echo "OK"
      ;;
    rpib)
      COMPILE_DIR=$BUILD_DIR/phoneme_advanced_mr2_b167/cdc/build/linux-arm-generic
      echo -n "Apply patch to linux-arm-generic GNUmakefile..."
      sed -i "s|^CC_ARCH_FLAGS[ \t]*=.*$|CC_ARCH_FLAGS = -march=armv6 -mthumb-interwork -O2 -pipe -g -feliminate-unused-debug-types -I$TOOLCHAIN_HOME/sysroots/armv6-vfp-poky-linux-gnueabi/usr/include --sysroot=$TOOLCHAIN_HOME/sysroots/armv6-vfp-poky-linux-gnueabi/|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^ASM_ARCH_FLAGS[ \t]*=.*$|ASM_ARCH_FLAGS = -march=armv6 -mthumb-interwork|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^LINK_ARCH_FLAGS[ \t]*=.*$|LINK_ARCH_FLAGS = -Wl,-O1 -Wl,--hash-style=gnu -Wl,--as-needed --sysroot=$TOOLCHAIN_HOME/sysroots/armv6-vfp-poky-linux-gnueabi/|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^CVM_JIT_USE_FP_HARDWARE[ \t]*?=.*$|CVM_JIT_USE_FP_HARDWARE = true|" $COMPILE_DIR/GNUmakefile
      sed -i "s|^CVM_TARGET_TOOLS_PREFIX[ \t]*=.*$|CVM_TARGET_TOOLS_PREFIX = arm-poky-linux-gnueabi-|" $COMPILE_DIR/GNUmakefile
      echo "OK"
      ;;
    ntc6200)
        COMPILE_DIR=$BUILD_DIR/phoneme_advanced_mr2_b167/cdc/build/linux-arm-generic
        echo -n "Apply patch to linux-arm-generic GNUmakefile..."
        sed -i "s|^CC_ARCH_FLAGS[ \t]*=.*$|CC_ARCH_FLAGS = -mcpu=arm926ej-s|" $COMPILE_DIR/GNUmakefile
        sed -i "s|^ASM_ARCH_FLAGS[ \t]*=.*$|ASM_ARCH_FLAGS = -mcpu=arm926ej-s|" $COMPILE_DIR/GNUmakefile
        sed -i "s|^LINK_ARCH_FLAGS[ \t]*=.*$|LINK_ARCH_FLAGS = -mcpu=arm926ej-s|" $COMPILE_DIR/GNUmakefile
        sed -i "s|^USE_AAPCS[ \t]*?=.*$|USE_AAPCS ?= true|" $COMPILE_DIR/GNUmakefile
        sed -i "s|^CVM_TARGET_TOOLS_PREFIX[ \t]*=.*$|CVM_TARGET_TOOLS_PREFIX = arm-cdcs-linux-gnueabi-|" $COMPILE_DIR/GNUmakefile
        echo "OK"
      ;;
    *)
      echo "Unsupported target"
      exit 1
      ;;
  esac

  cd $COMPILE_DIR
  if [ $PHONEME_DEBUG = 1 ]; then
    if ! make -r J2ME_CLASSLIB=foundation USE_JDBC=true CVM_INSPECTOR=true CVM_JVMTI=true CVM_DEBUG=true ; then
      echo "FAILED to compile"
      exit 1
    fi
  else
    if ! make -r J2ME_CLASSLIB=foundation USE_JDBC=true CVM_INSPECTOR=true ; then
      echo "FAILED to compile"
      exit 1
    fi
  fi

  echo -n "Package project into $TARGET_DIR..."
  cp -r $COMPILE_DIR/bin $TARGET_DIR  || (echo "FAILED" && exit 1)
  cp -r $COMPILE_DIR/lib $TARGET_DIR || (echo "FAILED" && exit 1)
  cp $COMPILE_DIR/btclasses.zip $TARGET_DIR || (echo "FAILED" && exit 1)
  cp $COMPILE_DIR/testclasses.zip $TARGET_DIR || (echo "FAILED" && exit 1)
  echo "OK"

  return 0
}

buildApuImage()
{
  ROOTACT=`pwd` && \
  BUILD_DIR=$ROOTACT/phoneme-advanced-mr2/build/ && \
  TARGET_DIR=$BUILD_DIR/target && \
  rm -rf apu && \
  mkdir -p apu/data/bin/phoneme && \
  mkdir -p apu/control && \
  cp -r $TARGET_DIR/* apu/data/bin/phoneme/ && \
  cp phoneme-advanced-mr2/apu/$1/postinst apu/control/postinst 
}

projectGroupId()
{
  echo "com.sun"
}

projectName()
{
  echo "phoneme-advanced-mr2"
}

projectVersion()
{
  cat phoneme-advanced-mr2/Version
}

projectApuRev()
{
  cat phoneme-advanced-mr2/apu/revision
}

projectDescription()
{
  echo "CDC Java Virtual Machine for Micro Edition"
}

projectArchDepend()
{
  # yes, need arch for building this apu
  return 0
}


