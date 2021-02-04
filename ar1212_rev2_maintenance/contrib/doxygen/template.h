#if !defined(__DOXYGEN__TEMPLATE_H__)  
#define __DOXYGEN__TEMPLATE_H__
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

//------------------------------------------------------------------------------
//                                  Constants
//------------------------------------------------------------------------------

#define TEMPLATE_INTERESTING_VALUE   1037 ///< Doxygen comment for define

//------------------------------------------------------------------------------
//                                  Types
//------------------------------------------------------------------------------

// Structure Definitions

/// Brief description of the structure and its purpose.
typedef struct
{
    uint8 element1;                  ///< Doxygen comment for element1
    int16 element2;                  ///< Doxygen comment for element2
} TEMPLATE_FOO_T;

// Specify a struct tag name if the structure definition includes a pointer to
// itself (otherwise the tag is not necessary). Note the element alignment.

/// Brief description of the structure and its purpose.
///
/// After a blank line, the detailed description can follow e.g. to describe
/// how the structure is used as a linked list.
typedef struct TEMPLATE_BAR_TAG
{
    uint8 element1;                 ///< Doxygen comment for element1
    int16 element2;                 ///< Doxygen comment for element2
    struct TEMPLATE_BAR_TAG *next;  ///< Doxygen comment for linked list
} TEMPLATE_BAR_T;

//------------------------------------------------------------------------------
//                                  Macros
//------------------------------------------------------------------------------

// Public macros (invoked by external components)

/// Description of the macro purpose.
#define TEMPLATE_VARNAMESCHED_DECL_NAME(name)   AS_STRING(name)

//------------------------------------------------------------------------------
//                                  Exports
//------------------------------------------------------------------------------

// All public functions exported by this component

/// Brief description of sched_create_task function.
///
/// Detailed description of function e.g. this function creates a new task,
/// with entry-point \a entry, and with optional task context \a context.
///
/// The \a isRT flag is used to specify if the task is real-time. A real-time
/// task will \b always be scheduled before any non-real-time task (regardless
/// of priority). A real-time task cannot be descheduled. In contrast, a
/// running non-real-time task will be descheduled if a real-time task becomes
/// ready-to-run (even if it is lower priority). All ready-to-run real-time
/// tasks will be scheduled before the non-real-time task is resumed.
///
/// \param[in] entry    The entry-point to the task
/// \param[in] context  Optional pointer to task-specific context (or NULL)
/// \param[in] isRT     Flag to indicate if task is real-time
/// \param[in] name     Optional pointer to the name of the task
///
/// \returns The (abstract) task handle (or \c NULL_TASK on error)
///
extern TASK_HANDLE_T sched_create_task(TASK_ENTRY_FN entry,
                                       TASK_CONTEXT_T context,
                                       bool isRT,
                                       SCHED_NAME_T name);

// End of Doxygen group
/// @}

#endif // !defined(__DOXYGEN__TEMPLATE_H__)

