#ifndef PATCHER_H
#define PATCHER_H
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

#define PATCHER_COMPARATOR_00                              (PATCHER_REG_BASE + 0x0000U)
#define PATCHER_COMPARATOR_01                              (PATCHER_REG_BASE + 0x0004U)
#define PATCHER_COMPARATOR_02                              (PATCHER_REG_BASE + 0x0008U)
#define PATCHER_COMPARATOR_03                              (PATCHER_REG_BASE + 0x000CU)
#define PATCHER_COMPARATOR_04                              (PATCHER_REG_BASE + 0x0010U)
#define PATCHER_COMPARATOR_05                              (PATCHER_REG_BASE + 0x0014U)
#define PATCHER_COMPARATOR_06                              (PATCHER_REG_BASE + 0x0018U)
#define PATCHER_COMPARATOR_07                              (PATCHER_REG_BASE + 0x001CU)
#define PATCHER_COMPARATOR_08                              (PATCHER_REG_BASE + 0x0020U)
#define PATCHER_COMPARATOR_09                              (PATCHER_REG_BASE + 0x0024U)
#define PATCHER_COMPARATOR_10                              (PATCHER_REG_BASE + 0x0028U)
#define PATCHER_COMPARATOR_11                              (PATCHER_REG_BASE + 0x002CU)
#define PATCHER_COMPARATOR_12                              (PATCHER_REG_BASE + 0x0030U)
#define PATCHER_COMPARATOR_13                              (PATCHER_REG_BASE + 0x0034U)
#define PATCHER_COMPARATOR_14                              (PATCHER_REG_BASE + 0x0038U)
#define PATCHER_COMPARATOR_15                              (PATCHER_REG_BASE + 0x003CU)
#define PATCHER_COMPARATOR_16                              (PATCHER_REG_BASE + 0x0040U)
#define PATCHER_COMPARATOR_17                              (PATCHER_REG_BASE + 0x0044U)
#define PATCHER_COMPARATOR_18                              (PATCHER_REG_BASE + 0x0048U)
#define PATCHER_COMPARATOR_19                              (PATCHER_REG_BASE + 0x004CU)
#define PATCHER_COMPARATOR_20                              (PATCHER_REG_BASE + 0x0050U)
#define PATCHER_COMPARATOR_21                              (PATCHER_REG_BASE + 0x0054U)
#define PATCHER_COMPARATOR_22                              (PATCHER_REG_BASE + 0x0058U)
#define PATCHER_COMPARATOR_23                              (PATCHER_REG_BASE + 0x005CU)
#define PATCHER_COMPARATOR_24                              (PATCHER_REG_BASE + 0x0060U)
#define PATCHER_COMPARATOR_25                              (PATCHER_REG_BASE + 0x0064U)
#define PATCHER_COMPARATOR_26                              (PATCHER_REG_BASE + 0x0068U)
#define PATCHER_COMPARATOR_27                              (PATCHER_REG_BASE + 0x006CU)
#define PATCHER_COMPARATOR_28                              (PATCHER_REG_BASE + 0x0070U)
#define PATCHER_COMPARATOR_29                              (PATCHER_REG_BASE + 0x0074U)
#define PATCHER_COMPARATOR_30                              (PATCHER_REG_BASE + 0x0078U)
#define PATCHER_COMPARATOR_31                              (PATCHER_REG_BASE + 0x007CU)
#define PATCHER_ENABLE                                     (PATCHER_REG_BASE + 0x0080U)
#define PATCHER_INDEX                                      (PATCHER_REG_BASE + 0x0084U)
#define PATCHER_CONTROL                                    (PATCHER_REG_BASE + 0x0088U)

// Register field definitions

