/* This file is part of the KDE project
   Copyright (C) 2004 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2004 Cedric Pasteur <cedric.pasteur@free.fr>

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
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include <kdebug.h>
#include <kgenericfactory.h>

#include "kexiviewbase.h"
#include "keximainwindow.h"
#include "kexiproject.h"
#include <kexipartitem.h>
#include <kexidialogbase.h>

#include <kexidb/connection.h>
#include <kexidb/fieldlist.h>
#include <kexidb/field.h>

#include <form.h>
#include <formIO.h>
#include <formmanager.h>

#include "kexiformview.h"
#include "kexiformpart.h"

KexiFormPart::KexiFormPart(QObject *parent, const char *name, const QStringList &l)
 : KexiPart::Part(parent, name, l)
{
	kdDebug() << "KexiFormPart::KexiFormPart()" << endl;
	m_names["instance"] = i18n("Form");
	m_supportedViewModes = Kexi::DataViewMode | Kexi::DesignViewMode;

	m_manager = new KFormDesigner::FormManager(this, "manager");
}

KexiFormPart::~KexiFormPart()
{
}

#if 0
void KexiFormPart::initPartActions(KActionCollection *collection)
{
//this is automatic? -no
//create child guicilent: guiClient()->setXMLFile("kexidatatableui.rc");

	kdDebug()<<"FormPart INIT ACTIONS***********************************************************************"<<endl;
	//TODO

	//guiClient()->setXMLFile("kexiformui.rc");
//js	m_manager->createActions(collection, 0);
}

void KexiFormPart::initInstanceActions( int mode, KActionCollection *col )
{
	if (mode==Kexi::DesignViewMode) {
		m_manager->createActions(col, 0);
		new KAction(i18n("Edit Tab Order"), "tab_order", KShortcut(0), m_manager, SLOT(editTabOrder()), col, "taborder");
		new KAction(i18n("Adjust Size"), "viewmagfit", KShortcut(0), m_manager, SLOT(ajustWidgetSize()), col, "adjust");
	}
	//TODO
}
#endif

void KexiFormPart::initActions()
{
	m_manager->createActions(actionCollectionForMode(Kexi::DesignViewMode));
	createSharedAction(Kexi::DesignViewMode, i18n("Edit Tab Order"), "tab_order", 0, "formpart_taborder");
	createSharedAction(Kexi::DesignViewMode, i18n("Edit Pixmap Collection"), "icons", 0, "formpart_pixmap_collection");
	createSharedAction(Kexi::DesignViewMode, i18n("Edit Form Connections"), "connections", 0, "formpart_connections");

	createSharedAction(Kexi::DesignViewMode, i18n("Lay Out Widgets &Horizontally"), QString::null, 0, "formpart_layout_hbox");
	createSharedAction(Kexi::DesignViewMode, i18n("Lay Out Widgets &Vertically"), QString::null, 0, "formpart_layout_vbox");
	createSharedAction(Kexi::DesignViewMode, i18n("Lay Out Widgets in &Grid"), QString::null, 0, "formpart_layout_grid");
	createSharedAction(Kexi::DesignViewMode, i18n("&Break Layout"), QString::null, 0, "formpart_break_layout");

	createSharedAction(Kexi::DesignViewMode, i18n("Bring Widget to Front"), "raise", 0, "formpart_format_raise");
	createSharedAction(Kexi::DesignViewMode, i18n("Send Widget to Back"), "lower", 0, "formpart_format_lower");

	KAction *action = createSharedAction(Kexi::DesignViewMode, i18n("Align Widgets position"), "aopos2grid", 0, "formpart_align_menu", "KActionMenu");
	KActionMenu *menu = static_cast<KActionMenu*>(action);
	menu->insert( createSharedAction(Kexi::DesignViewMode, i18n("To Left"), "aoleft", 0, "formpart_align_to_left") );
	menu->insert( createSharedAction(Kexi::DesignViewMode, i18n("To Right"), "aoright", 0, "formpart_align_to_right") );
	menu->insert( createSharedAction(Kexi::DesignViewMode, i18n("To Top"), "aotop", 0, "formpart_align_to_top") );
	menu->insert( createSharedAction(Kexi::DesignViewMode, i18n("To Bottom"), "aobottom", 0, "formpart_align_to_bottom") );
	menu->insert( createSharedAction(Kexi::DesignViewMode, i18n("To Grid"), "aopos2grid", 0, "formpart_align_to_grid") );

	action = createSharedAction(Kexi::DesignViewMode, i18n("Adjust Widgets size"), "aogrid", 0, "formpart_adjust_size_menu", "KActionMenu");
	menu = static_cast<KActionMenu*>(action);
	menu->insert( createSharedAction(Kexi::DesignViewMode, i18n("To Fit"), "aofit", 0, "formpart_adjust_to_fit") );
	menu->insert( createSharedAction(Kexi::DesignViewMode, i18n("To Grid"), "aogrid", 0, "formpart_adjust_size_grid") );
	menu->insert( createSharedAction(Kexi::DesignViewMode, i18n("To Shortest"), "aoshortest", 0, "formpart_adjust_height_small") );
	menu->insert( createSharedAction(Kexi::DesignViewMode, i18n("To Tallest"), "aotallest", 0, "formpart_adjust_height_big") );
	menu->insert( createSharedAction(Kexi::DesignViewMode, i18n("To Narrowest"), "aonarrowest", 0, "formpart_adjust_width_small") );
	menu->insert( createSharedAction(Kexi::DesignViewMode, i18n("To Widest"), "aowidest", 0, "formpart_adjust_width_big") );
}

KexiViewBase* KexiFormPart::createView(QWidget *parent, KexiDialogBase* dialog,
	KexiPart::Item &item, int viewMode)
{
	kdDebug() << "KexiFormPart::createView()" << endl;
	KexiMainWindow *win = dialog->mainWin();
	if (!win || !win->project() || !win->project()->dbConnection())
		return 0;

	if (!dialog->tempData()) {
		dialog->setTempData( new KexiFormPart::TempData(dialog) );
	}
	//KexiFormPart::TempData *tempData = static_cast<KexiFormPart::TempData*>(dialog->tempData());

	KexiFormView *view = new KexiFormView(win, parent, item.name().latin1(), (viewMode == Kexi::DataViewMode)
	, win->project()->dbConnection() );

	return view;
}
/*
bool
KexiFormPart::remove(KexiMainWindow *, KexiPart::Item &)
{
	return false;
}*/

