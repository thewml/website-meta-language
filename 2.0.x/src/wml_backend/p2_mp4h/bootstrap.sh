#! /bin/sh

#   Run this script when reconfiguring

libtoolize --automake -c -f
aclocal-1.10
automake-1.10 -a -c
autoheader
autoconf

