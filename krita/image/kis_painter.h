/*
 *  Copyright (c) 2002 Patrick Julien <freak@codepimps.org>
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

#ifndef KIS_PAINTER_H_
#define KIS_PAINTER_H_

#include <QBitArray>

#include "KoColor.h"
#include "kis_global.h"
#include "kis_types.h"
#include "kis_paint_device.h"

#include <KoColorSpace.h>
#include <KoProgressUpdater.h>
#include "kis_selection.h"

#include <krita_export.h>

class QUndoCommand;
class QRect;
class QStringList;
class KisTransaction;
class KisBrush;
class KisComplexColor;
class KisPattern;
class KisPaintOp;
class KisPaintInformation;

/**
 * KisPainter contains the graphics primitives necessary to draw on a
 * KisPaintDevice. This is the same kind of abstraction as used in Qt
 * itself, where you have QPainter and QPaintDevice.
 *
 * However, KisPainter works on a tiled image and supports different
 * color models, and that's a lot more complicated.
 *
 * KisPainter supports transactions that can group various paint operations
 * in one undoable step.
 *
 * For more complex operations, you might want to have a look at the subclasses
 * of KisPainter: KisConvolutionPainter, KisFillPainter and KisGradientPainter
 */
class KRITAIMAGE_EXPORT KisPainter {


public:
    /// Construct painter without a device
    KisPainter();

    /// Construct a painter, and begin painting on the device
    KisPainter( KisPaintDeviceSP device );

    /// Construct a painter, and begin painting on the device. All actions will be masked by the given selection.
    KisPainter(KisPaintDeviceSP device, KisSelectionSP selection);
    virtual ~KisPainter();

public:
    /**
     * Start painting on the specified device. Not undoable.
     */
    void begin( KisPaintDeviceSP device );

    /**
     * Start painting on the specified paint device. All actions will be masked by the given selection.
     */
    void begin( KisPaintDeviceSP device, KisSelectionSP selection );

    /**
     * Finish painting on the current device
     */
    QUndoCommand *end();

    /**
     * If set, the painter action is cancelable, if the action supports that.
     */
    void setProgress(KoUpdater * progressUpdater)
        {
            m_progressUpdater = progressUpdater;
        }

    /// Begin an undoable paint operation
    void beginTransaction(const QString& customName = QString::null);

    /// Finish the undoable paint operation
    QUndoCommand *endTransaction();

    /// begin a transaction with the given command
    void beginTransaction( KisTransaction* command);

    /// Return the current transcation
    KisTransaction  * transaction() { return m_transaction; }


    /// Returns the current paint device.
    KisPaintDeviceSP device() const { return m_device; }

    /**
     * Blast the specified region from src onto the current paint device.
     * @param dx the destination x-coordinate
     * @param dy the destination y-coordinate
     * @param op a pointer to the composite op use to blast the pixels from src on dst
     * @param src the source device
     * @param opacity the opacity of the source pixel
     * @param sx the source x-coordinate
     * @param sy the source y-coordinate
     * @param sw the width of the region
     * @param sh the height of the region
     */
    void bitBlt(qint32 dx, qint32 dy,
                const KoCompositeOp* op,
                const KisPaintDeviceSP src,
                quint8 opacity,
                qint32 sx, qint32 sy,
                qint32 sw, qint32 sh);

    /**
     * Convenience method that uses the opacity and composite op set
     * in the painter. If nothing is set, opaque and OVER are assumed.
     */
    void bitBlt(QPoint pos, const KisPaintDeviceSP src, QRect srcRect )
    {
        bitBlt( pos.x(), pos.y(), m_compositeOp, src, m_opacity, srcRect.x(), srcRect.y(), srcRect.width(), srcRect.height() );
    }