#define PATCHER_COMPARATOR_00_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_00_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_00_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_00_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_00_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_01_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_01_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_01_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_01_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_01_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_02_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_02_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_02_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_02_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_02_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_03_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_03_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_03_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_03_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_03_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_04_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_04_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_04_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_04_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_04_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_05_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_05_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_05_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_05_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_05_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_06_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_06_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_06_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_06_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_06_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_07_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_07_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_07_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_07_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_07_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_08_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_08_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_08_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_08_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_08_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_09_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_09_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_09_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_09_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_09_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_10_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_10_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_10_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_10_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_10_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_11_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_11_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_11_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_11_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_11_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_12_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_12_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_12_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_12_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_12_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_13_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_13_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_13_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_13_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_13_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_14_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_14_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_14_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_14_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_14_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_15_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_15_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_15_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_15_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_15_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_16_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_16_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_16_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_16_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_16_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_17_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_17_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_17_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_17_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_17_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_18_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_18_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_18_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_18_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_18_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_19_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_19_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_19_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_19_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_19_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_20_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_20_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_20_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_20_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_20_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_21_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_21_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_21_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_21_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_21_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_22_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_22_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_22_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_22_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_22_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_23_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_23_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_23_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_23_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_23_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_24_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_24_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_24_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_24_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_24_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_25_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_25_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_25_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_25_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_25_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_26_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_26_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_26_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_26_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_26_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_27_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_27_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_27_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_27_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_27_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_28_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_28_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_28_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_28_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_28_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_29_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_29_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_29_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_29_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_29_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_30_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_30_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_30_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_30_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_30_ADDRESS__SHIFT)

#define PATCHER_COMPARATOR_31_ADDRESS__SIZE                32U
#define PATCHER_COMPARATOR_31_ADDRESS__SHIFT               0U
#define PATCHER_COMPARATOR_31_ADDRESS__MASK                (((uint32_t)((1UL << PATCHER_COMPARATOR_31_ADDRESS__SIZE) - 1U)) << PATCHER_COMPARATOR_31_ADDRESS__SHIFT)

#define PATCHER_ENABLE_BITMASK__SIZE                       32U
#define PATCHER_ENABLE_BITMASK__SHIFT                      0U
#define PATCHER_ENABLE_BITMASK__MASK                       (((uint32_t)((1UL << PATCHER_ENABLE_BITMASK__SIZE) - 1U)) << PATCHER_ENABLE_BITMASK__SHIFT)

#define PATCHER_INDEX_INDEX__SIZE                          5U
#define PATCHER_INDEX_INDEX__SHIFT                         0U
#define PATCHER_INDEX_INDEX__MASK                          (((uint32_t)((1UL << PATCHER_INDEX_INDEX__SIZE) - 1U)) << PATCHER_INDEX_INDEX__SHIFT)

#define PATCHER_CONTROL_ACCESS_TYPE__SIZE                  1U
#define PATCHER_CONTROL_ACCESS_TYPE__SHIFT                 0U
#define PATCHER_CONTROL_ACCESS_TYPE__MASK                  (((uint32_t)((1UL << PATCHER_CONTROL_ACCESS_TYPE__SIZE) - 1U)) << PATCHER_CONTROL_ACCESS_TYPE__SHIFT)


//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

typedef struct
{
    __IOM uint32_t comparator_00;
    __IOM uint32_t comparator_01;
    __IOM uint32_t comparator_02;
    __IOM uint32_t comparator_03;
    __IOM uint32_t comparator_04;
    __IOM uint32_t comparator_05;
    __IOM uint32_t comparator_06;
    __IOM uint32_t comparator_07;
    __IOM uint32_t comparator_08;
    __IOM uint32_t comparator_09;
    __IOM uint32_t comparator_10;
    __IOM uint32_t comparator_11;
    __IOM uint32_t comparator_12;
    __IOM uint32_t comparator_13;
    __IOM uint32_t comparator_14;
    __IOM uint32_t comparator_15;
    __IOM uint32_t comparator_16;
    __IOM uint32_t comparator_17;
    __IOM uint32_t comparator_18;
    __IOM uint32_t comparator_19;
    __IOM uint32_t comparator_20;
    __IOM uint32_t comparator_21;
    __IOM uint32_t comparator_22;
    __IOM uint32_t comparator_23;
    __IOM uint32_t comparator_24;
    __IOM uint32_t comparator_25;
    __IOM uint32_t comparator_26;
    __IOM uint32_t comparator_27;
    __IOM uint32_t comparator_28;
    __IOM uint32_t comparator_29;
    __IOM uint32_t comparator_30;
    __IOM uint32_t comparator_31;
    __IOM uint32_t enable;
    __IM  uint32_t index;
    __IOM uint32_t control;
} PATCHER_REGS_T;

#endif /* PATCHER_H */
