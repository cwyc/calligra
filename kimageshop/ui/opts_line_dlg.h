/*
 *  opts_line_dlg.h - part of Krayon
 *
 *  Copyright (c) 2001 John Califf 
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
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __lineoptionsdialog_h__
#define __lineoptionsdialog_h__

#include <qspinbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>

#include <kdialog.h>

class LineOptionsDialog : public KDialog
{
  Q_OBJECT

public:

    LineOptionsDialog( bool _solid = false,  bool _pattern = false,
        int _thickness = 1, int _opacity = 255,
        QWidget *parent = 0, const char *name = 0 );

    int  thickness()    { return mpThickness->value(); };
    int  opacity()      { return mpOpacity->value(); };
    bool solid()        { return mpSolid->isChecked(); };
    bool usePattern()   { return mpUsePattern->isChecked(); };
    bool gradient()     { return mpUseGradient->isChecked(); };
    
private:

    QSpinBox  *mpThickness;
    QSpinBox  *mpOpacity;
    
    QCheckBox *mpSolid;
    QCheckBox *mpUsePattern;
    QCheckBox *mpUseGradient;
};

#endif // __lineoptionsdialog.h__
