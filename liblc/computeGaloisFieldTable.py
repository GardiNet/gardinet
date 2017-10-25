#---------------------------------------------------------------------------
# Generate tables for a field of a given order
#---------------------------------------------------------------------------
# Author: Cedric Adjih
# Copyright 2013-2017 Inria
# All rights reserved. Distributed only with permission.
#---------------------------------------------------------------------------

import json
import sys
sys.path.append("../tools")

import ExtField

#--------------------------------------------------

if len(sys.argv) < 2:
    sys.stderr.write("Syntax: %s <log2 field order>\n" % sys.argv[0])
    sys.exit(1)

LogFieldOrder = int(sys.argv[1])
FieldOrder = 2**LogFieldOrder

K = ExtField.ExtField(2, LogFieldOrder)
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
