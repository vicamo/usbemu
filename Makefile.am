ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS = . docs/reference

bin_PROGRAMS =
lib_LTLIBRARIES =

EXTRA_DIST = \
  autogen.sh \
  INSTALL \
  README

CLEANFILES =
DISTCLEANFILES =

GITIGNOREFILES = \
  $(DIST_ARCHIVES)

MAINTAINERCLEANFILES = \
  $(GITIGNORE_MAINTAINERCLEANFILES_TOPLEVEL) \
  $(GITIGNORE_MAINTAINERCLEANFILES_M4_LIBTOOL) \
  $(GITIGNORE_MAINTAINERCLEANFILES_MAKEFILE_IN) \
  $(srcdir)/ChangeLog \
  $(srcdir)/gtk-doc.make \
  $(srcdir)/INSTALL \
  $(srcdir)/m4/gtk-doc.m4 \
  $(srcdir)/README

DISTCHECK_CONFIGURE_FLAGS = \
  --enable-gtk-doc \
  --disable-maintainer-mode

AM_CPPFLAGS = \
  -DG_LOG_DOMAIN=\"usbemu\" \
  -DUSBEMU_VERSION=\"$(USBEMU_VERSION)\" \
  -I$(top_builddir) \
  -I$(top_srcdir)

AM_CFLAGS = \
  -Wall -Werror

###############################
## libusbemu - core

lib_LTLIBRARIES += \
  usbemu/libusbemu.la

usbemu_libusbemu_la_SOURCES = \
  usbemu/usbemu.h \
  usbemu/usbemu-device.c \
  usbemu/usbemu-device.h
usbemu_libusbemu_la_CFLAGS = \
  -DLIBUSBEMU_COMPILATION \
  $(BASE_DEPS_CFLAGS)
usbemu_libusbemu_la_LIBADD = \
  $(BASE_DEPS_LIBS)
usbemu_libusbemu_la_LDFLAGS = \
  -version-info $(LT_VERSION_INFO)

###############################
## libusbemu - dbus

libusbemu_dbus_csources = \
  usbemu/dbus/usbemu-dbus-manager.c \
  usbemu/dbus/usbemu-dbus-manager.h

usbemu_libusbemu_la_SOURCES += $(libusbemu_dbus_csources)

define _make_usbemu_dbus_rule
$(1): $(1:.c=.xml)
	$$(AM_V_GEN) gdbus-codegen \
	  --generate-c-code $$(basename $$@) \
	  --c-namespace UsbemuDBus \
	  --interface-prefix org.usbemu.UsbemuManager \
	  $$<

$(basename $(1)).h: $(1)
	@true
endef

$(foreach f,$(filter %.c,$(libusbemu_dbus_csources)),$(eval $(call _make_usbemu_dbus_rule,$(f))))

CLEANFILES += \
  $(libusbemu_dbus_csources)

GITIGNOREFILES += \
  $(libusbemu_dbus_csources)

EXTRA_DIST += \
  usbemu/dbus/usbemu-dbus-manager.xml

###############################
## pkg-config DATA

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = usbemu.pc
DISTCLEANFILES += usbemu.pc

###############################
## usbemud

bin_PROGRAMS += \
  src/usbemud

src_usbemud_SOURCES = \
  src/usbemu-manager.c \
  src/usbemu-manager.h \
  src/usbemud.c
src_usbemud_CFLAGS = \
  $(BASE_DEPS_CFLAGS)
src_usbemud_LDADD = \
  $(BASE_DEPS_LIBS) \
  usbemu/libusbemu.la

GITIGNOREFILES += \
  $(srcdir)/gtk-doc.m4

distclean-local:
	if test "$(srcdir)" = "."; then :; else \
	  rm -f ChangeLog; \
	fi

uninstall-local:
	rm -f $(DESTDIR)$(pkgconfigdir)/usbemu.pc

.PHONY: ChangeLog
ChangeLog:
	$(AM_V_GEN) if test -d "$(srcdir)/.git"; then \
	  (GIT_DIR=$(top_srcdir)/.git $(top_builddir)/build-aux/missing git log --stat) | fmt --split-only > $@.tmp \
	  && mv -f $@.tmp $@ \
	  || ($(RM) $@.tmp; \
	      echo Failed to generate ChangeLog, your ChangeLog may be outdated >&2; \
	      (test -f $@ || echo git-log is required to generate this file >> $@)); \
	else \
	  test -f $@ || \
	  (echo A git checkout and git-log is required to generate ChangeLog >&2 && \
	  echo A git checkout and git-log is required to generate this file >> $@); \
	fi

-include $(top_srcdir)/git.mk
