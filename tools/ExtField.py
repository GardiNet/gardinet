#---------------------------------------------------------------------------
# Galois Extension Fields
# (simple operations; for more, use "Sage" (sagemath.org) for instance)
#
# Cedric Adjih
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

from __future__ import print_function, division

try: from urllib.request import urlopen # Python 3
except: from urllib2 import urlopen     # Python 2

import zlib, re, os, gzip
import numpy as np

#---------------------------------------------------------------------------

ConwayPolyUrl = "http://www.math.rwth-aachen.de/~Frank.Luebeck/data/ConwayPol/CPimport.txt.gz"
ConwayPolyTextCache = "cache-conway-poly.txt.gz"

def getConwayPolyTable():
    if not os.path.exists(ConwayPolyTextCache):
        print ("+ retrieving: %s" % ConwayPolyUrl)
        conwayStrGz = urlopen(ConwayPolyUrl).read()
        with open(ConwayPolyTextCache, "wb") as f:
            f.write(conwayStrGz)
    
    with open(ConwayPolyTextCache, "rb") as f:
        conwayStrGz = f.read()
    conwayStr = (zlib.decompress(conwayStrGz, 15+32).decode("utf-8")
                 .replace("allConwayPolynomials := ", "")
                 .replace("0];", "]"))
    m = re.match(r"\A([[\]\n, 0-9]+)\Z", conwayStr)
    assert m != None and len(m.group(1)) == len(conwayStr) # sanity check
    conwayPolyList = eval(conwayStr)
    conwayPolyTable = { (p,n): poly  for p,n, poly in conwayPolyList }
    return conwayPolyTable

#--------------------------------------------------

# Note: both BasicConwayPolyTable and _cacheConwayPolyTable are stored
# with lowest coefficient first, e.g [a_0, a_1, ... ]

BasicConwayPolyTable = {
    (2, 1): [1, 1],
    (2, 2): [1, 1, 1],
    (2, 4): [1, 1, 0, 0, 1],
    (2, 8): [1, 0, 1, 1, 1, 0, 0, 0, 1],
    (2, 16): [1, 0, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1],
    (2, 32): [1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0,
              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1]
}

_cacheConwayPolyTable = None

def getConwayPoly(p,n):
    global _cacheConwayPolyTable
    if (p,n) in BasicConwayPolyTable:
        result = BasicConwayPolyTable[(p,n)]
    else:
        if _cacheConwayPolyTable == None:
            _cacheConwayPolyTable = getConwayPolyTable()
            for (_p,_n),poly in BasicConwayPolyTable.items():
                assert _cacheConwayPolyTable[(_p,_n)] == poly

        if (p,n) not in _cacheConwayPolyTable:
            raise ValueError(
                "No conway polynomial in the database for p**n", (p,n))
        result = _cacheConwayPolyTable[(p,n)]

    return list(reversed(result))

#---------------------------------------------------------------------------

# GF(p^n)
class ExtField:
    """A basic implementation of the finite field GF(p^n) through
    its isomorphic representation GF(p)[X] / (f(X)) where f(X) is
    the Conway polynomial f_p,n(X) for GF(p^n)"""
    
    def __init__(self, p, n):
        self.poly = np.array(getConwayPoly(p,n))
        self.p = p
        self.n = n

    def add(self, u1, u2):
        return self.adjust(np.polyadd(u1, u2))
    
    def sub(self, u1, u2):
        return self.adjust(np.polysub(u1, u2)) 
    
    def mul(self, u1, u2):
        return self.adjust(np.polymul(u1, u2))
       
    def div(self, u1, u2):
        # need to implement Euclidean algorithm for inversion first
        #return self.adjust(np.polydiv(u1, u2))
        raise NotImplementedError("XXX: this is not implemented")
        
    def adjust(self, u):
        assert self.poly[0] == 1
        u = np.array(u) % self.p
        div,mod = np.polydiv(u, self.poly)
        result = np.array(mod).astype(int) % self.p
        if len(result) == 0:
            result = np.array([0])
        return result

    def asTuple(self, u):
        return tuple(self.adjust(u))

    def fromTuple(self, v):
        return self.adjust(np.array(v))
    
    def asInt(self, u):
        return int(np.polyval(self.adjust(u), self.p))
    
    def fromInt(self, v):
        # fromInt(0) returns [0] -> note this is neutral element for add. "0"
        # fromInt(1) returns [1] -> note this is neutral element for mult. "1"
        result = []
        i = 0
        while v > 0:
            coef = v % self.p
            v = v // self.p
            result.append(coef)
        if len(result) == 0:
            result.append(0) 
        return np.array(list(reversed(result)))
    
    def __len__(self):
        return self.p ** self.n
    
    def iterElem(self):
        for i in range(len(self)):
            yield self.fromInt(i)

    def isEqual(self, u1, u2):
        assert isinstance(u1, np.ndarray)
        assert isinstance(u2, np.ndarray)
        return tuple(u1) == tuple(u2)

def gfMul(gf,x,y):
    return gf.asInt(gf.mul(gf.fromInt(x), gf.fromInt(y)))

def gfAdd(gf,x,y):
    return gf.asInt(gf.add(gf.fromInt(x), gf.fromInt(y)))

#---------------------------------------------------------------------------

def test():
    gf = ExtField(3,2)

    def dumpTable():
        for i in gf.iterElem():
            print(i, end=" ")
            for j in gf.iterElem():
                print(gf.asInt(gf.mul(i, j)), end=" ")
            print()

    dumpTable()
    
if __name__ == "__main__":
    test()

#---------------------------------------------------------------------------

