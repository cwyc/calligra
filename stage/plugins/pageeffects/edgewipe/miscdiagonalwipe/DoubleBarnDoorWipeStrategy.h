/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef DOUBLEBARNDOORWIPESTRATEGY_H
#define DOUBLEBARNDOORWIPESTRATEGY_H

#include <KPrPageEffectStrategy.h>

class DoubleBarnDoorWipeStrategy : public KPrPageEffectStrategy
{
public:
    DoubleBarnDoorWipeStrategy();
    ~DoubleBarnDoorWipeStrategy() override;

    /// reimplemented from KPrPageEffectStrategy
    void setup( const KPrPageEffect::Data &data, QTimeLine &timeLine ) override;
    /// reimplemented from KPrPageEffectStrategy
    void paintStep( QPainter &p, int currPos, const KPrPageEffect::Data &data ) override;
    /// reimplemented from KPrPageEffectStrategy
    void next( const KPrPageEffect::Data &data ) override;

private:
    /// calculates clip path for given step
    QPainterPath clipPath( int step, const QRect &area );
};

#endif // DOUBLEBARNDOORWIPESTRATEGY_H