    /**
     * Overloaded function of the previous which differs that you can pass the composite op using
     * the name
     * @param dx the destination x-coordinate
     * @param dy the destination y-coordinate
     * @param op the name of composite op use to blast the pixels from src on dst
     * @param src the source device
     * @param opacity the opacity of the source pixel
     * @param sx the source x-coordinate
     * @param sy the source y-coordinate
     * @param sw the width of the region
     * @param sh the height of the region
     */
    inline void bitBlt(qint32 dx, qint32 dy,
                const QString & op,
                const KisPaintDeviceSP src,
                quint8 opacity,
                qint32 sx, qint32 sy,
                qint32 sw, qint32 sh)
    {
        bitBlt(dx, dy, m_colorSpace->compositeOp(op), src, opacity, sx, sy, sw, sh);
    }

    /**
     * Overloaded version of the previous, differs in that the opacity is forced to OPACITY_OPAQUE
     * @param dx the destination x-coordinate
     * @param dy the destination y-coordinate
     * @param op a pointer to the composite op use to blast the pixels from src on dst
     * @param src the source device
     * @param sx the source x-coordinate
     * @param sy the source y-coordinate
     * @param sw the width of the region
     * @param sh the height of the region
     */
    inline void bitBlt(qint32 dx, qint32 dy,
                const KoCompositeOp* op,
                const KisPaintDeviceSP src,
                qint32 sx, qint32 sy,
                qint32 sw, qint32 sh)
    {
        bitBlt(dx, dy, op, src, OPACITY_OPAQUE, sx, sy, sw, sh);
    }

    /**
     * Overloaded function of the previous which differs that you can pass the composite op using
     * the name and the opacity is forced to OPACITY_OPAQUE
     * @param dx the destination x-coordinate
     * @param dy the destination y-coordinate
     * @param op the name of composite op use to blast the pixels from src on dst
     * @param src the source device
     * @param sx the source x-coordinate
     * @param sy the source y-coordinate
     * @param sw the width of the region
     * @param sh the height of the region
     */
    inline void bitBlt(qint32 dx, qint32 dy,
                const QString & op,
                const KisPaintDeviceSP src,
                qint32 sx, qint32 sy,
                qint32 sw, qint32 sh)
    {
        bitBlt(dx, dy, op, src, OPACITY_OPAQUE, sx, sy, sw, sh);
    }

    /**
     * Overloaded function of the previous that take a KisSelection instead of a KisPaintDevice.
     * @param dx the destination x-coordinate
     * @param dy the destination y-coordinate
     * @param op a pointer to the composite op use to blast the pixels from src on dst
     * @param src the source device
     * @param selMask the mask
     * @param opacity the opacity of the source pixel
     * @param sx the source x-coordinate
     * @param sy the source y-coordinate
     * @param sw the width of the region
     * @param sh the height of the region
     */
    void bltSelection(qint32 dx, qint32 dy,
                      const KoCompositeOp  *op,
                      const KisPaintDeviceSP src,
                      const KisSelectionSP seldev,
                      quint8 opacity,
                      qint32 sx, qint32 sy,
                      qint32 sw, qint32 sh);

    /**
     * Convenience method that uses the opacity and composite op set
     * in the painter. If noting is set, opaque and OVER are assumed.
     */
    void bltSelection(QPoint pos, const KisPaintDeviceSP src, const KisSelectionSP selDev, QRect srcRect )
    {
        bltSelection( pos.x(), pos.y(), m_compositeOp, src, selDev, m_opacity, srcRect.x(), srcRect.y(), srcRect.width(), srcRect.height() );
    }

    /**
     * Overloaded function of the previous that takes a KisSelection
     * instead of a KisPaintDevice and you can pass the composite op
     * using the name.
     *
     * @param dx the destination x-coordinate
     * @param dy the destination y-coordinate
     * @param op the name of the composite op use to blast the pixels from src on dst
     * @param src the source device
     * @param selMask the mask
     * @param opacity the opacity of the source pixel
     * @param sx the source x-coordinate
     * @param sy the source y-coordinate
     * @param sw the width of the region
     * @param sh the height of the region
     */
    void bltSelection(qint32 dx, qint32 dy,
                      const QString & op,
                      const KisPaintDeviceSP src,
                      const KisSelectionSP selMask,
                      quint8 opacity,
                      qint32 sx, qint32 sy,
                      qint32 sw, qint32 sh)
    {
        bltSelection(dx, dy, m_colorSpace->compositeOp(op), src, selMask, opacity, sx, sy, sw, sh);
    }

