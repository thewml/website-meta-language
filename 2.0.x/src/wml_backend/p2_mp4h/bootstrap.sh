#! /bin/sh

#   Run this script when reconfiguring

libtoolize --automake -c -f
aclocal-1.6
automake-1.6 -a -c
autoheader
autoconf

