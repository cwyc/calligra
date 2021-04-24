/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef CALLIGRA_SHEETS_DELETE_COMMAND
#define CALLIGRA_SHEETS_DELETE_COMMAND

#include "DataManipulators.h"

#include <QSet>

namespace Calligra
{
namespace Sheets
{
class ColumnFormat;
class RowFormat;

/**
 * \ingroup Commands
 * \brief Deletes cell contents and column/row styles.
 */
class DeleteCommand : public AbstractDataManipulator
{
public:
    /**
     * Constructor.
     */
    explicit DeleteCommand(KUndo2Command *parent = 0);

    /**
     * Destructor.
     */
    ~DeleteCommand() override;

    enum Mode {
        Everything,     ///< Delete also column and row formats.
        OnlyCells       ///< Delete only cell contents, styles, etc.
    };
    void setMode(Mode mode);

protected:
    /**
     * Processes \p element , a Region::Point or a Region::Range .
     * Invoked by mainProcessing() .
     */
    bool process(Element* element) override;

    bool mainProcessing() override;

    // dummy
    Value newValue(Element*, int, int, bool*, Format::Type*) override {
        return Value();
    }

protected:
    QSet<ColumnFormat*> m_columnFormats;
    QSet<RowFormat*>   m_rowFormats;
    Mode m_mode;
};

} // namespace Sheets
} // namespace Calligra

#endif // CALLIGRA_SHEETS_DELETE_COMMAND
