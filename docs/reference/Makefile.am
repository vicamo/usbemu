ACLOCAL_AMFLAGS = -I m4 ${ACLOCAL_FLAGS}

MAINTAINERCLEANFILES =

AM_CPPFLAGS = \
  -I$(top_builddir) \
  -I$(top_srcdir)

# gtk-doc.make defines EXTRA_DIST and CLEANFILES
include $(top_srcdir)/gtk-doc.make

# The name of the module.
DOC_MODULE = usbemu

# The top-level SGML file.
DOC_MAIN_SGML_FILE = $(DOC_MODULE)-docs.xml

# The directory containing the source code. Relative to $(srcdir).
# gtk-doc will search all .c & .h files beneath here for inline comments
# documenting functions and macros.
DOC_SOURCE_DIR = \
  $(top_srcdir)/usbemu \
  $(top_builddir)/usbemu

# Extra options to supply to gtkdoc-scangobj.
SCANGOBJ_OPTIONS =

# Extra options to supply to gtkdoc-mkdb
MKDB_OPTIONS = \
  --output-format=xml \
  --name-space=gtk

# Extra options to supply to gtkdoc-fixref.
FIXXREF_OPTIONS =

# Used for dependencies.
HFILE_GLOB = \
  $(top_srcdir)/usbemu/dbus/*.h
CFILE_GLOB = \
  $(top_srcdir)/usbemu/dbus/*.c

# Header files to ignore when scanning.
IGNORE_HFILES =

# Extra XML files that are included by $(DOC_MAIN_SGML_FILE).
content_files = \
  version.xml

# Other files to distribute.
extra_files =

# CFLAGS and LDFLAGS for compiling scan program. Only needed
# if $(DOC_MODULE).types is non-empty.
GTKDOC_CFLAGS = \
  -I$(top_builddir) \
  -I$(top_srcdir) \
  $(BASE_DEPS_CFLAGS)

GTKDOC_LIBS = \
  $(BASE_DEPS_LIBS) \
  $(top_builddir)/usbemu/libusbemu.la

CLEANFILES += \
  html/* \
  usbemu-decl-list.txt \
  usbemu-decl.txt \
  usbemu-overrides.txt \
  usbemu-sections.txt \
  usbemu.types \
  xml/*

EXTRA_DIST += \
  version.xml.in

-include $(top_srcdir)/git.mk
