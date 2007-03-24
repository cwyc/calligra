/* This file is part of the KDE project
 * Copyright (C) 2005 David Faure <faure@kde.org>
 * Copyright (C) 2007 Thomas Zander <zander@kde.org>
 * Copyright (C) 2007 Sebastian Sauer <mail@dipe.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "KWOpenDocumentLoader.h"
#include "KWDocument.h"
//#include "KWPageSettings.h"
#include "frame/KWTextFrameSet.h"
#include "frame/KWTextFrame.h"

// koffice
#include <KoOasisLoadingContext.h>
#include <KoOasisStyles.h>
#include <KoOasisSettings.h>
#include <KoXmlNS.h>
#include <KoDom.h>
#include <KoShapeRegistry.h>
#include <KoShapeFactory.h>
#include <KoStyleManager.h>
#include <KoParagraphStyle.h>
#include <KoCharacterStyle.h>
//#include <KoListStyle.h>
//#include <KoTextShapeData.h>
#include <KoPageLayout.h>

// KDE + Qt includes
#include <QDomDocument>
#include <QTextDocument>
#include <QTextCursor>
#include <QTextBlock>
#include <klocale.h>

/// \internal d-pointer class.
class KWOpenDocumentLoader::Private
{
    public:
        /// The KWord document.
        KWDocument *document;
        /// Current master-page name (OASIS loading)
        QString currentMasterPage;
};

KWOpenDocumentLoader::KWOpenDocumentLoader(KWDocument *parent)
    : d(new Private())
    //, m_pageSettings(&parent->m_pageSettings)
    //, m_pageManager(&parent->m_pageManager)
{
    d->document = parent;
    connect(this, SIGNAL(sigProgress(int)), d->document, SIGNAL(sigProgress(int)));
}

KWOpenDocumentLoader::~KWOpenDocumentLoader() {
    delete d;
}

//KWDocument::loadOasis
bool KWOpenDocumentLoader::load(const QDomDocument& doc, KoOasisStyles& styles, const QDomDocument& settings, KoStore* store) {
    QTime dt;
    dt.start();
    emit sigProgress( 0 );
    kDebug(32001) << "========================> KWOpenDocumentLoader::load START" << endl;

    QDomElement content = doc.documentElement();
    QDomElement realBody ( KoDom::namedItemNS( content, KoXmlNS::office, "body" ) );
    if ( realBody.isNull() )
    {
        kError(32001) << "No office:body found!" << endl;
        d->document->setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No office:body tag found." ) );
        return false;
    }

    QDomElement body = KoDom::namedItemNS( realBody, KoXmlNS::office, "text" );
    if ( body.isNull() )
    {
        kError(32001) << "No office:text found!" << endl;
        QDomElement childElem;
        QString localName;
        forEachElement( childElem, realBody ) {
            localName = childElem.localName();
        }
        if ( localName.isEmpty() )
            d->document->setErrorMessage( i18n( "Invalid OASIS OpenDocument file. No tag found inside office:body." ) );
        else
            d->document->setErrorMessage( i18n( "This is not a word processing document, but %1. Please try opening it with the appropriate application." ).arg( KoDocument::tagNameToDocumentType( localName ) ) );
        return false;
    }

    // TODO check versions and mimetypes etc.

    KoOasisLoadingContext context( d->document, styles, store );

    KoColumns columns;
    columns.columns = 1;
    columns.columnSpacing = d->document->m_defaultColumnSpacing;

    // In theory the page format is the style:master-page-name of the first paragraph...
    // But, hmm, in a doc with only a table there was no reference to the master page at all...
    // So we load the standard page layout to start with, and in KWTextParag
    // we might overwrite it with another one.
    d->currentMasterPage = "Standard";
    if ( !loadPageLayout( d->currentMasterPage, context ) )
        return false;

#if 0
    KWOasisLoader oasisLoader( this );
    // <text:page-sequence> oasis extension for DTP (2003-10-27 post by Daniel)
    m_processingType = ( !KoDom::namedItemNS( body, KoXmlNS::text, "page-sequence" ).isNull() ) ? DTP : WP;
    m_hasTOC = false;
    m_tabStop = MM_TO_POINT(15);
    const QDomElement* defaultParagStyle = styles.defaultStyle( "paragraph" );
    if ( defaultParagStyle ) {
        KoStyleStack stack;
        stack.push( *defaultParagStyle );
        stack.setTypeProperties( "paragraph" );
        QString tabStopVal = stack.attributeNS( KoXmlNS::style, "tab-stop-distance" );
        if ( !tabStopVal.isEmpty() ) m_tabStop = KoUnit::parseValue( tabStopVal );
    }
    m_initialEditing = 0;
    // TODO MAILMERGE
    // Variable settings
    // By default display real variable value
    if ( !isReadWrite())
        m_varColl->variableSetting()->setDisplayFieldCode(false);
#else
    /*
    const QDomElement* defaultParagStyle = styles.defaultStyle( "paragraph" );
    if ( defaultParagStyle ) {
        context.styleStack().push( *defaultParagStyle );
        context.styleStack().setTypeProperties( "paragraph" );
    }
    */
#endif

    // Load all styles before the corresponding paragraphs try to use them!
    loadStyles( context );

#if 0
    if ( m_frameStyleColl->loadOasisStyles( context ) == 0 ) {
         // no styles loaded -> load default styles
        loadDefaultFrameStyleTemplates();
    }
    if ( m_tableStyleColl->loadOasisStyles( context, *m_styleColl, *m_frameStyleColl ) == 0 ) {
        // no styles loaded -> load default styles
        loadDefaultTableStyleTemplates();
    }
    static_cast<KWVariableSettings *>( m_varColl->variableSetting() )->loadNoteConfiguration( styles.officeStyle() );
    loadDefaultTableTemplates();
