/* This file is part of the KDE project
   Copyright 2009 Thomas Zander <zander@kde.org>
   Copyright 2006-2007 Stefan Nikolaus <stefan.nikolaus@kdemail.net>
   Copyright 2006 Robert Knight <robertknight@gmail.com>
   Copyright 2006 Inge Wallin <inge@lysator.liu.se>
   Copyright 1999-2002,2004 Laurent Montel <montel@kde.org>
   Copyright 2002-2005 Ariya Hidayat <ariya@kde.org>
   Copyright 1999-2004 David Faure <faure@kde.org>
   Copyright 2004-2005 Meni Livne <livne@kde.org>
   Copyright 2001-2003 Philipp Mueller <philipp.mueller@gmx.de>
   Copyright 2002-2003 Norbert Andres <nandres@web.de>
   Copyright 2003 Hamish Rodda <rodda@kde.org>
   Copyright 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright 2003 Lukas Tinkl <lukas@kde.org>
   Copyright 2000-2002 Werner Trobin <trobin@kde.org>
   Copyright 2002 Harri Porten <porten@kde.org>
   Copyright 2002 John Dailey <dailey@vt.edu>
   Copyright 2002 Daniel Naber <daniel.naber@t-online.de>
   Copyright 1999-2000 Torben Weis <weis@kde.org>
   Copyright 1999-2000 Stephan Kulow <coolo@kde.org>
   Copyright 2000 Bernd Wuebben <wuebben@kde.org>
   Copyright 2000 Wilco Greven <greven@kde.org>
   Copyright 2000 Simon Hausmann <hausmann@kde.org
   Copyright 1999 Michael Reiher <michael.reiher@gmx.de>
   Copyright 1999 Boris Wedl <boris.wedl@kfunigraz.ac.at>
   Copyright 1999 Reginald Stadlbauer <reggie@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

// Local
#include "CanvasItem.h"

// std
#include <assert.h>
#include <float.h>
#include <stdlib.h>

// Qt
#include <QApplication>
#include <QBuffer>
#include <QByteArray>
#include <QClipboard>
#include <QDragLeaveEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QList>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QPoint>
#include <QScrollBar>
#include <QTextStream>
#include <QToolTip>
#include <QWidget>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneDragDropEvent>

// KDE
#include <kcursor.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <krun.h>
#include <kmimetype.h>
#include <ksharedptr.h>
#include <kwordwrap.h>
#include <kxmlguifactory.h>

// KOffice
#include <KoCanvasController.h>
#include <KoShapeManager.h>
#include <KoStore.h>
#include <KoToolManager.h>
#include <KoToolProxy.h>
#include <KoXmlWriter.h>
#include <KoZoomHandler.h>

// KSpread
#include "CellStorage.h"
#include "Doc.h"
#include "Global.h"
#include "HeaderItems.h"
#include "Localization.h"
#include "Map.h"
#include "RowColumnFormat.h"
#include "Selection.h"
#include "Sheet.h"
#include "Util.h"
#include "Validity.h"
#include "View.h"

// commands
#include "commands/CopyCommand.h"
#include "commands/DeleteCommand.h"
#include "commands/PasteCommand.h"
#include "commands/StyleCommand.h"
#include "commands/Undo.h"

// ui
#include "ui/CellView.h"
#include "ui/SheetView.h"

#define MIN_SIZE 10

using namespace KSpread;

/****************************************************************
 *
 * Canvas
 *
 ****************************************************************/
class CanvasItem::Private
{
public:
    View *view;

    // Non-visible range top-left from current screen
    // Example: If the first visible column is 'E', then offset stores
    // the width of the invisible columns 'A' to 'D'.
    // Example: If the first visible row is '5', then offset stores
    // the height of the invisible rows '1' to '4'.
    QPointF offset;

    QLabel *validationInfo;

    // flake
    KoShapeManager* shapeManager;
    QPointer<KoToolProxy> toolProxy;
};

