/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2009 Jan Hambrecht <jaham@gmx.net>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#ifndef BLUREFFECTFACTORY_H
#define BLUREFFECTFACTORY_H

#include "KoFilterEffectFactoryBase.h"

class KoFilterEffect;

class BlurEffectFactory : public KoFilterEffectFactoryBase
{
public:
    BlurEffectFactory();
    KoFilterEffect * createFilterEffect() const override;
    KoFilterEffectConfigWidgetBase * createConfigWidget() const override;
};

#endif // BLUREFFECTFACTORY_H
