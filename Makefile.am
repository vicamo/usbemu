ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

bin_PROGRAMS =
noinst_LTLIBRARIES =

CLEANFILES =

EXTRA_DIST = \
  autogen.sh \
  INSTALL \
  README

MAINTAINERCLEANFILES = \
  $(GITIGNORE_MAINTAINERCLEANFILES_TOPLEVEL) \
  $(GITIGNORE_MAINTAINERCLEANFILES_M4_LIBTOOL) \
  $(GITIGNORE_MAINTAINERCLEANFILES_MAKEFILE_IN) \
  $(srcdir)/ChangeLog \
  $(srcdir)/INSTALL \
  $(srcdir)/README

AM_CPPFLAGS = \
  -DG_LOG_DOMAIN=\"usbemu\" \
  -DUSBEMU_VERSION=\"$(USBEMU_VERSION)\" \
  -I$(top_builddir) \
  -I$(top_srcdir)

noinst_LTLIBRARIES += \
  usbemu/libusbemu-dbus.la

usbemu_libusbemu_dbus_la_SOURCES = \
  usbemu/usbemu-dbus-manager.c \
  usbemu/usbemu-dbus-manager.h
usbemu_libusbemu_dbus_la_CFLAGS = \
  $(BASE_DEPS_CFLAGS)
usbemu_libusbemu_dbus_la_LIBADD = \
  $(BASE_DEPS_LIBS)

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

$(foreach f,$(filter %.c,$(usbemu_libusbemu_dbus_la_SOURCES)),$(eval $(call _make_usbemu_dbus_rule,$(f))))

CLEANFILES += \
  $(usbemu_libusbemu_dbus_la_SOURCES)

EXTRA_DIST += \
  usbemu/usbemu-dbus-manager.xml

bin_PROGRAMS += \
  src/usbemud

src_usbemud_SOURCES = \
  src/usbemud.c
src_usbemud_CFLAGS = \
  $(BASE_DEPS_CFLAGS)
src_usbemud_LDADD = \
  $(BASE_DEPS_LIBS)

distclean-local:
	if test "$(srcdir)" = "."; then :; else \
	  rm -f ChangeLog; \
	fi

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

DISTCHECK_CONFIGURE_FLAGS = \
  --disable-maintainer-mode

GITIGNOREFILES = \
  $(DIST_ARCHIVES) \
  $(usbemu_libusbemu_dbus_la_SOURCES)

-include $(top_srcdir)/git.mk
