/* This file is part of the KDE project
   Copyright (C) 2002   Lucijan Busch <lucijan@gmx.at>
   Daniel Molkentin <molkentin@kde.org>
   Copyright (C) 2003-2004 Jaroslaw Staniek <js@iidea.pl>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
 
   Original Author:  Till Busch <till@bux.at>
   Original Project: buX (www.bux.at)
*/

#ifndef KEXITABLEVIEWDATA_H
#define KEXITABLEVIEWDATA_H

#include <qptrlist.h>
#include <qvariant.h>
#include <qvaluevector.h>
#include <qstring.h>
#include <qobject.h>

#include "kexitableitem.h"

#include <kexidb/error.h>

namespace KexiDB {
class Field;
class QuerySchema;
class RowEditBuffer;
class Cursor;
}

class KexiValidator;
class KexiTableViewData;


/*! Single column definition. */
class KEXIDATATABLE_EXPORT KexiTableViewColumn {
	public:
		typedef QPtrList<KexiTableViewColumn> List;
		typedef QPtrListIterator<KexiTableViewColumn> ListIterator;

		/*! Not db-aware ctor. if \a owner is true, the field \a will be owned by this column,
		 so you shouldn't care about destroying this field. */
		KexiTableViewColumn(KexiDB::Field& f, bool owner = false);

		/*! Convenience ctor, like above. The field is created using specifed parameters that are 
		 equal to these accepted by KexiDB::Field ctor. The column will be the owner 
		 of this automatically generated field.
		 */
		KexiTableViewColumn(const QString& name, KexiDB::Field::Type ctype,
			uint cconst=KexiDB::Field::NoConstraints,
			uint options = KexiDB::Field::NoOptions,
			uint length=0, uint precision=0,
			QVariant defaultValue=QVariant(),
			const QString& caption = QString::null,
			const QString& description = QString::null,
			uint width = 0);

		//! Db-aware version.
		KexiTableViewColumn(const KexiDB::QuerySchema &query, KexiDB::QueryColumnInfo& fi);

		virtual ~KexiTableViewColumn();

		virtual bool acceptsFirstChar(const QChar& ch) const;

		/*! \return true if the column is read-only
		 For db-aware column this can depend on whether the column 
		 is in parent table of this query. \sa setReadOnly() */
		inline bool readOnly() const { return m_readOnly; }

//TODO: syncroize this with table view:
		//! forces readOnly flag to be set to \a ro
		inline void setReadOnly(bool ro) { m_readOnly=ro; }

		//! Column visibility. By default column is visible.
		inline bool visible() const { return fieldinfo ? fieldinfo->visible : m_visible; }

		//! Changes column visibility.
		inline void setVisible(bool v) { if (fieldinfo) fieldinfo->visible=v;
			m_visible=v; }

		/*! \return whatever is available: 
		 - field's caption
		 - or field's alias (from query)
		 - or finally - field's name */
		inline QString captionAliasOrName() const { return m_captionAliasOrName; }

		/*! Assigns validator \a v for this column. 
		 If the validator has no parent obejct, it will be owned by the column, 
		 so you shouldn't care about destroying it. */
		void setValidator( KexiValidator* v );

		//! \return validator assigned for this column of 0 if there is no validator assigned.
		inline KexiValidator* validator() const { return m_validator; }

		/*! For not-db-aware data only:
		 Sets related data \a data for this column, what defines simple one-field, 
		 one-to-many relationship between this column and the primary key in \a data. 
		 The relationship will be used to generate a popup editor instead of just regular editor.
		 This assignment has no result if \a data has no primary key defined.
		 \a data is owned, so is will be destroyed when needed. It is also destroyed
		 when another data (or NULL) is set for the same column. */
		void setRelatedData(KexiTableViewData *data);

		/*! For not-db-aware data only:
		 Related data \a data for this column, what defines simple one-field. 
		 NULL by default. \sa setRelatedData() */
		inline KexiTableViewData *relatedData() const { return m_relatedData; }

		/*! \return field for this column. 
		 For db-aware information is taken from \a fieldinfo member. */
		inline KexiDB::Field* field() const { return m_field; }

