/*
 *  kis_tool_colorchanger.h - part of Krayon
 *
 *  Copyright (c) 2000 John Califf
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

#ifndef __colorchangertool_h__
#define __colorchangertool_h__

#include <qpoint.h>

#include "kis_color.h"
#include "kis_view.h"
#include "kis_tool.h"
#include "kis_layer.h"
#include "kis_framebuffer.h"


class ColorChangerTool : public KisTool
{
  public:
    ColorChangerTool(KisDoc *doc, KisView *view);
    ~ColorChangerTool();
  
    QString toolName() { return QString("Colorchanger Tool"); }
    bool changeColors(int startx, int starty);
    
  public slots:
    virtual void mousePress(QMouseEvent*); 
    virtual void optionsDialog();
      
  protected:
                
    // new colors (desired)
    int nRed;
    int nGreen;
    int nBlue;

    // source colors (existing)
    int sRed;
    int sGreen;
    int sBlue;

    int fillOpacity;
    bool layerAlpha;
    bool usePattern;
        
    int toleranceRed;
    int toleranceGreen;    
    int toleranceBlue;

    KisLayer *fLayer;
};

#endif //__colorchangertool_h__

