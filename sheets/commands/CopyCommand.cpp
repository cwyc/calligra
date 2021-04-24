/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2007, 2009 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   SPDX-FileCopyrightText: 1999-2007 The KSpread Team <calligra-devel@kde.org>
   SPDX-FileCopyrightText: 1998, 1999 Torben Weis <weis@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "CopyCommand.h"

#include "CellStorage.h"
#include "Map.h"
#include "Region.h"
#include "RowColumnFormat.h"
#include "RowFormatStorage.h"
#include "Sheet.h"

using namespace Calligra::Sheets;

// era: encode references absolutely
QDomDocument CopyCommand::saveAsXml(const Region& region, bool era)
{
    QDomDocument xmlDoc("spreadsheet-snippet");
    xmlDoc.appendChild(xmlDoc.createProcessingInstruction("xml", "version=\"1.0\" encoding=\"UTF-8\""));
    QDomElement root = xmlDoc.createElement("spreadsheet-snippet");
    xmlDoc.appendChild(root);

    // find the upper left corner of the selection
    const QRect boundingRect = region.boundingRect();
    int left = boundingRect.left();
    int top = boundingRect.top();

    // for tiling the clipboard content in the selection
    root.setAttribute("rows", QString::number(boundingRect.height()));
    root.setAttribute("columns", QString::number(boundingRect.width()));

    const Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it = region.constBegin(); it != end; ++it) {
        Sheet *const sheet = (*it)->sheet();
        const QRect range = (*it)->rect();
        CellStorage *const storage = sheet->cellStorage();

        //
        // Entire rows selected?
        //
        if ((*it)->isRow()) {
            QDomElement rows = xmlDoc.createElement("rows");
            rows.setAttribute("count", QString::number(range.height()));
            rows.setAttribute("row", QString::number(range.top() - top + 1));
            root.appendChild(rows);

            // Save all cells.
            for (int row = range.top(); row <= range.bottom(); ++row) {
                Cell cell = storage->firstInRow(row);
                for (; !cell.isNull(); cell = storage->nextInRow(cell.column(), cell.row())) {
                    if (!cell.isPartOfMerged()) {
                        root.appendChild(cell.save(xmlDoc, 0, top - 1, era));
                    }
                }
            }

            // TODO Stefan: Inefficient, use cluster functionality
            // Save the row formats if there are any
            //const RowFormat* format;
            for (int row = range.top(); row <= range.bottom(); ++row) {
                if (!sheet->rowFormats()->isDefaultRow(row)) {
                    QDomElement e = RowFormat(sheet->rowFormats(), row).save(xmlDoc, top - 1);
                    if (!e.isNull()) {
                        rows.appendChild(e);
                    }
                }
            }
            continue;
        }

        //
        // Entire columns selected?
        //
        if ((*it)->isColumn()) {
            QDomElement columns = xmlDoc.createElement("columns");
            columns.setAttribute("count", QString::number(range.width()));
            columns.setAttribute("column", QString::number(range.left() - left + 1));
            root.appendChild(columns);

            // Save all cells.
            for (int col = range.left(); col <= range.right(); ++col) {
                Cell cell = storage->firstInColumn(col);
                for (; !cell.isNull(); cell = storage->nextInColumn(cell.column(), cell.row())) {
                    if (!cell.isPartOfMerged()) {
                        root.appendChild(cell.save(xmlDoc, left - 1, 0, era));
                    }
                }
            }

            // TODO Stefan: Inefficient, use the cluster functionality
            // Save the column formats if there are any
            const ColumnFormat* format;
            for (int col = range.left(); col <= range.right(); ++col) {
                format = sheet->columnFormat(col);
                if (format && !format->isDefault()) {
                    QDomElement e = format->save(xmlDoc, left - 1);
                    if (!e.isNull()) {
                        columns.appendChild(e);
                    }
                }
            }
            continue;
        }

        // Save all cells.
        Cell cell;
        for (int row = range.top(); row <= range.bottom(); ++row) {
            if (range.left() == 1) {
                cell = storage->firstInRow(row);
            } else {
                cell = storage->nextInRow(range.left() - 1, row);
            }
            while (!cell.isNull() && cell.column() >= range.left() && cell.column() <= range.right()) {
                if (!cell.isPartOfMerged()) {
                    root.appendChild(cell.save(xmlDoc, left - 1, top - 1, era));
                }
                cell = storage->nextInRow(cell.column(), cell.row());
            }
        }
    }
    return xmlDoc;
}

static QString cellAsText(const Cell& cell, bool addTab)
{
    QString result;
    if (!cell.isDefault()) {
        result += cell.displayText();
    }
    if (addTab) {
        result += '\t';
    }
    return result;
}

QString CopyCommand::saveAsPlainText(const Region &region)
{
    // Only one cell selected? => copy active cell
    if (region.isSingular()) {
        const Cell cell(region.firstSheet(), region.firstRange().topLeft());
        return cell.displayText();
    }

    QString result;
    Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
      if (result.length()) result += QLatin1Char('\n');
      Region::Element *el = *it;
      QRect used = el->sheet()->usedArea (true);
      QRect rect = el->rect().intersected (used);
      for (int row = rect.top(); row <= rect.bottom(); ++row) {
        for (int col = rect.left(); col <= rect.right(); ++col) {
          Cell cell (el->sheet(), col, row);
          result += cellAsText (cell, col != rect.right());
        }
        result += QLatin1Char('\n');
      }
    }
    return result;
}

QDomDocument CopyCommand::saveAsHtml(const Region &region)
{
    QDomDocument doc("spreadsheet-html");
    QDomElement html = doc.createElement("html");
    doc.appendChild(html);
    QDomElement body = doc.createElement("body");
    html.appendChild(body);
    QDomElement table = doc.createElement("table");
    body.appendChild(table);

    const Region::ConstIterator end(region.constEnd());
    for (Region::ConstIterator it(region.constBegin()); it != end; ++it) {
        Sheet *const sheet = (*it)->sheet();
        const QRect range = (*it)->rect();

        // TODO
        Q_UNUSED(sheet);
        Q_UNUSED(range);
    }
    return doc;
}

QString CopyCommand::saveAsCSV(const Region &region)
{
    // TODO
    Q_UNUSED(region);
    return QString();
}