    /**
     * A version of bitBlt that renders using the src device's selection mask, if it has one.
     * @param dx the destination x-coordinate
     * @param dy the destination y-coordinate
     * @param op a pointer to the composite op use to blast the pixels from src on dst
     * @param src the source device
     * @param opacity the opacity of the source pixel
     * @param sx the source x-coordinate
     * @param sy the source y-coordinate
     * @param sw the width of the region
     * @param sh the height of the region
     */
    void bltSelection(qint32 dx, qint32 dy,
                      const KoCompositeOp *op,
                      const KisPaintDeviceSP srcdev,
                      quint8 opacity,
                      qint32 sx, qint32 sy,
                      qint32 sw, qint32 sh)
    {
        if (m_device.isNull()) return;
        if ( !m_selection ) {
            bitBlt(dx, dy, op, srcdev, opacity, sx, sy, sw, sh);
        }
        else {
            bltSelection(dx, dy, op, srcdev, m_selection, opacity, sx, sy, sw, sh );
        }
    }

    /**
     * Overloaded function of the previous that takes the name of the composite op using the name
     * @param dx the destination x-coordinate
     * @param dy the destination y-coordinate
     * @param op the name of the composite op use to blast the pixels from src on dst
     * @param src the source device
     * @param opacity the opacity of the source pixel
     * @param sx the source x-coordinate
     * @param sy the source y-coordinate
     * @param sw the width of the region
     * @param sh the height of the region
     */
    void bltSelection(qint32 dx, qint32 dy,
                      const QString & op,
                      const KisPaintDeviceSP src,
                      quint8 opacity,
                      qint32 sx, qint32 sy,
                      qint32 sw, qint32 sh)
    {
       bltSelection(dx, dy, m_colorSpace->compositeOp(op), src, opacity, sx, sy, sw, sh);
    }

    /**
     * The methods below are 'higher' level than the above methods. They need brushes, colors
     * etc. set before they can be called. The methods do not directly tell the image to
     * update, but you can call dirtyRegion() to get the region that needs to be notified by your
     * painting code.
     *
     * Call will RESET the dirtyRegion!
    */
    QRegion dirtyRegion();

    /**
     * Paint a line that connects the dots in points
     */
    void paintPolyline(const QVector <QPointF> &points,
                       int index = 0, int numPoints = -1);

    /**
     * Draw a line between pos1 and pos2 using the currently set brush and color.
     * If savedDist is less than zero, the brush is painted at pos1 before being
     * painted along the line using the spacing setting.
     * @return the drag distance, that is the remains of the distance between p1 and p2 not covered
     * because the currenlty set brush has a spacing greater than that distance.
     */
    double paintLine(const KisPaintInformation &pi1,
                     const KisPaintInformation &pi2,
                     double savedDist = -1);

    /**
     * Draw a Bezier curve between pos1 and pos2 using control points 1 and 2.
     * If savedDist is less than zero, the brush is painted at pos1 before being
     * painted along the curve using the spacing setting.
     * @return the drag distance, that is the remains of the distance between p1 and p2 not covered
     * because the currenlty set brush has a spacing greater than that distance.
     */
    double paintBezierCurve(const KisPaintInformation &pi1,
                const QPointF &control1,
                const QPointF &control2,
                const KisPaintInformation &pi2,
                const double savedDist = -1);
    /**
     * Fill the given vector points with the points needed to draw the Bezier curve between
     * pos1 and pos2 using control points 1 and 2, excluding the final pos2.
     */
    void getBezierCurvePoints(const QPointF &pos1,
                const QPointF &control1,
                const QPointF &control2,
                const QPointF &pos2,
                vQPointF& points) const;

    /**
     * Paint a rectangle.
     * @param rect the rectangle to paint.
     */
    void paintRect(const QRectF &rect,
                   const double pressure,
                   const double xTilt,
                   const double yTilt);

