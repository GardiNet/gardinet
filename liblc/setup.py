#---------------------------------------------------------------------------
# Setup file for compiling SWIG module for libnc
#---------------------------------------------------------------------------
# Author: Cedric Adjih
# Copyright 2013 Inria
# All rights reserved. Distributed only with permission.
#---------------------------------------------------------------------------

import distutils

from distutils.core import setup, Extension


unusedExtension = Extension("wraplibsew",
                            ["wraplibsew.i"],
                            include_dirs=["."],
                            library_dirs=["."],
                            libraries=[],
                            swig_opts=[],
                            language="c"
                      )


extension = Extension("libsewmodule", ["libsewmodule.c"])
extension.undef_macros.append("NDEBUG")

setup(name="libsewmodule", version="0.1",
      ext_modules = [ extension ])

#---------------------------------------------------------------------------
