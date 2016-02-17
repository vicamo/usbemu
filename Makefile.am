ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

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

bin_PROGRAMS = \
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
  $(DIST_ARCHIVES)

-include $(top_srcdir)/git.mk