CanvasItem::CanvasItem(View *view)
        : QGraphicsWidget(0)
        , KoCanvasBase(0)
        , d(new Private)
{
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_StaticContents);
    //setBackgroundRole(QPalette::Base);

    d->validationInfo = 0;

    QGraphicsWidget::setFocusPolicy(Qt::StrongFocus);

    d->offset = QPointF(0.0, 0.0);

    d->view = view;

    //setMouseTracking(true);
    setAcceptHoverEvents(true);

    installEventFilter(this);   // for TAB key processing, otherwise focus change
    setAcceptDrops(true);
    setAttribute(Qt::WA_InputMethodEnabled, true); // ensure using the InputMethod

    // flake
    d->shapeManager = new KoShapeManager(this);
    d->toolProxy = new KoToolProxy(this);
}

CanvasItem::~CanvasItem()
{
    delete d->shapeManager;
    delete d->toolProxy;
    delete d->validationInfo;
    delete d;
}

View* CanvasItem::view() const
{
    return d->view;
}

Doc* CanvasItem::doc() const
{
    return d->view->doc();
}

void CanvasItem::gridSize(qreal* horizontal, qreal* vertical) const
{
    *horizontal = doc()->map()->defaultColumnFormat()->width();
    *vertical = doc()->map()->defaultRowFormat()->height();
}

bool CanvasItem::snapToGrid() const
{
    return false; // FIXME
}

void CanvasItem::addCommand(QUndoCommand* command)
{
    doc()->addCommand(command);
}

KoShapeManager* CanvasItem::shapeManager() const
{
    return d->shapeManager;
}

void CanvasItem::updateCanvas(const QRectF& rc)
{
    QRect clipRect(viewConverter()->documentToView(rc.translated(-offset())).toRect());
    clipRect.adjust(-2, -2, 2, 2);   // Resize to fit anti-aliasing
    update(clipRect);
}

const KoViewConverter* CanvasItem::viewConverter() const
{
    return view()->zoomHandler();
}

KoUnit CanvasItem::unit() const
{
    return doc()->unit();
}

KoToolProxy* CanvasItem::toolProxy() const
{
    return d->toolProxy;
}

QPointF CanvasItem::offset() const
{
    return d->offset;
}

double CanvasItem::xOffset() const
{
    return d->offset.x();
}

double CanvasItem::yOffset() const
{
    return d->offset.y();
}

bool CanvasItem::eventFilter(QObject *o, QEvent *e)
{
    /* this canvas event filter acts on events sent to the line edit as well
       as events to this filter itself.
    */
    if (!o || !e)
        return true;
    switch (e->type()) {
    case QEvent::KeyPress: {
        QKeyEvent * keyev = static_cast<QKeyEvent *>(e);
        if ((keyev->key() == Qt::Key_Tab) || (keyev->key() == Qt::Key_Backtab)) {
            keyPressEvent(keyev);
            return true;
        }
        break;
    }
    case QEvent::InputMethod: {
        //QIMEvent * imev = static_cast<QIMEvent *>(e);
        //processIMEvent( imev );
        //break;
    }
    case QEvent::ToolTip: {
        QHelpEvent* helpEvent = static_cast<QHelpEvent*>(e);
        showToolTip(helpEvent->pos());
    }
    default:
        break;
    }
    return false;
}

Selection* CanvasItem::selection() const
{
    return d->view->selection();
}

ColumnHeaderItem* CanvasItem::columnHeader() const
{
    return 0;
    //return d->view->columnHeader();
}

RowHeaderItem* CanvasItem::rowHeader() const
{
    return 0;
    //return d->view->rowHeader();
}

QScrollBar* CanvasItem::horzScrollBar() const
{
    return d->view->horzScrollBar();
}

QScrollBar* CanvasItem::vertScrollBar() const
{
    return d->view->vertScrollBar();
}

Sheet* CanvasItem::activeSheet() const
{
    return d->view->activeSheet();
}

