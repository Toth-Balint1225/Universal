#!/bin/bash

# Tóth Bálint, University of Pannonia
# 2023.
#
# Build and install the universal C utilities.

# set the install destination
# TODO: use some config file
include_path=/usr/include
library_path=/usr/lib

## Single headers ## 
# Unilog #
rm $include_path/unilog.h
echo "Unilog removed"
# Unitest #
rm $include_path/unitest.h
echo "Unitest removed"

## Base Libraries ##
# Str #
make -C str uninstall && make -C str purge

# Regex #
make -C regex uninstall && make -C regex purge

# LiSON #
make -C lison uninstall && make -C lison purge