#else
    // We always needs at least one valid default paragraph style
    KoParagraphStyle *defaultParagraphStyle = d->document->styleManager()->defaultParagraphStyle();
    context.styleStack().setTypeProperties( "paragraph" );
    defaultParagraphStyle->loadOasis( context.styleStack() );
    context.styleStack().setTypeProperties( "text" );
    defaultParagraphStyle->characterStyle()->loadOasis( context.styleStack() );
    kDebug()<<"defaultParagraphStyle.name="<<defaultParagraphStyle->name()<<endl;

    /*
    //const KoXmlElement* defaultParagraphStyle = context.oasisStyles().defaultStyle("paragraph");
    //Q_ASSERT(defaultParagraphStyle);
    //if( ! defaultParagraphStyle ) {
        KoParagraphStyle *parastyle = new KoParagraphStyle();
        parastyle->setName("Standard");
        d->document->styleManager()->add(parastyle);

        context.styleStack().setTypeProperties( "paragraph" ); // load all style attributes from "style:paragraph-properties"
        parastyle->loadOasis(context.styleStack()); // load the KoParagraphStyle from the stylestack

        KoCharacterStyle *charstyle = parastyle->characterStyle();
        context.styleStack().setTypeProperties( "text" ); // load all style attributes from "style:text-properties"
        charstyle->loadOasis(context.styleStack()); // load the KoCharacterStyle from the stylestack

    //}
    //Q_ASSERT( context.oasisStyles().defaultStyle("paragraph") );
    */
#endif

#if 0
    if ( m_processingType == WP ) { // Create main frameset
        KWTextFrameSet *fs = new KWTextFrameSet( this, i18n( "Main Text Frameset" ) );
        m_lstFrameSet.append( fs ); // don't use addFrameSet here. We'll call finalize() once and for all in completeLoading
        fs->loadOasisContent( body, context );
        KWFrame* frame = new KWFrame( fs, 29, 42, 566-29, 798-42 );
        frame->setFrameBehavior( KWFrame::AutoCreateNewFrame );
        frame->setNewFrameBehavior( KWFrame::Reconnect );
        fs->addFrame( frame );
        // load padding, background and borders for the main frame
        const QDomElement* masterPage = context.styles().masterPages()[ d->currentMasterPage ];
        const QDomElement *masterPageStyle = masterPage ? context.styles().findStyle(masterPage->attributeNS( KoXmlNS::style, "page-layout-name", QString::null ) ) : 0;
        if ( masterPageStyle ) {
          KoStyleStack styleStack;
          styleStack.push(  *masterPageStyle );
          styleStack.setTypeProperties( "page-layout" );
          frame->loadBorderProperties( styleStack );
        }
        fs->renumberFootNotes( false /*no repaint*/ );
    } else { // DTP mode: the items in the body are page-sequence and then frames
        QDomElement tag;
        forEachElement( tag, body ) {
            context.styleStack().save();
            const QString localName = tag.localName();
            if ( localName == "page-sequence" && tag.namespaceURI() == KoXmlNS::text ) {
                // We don't have support for changing the page layout yet, so just take the
                // number of pages
                int pages=1;
                QDomElement page;
                forEachElement( page, tag ) ++pages;
                kDebug() << "DTP mode: found " << pages << "pages" << endl;
                //setPageCount ( pages );
            }
            else if ( localName == "frame" && tag.namespaceURI() == KoXmlNS::draw )
                oasisLoader.loadFrame( tag, context, KoPoint() );
            else
                kWarning(32001) << "Unsupported tag in DTP loading:" << tag.tagName() << endl;
        }
    }
#else
    KWord::TextFrameSetType type = KWord::MainTextFrameSet;
    KWTextFrameSet *fs = new KWTextFrameSet( d->document, type );
    fs->setAllowLayout(false);
    fs->setName( i18n( "Main Text Frameset" ) );
    d->document->addFrameSet(fs);
    //fs->loadOasisContent( body, context );

    KoShapeFactory *factory = KoShapeRegistry::instance()->get(TextShape_SHAPEID);
    Q_ASSERT(factory);
    KoShape *shape = factory->createDefaultShape();
    KWTextFrame *frame = new KWTextFrame(shape, fs);
    frame->setFrameBehavior(KWord::AutoExtendFrameBehavior);

    QTextCursor cursor( fs->document() );
    loadBody(body, context, cursor);
#endif

    if ( !loadMasterPageStyle( d->currentMasterPage, context ) )
        return false;

    loadSettings( settings );

#if 0
    // This sets the columns and header/footer flags, and calls recalcFrames, so it must be done last.
    setPageLayout( m_pageLayout, m_loadingInfo->columns, m_loadingInfo->hf, false );
#else
    d->document->m_pageSettings.setColumns( columns );
#endif

    kDebug(32001) << "========================> KWOpenDocumentLoader::load END" << endl;
    kDebug(32001) << "Loading took " << (float)(dt.elapsed()) / 1000 << " seconds" << endl;
    emit sigProgress(100);
    return true;
}

void KWOpenDocumentLoader::loadSettings(const QDomDocument& settingsDoc)
{
    if ( settingsDoc.isNull() )
        return;

    kDebug(32001)<<"KWOpenDocumentLoader::loadSettings"<<endl;
    KoOasisSettings settings( settingsDoc );
    KoOasisSettings::Items viewSettings = settings.itemSet( "view-settings" );
    if ( !viewSettings.isNull() )
        d->document->setUnit( KoUnit::unit(viewSettings.parseConfigItemString("unit")) );

    //KWOasisLoader::loadOasisIgnoreList
    KoOasisSettings::Items configurationSettings = settings.itemSet( "configuration-settings" );
    if ( !configurationSettings.isNull() ) {
        const QString ignorelist = configurationSettings.parseConfigItemString( "SpellCheckerIgnoreList" );
        kDebug(32001) << "Ignorelist: " << ignorelist << endl;
#if 0
        d->document->setSpellCheckIgnoreList( QStringList::split( ',', ignorelist ) );
#endif
    }

#if 0
    d->document->variableCollection()->variableSetting()->loadOasis( settings );
#endif
}

