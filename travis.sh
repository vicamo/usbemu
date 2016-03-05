#!/bin/sh

set -e

apt-get -qq update
apt-get install --no-install-recommends -y \
	build-essential \
	gtk-doc-tools \
	libglib2.0-dev

gcc --version

./autogen.sh --enable-gtk-doc
make V=1
make install V=1 DESTDIR=$(pwd)/x && rm -rf $(pwd)/x
make V=1 install
make V=1 distcheck
make V=1 clean

git status
