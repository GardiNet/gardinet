#---------------------------------------------------------------------------
# Author: Cedric Adjih
# Copyright 2017 Inria
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
	rm -rf build table-*.pydat table-*gf*.c 

#---------------------------------------------------------------------------