bool KWOpenDocumentLoader::loadPageLayout(const QString& masterPageName, KoOasisLoadingContext& context)
{
    kDebug(32001)<<"KWOpenDocumentLoader::loadPageLayout"<<endl;
    const KoOasisStyles& styles = context.oasisStyles();
    Q_ASSERT( styles.masterPages().contains(masterPageName) );
    const QDomElement* masterPage = styles.masterPages()[ masterPageName ];
    Q_ASSERT( masterPage );
    const QDomElement *masterPageStyle = masterPage ? styles.findStyle( masterPage->attributeNS( KoXmlNS::style, "page-layout-name", QString::null ) ) : 0;
    Q_ASSERT( masterPageStyle );
    if ( masterPageStyle ) {
        KoPageLayout pageLayout = KoPageLayout::standardLayout();
        pageLayout.loadOasis( *masterPageStyle );
        d->document->m_pageManager.setDefaultPage(pageLayout);
#if 0
        const QDomElement properties( KoDom::namedItemNS( *masterPageStyle, KoXmlNS::style, "page-layout-properties" ) );
        const QDomElement footnoteSep = KoDom::namedItemNS( properties, KoXmlNS::style, "footnote-sep" );
        if ( !footnoteSep.isNull() ) {
            // style:width="0.018cm" style:distance-before-sep="0.101cm"
            // style:distance-after-sep="0.101cm" style:adjustment="left"
            // style:rel-width="25%" style:color="#000000"
            const QString width = footnoteSep.attributeNS( KoXmlNS::style, "width", QString::null );
            if ( !width.isEmpty() ) m_footNoteSeparatorLineWidth = KoUnit::parseValue( width );
            QString pageWidth = footnoteSep.attributeNS( KoXmlNS::style, "rel-width", QString::null );
            if ( pageWidth.endsWith( "%" ) ) {
                pageWidth.truncate( pageWidth.length() - 1 ); // remove '%'
                m_iFootNoteSeparatorLineLength = qRound( pageWidth.toDouble() );
            }
            // Not in KWord: color, distance before and after separator
            const QString style = footnoteSep.attributeNS( KoXmlNS::style, "line-style", QString::null );
            if ( style == "solid" || style.isEmpty() ) m_footNoteSeparatorLineType = SLT_SOLID;
            else if ( style == "dash" ) m_footNoteSeparatorLineType = SLT_DASH;
            else if ( style == "dotted" ) m_footNoteSeparatorLineType = SLT_DOT;
            else if ( style == "dot-dash" ) m_footNoteSeparatorLineType = SLT_DASH_DOT;
            else if ( style == "dot-dot-dash" ) m_footNoteSeparatorLineType = SLT_DASH_DOT_DOT;
            else kdDebug() << "Unknown value for m_footNoteSeparatorLineType: " << style << endl;
            const QString pos = footnoteSep.attributeNS( KoXmlNS::style, "adjustment", QString::null );
            if ( pos == "centered" ) m_footNoteSeparatorLinePos = SLP_CENTERED;
            else if ( pos == "right") m_footNoteSeparatorLinePos = SLP_RIGHT;
            else // if ( pos == "left" ) m_footNoteSeparatorLinePos = SLP_LEFT;
        }
        const QDomElement columnsElem = KoDom::namedItemNS( properties, KoXmlNS::style, "columns" );
        if ( !columnsElem.isNull() ) {
            columns.columns = columnsElem.attributeNS( KoXmlNS::fo, "column-count", QString::null ).toInt();
            if ( columns.columns == 0 ) columns.columns = 1;
            // TODO OASIS OpenDocument supports columns of different sizes, using <style:column style:rel-width="...">
            // (with fo:start-indent/fo:end-indent for per-column spacing)
            // But well, it also allows us to specify a single gap.
            if ( columnsElem.hasAttributeNS( KoXmlNS::fo, "column-gap" ) ) columns.ptColumnSpacing = KoUnit::parseValue( columnsElem.attributeNS( KoXmlNS::fo, "column-gap", QString::null ) );
            // It also supports drawing a vertical line as a separator...
        }
        // TODO spHeadBody (where is this in OOo?)
        // TODO spFootBody (where is this in OOo?)
        // Answer: margins of the <style:header-footer> element
#endif
    }
#if 0
    else // this doesn't happen with normal documents, but it can happen if copying something,
         // pasting into konq as foo.odt, then opening that...
    {
        d->columns.columns = 1;
        d->columns.columnSpacing = 2;
        m_headerVisible = false;
        m_footerVisible = false;
        m_pageLayout = KoPageLayout::standardLayout();
        pageManager()->setDefaultPage(m_pageLayout);
    }
#endif
    return true;
}

bool KWOpenDocumentLoader::loadMasterPageStyle(const QString& masterPageName, KoOasisLoadingContext& context)
{
    kDebug(32001)<<"KWOpenDocumentLoader::loadMasterPageStyle"<<endl;
    const KoOasisStyles& styles = context.oasisStyles();
    Q_ASSERT( styles.masterPages().contains(masterPageName) );
    const QDomElement *masterPage = styles.masterPages()[ masterPageName ];
    const QDomElement *masterPageStyle = masterPage ? styles.findStyle( masterPage->attributeNS( KoXmlNS::style, "page-layout-name", QString::null ) ) : 0;
#if 0
    // This check is done here and not in loadOasisPageLayout in case the Standard master-page
    // has no page information but the first paragraph points to a master-page that does (#129585)
    if ( m_pageLayout.ptWidth <= 1e-13 || m_pageLayout.ptHeight <= 1e-13 ) {
        // Loading page layout failed, try to see why.
        QDomElement properties( KoDom::namedItemNS( *masterPageStyle, KoXmlNS::style, "page-layout-properties" ) );
        //if ( properties.isNull() )
        //    setErrorMessage( i18n( "Invalid document. No page layout properties were found. The application which produced this document isn't OASIS-compliant." ) );
        //else if ( properties.hasAttributeNS( KoXmlNS::fo, "page-width" ) )
        //    setErrorMessage( i18n( "Invalid document. Page layout has no page width. The application which produced this document isn't OASIS-compliant." ) );
        //else
        if ( properties.hasAttributeNS( "http://www.w3.org/1999/XSL/Format", "page-width" ) )
            setErrorMessage( i18n( "Invalid document. 'fo' has the wrong namespace. The application which produced this document is not OASIS-compliant." ) );
        else
            setErrorMessage( i18n( "Invalid document. Paper size: %1x%2" ).arg( m_pageLayout.ptWidth ).arg( m_pageLayout.ptHeight ) );
        return false;
    }
#endif
    if ( masterPageStyle ) {
        loadHeaderFooter(*masterPage, *masterPageStyle, context, true); // Load headers
        loadHeaderFooter(*masterPage, *masterPageStyle, context, false); // Load footers
    }
    return true;
}