void
KexiFormPart::generateForm(KexiDB::FieldList *list, QDomDocument &domDoc)
{
	//this form generates a .ui from FieldList list
	//basically that is a Label and a LineEdit for each field
	domDoc = QDomDocument("UI");
        QDomElement uiElement = domDoc.createElement("UI");
	domDoc.appendChild(uiElement);
	uiElement.setAttribute("version", "3.1");
	uiElement.setAttribute("stdsetdef", 1);

	QDomElement baseClass = domDoc.createElement("class");
	uiElement.appendChild(baseClass);
	QDomText baseClassV = domDoc.createTextNode("QWidget");
	baseClass.appendChild(baseClassV);
	QDomElement baseWidget = domDoc.createElement("widget");
	baseWidget.setAttribute("class", "QWidget");

	int y=0;

	for(unsigned int i=0; i < list->fieldCount(); i++)
	{
		QDomElement lclass = domDoc.createElement("widget");
		baseWidget.appendChild(lclass);
		lclass.setAttribute("class", "QLabel");
		QDomElement lNameProperty = domDoc.createElement("property");
		lNameProperty.setAttribute("name", "name");
		QDomElement lType = domDoc.createElement("cstring");
		QDomText lClassN = domDoc.createTextNode(QString("l%1").arg(list->field(i)->name()));
		lType.appendChild(lClassN);
		lNameProperty.appendChild(lType);
		lclass.appendChild(lNameProperty);

		QDomElement gNameProperty = domDoc.createElement("property");
		gNameProperty.setAttribute("name", "geometry");
		QDomElement lGType = domDoc.createElement("rect");

		QDomElement lx = domDoc.createElement("x");
		QDomText lxV = domDoc.createTextNode("10");
		lx.appendChild(lxV);
		QDomElement ly = domDoc.createElement("y");
		QDomText lyV = domDoc.createTextNode(QString::number(y + 10));
		ly.appendChild(lyV);
		QDomElement lWidth = domDoc.createElement("width");
		QDomText lWidthV = domDoc.createTextNode("100");
		lWidth.appendChild(lWidthV);
		QDomElement lHeight = domDoc.createElement("height");
		QDomText lHeightV = domDoc.createTextNode("20");
		lHeight.appendChild(lHeightV);

		lGType.appendChild(lx);
		lGType.appendChild(ly);
		lGType.appendChild(lWidth);
		lGType.appendChild(lHeight);

		gNameProperty.appendChild(lGType);
		lclass.appendChild(gNameProperty);

		QDomElement tNameProperty = domDoc.createElement("property");
		tNameProperty.setAttribute("name", "text");
		QDomElement lTType = domDoc.createElement("string");
		QDomText lTextV = domDoc.createTextNode(list->field(i)->name());
		lTType.appendChild(lTextV);
		tNameProperty.appendChild(lTType);
		lclass.appendChild(tNameProperty);


		///line edit!


		QDomElement vclass = domDoc.createElement("widget");
		baseWidget.appendChild(vclass);
		vclass.setAttribute("class", "KLineEdit");
		QDomElement vNameProperty = domDoc.createElement("property");
		vNameProperty.setAttribute("name", "name");
		QDomElement vType = domDoc.createElement("cstring");
		QDomText vClassN = domDoc.createTextNode(list->field(i)->name());
		vType.appendChild(vClassN);
		vNameProperty.appendChild(vType);
		vclass.appendChild(vNameProperty);

		QDomElement vgNameProperty = domDoc.createElement("property");
		vgNameProperty.setAttribute("name", "geometry");
		QDomElement vGType = domDoc.createElement("rect");

		QDomElement vx = domDoc.createElement("x");
		QDomText vxV = domDoc.createTextNode("110");
		vx.appendChild(vxV);
		QDomElement vy = domDoc.createElement("y");
		QDomText vyV = domDoc.createTextNode(QString::number(y + 10));
		vy.appendChild(vyV);
		QDomElement vWidth = domDoc.createElement("width");
		QDomText vWidthV = domDoc.createTextNode("200");
		vWidth.appendChild(vWidthV);
		QDomElement vHeight = domDoc.createElement("height");
		QDomText vHeightV = domDoc.createTextNode("20");
		vHeight.appendChild(vHeightV);

		vGType.appendChild(vx);
		vGType.appendChild(vy);
		vGType.appendChild(vWidth);
		vGType.appendChild(vHeight);

		vgNameProperty.appendChild(vGType);
		vclass.appendChild(vgNameProperty);

		y += 20;
	}

	QDomElement lNameProperty = domDoc.createElement("property");
	lNameProperty.setAttribute("name", "name");
	QDomElement lType = domDoc.createElement("cstring");
	QDomText lClassN = domDoc.createTextNode("DBForm");
	lType.appendChild(lClassN);
	lNameProperty.appendChild(lType);
	baseWidget.appendChild(lNameProperty);



	QDomElement wNameProperty = domDoc.createElement("property");
	wNameProperty.setAttribute("name", "geometry");
	QDomElement wGType = domDoc.createElement("rect");

	QDomElement wx = domDoc.createElement("x");
	QDomText wxV = domDoc.createTextNode("0");
	wx.appendChild(wxV);
	QDomElement wy = domDoc.createElement("y");
	QDomText wyV = domDoc.createTextNode("0");
	wy.appendChild(wyV);
	QDomElement wWidth = domDoc.createElement("width");
	QDomText wWidthV = domDoc.createTextNode("340");
	wWidth.appendChild(wWidthV);
	QDomElement wHeight = domDoc.createElement("height");
	QDomText wHeightV = domDoc.createTextNode(QString::number(y + 30));
	wHeight.appendChild(wHeightV);

	wGType.appendChild(wx);
	wGType.appendChild(wy);
	wGType.appendChild(wWidth);
	wGType.appendChild(wHeight);

	wNameProperty.appendChild(wGType);
	baseWidget.appendChild(wNameProperty);

	uiElement.appendChild(baseWidget);
}

//----------------

KexiFormPart::TempData::TempData(QObject* parent)
 : KexiDialogTempData(parent)
{
}

KexiFormPart::TempData::~TempData()
{
}

K_EXPORT_COMPONENT_FACTORY( kexihandler_form, KGenericFactory<KexiFormPart> )

#include "kexiformpart.moc"

