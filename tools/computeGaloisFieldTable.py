#---------------------------------------------------------------------------
# Generate tables for a field of a given order
#---------------------------------------------------------------------------
# Author: Cedric Adjih
#---------------------------------------------------------------------------
# Copyright 2013-2017 Inria
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

import json
import sys

import ExtField

#--------------------------------------------------

if len(sys.argv) < 2:
    sys.stderr.write("Syntax:")
    sys.stderr.write("for GF(2^(2^p)): %s <log2 field order>\n" % sys.argv[0])
    sys.stderr.write("or for GF(p^q): %s <p> <q>\n" % sys.argv[0])
    sys.exit(1)

if len(sys.argv) < 3:
    LogFieldOrder = int(sys.argv[1])
    FieldOrder = 2**LogFieldOrder
    K = ExtField.ExtField(2, LogFieldOrder)    
else: 
    p = int(sys.argv[1])
    q = int(sys.argv[2])
    FieldOrder = p**q
    K = ExtField.ExtField(p,q)

elemList = [K.fromInt(i) for i in range(len(K))]

toIntTable = { K.asTuple(x):i for (i,x) in enumerate(elemList) }

#--------------------------------------------------

mulTable = []
sumTable =[]
invActualTable = {}
for ix,x in enumerate(elemList):
    sys.stdout.write(".")
    sys.stdout.flush()
    for iy,y in enumerate(elemList):
        prodxy = toIntTable[K.asTuple(K.mul(x,y))]
        sumxy = toIntTable[K.asTuple(K.add(x,y))]
        mulTable.append((ix,iy,prodxy))
        sumTable.append((ix,iy,sumxy))
        if prodxy == 1:
            invActualTable[ix] = iy
            invActualTable[iy] = ix

invTable = [ (x,y) for x,y in invActualTable.items() ]

_0 = K.fromInt(0)
_1 = K.fromInt(1)

negTable = []
for x in elemList:
    _x = K.asInt(x)
    negTable.append([_x,K.asInt(K.sub(_0, x))])

expTable = []
logTable = []
if elemList[1] == _1:
    gen = elemList[2] # XXX:need to have a generator (mult. group) 
else: gen = elemList[1]
x = gen
expTable.append([0,K.asInt(_1)])
logTable.append([K.asInt(_1),0])
for i in range(1, FieldOrder):
    _x = K.asInt(x)
    expTable.append([i,_x])
    logTable.append([_x,i])
    x = K.mul(gen, x)

#--------------------------------------------------

info = {
     "sumTable": sumTable,
     "mulTable": mulTable,
     "invTable": invTable,
     "negTable": negTable,
     "expTable": expTable,
     "logTable": logTable
}

f = open("table-gf%s.json" % FieldOrder, "w")
f.write(json.dumps(info))
f.close()

#---------------------------------------------------------------------------
