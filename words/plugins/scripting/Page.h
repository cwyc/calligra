/* This file is part of the Calligra project
 * SPDX-FileCopyrightText: 2006 Sebastian Sauer <mail@dipe.org>
 * SPDX-FileCopyrightText: 2005, 2007-2009 Thomas Zander <zander@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef SCRIPTING_PAGE_H
#define SCRIPTING_PAGE_H

#include <QObject>
#include <KWPage.h>

#include "Module.h"

namespace Scripting
{

/**
* The Page class represents a printed page in the document.
*/
class Page : public QObject
{
    Q_OBJECT
public:
    Page(Module* module, KWPage &page)
            : QObject(module), m_page(page) {}
    virtual ~Page() {}

public Q_SLOTS:

    /** Return the number of this page as it will be shown to the user. */
    int pageNumber() const {
        return m_page.pageNumber();
    }

    /** Return the pageside of this page.
    *
    * The returned string could be one of the following;
    * \li "Left" for left page used for even-numbered pages.
    * \li "Right" for right page used for odd numbered pages.
    * \li "Spread" for page spread which is one page that represents 2 pagenumbers.
    */
    QString pageSide() const {
        switch (m_page.pageSide()) {
        case KWPage::Left: return "Left";
        case KWPage::Right: return "Right";
        case KWPage::PageSpread: return "Spread";
        }
        return QString();
    }
    /** Set the pageside of this page. See the pageSide() method above for a
    list of valid arguments. */
    void setPageSide(const QString& ps) {
        if (ps == "Left") m_page.setPageSide(KWPage::Left);
        else if (ps == "Right") m_page.setPageSide(KWPage::Right);
        else if (ps == "Spread") m_page.setPageSide(KWPage::PageSpread);
    }

    /** Return the width of this page in pt. */
    qreal width() const {
        return m_page.width();
    }

    /** Return the height of this page in pt. */
    qreal height() const {
        return m_page.height();
    }

    /** Return the height of the margin at top in pt. */
    qreal topMargin() const {
        return m_page.topMargin();
    }

    /** Return the height of the margin at bottom in pt. */
    qreal bottomMargin() const {
        return m_page.bottomMargin();
    }

    /** Return the width of the margin at left in pt. */
    qreal leftMargin() const {
        return m_page.leftMargin();
    }

    /** Return the width of the margin at right in pt. */
    qreal rightMargin() const {
        return m_page.rightMargin();
    }

private:
    KWPage m_page;
};

}

#endif
