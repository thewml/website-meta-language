#!/bin/bash

myfile=../wml_backend/p2_mp4h/modules/subdirs.am
if test -n "$CYGWIN" ; then
    echo "SUBDIRS = " > "$myfile"
else
    echo "SUBDIRS = intl system" > "$myfile"
fi
