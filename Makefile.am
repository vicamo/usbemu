ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

include $(top_srcdir)/glib-tap.mk

SUBDIRS = . docs/reference

usbemu_includedir = $(includedir)/usbemu-$(USBEMU_API_VERSION)/usbemu

usbemu_include_HEADERS =
bin_PROGRAMS =
lib_LTLIBRARIES =

EXTRA_DIST += \
  autogen.sh \
  INSTALL \
  README \
  tap-driver.sh \
  tap-test

GITIGNOREFILES = \
  $(DIST_ARCHIVES)

MAINTAINERCLEANFILES += \
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
  usbemu/usbemu-configuration.c \
  usbemu/usbemu-configuration.h \
  usbemu/usbemu-device.c \
  usbemu/usbemu-device.h \
  usbemu/usbemu-errors.c \
  usbemu/usbemu-errors.h \
  usbemu/usbemu-interface.c \
  usbemu/usbemu-interface.h \
  usbemu/usbemu-internal.h
usbemu_libusbemu_la_CFLAGS = \
  -DLIBUSBEMU_COMPILATION \
  $(BASE_DEPS_CFLAGS)
usbemu_libusbemu_la_LIBADD = \
  $(BASE_DEPS_LIBS)
usbemu_libusbemu_la_LDFLAGS = \
  -version-info $(LT_VERSION_INFO)

usbemu_include_HEADERS += \
  usbemu/usbemu.h \
  usbemu/usbemu-configuration.h \
  usbemu/usbemu-device.h \
  usbemu/usbemu-errors.h \
  usbemu/usbemu-interface.h

###############################
## libusbemu - enums

libusbemu_enum_built_sources = \
  usbemu/usbemu-enums.c \
  usbemu/usbemu-enums.h

libusbemu_enum_check_headers = \
  usbemu/usbemu-configuration.h \
  usbemu/usbemu-device.h \
  usbemu/usbemu-errors.h \
  usbemu/usbemu-interface.h

$(libusbemu_enum_built_sources): Makefile.am $(libusbemu_enum_check_headers) \
  $(libusbemu_enum_built_sources:=.template)
	$(AM_V_GEN) $(GLIB_MKENUMS) \
	  --template $(addprefix $(srcdir)/,$@.template) \
	  $(addprefix $(srcdir)/,$(libusbemu_enum_check_headers)) > $@

BUILT_SOURCES += $(libusbemu_enum_built_sources)
usbemu_libusbemu_la_SOURCES += $(libusbemu_enum_built_sources)

EXTRA_DIST += \
  $(libusbemu_enum_built_sources:=.template)

usbemu_include_HEADERS += \
  usbemu/usbemu-enums.h

###############################
## tests

test_cflags = \
  $(BASE_DEPS_CFLAGS)
test_ldadd = \
  $(BASE_DEPS_LIBS) \
  usbemu/libusbemu.la

test_programs = \
  tests/test-usbemu-enums \
  tests/test-usbemu-error \
  tests/test-usbemu-device \
  tests/test-usbemu-configuration \
  tests/test-usbemu-interface

tests_test_usbemu_enums_CFLAGS = $(test_cflags)
tests_test_usbemu_enums_LDADD = $(test_ldadd)
tests_test_usbemu_error_CFLAGS = $(test_cflags)
tests_test_usbemu_error_LDADD = $(test_ldadd)
tests_test_usbemu_device_CFLAGS = $(test_cflags)
tests_test_usbemu_device_LDADD = $(test_ldadd)
tests_test_usbemu_configuration_CFLAGS = $(test_cflags)
tests_test_usbemu_configuration_LDADD = $(test_ldadd)
tests_test_usbemu_interface_CFLAGS = $(test_cflags)
tests_test_usbemu_interface_LDADD = $(test_ldadd)

###############################
## pkg-config DATA

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = usbemu.pc
DISTCLEANFILES += usbemu.pc

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

CLEANFILES += $(BUILT_SOURCES)
GITIGNOREFILES += $(BUILT_SOURCES)

-include $(top_srcdir)/git.mk
