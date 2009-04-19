/*
 *  Copyright (c) 2007 Cyrille Berger <cberger@cberger.net>
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

#include "recorder/kis_recorded_action.h"
#include <QDomDocument>
#include <QDomElement>
#include <QString>

#include <kis_image.h>
#include <kis_node.h>
#include <kis_layer.h>
#include <kis_group_layer.h>

struct KisRecordedAction::Private {
    QString name;
    QString id;
};

KisRecordedAction::KisRecordedAction(const QString& name, const QString& id) : d(new Private)
{
    d->name = name;
    d->id = id;
}

KisRecordedAction::KisRecordedAction(const KisRecordedAction& rhs) : d(new Private(*rhs.d))
{

}

KisRecordedAction::~KisRecordedAction()
{
    delete d;
}

const QString& KisRecordedAction::id() const
{
    return d->id;
}

const QString& KisRecordedAction::name() const
{
    return d->name;
}

void KisRecordedAction::setName(const QString& name)
{
    d->name = name;
}

void KisRecordedAction::toXML(QDomDocument& , QDomElement& elt) const
{
    elt.setAttribute("name", name());
    elt.setAttribute("id", id());
}

QString KisRecordedAction::nodeToIndexPath(KisNodeSP _node)
{
    QString path;
    KisNodeSP node = _node;
    KisNodeSP parent = 0;
    while ((parent = node->parent())) {
        int index = parent->index(node);
        if (index >= 0) {
            path = (QString("\\%0").arg(index)) + path;
        }
        node = parent;
    }
    return path;
}

struct KisRecordedActionFactory::Private {
    QString id;
};

KisRecordedActionFactory::KisRecordedActionFactory(QString id) : d(new Private)
{
    d->id = id;
}

KisRecordedActionFactory::~KisRecordedActionFactory()
{
    delete d;
}

QString KisRecordedActionFactory::id() const
{
    return d->id;
}

QString KisRecordedActionFactory::name() const
{
    return QString();
}

KisNodeSP KisRecordedActionFactory::indexPathToNode(KisImageSP img, const QString & path)
{
    QStringList indexes = path.split("\\");
    KisNodeSP current = (img->rootLayer()->at(indexes[0].toUInt()));
    for (int i = 1; i < indexes.size(); i++) {
        KisGroupLayer* groupCurrent = dynamic_cast<KisGroupLayer*>(current.data());
        if (groupCurrent) {
            current = groupCurrent->at(indexes[i].toUInt());
        } else {
            break;
        }
    }
    return current;
}
