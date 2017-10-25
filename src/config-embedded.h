/*---------------------------------------------------------------------------
 * Definitions specific for compiling on embedded systems (IotLab.fr)
 *---------------------------------------------------------------------------
 * Author: Cedric Adjih
 * Copyright 2013 Inria
 * All rights reserved. Distributed only with permission.
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
