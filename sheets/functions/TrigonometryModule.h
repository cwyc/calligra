/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2008 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KSPREAD_TRIGONOMETRY_MODULE
#define KSPREAD_TRIGONOMETRY_MODULE

#include "FunctionModule.h"

#include <QVariantList>

namespace Calligra
{
namespace Sheets
{

class TrigonometryModule : public FunctionModule
{
    Q_OBJECT
public:
    explicit TrigonometryModule(QObject* parent, const QVariantList& args = QVariantList());

    QString descriptionFileName() const override;
};

} // namespace Sheets
} // namespace Calligra

#endif // KSPREAD_TRIGONOMETRY_MODULE