    /**
    * Paint a rectangle.
    * @param x x coordinate of the top-left corner
    * @param y y coordinate of the top-left corner
    * @param w the rectangle width
    * @param h the rectangle height
    */
    void paintRect(const double x,
                   const double y,
                   const double w,
                   const double h,
                   const double pressure,
                   const double xTilt,
                   const double yTilt);

    /**
     * Paint the ellipse that fills the given rectangle.
     * @param rect the rectangle containing the ellipse to paint.
     */
    void paintEllipse(const QRectF &rect,
                      const double pressure,
                      const double xTilt,
                      const double yTilt);

    /**
    * Paint the ellipse that fills the given rectangle.
    * @param x x coordinate of the top-left corner
    * @param y y coordinate of the top-left corner
    * @param w the rectangle width
    * @param h the rectangle height
    */
    void paintEllipse(const double x,
                      const double y,
                      const double w,
                      const double h,
                      const double pressure,
                      const double xTilt,
                      const double yTilt);

    /**
     * Paint the polygon with the points given in points. It automatically closes the polygon
     * by drawing the line from the last point to the first.
     */
    void paintPolygon(const vQPointF& points);

    /** Draw a spot at pos using the currently set paint op, brush and color */
    void paintAt(const KisPaintInformation &pos);

    /**
     * Stroke the given QPainterPath.
     */
    void paintPainterPath(const QPainterPath& path);

    /**
     * Fills the area enclosed by the given QPainterPath
     */
    void fillPainterPath(const QPainterPath& path);
    // ------------------------------------------------------------------------
    // Set the parameters for the higher level graphics primitives.

    /**
     * Set the channelflags: a bit array where true means that the
     * channel corresponding in position with the bit will be read
     * by the operation, and false means that it will not be affected.
     *
     * An empty channelFlags parameter means that all channels are
     * affected.
     *
     * @param the bit array that masks the source channels; only
     * the channels where the corresponding bit is true will will be
     * composited onto the destination device.
     */
    void setChannelFlags( QBitArray channelFlags )
        {
            m_channelFlags = channelFlags;
        }


    QBitArray channelFlags()
        {
            return m_channelFlags;
        }

    // Set the current brush
    void setBrush(KisBrush* brush) { m_brush = brush; }
    // Returns the currently set brush
    KisBrush * brush() const { return m_brush; }

    // Set the current pattern
    void setPattern(KisPattern * pattern) { m_pattern = pattern; }
    // Returns the currently set pattern
    KisPattern * pattern() const { return m_pattern; }

    // Set the color that will be used to paint with
    void setPaintColor(const KoColor& color) { m_paintColor = color;}
    /// Returns the color that will be used to paint with
    KoColor paintColor() const { return m_paintColor; }

    // Set the current background color
    void setBackgroundColor(const KoColor& color) {m_backgroundColor = color; }
    // Returns the current background color
    KoColor backgroundColor() const { return m_backgroundColor; }

    // Set the current fill color
    void setFillColor(const KoColor& color) { m_fillColor = color; }
    // Returns the current fill color
    KoColor fillColor() const { return m_fillColor; }

    // XXX
    void setComplexColor(KisComplexColor *color) { m_complexColor = color; }
    KisComplexColor *complexColor() { return m_complexColor; }

    /// This enum contains the styles with which we can fill things like polygons and ellipses
    enum FillStyle {
        FillStyleNone,
        FillStyleForegroundColor,
        FillStyleBackgroundColor,
        FillStylePattern,
        FillStyleGradient,
        FillStyleStrokes
    };

    /// Set the current style with which to fill
    void setFillStyle(FillStyle fillStyle) { m_fillStyle = fillStyle; }

    /// Returns the current fill style
    FillStyle fillStyle() const { return m_fillStyle; }

    /// Set whether a polygon's filled area should be anti-aliased or not. The default is true.
    void setAntiAliasPolygonFill(bool antiAliasPolygonFill) { m_antiAliasPolygonFill = antiAliasPolygonFill; }

