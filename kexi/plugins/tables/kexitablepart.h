/* This file is part of the KDE project
   Copyright (C) 2003 Lucijan Busch <lucijan@kde.org>
   Copyright (C) 2002, 2003 Joseph Wenninger <jowenn@kde.org>
   Copyright (C) 2004 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXITABLEPART_H
#define KEXITABLEPART_H

#include <kexi.h>
#include <kexipart.h>
#include <kexidialogbase.h>
//#include <kexipartdatasource.h>
#include <kexipartitem.h>
#include <kexidb/fieldlist.h>
//Added by qt3to4:
#include <Q3CString>

class KexiMainWin;
//class KexiTableDataSource;

class KexiTablePart : public KexiPart::Part
{
	Q_OBJECT

	public:
		KexiTablePart(QObject *parent, const char *name, const QStringList &);
		virtual ~KexiTablePart();

		virtual bool remove(KexiMainWindow *win, KexiPart::Item &item);

		virtual tristate rename(KexiMainWindow *win, KexiPart::Item &item, 
			const QString& newName);

//		virtual KexiPart::DataSource *dataSource();

		class TempData : public KexiDialogTempData
		{
			public:
				TempData(QObject* parent);
				KexiDB::TableSchema *table;
				/*! true, if \a table member has changed in previous view. Used on view switching.
				 We're checking this flag to see if we should refresh data for DataViewMode. */
				bool tableSchemaChangedInPreviousView : 1;
		};

		static tristate askForClosingObjectsUsingTableSchema(
			QWidget *parent, KexiDB::Connection& conn, 
			KexiDB::TableSchema& table, const QString& msg);

		virtual QString i18nMessage(const Q3CString& englishMessage, 
			KexiDialogBase* dlg) const;

	protected:
		virtual KexiDialogTempData* createTempData(KexiDialogBase* dialog);

		virtual KexiViewBase* createView(QWidget *parent, KexiDialogBase* dialog, 
			KexiPart::Item &item, int viewMode = Kexi::DataViewMode, QMap<QString,QString>* staticObjectArgs = 0);

		virtual void initPartActions();
		virtual void initInstanceActions();

	virtual KexiDB::SchemaData* loadSchemaData(KexiDialogBase *dlg, const KexiDB::SchemaData& sdata, int viewMode);
};

#if 0
class KexiTableDataSource : public KexiPart::DataSource
{
	public:
		KexiTableDataSource(KexiPart::Part *part);
		~KexiTableDataSource();

		virtual KexiDB::FieldList *fields(KexiProject *project, const KexiPart::Item &item);
		virtual KexiDB::Cursor *cursor(KexiProject *project, const KexiPart::Item &item, bool buffer);
};
#endif

#endif