		/*! Only usable if related data is set (ie. this is for combo boxes).
		 Sets 'editable' flag for this column, what means a new value can be entered
		 by hand. This is similar to QComboBox::setEditable(). */
		void setRelatedDataEditable(bool set);

		/*! Only usable if related data is set (ie. this is for combo boxes).
		 \return 'editable' flag for this column. 
		 False by default. @see setRelatedDataEditable(bool). */
		inline bool relatedDataEditable() const { return m_relatedDataEditable; }

		/*! A rich field information for db-aware data. 
		 For not-db-aware data it is always 0 (use field() instead. */
		KexiDB::QueryColumnInfo* fieldinfo;

		bool isDBAware : 1; //!< true if data is stored in DB, not only in memeory

/*		QString caption;
		int type; //!< one of KexiDB::Field::Type
		uint width;
*/
//		bool isNull() const;
		
/*		virtual QString caption() const;
		virtual void setCaption(const QString& c);
	*/	
	protected:
		//! special ctor that do not allocate d member;
		KexiTableViewColumn(bool);

		void init();

		QString m_captionAliasOrName;

		KexiValidator* m_validator;

		//! Data that this column is assigned to.
		KexiTableViewData* m_data;

		KexiTableViewData* m_relatedData;
		uint m_relatedDataPKeyID;

		KexiDB::Field* m_field;

		bool m_readOnly : 1;
		bool m_fieldOwned : 1;
		bool m_visible : 1;
		bool m_relatedDataEditable : 1;
		
	friend class KexiTableViewData;
};


/*! List of column definitions. */
//typedef QValueVector<KexiTableViewColumn> KexiTableViewColumnList;

typedef QPtrList<KexiTableItem> KexiTableViewDataBase;

/*! Reimplements QPtrList to allow configurable sorting and more.
	Original author: Till Busch.
	Reimplemented by Jaroslaw Staniek.
*/
class KEXIDATATABLE_EXPORT KexiTableViewData : public QObject, protected KexiTableViewDataBase
{
	Q_OBJECT

public: 
	typedef QPtrListIterator<KexiTableItem> Iterator;

	//! not db-aware version
	KexiTableViewData();

	//! db-aware version
	KexiTableViewData(KexiDB::Cursor *c);

//TODO: make this more generic: allow to add more columns!
	/*! Defines two-column table usually used with comboboxes.
	 First column is invisible and contains key values.
	 Second column and contains user-visible value.
	 @param keys a list of keys
	 @param values a list of text values (must be of the same length as keys list)
	 @param keyType a type for keys
	 @param valueType a type for values
	*/
	KexiTableViewData(
		const QValueList<QVariant> &keys, const QValueList<QVariant> &values,
		KexiDB::Field::Type keyType = KexiDB::Field::Text, 
		KexiDB::Field::Type valueType = KexiDB::Field::Text);

	/*! Like above constructor, but keys and values are not provided.
	 You can do this later by calling append(KexiTableItem*) method.
	 (KexiTableItem object must have exactly two columns)
	*/
	KexiTableViewData(KexiDB::Field::Type keyType, KexiDB::Field::Type valueType);

	~KexiTableViewData();
//js	void setSorting(int key, bool order=true, short type=1);

	/*! Preloads all rows provided by cursor (only for db-aware version). */
	void preloadAllRows();

	/*! Sets sorting for \a column. If \a column is -1, sorting is disabled. */
	void setSorting(int column, bool ascending=true);

	/*! \return the column number by which the data is sorted, 
	 or -1 if sorting is disabled. */
	inline int sortedColumn() const { return m_key; }

	/*! \return true if ascending sort order is set, or false if sorting is descending.
	 This is independant of whether data is sorted now.
	*/
	inline bool sortingAscending() const { return m_order == 1; }

	/*! Adds column \a col. 
	 Warning: \a col will be owned by this object, and deleted on its destruction. */
	void addColumn( KexiTableViewColumn* col );

	inline int globalColumnID(int visibleID) { return m_globalColumnsIDs.at( visibleID ); }
	inline int visibleColumnID(int globalID) { return m_visibleColumnsIDs.at( globalID ); }

	/*virtual?*/
	/*! \return true if this db-aware data set. */
	inline bool isDBAware() { return m_cursor!=0; }

