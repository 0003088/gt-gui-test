#!/bin/sh

# Bootstraping Elektra
echo "BOOTSTRAPING ELEKTRA"
echo "LIBTOOLIZE"
libtoolize --ltdl --copy

echo "ACLOCAL"
aclocal -I m4

echo "AUTOHEADER"
autoheader

echo "AUTOMAKE"
automake --add-missing

echo "AUTOCONF"
autoconf

# Bootstraping libltdl
echo "BOOTSTRAPING LIBLTDL"

cd libltdl

echo "ACLOCAL"
aclocal

echo "AUTOHEADER"
autoheader

echo "AUTOMAKE"
automake --add-missing

echo "AUTOCONF"
autoconf

cd ..
