#!/bin/sh

# Script to install and call qmake and Make without the 
# need for qtcreator.
#
# $Author$
# $Date$
# $Rev$
# $Id$

qmake
make clean
make
