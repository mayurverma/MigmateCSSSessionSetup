#ifndef CRYPTO_OTPM_H
#define CRYPTO_OTPM_H
//***********************************************************************************
/// \file
///
/// Hardware block register definitions.
///
//***********************************************************************************
// Copyright 2018 ON Semiconductor.  All rights reserved.
//
// This software and/or documentation is licensed by ON Semiconductor under limited
// terms and conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read and
// you agree to the limited terms and conditions. By using this software and/or
// documentation, you agree to the limited terms and conditions.
//***********************************************************************************

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DO NOT EDIT - This file is auto-generated by the generate_headers.py script
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

// Register address definitions (offsets relative to register base)

#define CRYPTO_OTPM_CONTROL                                (CRYPTO_OTPM_REG_BASE + 0x0000U)
#define CRYPTO_OTPM_CORE_MR                                (CRYPTO_OTPM_REG_BASE + 0x0004U)
#define CRYPTO_OTPM_CORE_MR_MRA                            (CRYPTO_OTPM_REG_BASE + 0x0008U)
#define CRYPTO_OTPM_CORE_MRA                               (CRYPTO_OTPM_REG_BASE + 0x000CU)
#define CRYPTO_OTPM_CORE_MRB                               (CRYPTO_OTPM_REG_BASE + 0x0010U)
#define CRYPTO_OTPM_CORE_MRB_TDS                           (CRYPTO_OTPM_REG_BASE + 0x0014U)
#define CRYPTO_OTPM_CORE_PROG_TIME                         (CRYPTO_OTPM_REG_BASE + 0x0018U)
#define CRYPTO_OTPM_CORE_RECOVERY                          (CRYPTO_OTPM_REG_BASE + 0x001CU)
#define CRYPTO_OTPM_CORE_SOAK_CYCLES                       (CRYPTO_OTPM_REG_BASE + 0x0020U)
#define CRYPTO_OTPM_CORE_PROGRAM                           (CRYPTO_OTPM_REG_BASE + 0x0024U)
#define CRYPTO_OTPM_CORE_WRITE_TIME                        (CRYPTO_OTPM_REG_BASE + 0x0028U)
#define CRYPTO_OTPM_CORE_READ_TIME                         (CRYPTO_OTPM_REG_BASE + 0x002CU)
#define CRYPTO_OTPM_CORE_READ_ERROR                        (CRYPTO_OTPM_REG_BASE + 0x0030U)
#define CRYPTO_OTPM_CORE_ECC_FLAGS                         (CRYPTO_OTPM_REG_BASE + 0x0034U)

// Register field definitions

#define CRYPTO_OTPM_CONTROL_LOCK_SECTOR__SIZE              2U
#define CRYPTO_OTPM_CONTROL_LOCK_SECTOR__SHIFT             0U
#define CRYPTO_OTPM_CONTROL_LOCK_SECTOR__MASK              (((uint32_t)((1UL << CRYPTO_OTPM_CONTROL_LOCK_SECTOR__SIZE) - 1U)) << CRYPTO_OTPM_CONTROL_LOCK_SECTOR__SHIFT)

#define CRYPTO_OTPM_CONTROL_LOCK_ALL__SIZE                 1U
#define CRYPTO_OTPM_CONTROL_LOCK_ALL__SHIFT                2U
#define CRYPTO_OTPM_CONTROL_LOCK_ALL__MASK                 (((uint32_t)((1UL << CRYPTO_OTPM_CONTROL_LOCK_ALL__SIZE) - 1U)) << CRYPTO_OTPM_CONTROL_LOCK_ALL__SHIFT)

#define CRYPTO_OTPM_CONTROL_EN_STANDBY__SIZE               1U
#define CRYPTO_OTPM_CONTROL_EN_STANDBY__SHIFT              3U
#define CRYPTO_OTPM_CONTROL_EN_STANDBY__MASK               (((uint32_t)((1UL << CRYPTO_OTPM_CONTROL_EN_STANDBY__SIZE) - 1U)) << CRYPTO_OTPM_CONTROL_EN_STANDBY__SHIFT)

#define CRYPTO_OTPM_CONTROL_USR_POWERDOWN__SIZE            1U
#define CRYPTO_OTPM_CONTROL_USR_POWERDOWN__SHIFT           4U
#define CRYPTO_OTPM_CONTROL_USR_POWERDOWN__MASK            (((uint32_t)((1UL << CRYPTO_OTPM_CONTROL_USR_POWERDOWN__SIZE) - 1U)) << CRYPTO_OTPM_CONTROL_USR_POWERDOWN__SHIFT)

