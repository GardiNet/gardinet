/*---------------------------------------------------------------------------
 * Definitions specific for compiling on embedded systems (IotLab.info)
 *---------------------------------------------------------------------------
 * Author: Cedric Adjih
 *---------------------------------------------------------------------------
 * Copyright 2013-2017 Inria
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *---------------------------------------------------------------------------*/

#ifndef __CONFIG_EMBEDDED_H__
#define __CONFIG_EMBEDDED_H__

/*---------------------------------------------------------------------------*/

#if 0
// MAX_CODED_PACKET must be larger than coding window size
#define CONF_MAX_CODED_PACKET 64
#define CONF_LOG2_HEADER_SIZE 4 //7
#define CONF_CODED_PACKET_SIZE 8
#endif

//#define CONF_MAX_CODED_PACKET 128
//#define CONF_LOG2_HEADER_SIZE 5 //7

/*---------------------------------------------------------------------------*/

#endif /* __CONFIG_EMBEDDED_H__ */
