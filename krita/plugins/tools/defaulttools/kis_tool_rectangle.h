/*
 *  kis_tool_rectangle.h - part of KImageShop^WKrayon^WKrita
 *
 *  Copyright (c) 1999 Michael Koch <koch@kde.org>
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
 *  Copyright (c) 2004 Boudewijn Rempt <boud@valdyas.org>
 *  Copyright (c) 2004 Clarence Dang <dang@kde.org>
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

#ifndef __KIS_TOOL_RECTANGLE_H__
#define __KIS_TOOL_RECTANGLE_H__

#include <QRect>

#include "kis_tool_shape.h"
#include "kis_types.h"
#include "KoToolFactory.h"
#include "KoPoint.h"

class QPainter;
class KisPainter;

class KisToolRectangle : public KisToolShape {

    typedef KisToolShape super;
    Q_OBJECT

public:
    KisToolRectangle();
    virtual ~KisToolRectangle();


    virtual void setup(KActionCollection *collection);
    virtual enumToolType toolType() { return TOOL_SHAPE; }
    virtual quint32 priority() { return 2; }
    virtual void buttonPress(KoPointerEvent *event);
    virtual void move(KoPointerEvent *event);
    virtual void buttonRelease(KoPointerEvent *event);

protected:
    virtual void draw(const KoPoint&, const KoPoint&);

protected:
    int m_lineThickness;

    KoPoint m_dragCenter;
    KoPoint m_dragStart;
    KoPoint m_dragEnd;
    QRect m_final_lines;

    bool m_dragging;
    KisImageSP m_currentImage;
};

class KisToolRectangleFactory : public KoToolFactory {

public:
    KisToolRectangleFactory(QObject *parent, const QStringList&)
        : KoToolFactory(parent, "KisToolRectangle", i18n( "Rectangle" ))
        {
            setToolTip( i18n( "Draw a rectangle" ) );
            setToolType( TOOL_TYPE_SHAPE );
            setIcon( "tool_rectange" );
            setShortcut( Qt::Key_F6 );
            setPriority( 0 );
        };

    virtual ~KisToolRectangleFactory(){};

    virtual KoTool * createTool(KoCanvasBase *canvas) {
        return  new KisToolRectangle(canvas);
    }

};


#endif // __KIS_TOOL_RECTANGLE_H__