    /// Return whether a polygon's filled area should be anti-aliased or not
    bool antiAliasPolygonFill() { return m_antiAliasPolygonFill; }

    /// The style of the brush stroke around polygons and so
    enum StrokeStyle {
        StrokeStyleNone,
        StrokeStyleBrush
    };

    /// Set the current brush stroke style
    void setStrokeStyle(StrokeStyle strokeStyle) { m_strokeStyle = strokeStyle; }
    /// Returns the current brush stroke style
    StrokeStyle strokeStyle() const { return m_strokeStyle; }

    /// Set the opacity which is used in painting (like filling polygons)
    void setOpacity(quint8 opacity) { m_opacity = opacity; }
    /// Returns the opacity that is used in painting
    quint8 opacity() const { return m_opacity; }

    /// Sets the current pressure for things that like to use this
    void setPressure(double pressure) { m_pressure = pressure; }
    /// Returns the current pressure
    double pressure() { return m_pressure; }

    /// Sets the bounds of the painter area; if not set, the painter
    /// will happily paint where you ask it, making the paint device
    /// larger as it goes
    void setBounds( const QRect & bounds ) { m_bounds = bounds;  }
    QRect bounds() { return m_bounds;  }

    /**
     * Set the current paint operation. This is used for all drawing functions.
     * The painter will DELETE the paint op itself!!
     * That means no that you should not delete it yourself (or put it on the stack)
     */
    void setPaintOp(KisPaintOp * paintOp);
    /// Returns the current paint operation
    KisPaintOp * paintOp() const { return m_paintOp; }

    /// Set a current 'dab'. This usually is a paint device containing a rendered brush
    void setDab(KisPaintDeviceSP dab) { m_dab = dab; }
    /// Get the currently set dab
    KisPaintDeviceSP dab() const { return m_dab; }

    /// Set the composite op for this painter
    void setCompositeOp(const KoCompositeOp * op) { m_compositeOp = op; }
    const KoCompositeOp * compositeOp() { return m_compositeOp; }

    /// Calculate the distance that point p is from the line made by connecting l0 and l1
    static double pointToLineDistance(const QPointF& p, const QPointF& l0, const QPointF& l1);

    /**
     * Add the r to the current dirty rect, and return the dirtyRegion after adding r to it.
     */
    QRegion addDirtyRect(QRect r);

    /**
     * Reset the selection to the given selection. All painter actions will be
     * masked by the specified selection.
     */
    void setSelection(KisSelectionSP selection) { m_selection = selection; }

    /**
     * @return the selection set on this painter.
     */
    KisSelectionSP selection() { return m_selection; }

protected:
    /// Initialize, set everything to '0' or defaults
    void init();

    /// Fill the polygon defined by points with the fillStyle
    void fillPolygon(const vQPointF& points, FillStyle fillStyle);

private:

    KisPainter(const KisPainter&);
    KisPainter& operator=(const KisPainter&);

protected:

    KisPaintDeviceSP m_device;
    KisSelectionSP m_selection;
    KisTransaction  *m_transaction;
    KoUpdater * m_progressUpdater;

    QRegion m_dirtyRegion;
    QRect m_dirtyRect;

    QRect m_bounds;
    KoColor m_paintColor;
    KoColor m_backgroundColor;
    KoColor m_fillColor;
    KisPaintLayer *m_sourceLayer;
    KisComplexColor *m_complexColor;
    FillStyle m_fillStyle;
    StrokeStyle m_strokeStyle;
    bool m_antiAliasPolygonFill;
    KisBrush *m_brush;
    KisPattern *m_pattern;
    QPointF m_duplicateOffset;
    quint8 m_opacity;
    KisPaintOp * m_paintOp;
    double m_pressure;
    qint32 m_pixelSize;
    const KoColorSpace * m_colorSpace;
    KoColorProfile *  m_profile;
    KisPaintDeviceSP m_dab;
    const KoCompositeOp * m_compositeOp;
    QBitArray m_channelFlags;
    bool m_useBoundingDirtyRect;

};


#endif // KIS_PAINTER_H_

