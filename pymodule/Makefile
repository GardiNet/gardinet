#---------------------------------------------------------------------------
# Author: Cedric Adjih
#---------------------------------------------------------------------------
# Copyright 2015-2017 Inria
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to
# the following conditions:
#
# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
# OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
# WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#---------------------------------------------------------------------------

LIBLCDIR?=../src

PYTHON=python

CFLAGS += -I{LIBLCDIR}
CPPFLAGS += -I{LIBLCDIR}
CXXFLAGS += -I{LIBLCDIR}

#---------------------------------------------------------------------------

#---------------------------------------------------------------------------
# Main targets
#---------------------------------------------------------------------------

all: swig-all

swig-all: ${LIBLCDIR}/liblc.a _liblcmodule.so

#---------------------------------------------------------------------------

${LIBLCDIR}/liblc.a:
	cd ${LIBLCDIR} && make liblc.a

#--------------------------------------------------------------------------
# SWIG wrapper
#---------------------------------------------------------------------------

_liblcmodule.so: liblcmodule.c setup.py # ${SRCS} ${OBJS}
	rm -rf build
	${PYTHON} setup.py build #|| python setup.py build
	ln -sf build/lib*/liblcmodule.so $@ || true

liblcmodule.c: liblc.i Makefile ${SRCS} ${HEADERS}
	swig -python -I${LIBLCDIR} -o $@ liblc.i 

unittest: all
	python testliblc.py

#---------------------------------------------------------------------------
# Cleaning
#---------------------------------------------------------------------------

clean:
	rm -f *.a *.so *.o *.d *~ *.pyc 
	rm -f liblcmodule.c liblcmodule.py
	rm -f test-coded-packet

really-clean: clean
	rm -rf build __pycache__

#---------------------------------------------------------------------------
