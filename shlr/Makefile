include ../global.mk
include ../config-user.mk
# for EXT_EXE
include ../libr/config.mk
include ../mk/platform.mk
_INCLUDE_MK_GCC_=
include ../mk/${COMPILER}.mk

# Build against last capstone release or next
CS_RELEASE=0

GIT?=git
WGET?=wget

ifeq ($(CS_RELEASE),1)
CS_VER=3.0.4
CS_TAR=https://codeload.github.com/aquynh/capstone/tar.gz/$(CS_VER)
#CS_TAR=http://capstone-engine.org/download/$(CS_VER)/capstone-$(CS_VER).tgz
CS_PATCHES=0
else
CS_TAR=
CS_URL=$(GIT_PREFIX)github.com/aquynh/capstone.git
CS_UPD=20160531
CS_BRA=next
CS_TIP=1f3877da0190219395dc3154e59b4c6dda6af9c9
# REVERT THIS COMMIT BECAUSE ITS WRONG
CS_REV=
CS_PATCHES=1
endif

.PHONY: capstone-sync capstone-build all clean mrproper libgdbr libwind bochs

HOST_CC?=gcc
SHLR?=$(shell pwd)
AR?=ar
RANLIB?=ranlib
MODS=sdb zip udis86 java tcc
ifeq (1,$(WITH_GPL))
MODS+=grub
endif
MODS+=gdb
MODS+=qnx
SDB_URL=git://github.com/radare/sdb
#SDB_URL=/Users/pancake/prg/sdb
PWD=$(shell pwd)
SDB_CONFIG=${PWD}/sdb-config.mk

CFLAGS_SHARED=${PIC_CFLAGS}

ifeq (${EXT_SO},)
all:
	echo EXT_SO not defined
	exit 1
endif

preall: targets libwind capstone-build bochs
	@for MOD in ${MODS} ; do \
		echo $(MAKE) -C $$MOD ; \
		$(MAKE) -C $$MOD HAVE_VALA= ROOT="${PWD}/../" CC="${CC}" ; \
	done

SDBFILES=sdb/src/.sdb${EXT_EXE} sdb/sdb${EXT_EXE} sdb/src/.sdb sdb/sdb
PREMODS=capstone gdb wind sdb bochs

clean mrproper:
	rm -f ${SDBFILES}
	@for MOD in ${PREMODS} ${MODS} ; do $(MAKE) -C $$MOD clean ; done

ifeq ($(BUILD_OS),windows)
BUILD_EXT_EXE=.exe
else
BUILD_EXT_EXE=
endif

SDB_TARGETS=$(SDBLIB)
ifneq (${EXT_EXE},)
SDB_TARGETS+=sdb/sdb${EXT_EXE}
else
SDB_TARGETS+=sdb/sdb
endif

ifeq ($(filter sdb/sdb${BUILD_EXT_EXE},$(SDB_TARGETS)),)
SDB_TARGETS+=sdb/sdb${BUILD_EXT_EXE}
endif

targets:
	for TARGET in ${SDB_TARGETS} ; do ${MAKE} $$TARGET ; done

${SDB_TARGETS}:
	@echo
	@echo BUILD SUMARY
	@echo ============
	@echo COMPILER ${COMPILER}
	@echo CC ${CC}
	@echo HOST_CC ${HOST_CC}
	@echo HOST_OS ${HOST_OS}
	@echo BUILD_OS ${BUILD_OS}
	@echo ============
	@echo
	@echo ">>>>>>>>>>>>>>>>"
	@echo "NATIVE BUILD SDB"
	@echo ">>>>>>>>>>>>>>>>"
	@echo
	$(MAKE) -C sdb clean
	$(MAKE) -C sdb src/sdb_version.h
	$(MAKE) -C sdb/src CC=${HOST_CC} CFLAGS='${HOST_CFLAGS} -fPIC' bin
	cp -f sdb/src/sdb${BUILD_EXT_EXE} sdb/src/.sdb${BUILD_EXT_EXE}
	@echo
	@echo ">>>>>>>>>>>>>>>>"
	@echo "TARGET BUILD SDB"
	@echo ">>>>>>>>>>>>>>>>"
	@echo
	$(MAKE) -C sdb clean
	cd sdb ; $(MAKE) src/sdb_version.h
	cd sdb/src ; $(MAKE) ARCH=xxx RANLIB="${RANLIB}" CFLAGS_SHARED="${CFLAGS_SHARED}" \
		CC="${CC}" AR="${AR}" CFLAGS="${CFLAGS}" LDFLAGS="${LDFLAGS}" libsdb.a
	${RANLIB} sdb/src/libsdb.a
ifneq (${EXT_AR},a)
	-cp -f sdb/src/libsdb.a sdb/src/libsdb.${EXT_AR}
endif
	rm -f $@
	cp -f sdb/src/.sdb${BUILD_EXT_EXE} $@

.PHONY: sdb-sync sync-sdb sdbclean
F=README.md config.mk src Makefile Jamroot
SYNCFILES=$(addprefix sdb.vc/,${F})
I=../libr/include

