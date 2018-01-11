ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

SUBDIRS = . docs/reference

usbemu_includedir = $(includedir)/usbemu-$(USBEMU_API_VERSION)/usbemu

usbemu_include_HEADERS =
bin_PROGRAMS =
lib_LTLIBRARIES =

EXTRA_DIST = \
  autogen.sh \
  INSTALL \
  README

BUILT_SOURCES =
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
  usbemu/usbemu-device.h \
  usbemu/usbemu-errors.c \
  usbemu/usbemu-errors.h \
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
  usbemu/usbemu-device.h \
  usbemu/usbemu-errors.h

###############################
## libusbemu - enums

libusbemu_enum_built_sources = \
  usbemu/usbemu-enums.c \
  usbemu/usbemu-enums.h

libusbemu_enum_check_headers = \
  usbemu/usbemu-device.h \
  usbemu/usbemu-errors.h

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
