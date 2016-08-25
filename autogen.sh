#!/bin/sh

aclocal --install -I m4 
autoreconf  --install
automake --add-missing --copy
automake
