/*
 *  Copyright (c) 2010 Dmitry Kazakov <dimula73@gmail.com>
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
#ifndef KIS_LEGACY_TILE_COMPRESSOR_TEST_H
#define KIS_LEGACY_TILE_COMPRESSOR_TEST_H

#include <QtTest/QtTest>

class KisAbstractTileCompressor;

class KisLegacyTileCompressorTest : public QObject
{
    Q_OBJECT
private:
    void doRoundTrip(KisAbstractTileCompressor *compressor);
    void doLowLevelRoundTrip(KisAbstractTileCompressor *compressor);
    void doLowLevelRoundTripIncompressible(KisAbstractTileCompressor *compressor);


private slots:
    void testRoundTripLegacy();
    void testLowLevelRoundTripLegacy();

    void testRoundTrip2();
    void testLowLevelRoundTrip2();
    void testLowLevelRoundTripIncompressible2();
};

#endif /* KIS_LEGACY_TILE_COMPRESSOR_TEST_H */

