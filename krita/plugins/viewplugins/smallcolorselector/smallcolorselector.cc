/*
 *  Copyright (c) 2008 Cyrille Berger <cberger@cberger.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; version 2 of the License.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "smallcolorselector.h"

#include <stdlib.h>

#include <QTimer>

#include <kactioncollection.h>
#include <kcomponentdata.h>
#include <kis_debug.h>
#include <kgenericfactory.h>
#include <klocale.h>
#include <kstandarddirs.h>

#include <KoDockFactory.h>

#include "kis_config.h"
#include "kis_cursor.h"
#include "kis_global.h"
#include "kis_types.h"
#include "kis_view2.h"

#include "smallcolorselector_dock.h"

typedef KGenericFactory<SmallColorSelectorPlugin> SmallColorSelectorPluginFactory;
K_EXPORT_COMPONENT_FACTORY( kritasmallcolorselector, SmallColorSelectorPluginFactory( "krita" ) )

class SmallColorSelectorDockFactory : public KoDockFactory {
public:
    SmallColorSelectorDockFactory(KisView2 * view)
        : m_view( view )
    {
    }

    virtual QString id() const
    {
        return QString( "SmallColorSelector" );
    }

    virtual Qt::DockWidgetArea defaultDockWidgetArea() const
    {
        return Qt::RightDockWidgetArea;
    }

    virtual QDockWidget* createDockWidget()
    {
        SmallColorSelectorDock * dockWidget = new SmallColorSelectorDock(m_view);
        
        dockWidget->setObjectName(id());

        return dockWidget;
    }

private:
    KisView2 * m_view;

};


SmallColorSelectorPlugin::SmallColorSelectorPlugin(QObject *parent, const QStringList &)
    : KParts::Plugin(parent)
{
    dbgPlugins << "SmallColorSelectorPlugin";
    if ( parent->inherits("KisView2") )
    {
        m_view = (KisView2*) parent;

        setComponentData(SmallColorSelectorPluginFactory::componentData());
        SmallColorSelectorDockFactory dockFactory( m_view);
        m_view->createDockWidget( &dockFactory );
    }
}

SmallColorSelectorPlugin::~SmallColorSelectorPlugin()
{
    m_view = 0;
}

#include "smallcolorselector.moc"
