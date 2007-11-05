/* This file is part of the KDE project
 Copyright (C) 1998, 1999, 2000 Torben Weis <weis@kde.org>
 Copyright (C) 2004, 2005 Dag Andersen <danders@get2net.dk>
 Copyright (C) 2006 Raphael Langerhorst <raphael.langerhorst@kdemail.net>
 Copyright (C) 2007 Thorsten Zachmann <zachmann@kde.org>

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

#include "kptpart.h"
#include "kptview.h"
#include "kptfactory.h"
#include "kptproject.h"
#include "kptresource.h"
#include "kptcontext.h"
//#include "KDGanttViewTaskLink.h"

#include <KoZoomHandler.h>
#include <KoStore.h>
#include <KoXmlReader.h>
#include <KoStore.h>
#include <KoStoreDevice.h>
#include <KoOdfReadStore.h>

#include <QApplication>
#include <qpainter.h>
#include <qfileinfo.h>
#include <QTimer>
#include <QUndoCommand>

#include <kdebug.h>
#include <kconfig.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kstandarddirs.h>
#include <kparts/partmanager.h>
#include <kmimetype.h>

#include <KoGlobal.h>

#define CURRENT_SYNTAX_VERSION XML_FILE_SYNTAX_VERSION

namespace KPlato
{

Part::Part( QWidget *parentWidget, QObject *parent, bool singleViewMode )
        : KoDocument( parentWidget, parent, singleViewMode ),
        m_project( 0 ), m_parentWidget( parentWidget ),
        m_context( 0 ), m_xmlLoader()
{
    setComponentData( Factory::global() );
    setTemplateType( "kplato_template" );
    m_config.setReadWrite( isReadWrite() || !isEmbedded() );
    m_config.load();

    setProject( new Project() ); // after config is loaded
}


Part::~Part()
{
    m_config.save();
    delete m_project;
}

void Part::setProject( Project *project )
{
    if ( m_project ) {
        disconnect( m_project, SIGNAL( changed() ), this, SIGNAL( changed() ) );
        delete m_project;
    }
    m_project = project;
    if ( m_project ) {
        connect( m_project, SIGNAL( changed() ), this, SIGNAL( changed() ) );
    }
    emit changed();
}

KoView *Part::createViewInstance( QWidget *parent )
{
    View *view = new View( this, parent );
    connect( view, SIGNAL( destroyed() ), this, SLOT( slotViewDestroyed() ) );
    return view;
}

bool Part::loadOdf( KoOdfReadStore & odfStore )
{
    Q_UNUSED( odfStore );
    kWarning()<< "OpenDocument not supported, let's try native xml format";
    return loadXML( 0, odfStore.contentDoc() ); // We have only one format, so try to load that!
}

bool Part::loadXML( QIODevice *, const KoXmlDocument &document )
{
    kDebug();
    QTime dt;
    dt.start();
    emit sigProgress( 0 );

    QString value;
    KoXmlElement plan = document.documentElement();

    // Check if this is the right app
    value = plan.attribute( "mime", QString() );
    if ( value.isEmpty() ) {
        kError() << "No mime type specified!" << endl;
        setErrorMessage( i18n( "Invalid document. No mimetype specified." ) );
        return false;
    } else if ( value != "application/x-vnd.kde.kplato" ) {
        kError() << "Unknown mime type " << value << endl;
        setErrorMessage( i18n( "Invalid document. Expected mimetype application/x-vnd.kde.kplato, got %1", value ) );
        return false;
    }
    QString m_syntaxVersion = plan.attribute( "version", CURRENT_SYNTAX_VERSION );
    m_xmlLoader.setVersion( m_syntaxVersion );
    if ( m_syntaxVersion > CURRENT_SYNTAX_VERSION ) {
        int ret = KMessageBox::warningContinueCancel(
                      0, i18n( "This document was created with a newer version of KPlato (syntax version: %1)\n"
                               "Opening it in this version of KPlato will lose some information.", m_syntaxVersion ),
                      i18n( "File-Format Mismatch" ), KGuiItem( i18n( "Continue" ) ) );
        if ( ret == KMessageBox::Cancel ) {
            setErrorMessage( "USER_CANCELED" );
            return false;
        }
    }
    emit sigProgress( 5 );

#ifdef KOXML_USE_QDOM
    int numNodes = plan.childNodes().count();
#else
    int numNodes = plan.childNodesCount();
#endif
    if ( numNodes > 2 ) {
        //TODO: Make a proper bitching about this
        kDebug() <<"*** Error ***";
        kDebug() <<"  Children count should be maximum 2, but is" << numNodes;
        return false;
    }
    m_xmlLoader.startLoad();
    KoXmlNode n = plan.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "project" ) {
            Project * newProject = new Project();
            m_xmlLoader.setProject( newProject );
            if ( newProject->load( e, m_xmlLoader ) ) {
                // The load went fine. Throw out the old project
                setProject( newProject );
            } else {
                delete newProject;
                m_xmlLoader.addMsg( XMLLoaderObject::Errors, "Loading of project failed" );
                //TODO add some ui here
            }
        } else if ( e.tagName() == "objects" ) {
            kDebug()<<"loadObjects";
            loadObjects( e );
        }
    }
    m_xmlLoader.stopLoad();
    emit sigProgress( 100 ); // the rest is only processing, not loading

    kDebug() <<"Loading took" << ( float ) ( dt.elapsed() ) / 1000 <<" seconds";

    // do some sanity checking on document.
    emit sigProgress( -1 );

    setModified( false );
    emit changed();
    return true;
}

QDomDocument Part::saveXML()
{
    kDebug();
    QDomDocument document( "kplato" );

    document.appendChild( document.createProcessingInstruction(
                              "xml",
                              "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement doc = document.createElement( "kplato" );
    doc.setAttribute( "editor", "KPlato" );
    doc.setAttribute( "mime", "application/x-vnd.kde.kplato" );
    doc.setAttribute( "version", CURRENT_SYNTAX_VERSION );
    document.appendChild( doc );

    // Save the project
    m_project->save( doc );
    
    if ( ! children().isEmpty() ) {
        QDomElement el = document.createElement( "objects" );
        foreach ( KoDocumentChild *ch, children() ) {
            if ( ch->isDeleted() ) {
                continue;
            }
            QDomElement e = ch->save( document, false );
            el.appendChild( e );
        }
        if ( el.childNodes().count() > 0 ) {
            doc.appendChild( el );
        }
    }
    return document;
}

QDomDocument Part::saveWorkPackageXML( const Node *node, long id )
{
    kDebug();
    QDomDocument document( "kplato" );

    document.appendChild( document.createProcessingInstruction(
                "xml",
    "version=\"1.0\" encoding=\"UTF-8\"" ) );

    QDomElement doc = document.createElement( "kplatowork" ); //??
    doc.setAttribute( "editor", "KPlatoWork" );
    doc.setAttribute( "mime", "application/x-vnd.kde.kplato.work" );
    doc.setAttribute( "version", CURRENT_SYNTAX_VERSION );
    document.appendChild( doc );

    // Save the project
    m_project->saveWorkPackageXML( doc, node, id );
    
    return document;
}

bool Part::saveWorkPackageToStream( QIODevice * dev, const Node *node, long id )
{
    QDomDocument doc = saveWorkPackageXML( node, id );
    // Save to buffer
    QByteArray s = doc.toByteArray(); // utf8 already
    dev->open( QIODevice::WriteOnly );
    int nwritten = dev->write( s.data(), s.size() );
    if ( nwritten != (int)s.size() )
        kWarning(30003) << "KoDocument::saveToStream wrote " << nwritten << "   - expected " <<  s.size();
    return nwritten == (int)s.size();
}

bool Part::saveWorkPackageFormat( const QString &file, const Node *node, long id  )
{
    kDebug() <<"Saving to store";

    KoStore::Backend backend = KoStore::Zip;
#ifdef QCA2
/*    if ( d->m_specialOutputFlag == SaveEncrypted ) {
        backend = KoStore::Encrypted;
        kDebug() <<"Saving using encrypted backend.";
    }*/