//KoStyleCollection::loadOasisStyles
void KWOpenDocumentLoader::loadStyles(KoOasisLoadingContext& context)
{
    kDebug(32001)<<"KWOpenDocumentLoader::loadStyles"<<endl;
#if 0
    QStringList followingStyles;
    QList<KoXmlElement*> userStyles = context.oasisStyles().customStyles( "paragraph" ).values();
    bool defaultStyleDeleted = false;
    int stylesLoaded = 0;
    const unsigned int nStyles = userStyles.count();
    for (unsigned int item = 0; item < nStyles; item++) {
        KoXmlElement* styleElem = userStyles[item];
        if ( !styleElem ) continue;
        Q_ASSERT( !styleElem->isNull() );
        if( !defaultStyleDeleted ) { // we are going to import at least one style.
            KoParagStyle *s = defaultStyle();
            //kDebug() << "loadOasisStyles looking for Standard, to delete it. Found " << s << endl;
            if(s) removeStyle(s); // delete the standard style.
            defaultStyleDeleted = true;
        }
        KoParagStyle *sty = new KoParagStyle( QString::null );
        // Load the style
        sty->loadStyle( *styleElem, context );
        // Style created, now let's try to add it
        const int oldStyleCount = count();
        sty = addStyle( sty );
        // the real value of followingStyle is set below after loading all styles
        sty->setFollowingStyle( sty );
        kDebug() << " Loaded style " << sty->name() << endl;
        if ( count() > oldStyleCount ) {
            const QString following = styleElem->attributeNS( KoXmlNS::style, "next-style-name", QString::null );
            followingStyles.append( following );
            ++stylesLoaded;
        }
        else kWarning() << "Found duplicate style declaration, overwriting former " << sty->name() << endl;
    }
    if( followingStyles.count() != styleList().count() )
        kDebug() << "Ouch, " << followingStyles.count() << " following-styles, but " << styleList().count() << " styles in styleList" << endl;
    unsigned int i = 0;
    QString tmpString;
    foreach( tmpString, followingStyles ) {
        const QString followingStyleName = tmpString;
        if ( !followingStyleName.isEmpty() ) {
            KoParagStyle * style = findStyle( followingStyleName );
            if ( style ) styleAt(i)->setFollowingStyle( style );
        }
    }
    // TODO the same thing for style inheritance (style:parent-style-name) and setParentStyle()
    Q_ASSERT( defaultStyle() );
    return stylesLoaded;
#else
    QList<KoXmlElement*> userStyles = context.oasisStyles().customStyles( "paragraph" ).values();
    foreach(KoXmlElement* styleElem, userStyles) {
        if ( !styleElem ) continue;
        Q_ASSERT( !styleElem->isNull() );

        //KoParagStyle::loadStyle
        QString name = styleElem->attributeNS( KoXmlNS::style, "name", QString::null );
        QString displayName = styleElem->attributeNS( KoXmlNS::style, "display-name", QString::null );
        if ( displayName.isEmpty() )
            displayName = name;

        kDebug(32001)<<"KWOpenDocumentLoader::loadStyles styleName="<<name<<" styleDisplayName="<<displayName<<endl;
#if 0
        // OOo hack:
        //m_bOutline = name.startsWith( "Heading" );
        // real OASIS solution:
        bool m_bOutline = styleElem->hasAttributeNS( KoXmlNS::style, "default-outline-level" );
#endif
        context.styleStack().save();
        context.addStyles( styleElem, "paragraph" ); // Load all parents - only because we don't support inheritance.
#if 0
        //KoParagLayout::loadOasisParagLayout
        context.styleStack().setTypeProperties( "paragraph" );
#endif
        KoStyleStack& styleStack = context.styleStack();

        KoParagraphStyle *parastyle = new KoParagraphStyle();
        parastyle->setName(name);
        //parastyle->setParent( d->document->styleManager()->defaultParagraphStyle() );
        d->document->styleManager()->add(parastyle);

        //KoTextParag::loadOasis => KoParagLayout::loadOasisParagLayout
        styleStack.setTypeProperties( "paragraph" ); // load all style attributes from "style:paragraph-properties"
        parastyle->loadOasis(styleStack); // load the KoParagraphStyle from the stylestack

        //KoTextFormat::load
        KoCharacterStyle *charstyle = parastyle->characterStyle();
        styleStack.setTypeProperties( "text" ); // load all style attributes from "style:text-properties"
        charstyle->loadOasis(styleStack); // load the KoCharacterStyle from the stylestack

        context.styleStack().restore();
    }
#endif
}

