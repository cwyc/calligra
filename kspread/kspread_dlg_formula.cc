/* This file is part of the KDE project
   Copyright (C) 1998, 1999 Torben Weis <weis@kde.org>
   Copyright (C) 1999, 2000 Montel Laurent <montell@club-internet.fr>
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

#include <qtextbrowser.h>
#include <qlayout.h>
#include <qtabwidget.h>
#include <qsizepolicy.h>

#include "kspread_dlg_formula.h"
#include "kspread_view.h"
#include "kspread_canvas.h"
#include "kspread_util.h"
#include "kspread_layout.h"
#include "kspread_table.h"
#include "kspread_editors.h"
#include "kspread_doc.h"
#include "kspread_map.h"

#include <kapp.h>
#include <kdebug.h>
#include <klocale.h>
#include <kbuttonbox.h>
#include <knumvalidator.h>
#include <kdialog.h>

KSpreadDlgFormula::KSpreadDlgFormula( KSpreadView* parent, const char* name,const QString& formulaName)
    : QDialog( parent, name )
{
    m_pView = parent;
    m_focus = 0;
    m_desc = 0;
    
    setCaption( name );

    KSpreadCell* cell = m_pView->activeTable()->cellAt( m_pView->canvasWidget()->markerColumn(),
							m_pView->canvasWidget()->markerRow() );
    QString m_oldText=cell->text();
    // Make shure that there is a cell editor running.
    if ( !m_pView->canvasWidget()->editor() )
    {
        m_pView->canvasWidget()->createEditor( KSpreadCanvas::CellEditor );
        if(cell->text().isEmpty())
                m_pView->canvasWidget()->editor()->setText( "=" );
        else
                if(cell->text().at(0)!='=')
                        m_pView->canvasWidget()->editor()->setText( "="+cell->text()+"+" );
                else
                        m_pView->canvasWidget()->editor()->setText( cell->text()+"+" );
    }

    ASSERT( m_pView->canvasWidget()->editor() );

    QGridLayout *grid1 = new QGridLayout(this,11,2,15,7);

    searchFunct = new KLineEdit(this);
    QSizePolicy sp3( QSizePolicy::Preferred, QSizePolicy::Fixed );
    searchFunct->setSizePolicy( sp3 );

    grid1->addWidget( searchFunct, 0, 0 );

    typeFunction = new QComboBox(this);
    QStringList cats = m_repo.groups();
    cats.prepend( i18n("All") );
    typeFunction->insertStringList( cats  );
    grid1->addWidget( typeFunction, 1, 0 );

    functions = new QListBox(this);
    QSizePolicy sp1( QSizePolicy::Preferred, QSizePolicy::Expanding );
    functions->setSizePolicy( sp1 );
    grid1->addWidget( functions, 2, 0 );

    selectFunction = new QPushButton( this );
    selectFunction->setPixmap( BarIcon( "down", KIcon::SizeSmall ) );
    grid1->addWidget( selectFunction, 3, 0 );

    result = new QLineEdit( this );
    grid1->addMultiCellWidget( result, 4, 4, 0, 1 );

    m_tabwidget = new QTabWidget( this );
    QSizePolicy sp2( QSizePolicy::Expanding, QSizePolicy::Expanding );
    m_tabwidget->setSizePolicy( sp2 );
    grid1->addMultiCellWidget( m_tabwidget, 0, 2, 1, 1 );

    m_browser = new QTextBrowser( m_tabwidget );
    m_browser->setMinimumWidth( 300 );

    m_tabwidget->addTab( m_browser, i18n("&Help") );
    int index = m_tabwidget->currentPageIndex();

    m_input = new QWidget( m_tabwidget );
    QVBoxLayout *grid2 = new QVBoxLayout( m_input, KDialog::marginHint(), KDialog::spacingHint() );

    // grid2->setResizeMode (QLayout::Minimum);

    label1 = new QLabel(m_input);
    grid2->addWidget( label1 );

    firstElement=new QLineEdit(m_input);
    grid2->addWidget( firstElement );

    label2=new QLabel(m_input);
    grid2->addWidget( label2 );

    secondElement=new QLineEdit(m_input);
    grid2->addWidget( secondElement );

    label3=new QLabel(m_input);
    grid2->addWidget( label3 );

    thirdElement=new QLineEdit(m_input);
    grid2->addWidget( thirdElement );

    label4=new QLabel(m_input);
    grid2->addWidget( label4 );

    fourElement=new QLineEdit(m_input);
    grid2->addWidget( fourElement );

    label5=new QLabel(m_input);
    grid2->addWidget( label5 );

    fiveElement=new QLineEdit(m_input);
    grid2->addWidget( fiveElement );

    grid2->addStretch( 10 );

    m_tabwidget->addTab( m_input, i18n("&Edit") );
    m_tabwidget->setTabEnabled( m_input, FALSE );

    m_tabwidget->setCurrentPage( index );

    // Create the Ok and Cancel buttons
    KButtonBox *bb = new KButtonBox( this );
    bb->addStretch();
    m_pOk = bb->addButton( i18n("OK") );
    m_pOk->setDefault( TRUE );
    m_pClose = bb->addButton( i18n( "Close" ) );
    bb->layout();
    grid1->addMultiCellWidget( bb, 5, 5, 0, 1 );

    refresh_result = true;

    connect( m_pClose, SIGNAL( clicked() ), this, SLOT( slotClose() ) );
    connect( m_pOk, SIGNAL( clicked() ), this, SLOT( slotOk() ) );
    connect( typeFunction, SIGNAL( activated(const QString &) ),
             this, SLOT( slotActivated(const QString &) ) );
    connect( functions, SIGNAL( highlighted(const QString &) ),
             this, SLOT( slotSelected(const QString &) ) );
    connect( functions, SIGNAL( selected(const QString &) ),
             this, SLOT( slotSelected(const QString &) ) );
    connect( functions, SIGNAL( doubleClicked(QListBoxItem * ) ),
             this ,SLOT( slotDoubleClicked(QListBoxItem *) ) );

    slotActivated(i18n("All"));

    connect( selectFunction, SIGNAL(clicked()),
	     this,SLOT(slotSelectButton()));

    connect( firstElement,SIGNAL(textChanged ( const QString & )),
	     this,SLOT(slotChangeText(const QString &)));
    connect( secondElement,SIGNAL(textChanged ( const QString & )),
	     this,SLOT(slotChangeText(const QString &)));
    connect( thirdElement,SIGNAL(textChanged ( const QString & )),
	     this,SLOT(slotChangeText(const QString &)));
    connect( fourElement,SIGNAL(textChanged ( const QString & )),
	     this,SLOT(slotChangeText(const QString &)));
    connect( fiveElement,SIGNAL(textChanged ( const QString & )),
	     this,SLOT(slotChangeText(const QString &)));

    connect( m_pView, SIGNAL( sig_chooseSelectionChanged( KSpreadTable*, const QRect& ) ),
             this, SLOT( slotSelectionChanged( KSpreadTable*, const QRect& ) ) );

    // Save the name of the active table.
    m_tableName = m_pView->activeTable()->tableName();
    // Save the cells current text.
    QString tmp_oldText = m_pView->canvasWidget()->editor()->text();
    // Position of the cell.
    m_column = m_pView->canvasWidget()->markerColumn();
    m_row = m_pView->canvasWidget()->markerRow();

    if( tmp_oldText.isEmpty() )
        result->setText("=");
    else
    {
        if( tmp_oldText.at(0)!='=')
	    result->setText( "=" + tmp_oldText );
        else
	    result->setText( tmp_oldText );
    }

    // Allow the user to select cells on the spreadsheet.
    m_pView->canvasWidget()->startChoose();

    qApp->installEventFilter( this );

    // Was a function name passed along with the constructor ? Then activate it.
    if( !formulaName.isEmpty() )
    {
        functions->setCurrentItem( functions->index( functions->findItem( formulaName ) ) );
        slotDoubleClicked( functions->findItem( formulaName ) );
    }
    else
    {
	// Set keyboard focus to allow selection of a formula.
        searchFunct->setFocus();
    }

    // Add auto completion.
    searchFunct->setCompletionMode( KGlobalSettings::CompletionAuto );
    searchFunct->setCompletionObject( &listFunct, true );

    if( functions->currentItem() == -1 )
        selectFunction->setEnabled( false );

    connect( searchFunct, SIGNAL( textChanged( const QString & ) ),
	     this, SLOT( slotSearchText(const QString &) ) );
    connect( searchFunct, SIGNAL( returnPressed() ),
	     this, SLOT( slotPressReturn() ) );
}

void KSpreadDlgFormula::slotPressReturn()
{
    if( !functions->currentText().isEmpty() )
        slotDoubleClicked( functions->findItem( functions->currentText() ) );
}

void KSpreadDlgFormula::slotSearchText(const QString &_text)
{
    QString result = listFunct.makeCompletion( _text );
    if( !result.isNull() )
        functions->setCurrentItem( functions->index( functions->findItem( result ) ) );
}

bool KSpreadDlgFormula::eventFilter( QObject* obj, QEvent* ev )
{
    if ( obj == firstElement && ev->type() == QEvent::FocusIn )
        m_focus = firstElement;
    else if ( obj == secondElement && ev->type() == QEvent::FocusIn )
        m_focus = secondElement;
    else if ( obj == thirdElement && ev->type() == QEvent::FocusIn )
        m_focus = thirdElement;
    else if ( obj == fourElement && ev->type() == QEvent::FocusIn )
        m_focus = fourElement;
    else if ( obj == fiveElement && ev->type() == QEvent::FocusIn )
        m_focus = fiveElement;
    else
        return FALSE;

    if ( m_focus )
        m_pView->canvasWidget()->startChoose();

    return FALSE;
}

void KSpreadDlgFormula::slotOk()
{
    m_pView->canvasWidget()->endChoose();
    // Switch back to the old table
    if( m_pView->activeTable()->tableName() !=  m_tableName )
    {
        KSpreadTable *table=m_pView->doc()->map()->findTable(m_tableName);
        if( table)
	    table->setActiveTable();
    }

    // Revert the marker to its original position
    m_pView->canvasWidget()->activeTable()->setMarker( QPoint( m_column, m_row ) );

    // If there is still an editor then set the text.
    // Usually the editor is always in place.
    if( m_pView->canvasWidget()->editor() != 0 )
    {
        ASSERT( m_pView->canvasWidget()->editor() );
        QString tmp = result->text();
        if( tmp.at(0) != '=')
	    tmp = "=" + tmp;
        int pos = m_pView->canvasWidget()->editor()->cursorPosition()+ tmp.length();
        m_pView->canvasWidget()->editor()->setText( tmp );
        m_pView->canvasWidget()->editor()->setFocus();
        m_pView->canvasWidget()->editor()->setCursorPosition( pos );
    }

    accept();
}

void KSpreadDlgFormula::slotClose()
{
    m_pView->canvasWidget()->endChoose();

    // Switch back to the old table
    if(m_pView->activeTable()->tableName() !=  m_tableName )
    {
        KSpreadTable *table=m_pView->doc()->map()->findTable(m_tableName);
        if( !table )
	    return;
	table->setActiveTable();
    }


    // Revert the marker to its original position
    m_pView->canvasWidget()->activeTable()->setMarker( QPoint( m_column, m_row ) );
    // If there is still an editor then reset the text.
    // Usually the editor is always in place.
    if( m_pView->canvasWidget()->editor() != 0 )
    {
        ASSERT( m_pView->canvasWidget()->editor() );
        m_pView->canvasWidget()->editor()->setText( m_oldText );
        m_pView->canvasWidget()->editor()->setFocus();
    }

    reject();
}

void KSpreadDlgFormula::slotSelectButton()
{
    if( functions->currentItem() != -1 )
    {
        slotDoubleClicked(functions->findItem(functions->text(functions->currentItem())));
    }
}

void KSpreadDlgFormula::slotChangeText( const QString& )
{
    // Test the lock
    if( !refresh_result )
	return;

    if ( m_focus == 0 )
	return;

    QString tmp = m_leftText+m_funcName+"(";
    tmp += createFormula();
    tmp = tmp+ ")" + m_rightText;

    result->setText( tmp );
}

QString KSpreadDlgFormula::createFormula()
{
    QString tmp( "" );

    if ( !m_desc )
	return QString::null;
    
    bool first = TRUE;

    int count = m_desc->params();

    if(!firstElement->text().isEmpty() && count >= 1 )
    {
        tmp=tmp+createParameter(firstElement->text(), 0 );
	first = FALSE;
    }

    if(!secondElement->text().isEmpty() && count >= 2 )
    {
	first = FALSE;
	if ( !first )
	    tmp=tmp+";"+createParameter(secondElement->text(), 1 );
	else
	    tmp=tmp+createParameter(secondElement->text(), 1 );
    }
    if(!thirdElement->text().isEmpty() && count >= 3 )
    {
	first = FALSE;
	if ( !first )
	    tmp=tmp+";"+createParameter(thirdElement->text(), 2 );
        else
	    tmp=tmp+createParameter(thirdElement->text(), 2 );
    }
    if(!fourElement->text().isEmpty() && count >= 4 )
    {
	first = FALSE;
	if ( !first )
	    tmp=tmp+";"+createParameter(fourElement->text(), 3 );
        else
	    tmp=tmp+createParameter(fourElement->text(), 3 );
    }
    if(!fiveElement->text().isEmpty() && count >= 5 )
    {
	first = FALSE;
	if ( !first )
	    tmp=tmp+";"+createParameter(fiveElement->text(), 4 );
        else
	    tmp=tmp+createParameter(fiveElement->text(), 4 );
    }

    return(tmp);
}

QString KSpreadDlgFormula::createParameter( const QString& _text, int param )
{
    if ( _text.isEmpty() )
	return QString( "" );

    if ( !m_desc )
	return QString( "" );
    
    QString text;

    KSpreadParameterType elementType = m_desc->param( param ).type();
	
    switch( elementType )
    {
    case KSpread_String:
	{			
	    // Does the text start with quotes?
	    if ( _text[0] == '"' )
	    {
		text = "\"";
	
		// Escape quotes
		QString tmp = _text;
		int pos;
		while( ( pos = tmp.find( '"', 1 ) ) != -1 )
		    tmp.replace( pos, 1, "\\\"" );
	
		text += tmp;
		text += "\"";
	    }
	    else
	    {
		KSpreadPoint p = KSpreadPoint( _text, m_pView->doc()->map() );
		KSpreadRange r = KSpreadRange( _text, m_pView->doc()->map() );
		if( !p.isValid() && !r.isValid() )
		{
		    text = "\"";
	
		    // Escape quotes
		    QString tmp = _text;
		    int pos;
		    while( ( pos = tmp.find( '"', 1 ) ) != -1 )
			tmp.replace( pos, 1, "\\\"" );
	
		    text += tmp;
		    text += "\"";
		}
		else
		    text = _text;
            }
        }
	return text;
    case KSpread_Float:
	return _text;
    case KSpread_Boolean:
	return _text;
    case KSpread_Int:
	return _text;
    case KSpread_Any:
	return _text;
    }

    // Never reached
    return text;
}

static void showEntry( QLineEdit* edit, QLabel* label, KSpreadFunctionDescription* desc, int param )
{
    edit->show();
    
    label->setText( desc->param( param ).helpText() );
    label->show();
}

void KSpreadDlgFormula::slotDoubleClicked( QListBoxItem* item )
{
    if ( !item )
	return;

    if ( !m_desc )
    {
	m_browser->setText( "" );
	return;
    }
    
    m_focus = 0;
    int old_length = result->text().length();

    // Dont change order of these function calls due to a bug in Qt 2.2
    m_browser->setText( m_desc->toQML() );
    m_tabwidget->setTabEnabled( m_input, TRUE );
    m_tabwidget->setCurrentPage( 1 );
	
    //
    // Show as many QLineEdits as needed.
    //
    if( m_desc->params() > 0 )
    {
        m_focus = firstElement;
        firstElement->setFocus();
	
	showEntry( firstElement, label1, m_desc, 0 );
    }
    else
    {
	label1->hide();
	firstElement->hide();
    }

    if( m_desc->params() > 1 )
    {
	showEntry( secondElement, label2, m_desc, 1 );
    }
    else
    {
	label2->hide();
	secondElement->hide();
    }
	
    if( m_desc->params() > 2 )
    {
	showEntry( thirdElement, label3, m_desc, 2 );
    }
    else
    {
	label3->hide();
	thirdElement->hide();
    }

    if( m_desc->params() > 3 )
    {
	showEntry( fourElement, label4, m_desc, 3 );
    }
    else
    {
	label4->hide();
	fourElement->hide();
    }

    if( m_desc->params() > 4 )
    {
	showEntry( fiveElement, label5, m_desc, 4 );
    }
    else
    {
	label5->hide();
	fiveElement->hide();
    }

    if( m_desc->params() > 5 )
    {
        kdDebug(36001) << "Error in param->nb_param" << endl;
    }

    //
    // Put the new function call in the result.
    //
    if( result->cursorPosition() < old_length )
    {
        m_rightText=result->text().right(old_length-result->cursorPosition());
        m_leftText=result->text().left(result->cursorPosition());
    }
    else
    {
        m_rightText="";
        m_leftText=result->text();
    }
    int pos = result->cursorPosition();
    result->setText( m_leftText+functions->text( functions->currentItem() ) + "()" + m_rightText);

    //
    // Put focus somewhere is there are no QLineEdits visible
    //
    if( m_desc->params() == 0 )
    {
	label1->show();
	label1->setText( i18n("This function has no parameters.") );
			 
        result->setFocus();
        result->setCursorPosition(pos+functions->text(functions->currentItem()).length()+2);
    }
}

void KSpreadDlgFormula::slotSelected( const QString& function )
{
    KSpreadFunctionDescription* desc = m_repo.function( function );
    if ( !desc )
    {
	m_browser->setText( "" );
	return;
    }
	
    if( functions->currentItem() !=- 1 )
        selectFunction->setEnabled( TRUE );

    // Lock
    refresh_result = false;

    m_funcName = function;
    m_desc = desc;
    
    // Set the help text
    m_browser->setText( m_desc->toQML() );
    
    m_focus=0;

    m_tabwidget->setCurrentPage( 0 );
    m_tabwidget->setTabEnabled( m_input, FALSE );
	
    // Unlock
    refresh_result=true;
}

void KSpreadDlgFormula::slotSelectionChanged( KSpreadTable* _table, const QRect& _selection )
{
    if ( !m_focus )
        return;

    if ( _selection.left() == 0 )
        return;

    if ( _selection.left() >= _selection.right() && _selection.top() >= _selection.bottom() )
    {
        int dx = _selection.right();
        int dy = _selection.bottom();
        QString tmp;
        tmp.setNum( dy );
        tmp = _table->tableName() + "!" + util_columnLabel( dx ) + tmp;
        m_focus->setText( tmp );
    }
    else
    {
        QString area = util_rangeName( _table, _selection );
        m_focus->setText( area );
    }
}

void KSpreadDlgFormula::slotActivated( const QString& category )
{
    QStringList lst;
    if ( category == i18n("All") )
	lst = m_repo.functionNames();
    else
	lst = m_repo.functionNames( category );
    
    functions->clear();
    functions->insertStringList( lst );
    listFunct.setItems( lst );

    // Go to the first function in the list.
    functions->setCurrentItem(0);
    slotSelected( functions->text(0) );
}

#include "kspread_dlg_formula.moc"
