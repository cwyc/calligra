/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2002-2003 Norbert Andres <nandres@web.de>
   SPDX-FileCopyrightText: 2002 Philipp Mueller <philipp.mueller@gmx.de>
   SPDX-FileCopyrightText: 2002 John Dailey <dailey@vt.edu>
   SPDX-FileCopyrightText: 1999-2001 Laurent Montel <montel@kde.org>
   SPDX-FileCopyrightText: 1998-1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_ADD_NAMED_AREA_DIALOG
#define CALLIGRA_SHEETS_ADD_NAMED_AREA_DIALOG

#include <KoDialog.h>

class KLineEdit;

namespace Calligra
{
namespace Sheets
{
class Selection;

/**
 * \ingroup UI
 * Dialog to add a named area.
 */
class AddNamedAreaDialog : public KoDialog
{
    Q_OBJECT

public:
    AddNamedAreaDialog(QWidget* parent, Selection* selection);

public Q_SLOTS:
    void slotOk();
    void slotAreaNameChanged(const QString& name);

protected:
    Selection*  m_selection;
    KLineEdit*  m_areaName;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_ADD_NAMED_AREA_DIALOG