//KWOasisLoader::loadOasisHeaderFooter
void KWOpenDocumentLoader::loadHeaderFooter(const QDomElement& masterPage, const QDomElement& masterPageStyle, KoOasisLoadingContext& context, bool isHeader)
{
    // Not OpenDocument compliant element to define the first header/footer.
    QDomElement firstElem = KoDom::namedItemNS( masterPage, KoXmlNS::style, isHeader ? "header-first" : "footer-first" );
    // The actual content of the header/footer.
    QDomElement elem = KoDom::namedItemNS( masterPage, KoXmlNS::style, isHeader ? "header" : "footer" );

    const bool hasFirst = !firstElem.isNull();
    if ( !hasFirst && elem.isNull() )
        return; // no header/footer

    const QString localName = elem.localName();
    kDebug()<<"KWOpenDocumentLoader::loadHeaderFooter localName="<<localName<<" isHeader="<<isHeader<<" hasFirst="<<hasFirst<<endl;

    // Formatting properties for headers and footers on a page.
    QDomElement styleElem = KoDom::namedItemNS( masterPageStyle, KoXmlNS::style, isHeader ? "header-style" : "footer-style" );

    // The two additional elements <style:header-left> and <style:footer-left> specifies if defined that even and odd pages
    // should be displayed different. If they are missing, the conent of odd and even (aka left and right) pages are the same.
    QDomElement leftElem = KoDom::namedItemNS( masterPage, KoXmlNS::style, isHeader ? "header-left" : "footer-left" );

    // Determinate the type of the frameset used for the header/footer.
    QString fsTypeName;
    KWord::TextFrameSetType fsType = KWord::OtherTextFrameSet;
    if ( localName == "header" ) {
        fsType = KWord::OddPagesHeaderTextFrameSet;
        fsTypeName = leftElem.isNull() ? i18n( "Header" ) : i18n("Odd Pages Header");
    }
    else if ( localName == "header-left" ) {
        fsType = KWord::EvenPagesHeaderTextFrameSet;
        fsTypeName = i18n("Even Pages Header");
    }
    else if ( localName == "footer" ) {
        fsType = KWord::OddPagesFooterTextFrameSet;
        fsTypeName = leftElem.isNull() ? i18n( "Footer" ) : i18n("Odd Pages Footer");
    }
    else if ( localName == "footer-left" ) {
        fsType = KWord::EvenPagesFooterTextFrameSet;
        fsTypeName = i18n("Even Pages Footer");
    }
    else if ( localName == "header-first" ) { // NOT OASIS COMPLIANT
        fsType = KWord::FirstPageHeaderTextFrameSet;
        fsTypeName = i18n("First Page Header");
    }
    else if ( localName == "footer-first" ) { // NOT OASIS COMPLIANT
        fsType = KWord::FirstPageFooterTextFrameSet;
        fsTypeName = i18n("First Page Footer");
    }
    else {
        kWarning(32001) << "Unknown tag in KWOpenDocumentLoader::loadHeaderFooter: " << localName << endl;
        return;
    }

    // Set the type of the header/footer in the KWPageSettings instance of our document.
    if ( !leftElem.isNull() ) {
        //d->hf.header = hasFirst ? HF_FIRST_EO_DIFF : HF_EO_DIFF;
        if( isHeader ) {
            d->document->m_pageSettings.setHeaderPolicy(KWord::HFTypeEvenOdd);
            //d->document->m_pageSettings.setFirstHeaderPolicy(KWord::HFTypeEvenOdd);
        }
        else {
            d->document->m_pageSettings.setFooterPolicy(KWord::HFTypeEvenOdd);
            //d->document->m_pageSettings.setFirstFooterPolicy(KWord::HFTypeEvenOdd);
        }
    }
    else {
        //d->hf.header = hasFirst ? HF_FIRST_DIFF : HF_SAME;
        if( isHeader ) {
            d->document->m_pageSettings.setHeaderPolicy(KWord::HFTypeSameAsFirst);
            d->document->m_pageSettings.setFirstHeaderPolicy(KWord::HFTypeEvenOdd);
        }
        else {
            d->document->m_pageSettings.setFooterPolicy(KWord::HFTypeSameAsFirst);
            d->document->m_pageSettings.setFirstFooterPolicy(KWord::HFTypeEvenOdd);
        }
    }

#if 0
    KWTextFrameSet *fs = new KWTextFrameSet( m_doc, headerTypeToFramesetName( localName, hasEvenOdd ) );
    fs->setFrameSetInfo( headerTypeToFrameInfo( localName, hasEvenOdd ) );
    m_doc->addFrameSet( fs, false );
    if ( !style.isNull() ) context.styleStack().push( style );
    KWFrame* frame = new KWFrame( fs, 29, isHeader?0:567, 798-29, 41 );
    frame->loadCommonOasisProperties( context, fs, "header-footer" );
    const QString minHeight = context.styleStack().attributeNS( KoXmlNS::fo, "min-height" );
    if ( !minHeight.isEmpty() ) frame->setMinimumFrameHeight( KoUnit::parseValue( minHeight ) );
    frame->setFrameBehavior( KWFrame::AutoExtendFrame );
    frame->setNewFrameBehavior( KWFrame::Copy );
    fs->addFrame( frame );
    if ( !style.isNull() ) context.styleStack().pop(); // don't let it be active when parsing the text
    context.setUseStylesAutoStyles( true ); // use auto-styles from styles.xml, not those from content.xml
    fs->loadOasisContent( headerFooter, context );
    context.setUseStylesAutoStyles( false );
    if ( isHeader ) m_doc->m_headerVisible = true; else m_doc->m_footerVisible = true;
    if ( !headerStyle.isNull() ) { // The bottom margin of headers is what we call headerBodySpacing
        context.styleStack().push( headerStyle );
        context.styleStack().setTypeProperties( "header-footer" );
        d->hf.ptHeaderBodySpacing = KoUnit::parseValue( context.styleStack().attributeNS( KoXmlNS::fo, "margin-bottom" ) );
        context.styleStack().pop();
    }
    if ( !footerStyle.isNull() ) { // The top margin of footers is what we call footerBodySpacing
        context.styleStack().push( footerStyle );
        context.styleStack().setTypeProperties( "header-footer" );
        d->hf.ptFooterBodySpacing = KoUnit::parseValue( context.styleStack().attributeNS( KoXmlNS::fo, "margin-top" ) );
        context.styleStack().pop();
    }
    // TODO ptFootNoteBodySpacing
#else
    // use auto-styles from styles.xml, not those from content.xml
    context.setUseStylesAutoStyles( true );

    // Add the frameset and the shape for the header/footer to the document.
    KWTextFrameSet *fs = new KWTextFrameSet( d->document, fsType );
    fs->setAllowLayout(false);
    fs->setName(fsTypeName);
    d->document->addFrameSet(fs);
    KoShapeFactory *factory = KoShapeRegistry::instance()->get(TextShape_SHAPEID);
    Q_ASSERT(factory);
    KoShape *shape = factory->createDefaultShape();
    KWTextFrame *frame = new KWTextFrame(shape, fs);
    frame->setFrameBehavior(KWord::AutoExtendFrameBehavior);

    QTextCursor cursor( fs->document() );
    //cursor.insertText(fsTypeName); //TESTCASE

    if ( !leftElem.isNull() ) // if "header-left" or "footer-left" was defined, the content is within the leftElem
        loadBody(leftElem, context, cursor);
    else if( hasFirst ) // if there was a "header-first" or "footer-first" defined, the content is within the firstElem
        loadBody(firstElem, context, cursor);
    else // else the content is within the elem
        loadBody(elem, context, cursor);

    // restore use of auto-styles from content.xml, not those from styles.xml
    context.setUseStylesAutoStyles( false );

    //TODO handle style, seems to be similar to what is done at KoPageLayout::loadOasis
#endif
}

















