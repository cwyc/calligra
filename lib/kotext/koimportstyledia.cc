/* This file is part of the KDE project
   Copyright (C)  2002 Montel Laurent <lmontel@mandrakesoft.com>

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

#include <klocale.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlistbox.h>
#include "koimportstyledia.h"

#include <kdebug.h>
#include <qlabel.h>

KoImportStyleDia::KoImportStyleDia( KoStyleCollection* currentCollection, QWidget *parent, const char *name )
    : KDialogBase( parent, name , true, "", Ok|Cancel|User1, Ok, true )
{
    setCaption( i18n("Import Styles") );
    m_currentCollection = currentCollection;
    QVBox *page = makeVBoxMainWidget();
    new QLabel(i18n("Select styles to import:"), page);
    m_listStyleName = new QListBox( page );
    m_listStyleName->setSelectionMode( QListBox::Multi );
    enableButtonOK( m_listStyleName->count() != 0 );
    setButtonText( KDialogBase::User1, i18n("Load...") );
    connect( this, SIGNAL( user1Clicked() ), this, SLOT(slotLoadFile()));
    setInitialSize( QSize( 300, 400 ) );
    setFocus();
}

KoImportStyleDia::~KoImportStyleDia()
{
    m_styleList.setAutoDelete(true);
    m_styleList.clear();
}

void KoImportStyleDia::generateStyleList()
{
    for (uint i = 0; i< m_listStyleName->count();i++)
    {
        if ( !m_listStyleName->isSelected( i ))
        {
            QString name = m_listStyleName->text( i );
            //remove this style from list
            QPtrListIterator<KoParagStyle> styleIt( m_styleList );
            for ( ; styleIt.current(); ++styleIt )
            {
                if ( styleIt.current()->displayName() == name )
                {
                    updateFollowingStyle( styleIt.current()->displayName() );
                    m_styleList.remove(styleIt.current());
                    break;
                }
            }
        }
    }
}

void KoImportStyleDia::updateFollowingStyle(const QString & _name)
{
    QPtrListIterator<KoParagStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
    {
        if ( styleIt.current()->followingStyle()->displayName() == _name )
        {
            styleIt.current()->setFollowingStyle(styleIt.current());
        }
    }
}

void KoImportStyleDia::slotLoadFile()
{
    loadFile();
    enableButtonOK( m_listStyleName->count() != 0 );
}

void KoImportStyleDia::initList()
{
    QStringList lst;
    QPtrListIterator<KoParagStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
        lst << styleIt.current()->displayName();
    m_listStyleName->insertStringList(lst);
}

void KoImportStyleDia::slotOk()
{
    generateStyleList();
    KDialogBase::slotOk();
}

QString KoImportStyleDia::generateStyleName( const QString & templateName ) const
{
    QString name;
    int num = 1;
    bool exists;
    do {
        name = templateName.arg( num );
        exists = m_currentCollection->findStyle( name ) != 0;
        ++num;
    } while ( exists );
    return name;
}

QString KoImportStyleDia::generateStyleDisplayName( const QString & templateName ) const
{
    QString name;
    int num = 1;
    bool exists;
    do {
        name = templateName.arg( num );
        exists = m_currentCollection->findTranslatedStyle( name ) != 0;
        ++num;
    } while ( exists );
    return name;
}

KoParagStyle *KoImportStyleDia::findStyle( const QString & name ) const
{
    QPtrListIterator<KoParagStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
    {
        if ( styleIt.current()->name() == name ) {
            return styleIt.current();
        }
    }
    return 0;
}

KoParagStyle * KoImportStyleDia::findTranslatedStyle( const QString & name ) const
{
    QPtrListIterator<KoParagStyle> styleIt( m_styleList );
    for ( ; styleIt.current(); ++styleIt )
    {
        if ( styleIt.current()->displayName() == name ) {
            return styleIt.current();
        }
    }
    return 0;
}

void KoImportStyleDia::clear()
{
    m_styleList.setAutoDelete(true);
    m_styleList.clear();
}

#include "koimportstyledia.moc"