void CanvasItem::validateSelection()
{
#if 0
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

    if (selection()->isSingular()) {
        const Cell cell = Cell(sheet, selection()->marker()).masterCell();
        Validity validity = cell.validity();
        if (validity.displayValidationInformation()) {
            const QString title = validity.titleInfo();
            QString message = validity.messageInfo();
            if (title.isEmpty() && message.isEmpty())
                return;

            if (!d->validationInfo) {
                d->validationInfo = new QLabel(this);
                QPalette palette = d->validationInfo->palette();
                palette.setBrush(QPalette::Window, palette.toolTipBase());
                palette.setBrush(QPalette::WindowText, palette.toolTipText());
                d->validationInfo->setPalette(palette);
//                 d->validationInfo->setWindowFlags(Qt::ToolTip);
                d->validationInfo->setFrameShape(QFrame::Box);
                d->validationInfo->setAlignment(Qt::AlignVCenter);
                d->validationInfo->setTextFormat(Qt::RichText);
            }

            QString resultText("<html><body>");
            if (!title.isEmpty()) {
                resultText += "<h2>" + title + "</h2>";
            }
            if (!message.isEmpty()) {
                message.replace(QChar('\n'), QString("<br>"));
                resultText += "<p>" + message + "</p>";
            }
            resultText += "</body></html>";
            d->validationInfo->setText(resultText);

            const double xpos = sheet->columnPosition(cell.column()) + cell.width();
            const double ypos = sheet->rowPosition(cell.row()) + cell.height();
            const QPointF position = QPointF(xpos, ypos) - offset();
            const QPoint viewPosition = viewConverter()->documentToView(position).toPoint();
            d->validationInfo->move(/*mapToGlobal*/(viewPosition)); // Qt::ToolTip!
            d->validationInfo->show();
        } else {
            delete d->validationInfo;
            d->validationInfo = 0;
        }
    } else {
        delete d->validationInfo;
        d->validationInfo = 0;
    }
#endif
}

void CanvasItem::setDocumentOffset(const QPoint& offset)
{
    const QPoint delta = offset - viewConverter()->documentToView(d->offset).toPoint();
    d->offset = viewConverter()->viewToDocument(offset);

    columnHeader()->scroll(delta.x(), 0);
    rowHeader()->scroll(0, delta.y());
}

void CanvasItem::setDocumentSize(const QSizeF& size)
{
    const QSize s = viewConverter()->documentToView(size).toSize();
    emit documentSizeChanged(s);
}

void CanvasItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
#if 0
    QMouseEvent *const origEvent = event;
    QPointF documentPosition;
    if (layoutDirection() == Qt::LeftToRight) {
        documentPosition = viewConverter()->viewToDocument(event->pos()) + offset();
    } else {
        const QPoint position(width() - event->x(), event->y());
        const QPointF offset(this->offset().x(), this->offset().y());
        documentPosition = viewConverter()->viewToDocument(position) + offset;
        kDebug() << "----------------------------";
        kDebug() << "event->pos():" << event->pos();
        kDebug() << "event->globalPos():" << event->globalPos();
        kDebug() << "position:" << position;
        kDebug() << "offset:" << offset;
        kDebug() << "documentPosition:" << documentPosition;
        event = new QMouseEvent(QEvent::MouseButtonPress, position, mapToGlobal(position), event->button(), event->buttons(), event->modifiers());
        kDebug() << "newEvent->pos():" << event->pos();
        kDebug() << "newEvent->globalPos():" << event->globalPos();
    }

    // flake
    if(d->toolProxy) {
        d->toolProxy->mousePressEvent(event, documentPosition);

        if (!event->isAccepted() && event->button() == Qt::RightButton) {
            d->view->unplugActionList("toolproxy_action_list");
            d->view->plugActionList("toolproxy_action_list", toolProxy()->popupActionList());
            QMenu* menu = dynamic_cast<QMenu*>(d->view->factory()->container("default_canvas_popup", d->view));
            // Only show the menu, if there are items. The plugged action list counts as one action.
            if (menu && menu->actions().count() > 1) {
                menu->exec(origEvent->globalPos());
            }
            origEvent->setAccepted(true);
        }
    }
    if (layoutDirection() == Qt::RightToLeft) {
        delete event;
    }
#endif
}