//KoTextDocument::loadOasisText
void KWOpenDocumentLoader::loadBody( const QDomElement& bodyElem, KoOasisLoadingContext& context, QTextCursor& cursor)
{
    kDebug(32001)<<"KWOpenDocumentLoader::loadBody"<<endl;
    KoXmlElement tag;
    forEachElement(tag, bodyElem) {
        context.styleStack().save();
        const QString localName = tag.localName();
        const bool isTextNS = ( tag.namespaceURI() == KoXmlNS::text );

        if ( isTextNS && localName == "p" ) {  // text paragraph
            loadParagraph(tag, context, cursor);
        }
        else if ( isTextNS && localName == "h" ) { // heading
            loadHeading(tag, context, cursor);
        }
        else if ( isTextNS &&
                  ( localName == "unordered-list" || localName == "ordered-list" // OOo-1.1
                    || localName == "list" || localName == "numbered-paragraph" ) ) { // OASIS
            loadList(tag, context, cursor);
        }
        else if ( isTextNS && localName == "section" ) { // Temporary support (###TODO)
            loadSection(tag, context, cursor);
        }

        context.styleStack().restore(); // remove the styles added by the paragraph or list
    }
}

//KoTextDocument::loadOasisText
void KWOpenDocumentLoader::loadParagraph(const KoXmlElement& parent, KoOasisLoadingContext& context, QTextCursor& cursor)
{
    kDebug(32001)<<"KWOpenDocumentLoader::loadParagraph"<<endl;
    context.fillStyleStack( parent, KoXmlNS::text, "style-name", "paragraph" );
#if 0
    KoTextParag *parag = createParag( this, lastParagraph, nextParagraph );
    parag->loadOasis( tag, context, styleColl, pos );
#endif
    QString userStyleName = context.styleStack().userStyleName( "paragraph" );
    KoParagraphStyle *userStyle = d->document->styleManager()->paragraphStyle(userStyleName);
    //if( ! userStyle ) userStyle = d->document->styleManager()->defaultParagraphStyle();
    if( userStyle ) {
        context.styleStack().setTypeProperties( "paragraph" );
        userStyle->loadOasis( context.styleStack() );
        QTextBlock block = cursor.block();
        userStyle->applyStyle(block);
    }

    //KWTextParag::loadOasis
    QString styleName = parent.attributeNS( KoXmlNS::text, "style-name", QString::null );
    kDebug()<<"==> PARAGRAPH styleName="<<styleName<<" userStyleName="<<userStyleName<<" userStyle="<<(userStyle?"YES":"NULL")<<endl;
    if ( !styleName.isEmpty() ) {
        const QDomElement* paragraphStyle = context.oasisStyles().findStyle( styleName, "paragraph" );
        QString masterPageName = paragraphStyle ? paragraphStyle->attributeNS( KoXmlNS::style, "master-page-name", QString::null ) : QString::null;
        if ( masterPageName.isEmpty() )
            masterPageName = "Standard";

        kDebug(32001) << "KWOpenDocumentLoader::loadSpan paragraphStyle.localName=" << (paragraphStyle ? paragraphStyle->localName() : "NULL") << " masterPageName=" << masterPageName << endl;

        /*
        QString styleName = context.styleStack().userStyleName( "paragraph" );
        KoParagraphStyle *style = d->document->styleManager()->paragraphStyle(styleName);
        if ( !style ) {
            kDebug(32001) << "KWOpenDocumentLoader::loadSpan: Unknown style. Using default!" << endl;
            style = d->document->styleManager()->defaultParagraphStyle();
        }
        */

        //d->currentMasterPage = masterPageName; // do this first to avoid recursion
        context.styleStack().save();
        context.styleStack().setTypeProperties( "paragraph" );
        context.addStyles( paragraphStyle, "paragraph" );

        KoParagraphStyle *style = d->document->styleManager()->paragraphStyle(styleName);
        if ( style ) {
            style->loadOasis( context.styleStack() );
            QTextBlock block = cursor.block();
            style->applyStyle(block);
        }

#if 0
        // This is quite ugly... OOo stores the starting page-number in the first paragraph style...
        QString pageNumber = context.styleStack().attributeNS( KoXmlNS::style, "page-number" );
        if ( !pageNumber.isEmpty() )
            doc->variableCollection()->variableSetting()->setStartingPageNumber( pageNumber.toInt() );
#endif
        context.styleStack().restore();
        loadPageLayout( masterPageName, context ); // page layout
    }

    //KoTextParag::loadOasisSpan
    loadSpan(parent, context, cursor);

    QTextBlockFormat emptyTbf;
    QTextCharFormat emptyCf;
    cursor.insertBlock(emptyTbf, emptyCf);
}