libgdbr:
	$(MAKE) -C gdb all

libwind:
	$(MAKE) -C wind all

bochs:
	$(MAKE) -C bochs all

sdb-clean clean-sdb:
	$(MAKE) -C sdb clean
	rm -f sdb/sdb.exe
	rm -f sdb/src/libsdb.a

sdb-sync sync-sdb:
	rm -rf sdb sdb.vc
	git clone --depth 1 ${SDB_URL} sdb.vc
	mkdir -p sdb
	cp -rf ${SYNCFILES} sdb
	rm -rf sdb.vc $I/sdb
	mkdir -p $I/sdb
	rm -f src/sdb_version.h
	cd sdb ; $(MAKE) src/sdb_version.h
	cp -f sdb/src/*.h $I/sdb
	echo '#include <sdb/sdb.h>' > $I/sdb.h
	mkdir -p sdb/test sdb/memcache
	sed -e 's,HAVE_VALA=,HAVE_VALA=#,' sdb/config.mk > .t
	mv .t sdb/config.mk
	echo all clean mrproper: | tee sdb/test/Makefile > sdb/memcache/Makefile
	git add $I/sdb*
	git add sdb

$(TCCLIB) libr_tcc/libr_tcc.a:
	cd libr_tcc ; $(MAKE)

udis-sync udis86-sync sync-udis sync-udis86:
	cd udis86 ; $(MAKE) sync

tcc-clean tccclean:
	cd libr_tcc ; $(MAKE) clean

CFILES=i386-asm.c i386-gen.c libtcc.c tccasm.c tccelf.c tccgen.c tccpp.c
HFILES=tcc.h i386-asm.h tcclib.h tcctok.h stab.h
HFILES+=elf.h config.h i386-tok.h

# TODO: use mob branch?

.PHONY: sdb-sync sync-sdb sdbclean
tcc-sync sync-tcc:
	rm -rf _
	$(GIT) clone --depth 1 git://repo.or.cz/tinycc.git _
	cd _ ; ./configure --prefix=${PREFIX}
	mkdir -p tcc
	for FILE in ${CFILES} ${HFILES} ; do cp -f _/$$FILE tcc ; done
	cp -f _/VERSION _/COPYING _/stab.def tcc
	$(GIT) add tcc
	rm -rf _

install:
	#mkdir -p ${DESTDIR}${LIBDIR}
	#cp -f tcc/libr_tcc* ${DESTDIR}${LIBDIR}
	#cp -f zip/libr_z* ${DESTDIR}${LIBDIR}

install-symlink symstall:
	mkdir -p "${DESTDIR}${LIBDIR}"
	#cd tcc ; for FILE in libr_tcc* ; do \
	#	ln -fs `pwd`/$$FILE ${DESTDIR}${LIBDIR}/$$FILE ; done

uninstall deinstall:
	#rm -f ${DESTDIR}${LIBDIR}/libr_tcc*
	#rm -f ${DESTDIR}${LIBDIR}/libr_z*

ifeq ($(USE_CAPSTONE),1)
# Do nothing here
capstone:

capstone-sync:

capstone-build:

else

ifeq ($(CS_RELEASE),1)
capstone-sync: capstone

capstone-clean:
	cd capstone ; $(MAKE) clean

capstone: capstone-$(CS_VER).tar.gz
	tar xzvf capstone-$(CS_VER).tar.gz
	rm -rf capstone
	mv capstone-$(CS_VER) capstone
ifeq ($(CS_PATCHES),1)
	cd capstone ; for PATCH in ../capstone-patches/* ; do patch -tp1 < $$PATCH ; done
endif
	mkdir -p capstone/include/capstone
	cp -rf capstone/include/*.h capstone/include/capstone

capstone-$(CS_VER).tar.gz:
	$(WGET) --no-check-certificate -O capstone-$(CS_VER).tar.gz -c $(CS_TAR)
else

capstone: capstone-sync

capstone-sync:
	"$(SHELL)" capstone.sh "${CS_URL}" "${CS_BRA}" "${CS_TIP}" "${CS_REV}"
ifeq ($(CS_PATCHES),1)
	-cd capstone ; $(GIT) reset --hard
	cd capstone ; for PATCH in ../capstone-patches/* ; do patch -tp1 < $$PATCH ; done
endif

.PHONY: capstone
endif

CAPSTONE_CFLAGS=-g

ifeq (${EXT_AR},)
all:
	echo EXT_AR is not defined
	exit 1
endif

capstone-build: capstone
	cd capstone && CFLAGS="-Dmips=mips ${CAPSTONE_CFLAGS} $(CFLAGS)" LDFLAGS="$(LDFLAGS)" \
		$(MAKE) CC="$(CC)" AR_EXT=a IS_CYGWIN=0 CAPSTONE_STATIC=yes CAPSTONE_SHARED=no \
		RANLIB="$(RANLIB)" AR="$(AR)" IS_MINGW=0 libcapstone.a
ifneq (${EXT_AR},a)
	-cp -f capstone/libcapstone.a capstone/libcapstone.${EXT_AR}
endif
endif