void CanvasItem::mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
#if 0
    QPointF documentPosition;
    if (layoutDirection() == Qt::LeftToRight) {
        documentPosition = viewConverter()->viewToDocument(event->pos()) + offset();
    } else {
        const QPoint position(width() - event->x(), event->y());
        const QPointF offset(this->offset().x(), this->offset().y());
        documentPosition = viewConverter()->viewToDocument(position) + offset;
        event = new QMouseEvent(QEvent::MouseButtonRelease, position, mapToGlobal(position), event->button(), event->buttons(), event->modifiers());
    }

    // flake
    if(d->toolProxy)
        d->toolProxy->mouseReleaseEvent(event, documentPosition);

    if (layoutDirection() == Qt::RightToLeft) {
        delete event;
    }
#endif
}

void CanvasItem::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
#if 0
    QPointF documentPosition;
    if (layoutDirection() == Qt::LeftToRight) {
        documentPosition = viewConverter()->viewToDocument(event->pos()) + offset();
    } else {
        const QPoint position(width() - event->x(), event->y());
        const QPointF offset(this->offset().x(), this->offset().y());
        documentPosition = viewConverter()->viewToDocument(position) + offset;
        event = new QMouseEvent(QEvent::MouseMove, position, mapToGlobal(position), event->button(), event->buttons(), event->modifiers());
    }

    // flake
    if(d->toolProxy)
        d->toolProxy->mouseMoveEvent(event, documentPosition);

    if (layoutDirection() == Qt::RightToLeft) {
        delete event;
    }
#endif
}

void CanvasItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
#if 0
    QPointF documentPosition;
    if (layoutDirection() == Qt::LeftToRight) {
        documentPosition = viewConverter()->viewToDocument(event->pos()) + offset();
    } else {
        const QPoint position(width() - event->x(), event->y());
        const QPointF offset(this->offset().x(), this->offset().y());
        documentPosition = viewConverter()->viewToDocument(position) + offset;
        event = new QMouseEvent(QEvent::MouseButtonDblClick, position, mapToGlobal(position), event->button(), event->buttons(), event->modifiers());
    }

    // flake
    if(d->toolProxy)
        d->toolProxy->mouseDoubleClickEvent(event, documentPosition);

    if (layoutDirection() == Qt::RightToLeft) {
        delete event;
    }
#endif
}

void CanvasItem::keyPressEvent(QKeyEvent* event)
{
    // flake
    if(d->toolProxy)
        d->toolProxy->keyPressEvent(event);
}

void CanvasItem::tabletEvent(QTabletEvent *e)
{
    // flake
    if(d->toolProxy)
        d->toolProxy->tabletEvent(e, viewConverter()->viewToDocument(e->pos() + offset()));
}

QVariant CanvasItem::inputMethodQuery(Qt::InputMethodQuery query) const
{
    // flake
    return d->toolProxy ? d->toolProxy->inputMethodQuery(query, *(viewConverter())) : 0;
}

void CanvasItem::inputMethodEvent(QInputMethodEvent *event)
{
    // flake
    if(d->toolProxy)
        d->toolProxy->inputMethodEvent(event);
}

void CanvasItem::paint( QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget)
{
    Q_UNUSED(widget);

    if (d->view->doc()->map()->isLoading() || d->view->isLoading())
        return;

    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

//     ElapsedTime et("Painting cells", ElapsedTime::PrintOnlyTime);

    painter->setClipRect(option->exposedRect);
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
    painter->save();

    // After the scaling, the painter methods need document coordinates!
    qreal zoomX, zoomY;
    viewConverter()->zoom(&zoomX, &zoomY);
    painter->scale(zoomX, zoomY);

    const bool layoutReversed = sheet->layoutDirection() == Qt::RightToLeft;
    const QPointF offset(layoutReversed ? -this->offset().x() : this->offset().x(), this->offset().y());
    painter->translate(-offset);

    // erase background
    const QRectF paintRect(viewConverter()->viewToDocument(rect()).translated(offset));
    painter->fillRect(paintRect, painter->background());

    // paint visible cells
    const QRect visibleRect = visibleCells();
    const QPointF topLeft(sheet->columnPosition(visibleRect.left()), sheet->rowPosition(visibleRect.top()));
    view()->sheetView(sheet)->setPaintCellRange(visibleRect);
    view()->sheetView(sheet)->paintCells(*painter, paintRect, topLeft);

    // flake
    painter->restore();
    // d->offset is the negated CanvasController offset in document coordinates.
//     painter->save();
    painter->translate(-viewConverter()->documentToView(offset));
    d->shapeManager->paint(*painter, *viewConverter(), false);
//     painter->restore();
//     const QPointF p = -viewConverter()->documentToView(this->offset());
//     painter->translate(p.x() /*+ width()*/, p.y());
    painter->setRenderHint(QPainter::Antialiasing, false);
    if(d->toolProxy)
        d->toolProxy->paint(*painter, *viewConverter());

}

