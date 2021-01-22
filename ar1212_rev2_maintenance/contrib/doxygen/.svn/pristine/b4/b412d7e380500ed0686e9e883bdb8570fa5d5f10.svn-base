//******************************************************************************
/// \file
///
/// A detailed description of what the file contains and how it fits into the
/// grand scheme of things. The detailed description can span multiple lines in
/// which case the first sentence is treated as the brief description.
///
/// \addtogroup doxygen_template
/// @{
//******************************************************************************
// Copyright 2017 ON Semiconductor. All rights reserved. This software and/or
// documentation is licensed by ON Semiconductor under limited terms and
// conditions. The terms and conditions pertaining to the software and/or
// documentation are available at http://www.onsemi.com/site/pdf/ONSEMI_T&C.pdf
// ("ON Semiconductor Standard Terms and Conditions of Sale, Section 8 Software").
// Do not use this software and/or documentation unless you have carefully read
// and you agree to the limited terms and conditions. By using this software
// and/or documentation, you agree to the limited terms and conditions.
//******************************************************************************

// Major block comments should be complete sentences.  They are preceded
// by a blank line, and may optionally be followed by a blank line.
// Major block comments give high level descriptions of code
// structure and intention, including formulas, assumptions, and
// dependencies.  The following is an example of the kind of detail that should
// be included.

#include "template.h"            // if not obvious, say why it's needed

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

#define CONSTANT_VALUE1     10  ///< Tab-align (with spaces)
#define CONSTANT_VALUE2     20  ///< Line up for a neat look

const int CONSTANT_INT = 30;    ///< Doxygen comment

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

// typedefs local to this module

//------------------------------------------------------------------------------
//                                  Macros
//------------------------------------------------------------------------------

// macros local to this module

//------------------------------------------------------------------------------
//                                  Locals
//------------------------------------------------------------------------------

// PRIVATE helper functions - only called by this module

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

// Global variables exported by this component

/// The scheduler global variable
SCHEDULER_T  g_scheduler;

// PUBLIC and PRIVATE functions exported by this module

/// Brief description of foo function.
///
/// Detailed description of foo function, including ranges and restrictions of
/// the arguments and any returned values. The description should also define
/// units and implementation constraints.
///
/// \param[in]  arg1    Description of argument 1.
/// \param[out] arg2    Description of argument 2.
/// \param[in]  arg3    Description of argument 3.
/// \returns Description of what the function returns.
//
PRIVATE int foo(int arg1, int *arg2, char arg3)
{
    fint local1, local2 = 10;

    // Braces may only be omitted in the unusual case that the statement cannot
    // contain a macro & there are many similar single line ifs together, such
    // that inserting braces would make the code cumbersome to read
    if (NULL == arg2)   return;
    if (VALUE1 == arg2) return;
    if (VALUE2 == arg2) return;

    // An if with braces.  Note blank line following above if statement.
    if (arg1 == local2)
    {
        fint i;

        // A 'for' without braces, on the same line
        for (i = 0; i < 10; i++) arg1--;

        // A 'for' with braces.
        for (i = 10; i > 0; i--)
        {
            // Doing some stuff here.
        }
    }

    // TODO: <MF> need to add comment about todo comments with syncnote reference

    // Even for single statement if's, the braces are required
    if (local2 < 10)
    {
        *arg2 = 10;
    }
}

/// Brief description of bar function.
///
/// Detailed description of bar function, including ranges and restrictions of
/// the arguments and any returned values. The descriptions should also define
/// units and implementation constraints. In this case the values assigned to
/// analogGain must be documented.
///
/// \returns Description of what the function returns.
///
PRIVATE int bar(void)
{
    int j;

    j = 0;
    while (j < 10)
    {
        // Do some calculations using global variables that
        // represent analog gain and digital gain.
        j = analogGain * digitalGain;
    }

    // Use foo to refine j.  This call can be coded on one line if it fits
    // within the 120 character line limit. Otherwise, line up the arguments
    // for a clean, organized look.
    foo(20,
        &j,
        'Y');

    foo(30, &j, 'N');

    do
    {
        j--;

    } while (j > 0);

    switch (j)
    {
    case 1:
        analogGain += 2;
        break;

    case 2:
        analogGain += digitalGain;
        digitalGain = 0;
        break;

    default:
        analogGain = digitalGain   +
                     someOtherGain +
                     A_CONSTANT_GAIN;
        break;
    }

    return j;
}

/// Brief description of foo_bar function.
///
/// This function shows how to declare a routine with a lot of arguments. This
/// could be in a header file or source file. Alignment of text keeps it neat
/// and readable.
///
PUBLIC FANCY_TYPE_T foo_bar(A_STRUCT_NAME1_T     * name1Struct,
                            B_STRUCT_LONG_NAME_T * name2LongStruct);

/// @}

