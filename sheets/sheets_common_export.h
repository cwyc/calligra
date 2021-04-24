/*  This file is part of the KDE project
    SPDX-FileCopyrightText: 2007 David Faure <faure@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef SHEETS_COMMON_EXPORT_H_WRAPPER
#define SHEETS_COMMON_EXPORT_H_WRAPPER

/* This is the one which does the actual work, generated by cmake */
#include "sheets_common_generated_export.h"

/* Now the same for CALLIGRA_SHEETS_COMMON_TEST_EXPORT, if compiling with unit tests enabled */
#ifdef COMPILING_TESTS
#   define CALLIGRA_SHEETS_COMMON_TEST_EXPORT CALLIGRA_SHEETS_COMMON_EXPORT
#else /* not compiling tests */
#   define CALLIGRA_SHEETS_COMMON_TEST_EXPORT
#endif

#endif
