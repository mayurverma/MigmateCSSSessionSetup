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

#ifndef _CC_LOG_MASK_H_
#define _CC_LOG_MASK_H_

#define CC_LOG_MASK_CCLIB                   (1<<1)
#define CC_LOG_MASK_SECURE_BOOT             (1<<2)
#define CC_LOG_MASK_CMPU                    (1<<3)
#define CC_LOG_MASK_DMPU                    (1<<4)
#define CC_LOG_MASK_CC_API                  (1<<5)
#define CC_LOG_MASK_CC_SYM_DRIVER           (1<<6)
#define CC_LOG_MASK_MLLI                    (1<<7)
#define CC_LOG_MASK_HW_QUEUE                (1<<8)
#define CC_LOG_MASK_COMPLETION              (1<<9)
#define CC_LOG_MASK_INFRA                   (1<<10)
#define CC_LOG_MASK_LLF                     (1<<13)
#define CC_LOG_MASK_ASYM_ECC                (1<<14)
#define CC_LOG_MASK_ASYM_RSA_DH             (1<<15)
#define CC_LOG_MASK_ASYM_KDF                (1<<16)
#define CC_LOG_MASK_ASYM_LLF                (1<<17)
#define CC_LOG_MASK_ASYM_RND                (1<<18)
#define CC_LOG_MASK_UTILS                   (1<<19)


#endif