	/*! For db-aware data set only: table name is returned;
	 equivalent to cursor()->query()->parentTable()->name(). */
	QString dbTableName() const;

	inline KexiDB::Cursor* cursor() const { return m_cursor; }

	inline uint columnsCount() const { return columns.count(); }

	inline KexiTableViewColumn* column(uint c) { return columns.at(c); }

	/*! Columns information */
	KexiTableViewColumn::List columns;

	/*! \return true if data is not editable. */
	virtual bool isReadOnly() const { return m_readOnly; }

	/*! Sets readOnly flag for this data.
	 If \a set is true, insertingEnabled flag will be cleared automatically.
	 \sa isInsertingEnabled() */
	virtual void setReadOnly(bool set);

	/*! \return true if data inserting is enabled (the default). */
	virtual bool isInsertingEnabled() const { return m_insertingEnabled; }

	/*! Sets insertingEnabled flag. If true, empty row is available 
	 If \a set is true, read-only flag will be cleared automatically.
	 \sa setReadOnly() */
	virtual void setInsertingEnabled(bool set);

	/*! Clears and initializes internal row edit buffer for incoming editing. 
	 Creates buffer using KexiDB::RowEditBuffer(false) (false means not db-aware type) 
	 if our data is not db-aware,
	 or db-aware buffer if data is db-aware (isDBAware()==true).
	 \sa KexiDB::RowEditBuffer
	*/
	void clearRowEditBuffer();

	/*! Updates internal row edit buffer: currently edited column (number \colnum) 
	 has now assigned new value of \a newval.
	 Uses column's caption to address the column in buffer 
	 if the buffer is of simple type, or db-aware buffer if (isDBAware()==true).
	 (then fields are addressed with KexiDB::Field, instead of caption strings).
	 If \a allowSignals is true (the default), aboutToChangeCell() signal is emitted.
	 Note that \a newval may be changed in aboutToChangeCell() signal handler.
	 \sa KexiDB::RowEditBuffer */
	bool updateRowEditBufferRef(KexiTableItem *item, int colnum, QVariant& newval, 
		bool allowSignals = true);

	/*! Added for conveniency. Like above but \a newval is passed by value. */
	inline bool updateRowEditBuffer(KexiTableItem *item, int colnum, QVariant newval, 
		bool allowSignals = true)
	{
		QVariant newv(newval);
		return updateRowEditBufferRef(item, colnum, newv, allowSignals);
	}

	inline KexiDB::RowEditBuffer* rowEditBuffer() const { return m_pRowEditBuffer; }

	/*! \return last operation's result information (always not null). */
	inline KexiDB::ResultInfo* result() { return &m_result; }

	bool saveRowChanges(KexiTableItem& item, bool repaint = false);

	bool saveNewRow(KexiTableItem& item, bool repaint = false);

	bool deleteRow(KexiTableItem& item, bool repaint = false);

	/*! Deletes rows (by number) passed with \a rowsToDelete. 
	 Currently, this method is only for non data-aware tables. */
	void deleteRows( const QValueList<int> &rowsToDelete, bool repaint = false );

	/*! Deletes all rows. Works either for db-aware and non db-aware tables.
	 Column's definition is not changed.
	 For db-aware version, all rows are removed from a database.
	 Row-edit buffer is cleared.

	 If \a repaint is true, refreshRequested() signal 
	 is emitted after deleting (if at least one row was deleted), 
	 so presenters can repaint their contents. 

	 \return true on success. */
	virtual bool deleteAllRows(bool repaint = false);

	/*! @internal method, used mostly by specialized classes like KexiTableView.
	 Clears internal row structures. Row-edit buffer is cleared. 
	 Does not touch data @ database backend.
	 Use deleteAllRows() to safely delete all rows. */
	virtual void clearInternal();

	/*! Inserts new \a item at index \a index. 
	 \a item will be owned by this data object.
	 Note: Reasonable only for not not-db-aware version. */
	void insertRow(KexiTableItem& item, uint index, bool repaint = false);

/*TODO: add this as well? 
	void insertRow(KexiTableItem& item, KexiTableItem& aboveItem); */

	//! \return index of autoincremented column. The result is cached.
//! \todo what about multiple autoinc columns?
//! \todo what about changing column order?
	int autoIncrementedColumn();