//KoTextDocument::loadOasisText
void KWOpenDocumentLoader::loadHeading(const KoXmlElement& parent, KoOasisLoadingContext& context, QTextCursor& cursor)
{
    kDebug(32001)<<"KWOpenDocumentLoader::loadHeading"<<endl;
    context.fillStyleStack( parent, KoXmlNS::text, "style-name", "paragraph" );
#if 0
    int level = tag.attributeNS( KoXmlNS::text, "outline-level", QString::null ).toInt();
    bool listOK = false;
    // When a heading is inside a list, it seems that the list prevails.
    // Example:
    //    <text:list text:style-name="Numbering 1">
    //      <text:list-item text:start-value="5">
    //        <text:h text:style-name="P2" text:level="4">The header</text:h>
    // where P2 has list-style-name="something else"
    // Result: the numbering of the header follows "Numbering 1".
    // So we use the style for the outline level only if we're not inside a list:
    //if ( !context.atStartOfListItem() )
    // === The new method for this is that we simply override it after loading.
    listOK = context.pushOutlineListLevelStyle( level );
    int restartNumbering = -1;
    if ( tag.hasAttributeNS( KoXmlNS::text, "start-value" ) )
        // OASIS extension http://lists.oasis-open.org/archives/office/200310/msg00033.html
        restartNumbering = tag.attributeNS( KoXmlNS::text, "start-value", QString::null ).toInt();
    KoTextParag *parag = createParag( this, lastParagraph, nextParagraph );
    parag->loadOasis( tag, context, styleColl, pos );
    if ( !lastParagraph ) setFirstParag( parag ); // First parag
    lastParagraph = parag;
    if ( listOK ) {
        parag->applyListStyle( context, restartNumbering, true /*ordered*/, true /*heading*/, level );
        context.listStyleStack().pop();
    }
#else
    //KWTextParag::loadOasis
    const QString styleName = parent.attributeNS( KoXmlNS::text, "style-name", QString::null );
    kDebug()<<"==> HEADING styleName="<<styleName<<endl;
    if ( !styleName.isEmpty() ) {
        const QDomElement* paragraphStyle = context.oasisStyles().findStyle( styleName, "paragraph" );
        //QString masterPageName = paragraphStyle ? paragraphStyle->attributeNS( KoXmlNS::style, "master-page-name", QString::null ) : QString::null;
        //if ( masterPageName.isEmpty() ) masterPageName = "Standard"; // Seems to be a builtin name for the default layout...
        kDebug(32001) << "KWOpenDocumentLoader::loadBody styleName=" << styleName << endl;
        context.styleStack().save();
        context.styleStack().setTypeProperties( "paragraph" );
        context.addStyles( paragraphStyle, "paragraph" );
        context.styleStack().restore();
        //loadPageLayout( masterPageName, context ); // page layout
    }
#endif

    //KoTextParag::loadOasisSpan
    loadSpan(parent, context, cursor);

    QTextBlockFormat emptyTbf;
    QTextCharFormat emptyCf;
    cursor.insertBlock(emptyTbf, emptyCf);
}

//KoTextDocument::loadOasisText
void KWOpenDocumentLoader::loadList(const KoXmlElement& parent, KoOasisLoadingContext& context, QTextCursor& cursor)
{
    kDebug(32001)<<"KWOpenDocumentLoader::loadList"<<endl;
    //TODO
    //lastParagraph = loadList( tag, context, lastParagraph, styleColl, nextParagraph );
    Q_UNUSED(parent);
    Q_UNUSED(context);
    Q_UNUSED(cursor);
}

//KoTextDocument::loadOasisText
void KWOpenDocumentLoader::loadSection(const KoXmlElement& parent, KoOasisLoadingContext& context, QTextCursor& cursor)
{
    kDebug(32001)<<"KWOpenDocumentLoader::loadSection"<<endl;
    //TODO
    //kdDebug(32500) << "Section found!" << endl;
    //context.fillStyleStack( tag, KoXmlNS::text, "style-name", "section" );
    //lastParagraph = loadOasisText( tag, context, lastParagraph, styleColl, nextParagraph );
    Q_UNUSED(parent);
    Q_UNUSED(context);
    Q_UNUSED(cursor);
}

// we cannot use QString::simplifyWhiteSpace() because it removes
// leading and trailing whitespace, but such whitespace is significant
// in ODF -- so we use this function to compress sequences of space characters
// into single spaces
static QString normalizeWhitespace( const QString& in, bool leadingSpace )
{
    QString text = in;
    int r, w = 0;
    int len = text.length();

    for ( r = 0; r < len; ++r )
    {
        QCharRef ch = text[r];

        // check for space, tab, line feed, carriage return
        if ( ch == ' ' || ch == '\t' ||ch == '\r' ||  ch == '\n')
        {
            // if we were lead by whitespace in some parent or previous sibling element,
            // we completely collapse this space
            if ( r != 0 || !leadingSpace )
                text[w++] = QChar( ' ' );

            // find the end of the whitespace run
            while ( r < len && text[r].isSpace() )
                ++r;

            // and then record the next non-whitespace character
            if ( r < len )
               text[w++] = text[r];
        }
        else
        {
            text[w++] = ch;
        }
    }

    // and now trim off the unused part of the string
    text.truncate(w);

    return text;
}

