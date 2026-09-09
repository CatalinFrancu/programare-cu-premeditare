#!/bin/bash

BUILD_PATH=/tmp/make4ht

# make4ht only looks in BUILD_PATH for files to include
mkdir -p $BUILD_PATH
cp mathjax-macros.tex $BUILD_PATH

make4ht -l -B $BUILD_PATH -d html/ -c make4ht.cfg main.tex "mathjax"

# Emojis are prefixed by some garbage. Strip it.
perl -pi -e 's|/m/n/\d+(?:/m/n)+||g' html/main.html