#endif

    QByteArray mimeType = "application/x-vnd.kde.kplato.work";
    kDebug() <<"MimeType=" << mimeType;
    
    KoStore* store = KoStore::createStore( file, KoStore::Write, mimeType, backend );
/*    if ( d->m_specialOutputFlag == SaveEncrypted && !d->m_password.isNull( ) ) {
        store->setPassword( d->m_password );
    }*/
    if ( store->bad() ) {
        setErrorMessage( i18n( "Could not create the workpackage file for saving" ) ); // more details needed?
        delete store;
        return false;
    }
    // Tell KoStore not to touch the file names
    store->disallowNameExpansion();

    if ( store->open( "root" ) ) {
        KoStoreDevice dev( store );
        if ( !saveWorkPackageToStream( &dev, node, id ) || !store->close() ) {
            kDebug(30003) <<"saveToStream failed";
            delete store;
            return false;
        }
    } else {
        setErrorMessage( i18n( "Not able to write '%1'. Partition full?", QString( "maindoc.xml") ) );
        delete store;
        return false;
    }
//     if ( store->open( "documentinfo.xml" ) )
//     {
//         QDomDocument doc = d->m_docInfo->save();
//         KoStoreDevice dev( store );
// 
//         QByteArray s = doc.toByteArray(); // this is already Utf8!
//         (void)dev.write( s.data(), s.size() );
//         (void)store->close();
//     }
// 
//     if ( store->open( "preview.png" ) )
//     {
//         // ### TODO: missing error checking (The partition could be full!)
//         savePreview( store );
//         (void)store->close();
//     }

    kDebug(30003) <<"Saving done of url:" << file;
    if ( !store->finalize() ) {
        delete store;
        return false;
    }
    // Success
    delete store;

    return true;
}

