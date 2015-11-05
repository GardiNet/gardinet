#---------------------------------------------------------------------------
# Galois Extension Fields
# (simple operations; for more, use "Sage" (sagemath.org) for instance)
#
# Cedric Adjih - Inria - 2015
#---------------------------------------------------------------------------

import urllib2
import zlib, re, os, gzip
import numpy as np

#---------------------------------------------------------------------------

ConwayPolyUrl = "http://www.math.rwth-aachen.de/~Frank.Luebeck/data/ConwayPol/CPimport.txt.gz"
ConwayPolyTextCache = "cache-conway-poly.txt.gz"

def getConwayPolyTable():
    if not os.path.exists(ConwayPolyTextCache):
        print ("+ retrieving: %s" % ConwayPolyUrl)
        conwayStrGz = urllib2.urlopen(ConwayPolyUrl).read()
        with open(ConwayPolyTextCache, "wb") as f:
            f.write(conwayStrGz)
    
    with open(ConwayPolyTextCache) as f:
        conwayStrGz = f.read()
    conwayStr = (zlib.decompress(conwayStrGz, 15+32)
                 .replace("allConwayPolynomials := ", "")
                 .replace("0];", "]"))
    m = re.match(r"\A([[\]\n, 0-9]+)\Z", conwayStr)
    assert m != None and len(m.group(1)) == len(conwayStr) # sanity check
    conwayPolyList = eval(conwayStr)
    conwayPolyTable = { (p,n): poly  for p,n, poly in conwayPolyList }
    return conwayPolyTable

#---------------------------------------------------------------------------

class ExtField:
    conwayPolyTable = getConwayPolyTable()

    # GF(p^n)
    def __init__(self, p, n):
        self.poly = np.array(list(reversed(self.conwayPolyTable[(p,n)])))
        self.p = p
        self.n = n

    def add(self, u1, u2):
        return self.adjust(np.polyadd(u1, u2))
    
    def sub(self, u1, u2):
        return self.adjust(np.polysub(u1, u2)) 
    
    def mul(self, u1, u2):
        return self.adjust(np.polymul(u1, u2))
       
    def div(self, u1, u2):
        return self.adjust(np.polydiv(u1,u2))
        
    def adjust(self, u):
        assert self.poly[0] == 1
        u = np.array(u) % self.p
        div,mod = np.polydiv(u, self.poly)
        result = np.array(mod).astype(int) % self.p
        if len(result) == 0:
            result = np.array([0])
        return result
    
    def asInt(self, u):
        return np.polyval(self.adjust(u), self.p)
    
    def fromInt(self, v):
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


def gfMul(x,y):
    return gf.asInt(gf.mul(gf.fromInt(x), gf.fromInt(y)))

def gfAdd(x,y):
    return gf.asInt(gf.add(gf.fromInt(x), gf.fromInt(y)))

#---------------------------------------------------------------------------

def test():
    gf = ExtField(3,2)

    gf.add([1,2,3], [2,3,4])
    #gf.poly
    u = gf.adjust([1,2,3,4,5,1,4])
    u, gf.asInt(u), gf.fromInt(0)
    #u
    gf.mul(gf.fromInt(0), gf.fromInt(3))
    len(gf)

    def dumpTable():
        for i in gf.iterElem():
            print i,
            for j in gf.iterElem():
                print gf.asInt(gf.mul(i, j)),
            print

    dumpTable()
    
if __name__ == "__main__":
    test()

#---------------------------------------------------------------------------