#define CRYPTO_OTPM_CONTROL_IGNORE_ECC_ERR_LOWER__SIZE     1U
#define CRYPTO_OTPM_CONTROL_IGNORE_ECC_ERR_LOWER__SHIFT    5U
#define CRYPTO_OTPM_CONTROL_IGNORE_ECC_ERR_LOWER__MASK     (((uint32_t)((1UL << CRYPTO_OTPM_CONTROL_IGNORE_ECC_ERR_LOWER__SIZE) - 1U)) << CRYPTO_OTPM_CONTROL_IGNORE_ECC_ERR_LOWER__SHIFT)

#define CRYPTO_OTPM_CONTROL_DIS_PSLVERR__SIZE              1U
#define CRYPTO_OTPM_CONTROL_DIS_PSLVERR__SHIFT             6U
#define CRYPTO_OTPM_CONTROL_DIS_PSLVERR__MASK              (((uint32_t)((1UL << CRYPTO_OTPM_CONTROL_DIS_PSLVERR__SIZE) - 1U)) << CRYPTO_OTPM_CONTROL_DIS_PSLVERR__SHIFT)

#define CRYPTO_OTPM_CORE_MR_PGM_SOAK__SIZE                 16U
#define CRYPTO_OTPM_CORE_MR_PGM_SOAK__SHIFT                0U
#define CRYPTO_OTPM_CORE_MR_PGM_SOAK__MASK                 (((uint32_t)((1UL << CRYPTO_OTPM_CORE_MR_PGM_SOAK__SIZE) - 1U)) << CRYPTO_OTPM_CORE_MR_PGM_SOAK__SHIFT)

#define CRYPTO_OTPM_CORE_MR_PGM__SIZE                      16U
#define CRYPTO_OTPM_CORE_MR_PGM__SHIFT                     16U
#define CRYPTO_OTPM_CORE_MR_PGM__MASK                      (((uint32_t)((1UL << CRYPTO_OTPM_CORE_MR_PGM__SIZE) - 1U)) << CRYPTO_OTPM_CORE_MR_PGM__SHIFT)

#define CRYPTO_OTPM_CORE_MR_MRA_READ__SIZE                 16U
#define CRYPTO_OTPM_CORE_MR_MRA_READ__SHIFT                0U
#define CRYPTO_OTPM_CORE_MR_MRA_READ__MASK                 (((uint32_t)((1UL << CRYPTO_OTPM_CORE_MR_MRA_READ__SIZE) - 1U)) << CRYPTO_OTPM_CORE_MR_MRA_READ__SHIFT)

#define CRYPTO_OTPM_CORE_MR_MRA_PGM__SIZE                  16U
#define CRYPTO_OTPM_CORE_MR_MRA_PGM__SHIFT                 16U
#define CRYPTO_OTPM_CORE_MR_MRA_PGM__MASK                  (((uint32_t)((1UL << CRYPTO_OTPM_CORE_MR_MRA_PGM__SIZE) - 1U)) << CRYPTO_OTPM_CORE_MR_MRA_PGM__SHIFT)

#define CRYPTO_OTPM_CORE_MRA_PGM_SOAK__SIZE                16U
#define CRYPTO_OTPM_CORE_MRA_PGM_SOAK__SHIFT               0U
#define CRYPTO_OTPM_CORE_MRA_PGM_SOAK__MASK                (((uint32_t)((1UL << CRYPTO_OTPM_CORE_MRA_PGM_SOAK__SIZE) - 1U)) << CRYPTO_OTPM_CORE_MRA_PGM_SOAK__SHIFT)

#define CRYPTO_OTPM_CORE_MRA_READ__SIZE                    16U
#define CRYPTO_OTPM_CORE_MRA_READ__SHIFT                   16U
#define CRYPTO_OTPM_CORE_MRA_READ__MASK                    (((uint32_t)((1UL << CRYPTO_OTPM_CORE_MRA_READ__SIZE) - 1U)) << CRYPTO_OTPM_CORE_MRA_READ__SHIFT)

#define CRYPTO_OTPM_CORE_MRB_PGM__SIZE                     16U
#define CRYPTO_OTPM_CORE_MRB_PGM__SHIFT                    0U
#define CRYPTO_OTPM_CORE_MRB_PGM__MASK                     (((uint32_t)((1UL << CRYPTO_OTPM_CORE_MRB_PGM__SIZE) - 1U)) << CRYPTO_OTPM_CORE_MRB_PGM__SHIFT)

