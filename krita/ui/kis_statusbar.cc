/* This file is part of KimageShop^WKrayon^WKrita
 *
 *  Copyright (c) 2006 Boudewijn Rempt <boud@valdyas.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "kis_statusbar.h"


#include <QLabel>
#include <QFontMetrics>
#include <KoProgressBar.h>
#include <ksqueezedtextlabel.h>
#include <kstatusbar.h>
#include <klocale.h>

#include <KoColorProfile.h>
#include <KoColorSpace.h>

#include <kis_types.h>
#include <kis_image.h>
#include <kis_selection.h>
#include <kis_paint_device.h>

#include "kis_view2.h"
#include "canvas/kis_canvas2.h"
#include "KoViewConverter.h"
#include "KoProgressBar.h"

enum {
    IMAGE_SIZE_ID,
    POINTER_POSITION_ID
};

KisStatusBar::KisStatusBar(KStatusBar * sb, KisView2 * view )
    : m_view( view )
    , m_statusbar( sb )
{
    m_selectionStatusLabel = new QLabel(sb);
    m_selectionStatusLabel->setPixmap(KIcon("tool_rect_selection").pixmap(22));
    m_selectionStatusLabel->setEnabled(false);
    sb->addWidget(m_selectionStatusLabel);

    sb->insertFixedItem("99999 x 99999", IMAGE_SIZE_ID);
    sb->insertFixedItem("99999, 99999", POINTER_POSITION_ID);

    sb->changeItem("", POINTER_POSITION_ID);
    sb->changeItem("", IMAGE_SIZE_ID);

    // XXX: Use the KStatusbar fixed size labels!
    m_statusBarStatusLabel = new KSqueezedTextLabel(sb);
    connect( KoToolManager::instance(), SIGNAL(changedStatusText(const QString &)),
             m_statusBarStatusLabel, SLOT(setText(const QString &)) );
    sb->addWidget(m_statusBarStatusLabel,2);

    m_statusBarProfileLabel = new KSqueezedTextLabel(sb);
    sb->addWidget(m_statusBarProfileLabel,3);

    //int height = m_statusBarProfileLabel->height();

    m_progress = new KoProgressBar(sb);
    m_progress->setMaximumWidth(225);
    m_progress->setMinimumWidth(225);
    m_progress->setMaximumHeight(sb->fontMetrics().height() );
    sb->addPermanentWidget(m_progress, 2);

    m_progress->hide();
}

KisStatusBar::~KisStatusBar()
{
}

#define EPSILON 1e-6

void KisStatusBar::setZoom( int zoom )
{
    Q_UNUSED(zoom);
/*
    if (zoom < 1 - EPSILON) {
        m_statusBarZoomLabel->setText(i18n("Zoom %1%",zoom * 100, 0, 'g', 4));
    } else {
        m_statusBarZoomLabel->setText(i18n("Zoom %1%",zoom * 100, 0, 'f', 0));
    }
*/}

void KisStatusBar::documentMousePositionChanged( const QPointF &pos )
{
    QPoint pixelPos = m_view->image()->documentToIntPixel(pos);

    pixelPos.setX(qBound(0, pixelPos.x(), m_view->image()->width() - 1));
    pixelPos.setY(qBound(0, pixelPos.y(), m_view->image()->height() - 1));
    m_statusbar->changeItem(QString("%1, %2").arg(pixelPos.x()).arg(pixelPos.y()), POINTER_POSITION_ID);
}

void KisStatusBar::imageSizeChanged( qint32 w, qint32 h )
{
    m_statusbar->changeItem(QString("%1 x %2").arg(w).arg(h), IMAGE_SIZE_ID);
}

void KisStatusBar::setSelection( KisImageSP img )
{
    Q_UNUSED( img );

    KisSelectionSP selection = m_view->selection();
    if ( selection ) {
        m_selectionStatusLabel->setEnabled(true);

        QRect r = selection->selectedExactRect();
        m_selectionStatusLabel->setToolTip( i18n("Selection Active: x = %1 y = %2 width = %3 height = %4",r.x(),r.y(), r.width(), r.height()));
        return;
    }
    m_selectionStatusLabel->setEnabled(false);
    m_selectionStatusLabel->setToolTip(i18n("No Selection"));
}

void KisStatusBar::setProfile( KisImageSP img )
{
    if (m_statusBarProfileLabel == 0) {
        return;
    }

    if (!img) return;

    if (img->profile() == 0) {
        m_statusBarProfileLabel->setText(i18n("No profile"));
    }
    else {
        m_statusBarProfileLabel->setText(img->colorSpace()->name() + "  " + img->profile()->name());
    }

}

void KisStatusBar::setHelp( const QString &t )
{
    Q_UNUSED(t);
}

void KisStatusBar::updateStatusBarProfileLabel()
{
    setProfile( m_view->image() );
}

#include "kis_statusbar.moc"
