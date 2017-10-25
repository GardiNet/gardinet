#---------------------------------------------------------------------------
# Setup file for compiling SWIG module for liblc
#---------------------------------------------------------------------------
# Author: Cedric Adjih
# Copyright 2013-2017 Inria
# All rights reserved. Distributed only with permission.
#---------------------------------------------------------------------------

import distutils

from distutils.core import setup, Extension

LIBLCDIR = "../liblc"

extension = Extension("liblcmodule", ["liblcmodule.c"])
extension.undef_macros.append("NDEBUG")
extension.include_dirs.append(LIBLCDIR)

setup(name="liblcmodule", version="0.1",
      ext_modules = [ extension ])

#---------------------------------------------------------------------------
