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
cp ./unilog.h $include_path
echo "Unilog copied"
# Unitest #
cp ./unitest.h $include_path
echo "Unitest copied"

## Base Libraries ##
# Str #
make -C str && make -C str install && make -C str clean

# Regex #
make -C regex && make -C regex install && make -C regex clean

# LiSON #
make -C lison && make -C lison install && make -C lison clean