bool Part::saveWorkPackageUrl( const KUrl & _url, const Node *node, long id  )
{
    kDebug()<<_url;

    QApplication::setOverrideCursor( Qt::WaitCursor );

    emit statusBarMessage( i18n("Saving...") );
    bool ret = false;
    bool suppressErrorDialog = false;

    ret = saveWorkPackageFormat( _url.url(), node, id );


    QApplication::restoreOverrideCursor();
    if ( ! ret ) {
        if ( !suppressErrorDialog ) {
            showSavingErrorDialog();
        }
    }
    emit clearStatusBarMessage();
    return ret;
}


void Part::paintContent( QPainter &, const QRect &)
{
    // Don't embed this app!!!
}

void Part::slotViewDestroyed()
{
}

void Part::generateWBS()
{
    m_project->generateWBS( 1, m_wbsDefinition );
}

void Part::activate( QWidget *w )
{
    if ( manager() )
        manager()->setActivePart( this, w );
}

DocumentChild *Part::createChild( KoDocument *doc, const QRect& geometry )
{
    DocumentChild *ch = new DocumentChild( this, doc, geometry );
    insertChild( ch );
    return ch;
}

void Part::loadObjects( const KoXmlElement &element )
{
    kDebug();
    KoXmlNode n = element.firstChild();
    for ( ; ! n.isNull(); n = n.nextSibling() ) {
        if ( ! n.isElement() ) {
            continue;
        }
        KoXmlElement e = n.toElement();
        if ( e.tagName() == "object" ) {
            DocumentChild *ch = new DocumentChild( this );
            if ( ch->load( e ) ) {
                kDebug()<<"loaded";
                insertChild( ch );
            } else {
                kDebug()<<"Failed to load object";
                delete ch;
            }
        }
    }
}

bool Part::loadChildren( KoStore* store )
{
    kDebug();
    foreach ( KoDocumentChild *ch, children() ) {
        ch->loadDocument( store );
    }
    return true;
}

