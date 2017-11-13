# liblc [(home)](https://gitlab.inria.fr/GardiNet/liblc/)

This library implements a minimal linear coding library, with gaussian elimination decoding. 
It supports GF(2), GF(4), GF(16) and GF(256) fields.

The initial design constraints were:
- Be able to run in constrained embedded systems.
- Be able to support sliding windows mechanisms.

It actually ran on 16 bit [MSP430 nodes](https://www.iot-lab.info/hardware/wsn430/) with 10 KB of RAM and 48 kB of Flash.
As a result, everything in the library is hardcoded at compile time (vector size, maximum encoding vector size, ...).

It is part of the "Sliding Encoding Window" (SEW) module of the network coding protocol DragonNet (follow-up of DRAGONCAST).

# Structures

It offers 3 levels of structures:
- Basic algebraic (linear) operations on vectors of symbols in GF(2), GF(4), GF(16), GF(256). 
- Coded packets with an encoding header and a coded payload. (internally: sliding window support is implicit in the fact that coefficient indices wrap-around in the fixed-size encoding vector).
- A *packet set* that performs Gaussian Elimination on a set of coded packets. Using the logic of DRAGONCAST (SEW), it actually the "RREF" is actually performed in the opposite direction (e.g. we get a lower triangular matrix).

# References

If use this code for research, you can cite:
- Ichrak Amdouni, Antonia Masucci, Hana Baccouch, Cedric Adjih, "DragonNet: Specification, Implementation, Experimentation and Performance Evaluation", Research Report, Inria, September 2014
([link](https://hal.inria.fr/hal-01632790v1))

The Sliding Encoding Window (SEW) module itself was introducted in:
- Song-Yean Cho and Cedric Adjih, "Wireless Broadcast with Network Coding: DRAGONCAST", Inria RR-6569, July 2008 ([link](https://hal.archives-ouvertes.fr/inria-00292867/document))

# LICENCE

This version is released under an X11 licence.

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

# History

- Another C++ library was written by Song Yean Cho and C. Adjih starting from 2006, for the protocol DRAGONCAST (implemented in NS2 simulator). The same C++ library was also used by Farhan Mirani, Anthony Busson, C.A. for the "DONC" opportunistic NC protocol for VANETs.

- As part of the [GETRF project](http://getrf.gforge.inria.fr/category/documentation.html), an evolution of DRAGONCAST for wireless sensor networks was initiated in 2013 as DragonNet/GardiNet with Hana Baccouch, Ichrak Amdouni, Antonia Masucci, C. Adjih - one of result of which is this coding library `liblc` (ran on WSN, and also NS3).