void CanvasItem::focusInEvent(QFocusEvent *event)
{
    // If we are in editing mode, we redirect the
    // focus to the CellEditor or ExternalEditor.
    // Using a focus proxy does not work here, because in reference selection
    // mode clicking on the canvas to select a reference should end up in the
    // editor, which got the focus before. This is determined by storing the
    // last editor with focus. It is set by the editors on getting focus by user
    // interaction. Setting a focus proxy would always result in the proxy being
    // the last editor, because clicking the canvas is a user interaction.
    // This screws up <Tab> though (David)
    selection()->emitRequestFocusEditor();
    QGraphicsWidget::focusInEvent(event);
}

void CanvasItem::focusOutEvent(QFocusEvent *event)
{
    QGraphicsWidget::focusOutEvent(event);
}

void CanvasItem::dragEnterEvent(QGraphicsSceneDragDropEvent* event)
{
    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasText() ||
            mimeData->hasFormat("application/x-kspread-snippet")) {
        event->acceptProposedAction();
    }
}

void CanvasItem::dragMoveEvent(QGraphicsSceneDragDropEvent* event)
{
    register Sheet * const sheet = activeSheet();
    if (!sheet) {
        event->ignore();
        return;
    }

    const QMimeData* mimeData = event->mimeData();
    if (mimeData->hasText() || mimeData->hasFormat("application/x-kspread-snippet")) {
        event->acceptProposedAction();
    } else {
        event->ignore();
        return;
    }
#if 0 // TODO Stefan: implement drag marking rectangle
    QRect dragMarkingRect;
    if (mimeData->hasFormat("application/x-kspread-snippet")) {
        if (event->source() == this) {
            kDebug(36005) << "source == this";
            dragMarkingRect = selection()->boundingRect();
        } else {
            kDebug(36005) << "source != this";
            QByteArray data = mimeData->data("application/x-kspread-snippet");
            QString errorMsg;
            int errorLine;
            int errorColumn;
            QDomDocument doc;
            if (!doc.setContent(data, false, &errorMsg, &errorLine, &errorColumn)) {
                // an error occurred
                kDebug(36005) << "CanvasItem::daragMoveEvent: an error occurred" << endl
                << "line: " << errorLine << " col: " << errorColumn
                << ' ' << errorMsg << endl;
                dragMarkingRect = QRect(1, 1, 1, 1);
            } else {
                QDomElement root = doc.documentElement(); // "spreadsheet-snippet"
                dragMarkingRect = QRect(1, 1,
                                        root.attribute("columns").toInt(),
                                        root.attribute("rows").toInt());
            }
        }
    } else { // if ( mimeData->hasText() )
        kDebug(36005) << "has text";
        dragMarkingRect = QRect(1, 1, 1, 1);
    }
#endif
    const QPoint dragAnchor = selection()->boundingRect().topLeft();
    double xpos = sheet->columnPosition(dragAnchor.x());
    double ypos = sheet->rowPosition(dragAnchor.y());
    double width  = sheet->columnFormat(dragAnchor.x())->width();
    double height = sheet->rowFormat(dragAnchor.y())->height();

    // consider also the selection rectangle
    const QRectF noGoArea(xpos - 1, ypos - 1, width + 3, height + 3);

    // determine the current position
    double eventPosX;
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        eventPosX = viewConverter()->viewToDocumentX(this->boundingRect().width() - event->pos().x()) + xOffset();
    } else {
        eventPosX = viewConverter()->viewToDocumentX(event->pos().x()) + xOffset();
    }
    double eventPosY = viewConverter()->viewToDocumentY(event->pos().y()) + yOffset();

    if (noGoArea.contains(QPointF(eventPosX, eventPosY))) {
        event->ignore();
        return;
    }