#define CRYPTO_OTPM_CORE_MRB_PGM_SOAK__SIZE                16U
#define CRYPTO_OTPM_CORE_MRB_PGM_SOAK__SHIFT               16U
#define CRYPTO_OTPM_CORE_MRB_PGM_SOAK__MASK                (((uint32_t)((1UL << CRYPTO_OTPM_CORE_MRB_PGM_SOAK__SIZE) - 1U)) << CRYPTO_OTPM_CORE_MRB_PGM_SOAK__SHIFT)

#define CRYPTO_OTPM_CORE_MRB_TDS_MRB_READ__SIZE            16U
#define CRYPTO_OTPM_CORE_MRB_TDS_MRB_READ__SHIFT           0U
#define CRYPTO_OTPM_CORE_MRB_TDS_MRB_READ__MASK            (((uint32_t)((1UL << CRYPTO_OTPM_CORE_MRB_TDS_MRB_READ__SIZE) - 1U)) << CRYPTO_OTPM_CORE_MRB_TDS_MRB_READ__SHIFT)

#define CRYPTO_OTPM_CORE_MRB_TDS_TDS__SIZE                 16U
#define CRYPTO_OTPM_CORE_MRB_TDS_TDS__SHIFT                16U
#define CRYPTO_OTPM_CORE_MRB_TDS_TDS__MASK                 (((uint32_t)((1UL << CRYPTO_OTPM_CORE_MRB_TDS_TDS__SIZE) - 1U)) << CRYPTO_OTPM_CORE_MRB_TDS_TDS__SHIFT)

#define CRYPTO_OTPM_CORE_PROG_TIME_TPAS__SIZE              16U
#define CRYPTO_OTPM_CORE_PROG_TIME_TPAS__SHIFT             0U
#define CRYPTO_OTPM_CORE_PROG_TIME_TPAS__MASK              (((uint32_t)((1UL << CRYPTO_OTPM_CORE_PROG_TIME_TPAS__SIZE) - 1U)) << CRYPTO_OTPM_CORE_PROG_TIME_TPAS__SHIFT)

#define CRYPTO_OTPM_CORE_PROG_TIME_TPP__SIZE               16U
#define CRYPTO_OTPM_CORE_PROG_TIME_TPP__SHIFT              16U
#define CRYPTO_OTPM_CORE_PROG_TIME_TPP__MASK               (((uint32_t)((1UL << CRYPTO_OTPM_CORE_PROG_TIME_TPP__SIZE) - 1U)) << CRYPTO_OTPM_CORE_PROG_TIME_TPP__SHIFT)

#define CRYPTO_OTPM_CORE_RECOVERY_TPR__SIZE                16U
#define CRYPTO_OTPM_CORE_RECOVERY_TPR__SHIFT               0U
#define CRYPTO_OTPM_CORE_RECOVERY_TPR__MASK                (((uint32_t)((1UL << CRYPTO_OTPM_CORE_RECOVERY_TPR__SIZE) - 1U)) << CRYPTO_OTPM_CORE_RECOVERY_TPR__SHIFT)

#define CRYPTO_OTPM_CORE_RECOVERY_TPSR__SIZE               16U
#define CRYPTO_OTPM_CORE_RECOVERY_TPSR__SHIFT              16U
#define CRYPTO_OTPM_CORE_RECOVERY_TPSR__MASK               (((uint32_t)((1UL << CRYPTO_OTPM_CORE_RECOVERY_TPSR__SIZE) - 1U)) << CRYPTO_OTPM_CORE_RECOVERY_TPSR__SHIFT)

#define CRYPTO_OTPM_CORE_SOAK_CYCLES_TSPP__SIZE            32U
#define CRYPTO_OTPM_CORE_SOAK_CYCLES_TSPP__SHIFT           0U
#define CRYPTO_OTPM_CORE_SOAK_CYCLES_TSPP__MASK            (((uint32_t)((1UL << CRYPTO_OTPM_CORE_SOAK_CYCLES_TSPP__SIZE) - 1U)) << CRYPTO_OTPM_CORE_SOAK_CYCLES_TSPP__SHIFT)

#define CRYPTO_OTPM_CORE_PROGRAM_TVPPS__SIZE               16U
#define CRYPTO_OTPM_CORE_PROGRAM_TVPPS__SHIFT              0U
#define CRYPTO_OTPM_CORE_PROGRAM_TVPPS__MASK               (((uint32_t)((1UL << CRYPTO_OTPM_CORE_PROGRAM_TVPPS__SIZE) - 1U)) << CRYPTO_OTPM_CORE_PROGRAM_TVPPS__SHIFT)