bool Part::completeLoading( KoStore *store )
{
    if ( store == 0 ) {
        // can happen if loading a template
        kDebug()<<"No store"<<endl;
        return true; // continue anyway
    }
    delete m_context;
    m_context = new Context();
    KoXmlDocument doc;
    if ( loadAndParse( store, "context.xml", doc ) ) {
        store->close();
        m_context->load( doc );
    } else kWarning()<<"No context"<<endl;
    return true;
}

bool Part::completeSaving( KoStore *store )
{
    delete m_context;
    m_context = 0;
    // Seems like a hack, but imo the best to do
    View *view = dynamic_cast<View*>( views().value( 0 ) );
    if ( view ) {
        m_context = new Context();
        m_context->save( view );
        if ( store->open( "context.xml" ) )
        {
            QDomDocument doc = m_context->save( view );
            KoStoreDevice dev( store );

            QByteArray s = doc.toByteArray(); // this is already Utf8!
            (void)dev.write( s.data(), s.size() );
            (void)store->close();
        }
    }
    return true;
}

bool Part::loadAndParse(KoStore* store, const QString& filename, KoXmlDocument& doc)
{
    //kDebug(30003) << "oldLoadAndParse: Trying to open " << filename << endl;

    if (!store->open(filename))
    {
        kWarning() << "Entry " << filename << " not found!" << endl;
//        d->lastErrorMessage = i18n( "Could not find %1",filename );
        return false;
    }
    // Error variables for QDomDocument::setContent
    QString errorMsg;
    int errorLine, errorColumn;
    bool ok = doc.setContent( store->device(), &errorMsg, &errorLine, &errorColumn );
    if ( !ok )
    {
        kError() << "Parsing error in " << filename << "! Aborting!" << endl
            << " In line: " << errorLine << ", column: " << errorColumn << endl
            << " Error message: " << errorMsg << endl;
/*        d->lastErrorMessage = i18n( "Parsing error in %1 at line %2, column %3\nError message: %4"
                              ,filename  ,errorLine, errorColumn ,
                              QCoreApplication::translate("QXml", errorMsg.toUtf8(), 0,
                                  QCoreApplication::UnicodeUTF8));*/
        store->close();
        return false;
    }
    kDebug() << "File " << filename << " loaded and parsed" << endl;
    return true;
}

//--------------------------------

DocumentChild::DocumentChild ( KoDocument* parent )
    : KoDocumentChild ( parent )
{
}

DocumentChild::DocumentChild ( KoDocument* parent, KoDocument* doc, const QRect& geometry )
    : KoDocumentChild ( parent, doc, geometry )
{
}

void DocumentChild::setActivated( bool act, QWidget *w )
{
    if ( document()->manager() ) {
        if ( act ) {
            document()->manager()->setActivePart( document(), w );
        } else if ( parentDocument()->manager() ) {
            parentDocument()->manager()->setActivePart( parentDocument(), w );
        } else {
            document()->manager()->setActivePart( 0, w );
        }
    }
}

KoDocument* DocumentChild::hitTest( const QPoint& , KoView* view, const QMatrix& )
{
    if ( document()->views().contains( view ) ) {
        return document();
    }
    return 0;
}


QDomElement DocumentChild::save( QDomDocument &doc, bool uppercase )
{
    if ( document() == 0 ) {
        return QDomElement();
    }
    QDomElement e = KoDocumentChild::save( doc, uppercase );
    kDebug()<<m_title;
    e.setAttribute( "title", m_title );
    e.setAttribute( "icon", m_icon );
    return e;
}

bool DocumentChild::load( const KoXmlElement& element, bool uppercase )
{
    if ( KoDocumentChild::load( element, uppercase ) ) {
        m_icon = element.attribute( "icon", QString() );
        m_title = element.attribute( "title", QString() );
        kDebug()<<m_title;
        return true;
    }
    return false;
}


}  //KPlato namespace

#include "kptpart.moc"