#if 0 // TODO Stefan: implement drag marking rectangle
    // determine the cell position under the mouse
    double tmp;
    const int col = sheet->leftColumn(eventPosX, tmp);
    const int row = sheet->topRow(eventPosY, tmp);
    dragMarkingRect.moveTo(QPoint(col, row));
    kDebug(36005) << "MARKING RECT =" << dragMarkingRect;
#endif
}

void CanvasItem::dragLeaveEvent(QGraphicsSceneDragDropEvent *)
{
}

void CanvasItem::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    register Sheet * const sheet = activeSheet();
    // FIXME Sheet protection: Not all cells have to be protected.
    if (!sheet || sheet->isProtected()) {
        event->ignore();
        return;
    }

    const QMimeData* mimeData = event->mimeData();
    if (!PasteCommand::supports(mimeData)) {
        event->ignore();
        return;
    }

    // Do not allow dropping onto the same position.
    const QPoint topLeft(selection()->boundingRect().topLeft());
    const double xpos = sheet->columnPosition(topLeft.x());
    const double ypos = sheet->rowPosition(topLeft.y());
    const double width  = sheet->columnFormat(topLeft.x())->width();
    const double height = sheet->rowFormat(topLeft.y())->height();

    const QRectF noGoArea(xpos - 1, ypos - 1, width + 3, height + 3);

    double ev_PosX;
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        ev_PosX = viewConverter()->viewToDocumentX(this->boundingRect().width() - event->pos().x()) + xOffset();
    } else {
        ev_PosX = viewConverter()->viewToDocumentX(event->pos().x()) + xOffset();
    }
    double ev_PosY = viewConverter()->viewToDocumentY(event->pos().y()) + yOffset();

    if (noGoArea.contains(QPointF(ev_PosX, ev_PosY))) {
        event->ignore();
        return;
    } else {
        event->setAccepted(true);
    }

    // The destination cell location.
    double tmp;
    const int col = sheet->leftColumn(ev_PosX, tmp);
    const int row = sheet->topRow(ev_PosY, tmp);

    PasteCommand *const command = new PasteCommand();
    command->setSheet(sheet);
    command->add(Region(col, row, 1, 1, sheet));
    command->setMimeData(mimeData);
// XXX: Port
//    if (event->source() == this) {
//        DeleteCommand *const deleteCommand = new DeleteCommand(command);
//        deleteCommand->setSheet(sheet);
//        deleteCommand->add(*selection()); // selection is still, where the drag started
//        deleteCommand->setRegisterUndo(false);
//    }
    command->execute();

    // Select the pasted cells
    const int columns = selection()->boundingRect().width();
    const int rows = selection()->boundingRect().height();
    selection()->initialize(QRect(col, row, columns, rows), sheet);
}

QRect CanvasItem::viewToCellCoordinates(const QRectF& viewRect) const
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return QRect();

    // NOTE Stefan: Do not consider the layout direction in this case.
    const QRectF rect = viewConverter()->viewToDocument(viewRect.normalized()).translated(offset());

    double tmp;
    const int left = sheet->leftColumn(rect.left(), tmp);
    const int right = sheet->rightColumn(rect.right());
    const int top = sheet->topRow(rect.top(), tmp);
    const int bottom = sheet->bottomRow(rect.bottom());

    return QRect(left, top, right - left + 1, bottom - top + 1);
}

QRect CanvasItem::visibleCells() const
{
    return viewToCellCoordinates(rect());
}

//---------------------------------------------
//
// Drawing Engine
//
//---------------------------------------------