#define CRYPTO_OTPM_CORE_PROGRAM_SOAK_COUNT_MAX__SIZE      4U
#define CRYPTO_OTPM_CORE_PROGRAM_SOAK_COUNT_MAX__SHIFT     16U
#define CRYPTO_OTPM_CORE_PROGRAM_SOAK_COUNT_MAX__MASK      (((uint32_t)((1UL << CRYPTO_OTPM_CORE_PROGRAM_SOAK_COUNT_MAX__SIZE) - 1U)) << CRYPTO_OTPM_CORE_PROGRAM_SOAK_COUNT_MAX__SHIFT)

#define CRYPTO_OTPM_CORE_WRITE_TIME_TWR__SIZE              16U
#define CRYPTO_OTPM_CORE_WRITE_TIME_TWR__SHIFT             0U
#define CRYPTO_OTPM_CORE_WRITE_TIME_TWR__MASK              (((uint32_t)((1UL << CRYPTO_OTPM_CORE_WRITE_TIME_TWR__SIZE) - 1U)) << CRYPTO_OTPM_CORE_WRITE_TIME_TWR__SHIFT)

#define CRYPTO_OTPM_CORE_WRITE_TIME_TWP__SIZE              16U
#define CRYPTO_OTPM_CORE_WRITE_TIME_TWP__SHIFT             16U
#define CRYPTO_OTPM_CORE_WRITE_TIME_TWP__MASK              (((uint32_t)((1UL << CRYPTO_OTPM_CORE_WRITE_TIME_TWP__SIZE) - 1U)) << CRYPTO_OTPM_CORE_WRITE_TIME_TWP__SHIFT)

#define CRYPTO_OTPM_CORE_READ_TIME_TRAS__SIZE              16U
#define CRYPTO_OTPM_CORE_READ_TIME_TRAS__SHIFT             0U
#define CRYPTO_OTPM_CORE_READ_TIME_TRAS__MASK              (((uint32_t)((1UL << CRYPTO_OTPM_CORE_READ_TIME_TRAS__SIZE) - 1U)) << CRYPTO_OTPM_CORE_READ_TIME_TRAS__SHIFT)

#define CRYPTO_OTPM_CORE_READ_TIME_TRR__SIZE               16U
#define CRYPTO_OTPM_CORE_READ_TIME_TRR__SHIFT              16U
#define CRYPTO_OTPM_CORE_READ_TIME_TRR__MASK               (((uint32_t)((1UL << CRYPTO_OTPM_CORE_READ_TIME_TRR__SIZE) - 1U)) << CRYPTO_OTPM_CORE_READ_TIME_TRR__SHIFT)

#define CRYPTO_OTPM_CORE_READ_ERROR_ADDRESS__SIZE          16U
#define CRYPTO_OTPM_CORE_READ_ERROR_ADDRESS__SHIFT         0U
#define CRYPTO_OTPM_CORE_READ_ERROR_ADDRESS__MASK          (((uint32_t)((1UL << CRYPTO_OTPM_CORE_READ_ERROR_ADDRESS__SIZE) - 1U)) << CRYPTO_OTPM_CORE_READ_ERROR_ADDRESS__SHIFT)

#define CRYPTO_OTPM_CORE_ECC_FLAGS_SEC__SIZE               1U
#define CRYPTO_OTPM_CORE_ECC_FLAGS_SEC__SHIFT              0U
#define CRYPTO_OTPM_CORE_ECC_FLAGS_SEC__MASK               (((uint32_t)((1UL << CRYPTO_OTPM_CORE_ECC_FLAGS_SEC__SIZE) - 1U)) << CRYPTO_OTPM_CORE_ECC_FLAGS_SEC__SHIFT)

#define CRYPTO_OTPM_CORE_ECC_FLAGS_DED__SIZE               1U
#define CRYPTO_OTPM_CORE_ECC_FLAGS_DED__SHIFT              1U
#define CRYPTO_OTPM_CORE_ECC_FLAGS_DED__MASK               (((uint32_t)((1UL << CRYPTO_OTPM_CORE_ECC_FLAGS_DED__SIZE) - 1U)) << CRYPTO_OTPM_CORE_ECC_FLAGS_DED__SHIFT)


//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

typedef struct
{
    __IOM uint32_t control;
    __IOM uint32_t core_mr;
    __IOM uint32_t core_mr_mra;
    __IOM uint32_t core_mra;
    __IOM uint32_t core_mrb;
    __IOM uint32_t core_mrb_tds;
    __IOM uint32_t core_prog_time;
    __IOM uint32_t core_recovery;
    __IOM uint32_t core_soak_cycles;
    __IOM uint32_t core_program;
    __IOM uint32_t core_write_time;
    __IOM uint32_t core_read_time;
    __IOM uint32_t core_read_error;
    __IOM uint32_t core_ecc_flags;
} CRYPTO_OTPM_REGS_T;

#endif /* CRYPTO_OTPM_H */
