/**************************************************************************************
* Copyright (c) 2016-2019, Arm Limited (or its affiliates). All rights reserved       *
*                                                                                     *
* This file and the related binary are licensed under the following license:          *
*                                                                                     *
* ARM Object Code and Header Files License, v1.0 Redistribution.                      *
*                                                                                     *
* Redistribution and use of object code, header files, and documentation, without     *
* modification, are permitted provided that the following conditions are met:         *
*                                                                                     *
* 1) Redistributions must reproduce the above copyright notice and the                *
*    following disclaimer in the documentation and/or other materials                 *
*    provided with the distribution.                                                  *
*                                                                                     *
* 2) Unless to the extent explicitly permitted by law, no reverse                     *
*    engineering, decompilation, or disassembly of is permitted.                      *
*                                                                                     *
* 3) Redistribution and use is permitted solely for the purpose of                    *
*    developing or executing applications that are targeted for use                   *
*    on an ARM-based product.                                                         *
*                                                                                     *
* DISCLAIMER. THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND                  *
* CONTRIBUTORS "AS IS." ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT             *
* NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, NON-INFRINGEMENT,        *
* AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE          *
* COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,   *
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED            *
* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR              *
* PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF              *
* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING                *
* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS                  *
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                        *
**************************************************************************************/

#ifndef MBEDTLS_DHM_ALT_H
#define MBEDTLS_DHM_ALT_H


#if defined(MBEDTLS_DHM_ALT)


#if defined(MBEDTLS_CONFIG_FILE)
#include MBEDTLS_CONFIG_FILE
#endif

#include <stddef.h>

/*
 * DHM Error codes
 */
#define MBEDTLS_ERR_DHM_BAD_INPUT_DATA                    -0x3080  /**< Bad input parameters. */
#define MBEDTLS_ERR_DHM_READ_PARAMS_FAILED                -0x3100  /**< Reading of the DHM parameters failed. */
#define MBEDTLS_ERR_DHM_MAKE_PARAMS_FAILED                -0x3180  /**< Making of the DHM parameters failed. */
#define MBEDTLS_ERR_DHM_READ_PUBLIC_FAILED                -0x3200  /**< Reading of the public values failed. */
#define MBEDTLS_ERR_DHM_MAKE_PUBLIC_FAILED                -0x3280  /**< Making of the public value failed. */
#define MBEDTLS_ERR_DHM_CALC_SECRET_FAILED                -0x3300  /**< Calculation of the DHM secret failed. */
#define MBEDTLS_ERR_DHM_INVALID_FORMAT                    -0x3380  /**< The ASN.1 data is not formatted correctly. */
#define MBEDTLS_ERR_DHM_ALLOC_FAILED                      -0x3400  /**< Allocation of memory failed. */
#define MBEDTLS_ERR_DHM_FILE_IO_ERROR                     -0x3480  /**< Read or write of file failed. */
#define MBEDTLS_ERR_DHM_HW_ACCEL_FAILED                   -0x3500  /**< DHM hardware accelerator failed. */
#define MBEDTLS_ERR_DHM_SET_GROUP_FAILED                  -0x3580  /**< Setting the modulus and generator failed. */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief          The DHM context structure.
 */
typedef struct
{
    size_t len;         /*!<  The size of \p P in Bytes. */
    mbedtls_mpi P;      /*!<  The prime modulus. */
    mbedtls_mpi G;      /*!<  The generator. */
    mbedtls_mpi X;      /*!<  Our secret value. */
    mbedtls_mpi GX;     /*!<  Our public key = \c G^X mod \c P. */
    mbedtls_mpi GY;     /*!<  The public key of the peer = \c G^Y mod \c P. */
    mbedtls_mpi K;      /*!<  The shared secret = \c G^(XY) mod \c P. */
    mbedtls_mpi RP;     /*!<  The cached value = \c R^2 mod \c P. */
    mbedtls_mpi Vi;     /*!<  The blinding value. */
    mbedtls_mpi Vf;     /*!<  The unblinding value. */
    mbedtls_mpi pX;     /*!<  The previous \c X. */
}
mbedtls_dhm_context;

#ifdef __cplusplus
}
#endif

#endif  /* MBEDTLS_DHM_ALT  - use alternative code */
#endif  /* MBEDTLS_DHM_ALT_H  - include only once  */
