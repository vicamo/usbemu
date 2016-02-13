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

AM_INIT_AUTOMAKE([dist-xz foreign subdir-objects -Wall -Werror])
AM_MAINTAINER_MODE([enable])
AM_SILENT_RULES([yes])

AC_PROG_CC

AC_OUTPUT([
  Makefile
])
