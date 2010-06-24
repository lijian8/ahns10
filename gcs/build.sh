#!/bin/sh

# Script to call qmake and Make without the 
# need for qtcreator.
#
# $Author$
# $Date$
# $Rev$
# $Id$

qmake
make clean
make
