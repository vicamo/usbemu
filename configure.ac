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
        [https://github.com/vicamo/usbemu/issues],
        [usbemu],
        [https://github.com/vicamo/usbemu])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([build-aux])

# Depends on automake 1.14 or newer so that we don't need AM_PROG_CC_C_O
AM_INIT_AUTOMAKE([1.14 dist-xz foreign subdir-objects no-define -Wall -Werror])
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

AC_PROG_CC

# Define _GNU_SOURCE etc. where appropriate, e.g. for strptime() on glibc
AC_USE_SYSTEM_EXTENSIONS
# Make sure we use 64-bit versions of various file stuff.
AC_SYS_LARGEFILE

PKG_CHECK_MODULES(BASE_DEPS, [glib-2.0 >= 2.6])

AC_OUTPUT([
  Makefile
])
