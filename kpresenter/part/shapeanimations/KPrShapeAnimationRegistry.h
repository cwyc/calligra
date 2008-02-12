/* This file is part of the KDE project
   Copyright (C) 2007-2008 Thorsten Zachmann <zachmann@kde.org>

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
 * Boston, MA 02110-1301, USA.
*/

#ifndef KPRSHAPEANIMATIONREGISTRY_H
#define KPRSHAPEANIMATIONREGISTRY_H

#include <KoGenericRegistry.h>

class KoXmlElement;
class KPrShapeAnimation;
class KPrShapeAnimationFactory;

class KPrShapeAnimationRegistry : public KoGenericRegistry<KPrShapeAnimationFactory *>
{
public:
    class Singleton;
    static KPrShapeAnimationRegistry * instance();

    KPrShapeAnimation * createShapeAnimation( const KoXmlElement & element );

private:
    friend class Singleton;
    KPrShapeAnimationRegistry();
    ~KPrShapeAnimationRegistry();
};

#endif /* KPRSHAPEANIMATIONREGISTRY_H */
