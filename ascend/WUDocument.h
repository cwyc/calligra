/* This file is part of the KDE project
   Copyright (C) 2005 Inge Wallin <inge@lysator.liu.se>

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

#ifndef wudoc_h
#define wudoc_h

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "WUView.h"
#include "defs.h" // for MouseMeaning

#include <KWDocument.h>


/******************************************************************/
/* Class: WUDocument                                           */
/******************************************************************/


class WUDocument : public KWDocument
{
    Q_OBJECT

public:

    WUDocument( QWidget *parentWidget = 0, const char *widname = 0, QObject* parent = 0, const char* name = 0, bool singleViewMode = false );
    ~WUDocument();

    void sig_frameSelectedChanged();

public slots:


protected slots:


protected:


private:
};


#endif
