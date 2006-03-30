/* This file is part of the KDE project
   Copyright (C) 2005 Jaroslaw Staniek <js@iidea.pl>

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

#ifndef KEXI_CSVWIDGETS_H
#define KEXI_CSVWIDGETS_H

#include <q3valuevector.h>
//Added by qt3to4:
#include <QLabel>
#include <Q3Frame>
#include <kcombobox.h>

class KLineEdit;
class KActiveLabel;
class QLabel;

#define KEXICSV_DEFAULT_TEXT_QUOTE "\""
#define KEXICSV_DEFAULT_FILE_DELIMITER ","
#define KEXICSV_DEFAULT_CLIPBOARD_DELIMITER "\t"
#define KEXICSV_DEFAULT_FILE_DELIMITER_INDEX 0

//! \return a list of mimetypes usable for handling CSV format handling
QStringList csvMimeTypes();

/*! @short A helper widget showing a short text information with an icon.
 See ctor for details.
 Used by CSV import and export dialogs. */
class KexiCSVInfoLabel : public QWidget
{
	public:
		/* Sets up a new info label with following parts:
		 - \a labelText label with text like "Preview of data from file:"
		 - \a fileName (active) label that can contain a filename or be empty.
		 If \a setIconForFilename is true, icon will be set using a pixmap get 
		 from a mime type for \a fileName. */
		KexiCSVInfoLabel( const QString& labelText, QWidget* parent );

		void setFileName( const QString& fileName );
		void setLabelText( const QString& text );
//		void setIconForFileName();

		//! sets icon pixmap to \a iconName. Used wher setIconForFilename was false in ctor.
		void setIcon(const QString& iconName);

		QLabel* leftLabel() const { return m_leftLabel; }
		KActiveLabel* fileNameLabel() const { return m_fnameLbl; }
		Q3Frame* separator() const { return m_separator; }

	protected:
		QLabel *m_leftLabel, *m_iconLbl;
		KActiveLabel *m_fnameLbl;
		Q3Frame* m_separator;
};

//! @short A combo box widget providing a list of possible delimiters
//! Used by CSV import and export dialogs
class KexiCSVDelimiterWidget : public QWidget
{
	Q_OBJECT

	public:
		KexiCSVDelimiterWidget( bool lineEditOnBottom, QWidget * parent = 0 );

		QString delimiter() const { return m_delimiter; }
		void setDelimiter(const QString& delimiter);

	signals:
		void delimiterChanged(const QString& delimiter);

	protected slots:
		//! only called when a delimiter was set by user directly
		void slotDelimiterChanged(int idx);
		void slotDelimiterChangedInternal(int idx);
		void slotDelimiterLineEditTextChanged( const QString & );
		void slotDelimiterLineEditReturnPressed();

	protected:
		QString m_delimiter;
		Q3ValueVector<QString> m_availableDelimiters;
		KComboBox* m_combo;
		KLineEdit* m_delimiterEdit;
};

//! @short A combo box widget providing a list of possible quote characters
//! Used by CSV import and export dialogs
class KexiCSVTextQuoteComboBox : public KComboBox
{
	public:
		KexiCSVTextQuoteComboBox( QWidget * parent = 0 );

		QString textQuote() const;

		//! Sets text quote. Only available are: ", ', and empty string.
		void setTextQuote(const QString& textQuote);
};

#endif