//KoTextParag::loadOasisSpan
void KWOpenDocumentLoader::loadSpan(const KoXmlElement& parent, KoOasisLoadingContext& context, QTextCursor& cursor)
{
    QString styleName = context.styleStack().userStyleName( "paragraph" );
    kDebug(32001) << "KWOpenDocumentLoader::loadSpan styleName=" << styleName << endl;
    KoParagraphStyle *style = d->document->styleManager()->paragraphStyle(styleName);
    if ( !style ) {
        kDebug(32001) << "KWOpenDocumentLoader::loadSpan: Unknown style. Using default!" << endl;
        style = d->document->styleManager()->defaultParagraphStyle();
    }

#if 0
//TODO get right of this dirty hack
if(parent.localName()!="span") {
    context.styleStack().setTypeProperties( "paragraph" );
    style->loadOasis( context.styleStack() );
    QTextBlock block = cursor.block();
    style->applyStyle(block);
}
#endif

    for (KoXmlNode node = parent.firstChild(); !node.isNull(); node = node.nextSibling() )
    {
        KoXmlElement ts = node.toElement();
        const QString localName( ts.localName() );
        const bool isTextNS = ts.namespaceURI() == KoXmlNS::text;

        // allow loadSpanTag to modify the stylestack
        context.styleStack().save();

        if ( node.isText() )
        {
            QString text = node.toText().data();
            kDebug() << "  <text> localName=" << localName << " parent.localName="<<parent.localName()<<" text=" << text << endl;

            int prevpos = cursor.position();
            //context.fillStyleStack( ts, KoXmlNS::text, "style-name", "text" );
            context.styleStack().setTypeProperties( "text"/*"paragraph"*/ );
            //const QString textStyleName = ts.attributeNS( KoXmlNS::text, "style-name", QString::null );
            const QString textStyleName = parent.attributeNS( KoXmlNS::text, "style-name", QString::null );
            const KoXmlElement* textStyleElem = textStyleName.isEmpty() ? 0 : context.oasisStyles().findStyle( textStyleName, "text"/*"paragraph"*/ );
            KoCharacterStyle *charstyle1 = 0;
            if( textStyleElem ) {
                kDebug()<<"textStyleName="<<textStyleName<<endl;
                context.addStyles( textStyleElem, "text"/*"paragraph"*/ );
                //style->applyStyle(cursor);
                charstyle1 = d->document->styleManager()->characterStyle(textStyleName);
                if( ! charstyle1 ) {
                    charstyle1 = new KoCharacterStyle();
                    charstyle1->setName(textStyleName);
                    charstyle1->loadOasis( context.styleStack() );
                    d->document->styleManager()->add(charstyle1);
                }
                charstyle1->applyStyle(&cursor);
            }
            else {
                //style->characterStyle()->applyStyle(&cursor);
            }

            bool stripLeadingSpace = true;
            cursor.insertText( normalizeWhitespace(text.replace('\n', QChar(0x2028)), stripLeadingSpace) );

            int currentpos = cursor.position();
            cursor.setPosition(prevpos, QTextCursor::MoveAnchor);
            cursor.setPosition(currentpos, QTextCursor::KeepAnchor);
            if( charstyle1 ) {
                kDebug()<<"  5 => ///"<<endl;
                kDebug()<<"  selectionStart="<<cursor.selectionStart()<<" selectionEnd="<<cursor.selectionEnd()<<" selectedText="<<cursor.selectedText()<<endl;
                charstyle1->applyStyle(&cursor);
            } else {
                kDebug()<<"  6 => ///"<<endl;
                /*KoCharacterStyle *charstyle2 = d->document->styleManager()->characterStyle( cursor.blockCharFormat().intProperty(KoCharacterStyle::StyleId) );
                if(charstyle2) {
                    QTextBlock block = cursor.block();
                    charstyle2->applyStyle(block);
                }*/
                //QTextBlock block = cursor.block();
                style->characterStyle()->applyStyle(&cursor);
            }
            cursor.setPosition(currentpos, QTextCursor::MoveAnchor);

        }
        else if ( isTextNS && localName == "span" ) // text:span
        {
            kDebug() << "  <span> localName=" << localName << endl;

            context.styleStack().save();
            context.fillStyleStack( ts, KoXmlNS::text, "style-name", "text" );
            //context.styleStack().setTypeProperties( "text"/*"paragraph"*/ );
#if 0
            const QString textStyleName = ts.attributeNS( KoXmlNS::text, "style-name", QString::null );
            const KoXmlElement* textStyleElem = textStyleName.isEmpty() ? 0 : context.oasisStyles().findStyle( textStyleName, "text"/*"paragraph"*/ );
            if ( textStyleElem ) {
                context.styleStack().setTypeProperties( "text"/*"paragraph"*/ );
                context.addStyles( textStyleElem, "text"/*"paragraph"*/ );
                KoCharacterStyle *charstyle = d->document->styleManager()->characterStyle(textStyleName);
                if( ! charstyle ) {
                    charstyle = new KoCharacterStyle();
                    charstyle->loadOasis( context.styleStack() );
                    d->document->styleManager()->add(charstyle);
                }
                charstyle->applyStyle(&cursor);
            }
#endif
            loadSpan( ts, context, cursor ); // recurse

            context.styleStack().restore();
        }
        else if ( isTextNS && localName == "s" ) // text:s
        {
            int howmany = 1;
            if (ts.hasAttributeNS( KoXmlNS::text, "c"))
                howmany = ts.attributeNS( KoXmlNS::text, "c", QString::null).toInt();
            cursor.insertText( QString().fill(32, howmany) );
        }
#if 0
        else if ( isTextNS && localName == "tab" ) // text:tab (it's tab-stop in OO-1.1 but tab in oasis)
        {
            textData = '\t';
        }
#endif
        else if ( isTextNS && localName == "line-break" ) // text:line-break
        {
            kDebug() << "  <line-break> Node localName=" << localName << endl;
            QTextBlockFormat emptyTbf;
            QTextCharFormat emptyCf;
            cursor.insertBlock(emptyTbf, emptyCf);
        }
        else if ( isTextNS && localName == "number" ) // text:number
        {
            // This is the number in front of a numbered paragraph,
            // written out to help export filters. We can ignore it.
        }
#if 0
#ifdef KOXML_USE_QDOM
        else if ( node.isProcessingInstruction() )
        {
            QDomProcessingInstruction pi = node.toProcessingInstruction();
            if ( pi.target() == "opendocument" && pi.data().startsWith( "cursor-position" ) )
            {
                context.setCursorPosition( this, pos );
            }
        }
#endif
#endif
        else
        {
#if 0
            bool handled = false;
            // Check if it's a variable
            KoVariable* var = context.variableCollection().loadOasisField( textDocument(), ts, context );
            if ( var ) {
                textData = "#";     // field placeholder
                customItem = var;
                handled = true;
            }
            if ( !handled ) {
                handled = textDocument()->loadSpanTag( ts, context, this, pos, textData, customItem );
                if ( !handled ) {
                    kWarning(32500) << "Ignoring tag " << ts.tagName() << endl;
                    context.styleStack().restore();
                    continue;
                }
            }
#else
            kDebug() << "  Node localName=" << localName << " is UNHANDLED" << endl;
#endif
        }

        // restore the propably by loadSpanTag modified stylestack
        context.styleStack().restore();
    }
}

#include "KWOpenDocumentLoader.moc"