QRectF CanvasItem::cellCoordinatesToView(const QRect& cellRange) const
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return QRectF();

    QRectF rect = sheet->cellCoordinatesToDocument(cellRange);
    // apply scrolling offset
    rect.translate(-xOffset(), -yOffset());
    // convert it to view coordinates
    rect = d->view->zoomHandler()->documentToView(rect);
    // apply layout direction
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        const double left = rect.left();
        const double right = rect.right();
        rect.setLeft(boundingRect().width() - right);
        rect.setRight(boundingRect().width() - left);
    }
    return rect;
}

void CanvasItem::showToolTip(const QPoint& p)
{
    register Sheet * const sheet = activeSheet();
    if (!sheet)
        return;

    // Over which cell is the mouse ?
    double ypos, xpos;
    double dwidth = d->view->zoomHandler()->viewToDocumentX(boundingRect().width());
    int col;
    if (sheet->layoutDirection() == Qt::RightToLeft)
        col = sheet->leftColumn((dwidth - d->view->zoomHandler()->viewToDocumentX(p.x()) +
                                 xOffset()), xpos);
    else
        col = sheet->leftColumn((d->view->zoomHandler()->viewToDocumentX(p.x()) +
                                 xOffset()), xpos);


    int row = sheet->topRow((d->view->zoomHandler()->viewToDocumentY(p.y()) +
                             yOffset()), ypos);

    Cell cell = Cell(sheet, col, row).masterCell();
    CellView cellView = view()->sheetView(sheet)->cellView(cell.column(), cell.row());
    if (cellView.isObscured()) {
        cell = Cell(sheet, cellView.obscuringCell());
        cellView = view()->sheetView(sheet)->cellView(cellView.obscuringCell().x(), cellView.obscuringCell().y());
    }

    // displayed tool tip, which has the following priorities:
    //  - cell content if the cell dimension is too small
    //  - cell comment
    //  - hyperlink
    // Ensure that it is plain text.
    // Not funny if (intentional or not) <a> appears as hyperlink.
    QString tipText;
    // If cell is too small, show the content
    if (!cellView.dimensionFits())
        tipText = cell.displayText().replace('<', "&lt;");

    // Show hyperlink, if any
    if (tipText.isEmpty())
        tipText = cell.link().replace('<', "&lt;");

    // Nothing to display, bail out
    if (tipText.isEmpty() && cell.comment().isEmpty())
        return;

    // Cut if the tip is ridiculously long
    const int maxLen = 256;
    if (tipText.length() > maxLen)
        tipText = tipText.left(maxLen).append("...");

    // Determine position and width of the current cell.
    const double cellWidth = cellView.cellWidth();
    const double cellHeight = cellView.cellHeight();

    // Get the cell dimensions
    QRect cellRect;
    bool insideCellRect = false;
    if (sheet->layoutDirection() == Qt::RightToLeft) {
        const QRectF rect(dwidth - cellWidth - xpos + xOffset(), ypos - yOffset(), cellWidth, cellHeight);
        cellRect = viewConverter()->documentToView(rect).toRect();
        insideCellRect = cellRect.contains(p);
    } else {
        QRectF rect(xpos - xOffset(), ypos - yOffset(), cellWidth, cellHeight);
        cellRect = viewConverter()->documentToView(rect).toRect();
        insideCellRect = cellRect.contains(p);
    }

    // No use if mouse is somewhere else
    if (!insideCellRect)
        return;

    // Show comment, if any.
    if (tipText.isEmpty())
        tipText = cell.comment().replace('<', "&lt;");
    else if (!cell.comment().isEmpty())
        tipText += "</p><h4>" + i18n("Comment:") + "</h4><p>" + cell.comment().replace('<', "&lt;");
// XXX: Port
//    // Now we show the tip
//    QToolTip::showText(mapToGlobal(cellRect.bottomRight()),
//                       "<p>" + tipText.replace('\n', "<br>") + "</p>",
//                       this, cellRect.translated(-mapToGlobal(cellRect.topLeft())));
}

void CanvasItem::updateInputMethodInfo()
{
//    updateMicroFocus();
}

#include "CanvasItem.moc"
