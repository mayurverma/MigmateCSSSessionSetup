/****************************************************************************
* The confidential and proprietary information contained in this file may    *
* only be used by a person authorised under and to the extent permitted      *
* by a subsisting licensing agreement from ARM Limited or its affiliates.    *
*   (C) COPYRIGHT [2001-2017] ARM Limited or its affiliates.         *
*       ALL RIGHTS RESERVED                          *
* This entire notice must be reproduced on all copies of this file           *
* and copies of this file may only be made by a person if such person is     *
* permitted to do so under the terms of a subsisting license agreement       *
* from ARM Limited or its affiliates.                        *
*****************************************************************************/

/*!
@file
@brief This file contains platform-dependent definitions used in the Boot Services code.
*/

#ifndef _CC_PAL_SB_PLAT_H
#define _CC_PAL_SB_PLAT_H

#include "cc_pal_types.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*! Defines dma address type, can be 32-bits or 64-bits, according to platform. */
typedef uint32_t        CCDmaAddr_t;
/*! Defines CryptocCell address type, can be 32-bits or 64-bits, according to platform. */
typedef uint32_t        CCAddr_t;


#ifdef __cplusplus
}
#endif

#endif