	//! Emits refreshRequested() signal to refresh presenters.
	void refresh() { emit refreshRequested(); }

	inline KexiTableItem* at( uint index ) { return KexiTableViewDataBase::at(index); }
	inline virtual uint count() const { return KexiTableViewDataBase::count(); }
	inline bool isEmpty () const { return KexiTableViewDataBase::isEmpty(); }
	inline KexiTableItem* first() { return KexiTableViewDataBase::first(); }
	inline KexiTableItem* last() { return KexiTableViewDataBase::last(); }
	inline int findRef( const KexiTableItem* item ) { return KexiTableViewDataBase::findRef(item); }
	inline void sort() { KexiTableViewDataBase::sort(); }
	inline bool removeFirst() { return KexiTableViewDataBase::removeFirst(); }
	inline bool removeLast() { return KexiTableViewDataBase::removeLast(); }
	inline void append( const KexiTableItem* item ) { KexiTableViewDataBase::append(item); }
	inline void prepend( const KexiTableItem* item ) { KexiTableViewDataBase::prepend(item); }
	inline Iterator iterator() { return Iterator(*this); }

signals:
	void destroying();

	/*! Emitted before change of the single, currently edited cell.
	 Connect this signal to your slot and set \a result->success to false
	 to disallow this change. You can also change \a newValue to other value,
	 or change other columns in \a item row. */
	void aboutToChangeCell(KexiTableItem *item, int colnum, QVariant& newValue,
		KexiDB::ResultInfo* result);

	/*! Emited before inserting of a new, current row.
	 Connect this signal to your slot and set \a result->success to false 
	 to disallow this inserting. You can also change columns in \a item row. */
	void aboutToInsertRow(KexiTableItem *item, KexiDB::ResultInfo* result, bool repaint);

	/*! Emited before changing of an edited, current row.
	 Connect this signal to your slot and set \a result->success to false 
	 to disallow this change. You can also change columns in \a item row. */
	void aboutToUpdateRow(KexiTableItem *item, KexiDB::RowEditBuffer* buffer,
		KexiDB::ResultInfo* result);

	void rowUpdated(KexiTableItem*); //!< Current row has been updated

	void rowInserted(KexiTableItem*, bool repaint); //!< A row has been inserted

	//! A row has been inserted at \a index position (not db-aware data only)
	void rowInserted(KexiTableItem*, uint index, bool repaint);

	/*! Emited before deleting of a current row.
	 Connect this signal to your slot and set \a result->success to false 
	 to disallow this deleting. */
	void aboutToDeleteRow(KexiTableItem& item, KexiDB::ResultInfo* result, bool repaint);

	//! Current row has been deleted
	void rowDeleted(); 

	//! Rows have been deleted
	void rowsDeleted( const QValueList<int> &rowsToDelete );

	//! Displayed data needs to be refreshed in all presenters.
	void refreshRequested();

	void rowRepaintRequested(KexiTableItem&);

protected:
	void init();
	void init(
		const QValueList<QVariant> &keys, const QValueList<QVariant> &values,
		KexiDB::Field::Type keyType, KexiDB::Field::Type valueType);

	virtual int compareItems(Item item1, Item item2);
	int cmpStr(Item item1, Item item2);
	int cmpInt(Item item1, Item item2);

	//! internal: for saveRowChanges() and saveNewRow()
	bool saveRow(KexiTableItem& item, bool insert, bool repaint);

	int			m_key;
	short		m_order;
	short		m_type;
	static unsigned short charTable[];
	KexiDB::RowEditBuffer *m_pRowEditBuffer;
	KexiDB::Cursor *m_cursor;

	//! used to faster lookup columns of simple type (not dbaware)
//	QDict<KexiTableViewColumn> *m_simpleColumnsByName;

	KexiDB::ResultInfo m_result;

	uint m_visibleColumnsCount;
	QValueVector<int> m_visibleColumnsIDs, m_globalColumnsIDs;

	bool m_readOnly : 1;
	bool m_insertingEnabled : 1;

	int m_autoIncrementedColumn;

	int (KexiTableViewData::*cmpFunc)(void *, void *);
};

#endif
