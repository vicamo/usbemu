m4_define([usbemu_major_version], [0])
m4_define([usbemu_minor_version], [0])
m4_define([usbemu_micro_version], [1])
m4_define([usbemu_interface_age], [1])
m4_define([usbemu_binary_age],
          [m4_eval(100 * usbemu_minor_version + usbemu_micro_version)])
m4_define([usbemu_version],
          [usbemu_major_version.usbemu_minor_version.usbemu_micro_version])
m4_define([usbemu_api_version], [0.0])

AC_PREREQ([2.62])
AC_INIT([usbemu], [usbemu_version],
        [https://github.com/usbemu/usbemu/issues],
        [usbemu],
        [https://github.com/usbemu/usbemu])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

# Depends on automake 1.14 or newer so that we don't need AM_PROG_CC_C_O
AM_INIT_AUTOMAKE([1.14 dist-xz foreign subdir-objects no-define -Wall])
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])

USBEMU_MAJOR_VERSION=usbemu_major_version
USBEMU_MINOR_VERSION=usbemu_minor_version
USBEMU_MICRO_VERSION=usbemu_micro_version
USBEMU_INTERFACE_AGE=usbemu_interface_age
USBEMU_BINARY_AGE=usbemu_binary_age
USBEMU_VERSION=usbemu_version
USBEMU_API_VERSION=usbemu_api_version
AC_SUBST(USBEMU_MACRO_VERSION)
AC_SUBST(USBEMU_MINOR_VERSION)
AC_SUBST(USBEMU_MICRO_VERSION)
AC_SUBST(USBEMU_INTERFACE_AGE)
AC_SUBST(USBEMU_BINARY_AGE)
AC_SUBST(USBEMU_VERSION)
AC_SUBST(USBEMU_API_VERSION)

m4_define([lt_current], [m4_eval(100 * usbemu_minor_version + usbemu_micro_version - usbemu_interface_age)])
m4_define([lt_revision], [usbemu_interface_age])
m4_define([lt_age], [m4_eval(usbemu_binary_age - usbemu_interface_age)])
LT_VERSION_INFO="lt_current:lt_revision:lt_age"
LT_CURRENT_MINUS_AGE=m4_eval(lt_current - lt_age)
AC_SUBST(LT_VERSION_INFO)
AC_SUBST(LT_CURRENT_MINUS_AGE)

AX_CHECK_ENABLE_DEBUG

AC_PROG_CC

# Define _GNU_SOURCE etc. where appropriate, e.g. for strptime() on glibc
AC_USE_SYSTEM_EXTENSIONS
# Make sure we use 64-bit versions of various file stuff.
AC_SYS_LARGEFILE

AM_PROG_AR

dnl Initialize libtool
LT_PREREQ([2.2.6])
LT_INIT([disable-static])
# Create libtool early, because it's used in configure
LT_OUTPUT

PKG_CHECK_MODULES(BASE_DEPS, [glib-2.0 >= 2.42 dnl
                              gio-2.0 >= 2.37 dnl
                              gio-unix-2.0 >= 2.24])

PKG_CHECK_MODULES(GUDEV, [gudev-1.0])

# GTK-DOC generation
GTK_DOC_CHECK([1.20],[--flavour no-tmpl])

GLIB_MKENUMS=`$PKG_CONFIG --variable=glib_mkenums glib-2.0`
AC_SUBST(GLIB_MKENUMS)

GDBUS_CODEGEN=`$PKG_CONFIG --variable=gdbus_codegen gio-2.0`
AC_SUBST(GDBUS_CODEGEN)

AC_CONFIG_FILES([
  docs/reference/Makefile
  docs/reference/version.xml
  usbemu.pc
  Makefile
])

AC_OUTPUT
