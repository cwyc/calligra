/* This file is part of the KDE project
   Copyright (C) 2005 David Faure <faure@kde.org>

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public
   License version 2 as published by the Free Software Foundation.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public License
   along with this library; see the file COPYING.LIB.  If not, write to
   the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.
*/

#include "kwoasissaver.h"
#include <koStore.h>
#include <koOasisStore.h>
#include <kooasiscontext.h>
#include <koxmlwriter.h>
#include "kwdoc.h"

KWOasisSaver::KWOasisSaver( KWDocument* doc )
    : m_doc( doc )
{
    const QCString mimeType = selectionMimeType();
    m_store = KoStore::createStore( &m_buffer, KoStore::Write, mimeType );
    Q_ASSERT( m_store );
    Q_ASSERT( !m_store->bad() );

    m_oasisStore = new KoOasisStore( m_store );

    //KoXmlWriter* manifestWriter = oasisStore.manifestWriter( mimeType );

    m_savingContext = new KoSavingContext( m_mainStyles, KoSavingContext::Store );

    KoXmlWriter* bodyWriter = m_oasisStore->bodyWriter();
    bodyWriter->startElement( "office:body" );
    bodyWriter->startElement( "office:text" );
}

void KWOasisSaver::saveParagraphs( const QValueList<const KoTextParag *>& paragraphs )
{
    for ( QValueList<const KoTextParag *>::const_iterator it = paragraphs.begin(),
                                                   end = paragraphs.end();
                  it != end ; ++it ) {
        saveParagraph( *it );
    }
}

void KWOasisSaver::saveParagraph( const KoTextParag* parag )
{
    // keep in sync with KoTextDocument::copySelection
    KoXmlWriter* bodyWriter = m_oasisStore->bodyWriter();
    parag->saveOasis( *bodyWriter, *m_savingContext, 0, parag->length()-2, true );
}

QByteArray KWOasisSaver::data() const
{
    Q_ASSERT( !m_store ); // forgot to call finish() (or to check its return value) ?
    return m_buffer.buffer();
}

KWOasisSaver::~KWOasisSaver()
{
    delete m_store;
    delete m_oasisStore;
    delete m_savingContext;
}

bool KWOasisSaver::finish()
{
    KoXmlWriter* bodyWriter = m_oasisStore->bodyWriter();
    bodyWriter->endElement(); // office:text
    bodyWriter->endElement(); // office:body

    KoXmlWriter* contentWriter = m_oasisStore->contentWriter();
    Q_ASSERT( contentWriter );

    writeAutomaticStyles( *contentWriter, m_mainStyles );

    m_oasisStore->closeContentWriter();

    if ( !m_store->open( "styles.xml" ) )
        return false;
    //manifestWriter->addManifestEntry( "styles.xml", "text/xml" );
    m_doc->saveOasisDocumentStyles( m_store, m_mainStyles, *m_savingContext,
                                    KWDocument::SaveSelected /* simply means not SaveAll */ );
    if ( !m_store->close() ) // done with styles.xml
        return false;

    delete m_oasisStore; m_oasisStore = 0;
    delete m_store; m_store = 0;

    return true;
}

void KWOasisSaver::writeAutomaticStyles( KoXmlWriter& contentWriter, KoGenStyles& mainStyles )
{
    contentWriter.startElement( "office:automatic-styles" );
    QValueList<KoGenStyles::NamedStyle> styles = mainStyles.styles( KoGenStyle::STYLE_AUTO );
    QValueList<KoGenStyles::NamedStyle>::const_iterator it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name, "style:paragraph-properties" );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_AUTO_LIST );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "text:list-style", (*it).name, 0 );
    }

    styles = mainStyles.styles( KWDocument::STYLE_FRAME );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name , "style:graphic-properties"  );
    }

    styles = mainStyles.styles( KWDocument::STYLE_TABLE );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name , "style:table-properties"  );
    }

    styles = mainStyles.styles( KWDocument::STYLE_TABLE_COLUMN );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name , "style:table-column-properties"  );
    }

    styles = mainStyles.styles( KWDocument::STYLE_TABLE_CELL );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "style:style", (*it).name , "style:table-cell-properties"  );
    }

    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_DATE );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "number:date-style", (*it).name, 0 /*TODO ????*/  );
    }
    styles = mainStyles.styles( KoGenStyle::STYLE_NUMERIC_TIME );
    it = styles.begin();
    for ( ; it != styles.end() ; ++it ) {
        (*it).style->writeStyle( &contentWriter, mainStyles, "number:time-style", (*it).name, 0 /*TODO ????*/  );
    }

    contentWriter.endElement(); // office:automatic-styles
}

const char* KWOasisSaver::selectionMimeType()
{
    return "application/vnd.oasis.opendocument.text";
}
