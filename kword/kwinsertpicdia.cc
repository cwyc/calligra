/* This file is part of the KDE project
   Copyright (C)  2001 David Faure <faure@kde.org>
   Copyright (C) 2002 Nicolas GOUTTE <nicog@snafu.de>

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

#include "kwinsertpicdia.h"
#include <klocale.h>
#include <kfiledialog.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qbitmap.h>
#include <qlayout.h>
#include <qcheckbox.h>

#include <kimageio.h>
#include <kio/netaccess.h>
#include <kdebug.h>

#include <koPicture.h>
#include <koPictureFilePreview.h>

/**
 * This is the preview that appears on the right of the "Insert picture" dialog.
 * Not to be confused with KoPictureFilePreview, the one inside the file dialog!
 * (Note: this one has to remain separate, for the day we add options like flipping, rotating, etc.)
 */
class KWInsertPicPreview : public QFrame
{
public:
    KWInsertPicPreview( QWidget *parent )
        : QFrame( parent )
    {
        setFrameStyle( WinPanel | Sunken );
        setMinimumSize( 300, 200 );
        m_type = IPD_IMAGE;
    }

    virtual ~KWInsertPicPreview() {}

    bool setPixmap( const QString & filename )
    {
        m_pixmap.load( filename );
        if ( m_pixmap.height() > 0 ) // we divide by the height in kwcanvas...
        {
            m_type = IPD_IMAGE;
            //kdDebug() << "setPixmap: m_pixmap is " << m_pixmap.width() << ", " << m_pixmap.height() << endl;
            const QBitmap nullBitmap;
            m_pixmap.setMask( nullBitmap );  //don't show transparency
            repaint(false);
            return true;
        }
        return false;
    }

    QSize pixmapSize() const { return m_pixmap.size(); }

    bool setClipart( const QString & filename )
    {
        m_type = IPD_CLIPART;
        KoPicture picture;
        if (picture.loadFromFile( filename ))
        {
            m_pixmap = QPixmap( 200, 200 );
            m_pixmap.fill( Qt::white );

            QPainter p;
            p.begin( &m_pixmap );
            p.setBackgroundColor( Qt::white );

            picture.draw(p, 0, 0, m_pixmap.width(), m_pixmap.height());
            p.end();
            repaint( false );
            return true;
        }
        else
            return false;
    }

    void drawContents( QPainter *p )
    {
        QFrame::drawContents( p );
        p->save();
        p->translate( contentsRect().x(), contentsRect().y() );
        p->drawPixmap( QPoint( 0, 0 ), m_pixmap );
        p->restore();
    }
private:
    enum { IPD_IMAGE, IPD_CLIPART } m_type;
    QPixmap m_pixmap;
};

//////////////

KWInsertPicDia::KWInsertPicDia( QWidget *parent, bool _inline, bool _keepRatio, const char *name )
    : KDialogBase( Plain, i18n("Insert Picture"), Ok|Cancel, Ok, parent, name, true )
{
    setInitialSize( QSize(400, 300) );
    m_bFirst =true;
    QWidget *page = plainPage();
    QGridLayout *grid = new QGridLayout( page, 4, 2, KDialog::marginHint(), KDialog::spacingHint() );

    QPushButton *pbImage = new QPushButton( i18n( "Choose &Image" ), page );
    grid->addWidget( pbImage, 0, 0 );
    connect( pbImage, SIGNAL( clicked() ), SLOT( slotChooseImage() ) );

    m_cbInline = new QCheckBox( i18n( "Insert picture inline" ), page );
    grid->addWidget( m_cbInline, 1, 0 );

    m_cbKeepRatio= new QCheckBox( i18n("Retain original aspect ratio"),page);
    grid->addWidget( m_cbKeepRatio, 2, 0);

    m_preview = new KWInsertPicPreview( page );
    grid->addMultiCellWidget( m_preview, 0, 3, 1, 1 );

    // Stretch the buttons and checkboxes a little, but stretch the preview much more
    grid->setRowStretch( 0, 1 );
    grid->setRowStretch( 1, 1 );
    grid->setRowStretch( 2, 1 );
    grid->setRowStretch( 3, 10 );
    grid->setColStretch( 0, 1 );
    grid->setColStretch( 1, 10 );
    m_cbKeepRatio->setChecked(_keepRatio);
    m_cbInline->setChecked( _inline );
    enableButtonOK( false );
    setFocus();
    slotChooseImage(); // save the user time, directly open the dialog
}

bool KWInsertPicDia::makeInline() const
{
    return m_cbInline->isChecked();
}

bool KWInsertPicDia::keepRatio() const
{
    return m_cbKeepRatio->isChecked();
}

void KWInsertPicDia::slotChooseImage()
{
    int result = KWInsertPicDia::selectPictureDia( m_filename, SelectImage | SelectClipart );
    if ( m_filename.isEmpty() && m_bFirst)
    {
        KDialogBase::close();
    }
    if ( result == SelectImage )
    {
        if ( m_preview->setPixmap( m_filename ) )
        {
            m_type = IPD_IMAGE;
            enableButtonOK( true );
            m_cbKeepRatio->setEnabled( true );
        }
    } else if ( result == SelectClipart )
    {
        if ( m_preview->setClipart( m_filename ) )
        {
            m_type = IPD_CLIPART;
            enableButtonOK( true );
            m_cbKeepRatio->setEnabled( false );
            m_cbKeepRatio->setChecked( false );
        }
    }
    m_bFirst= false;
}

int KWInsertPicDia::selectPictureDia( QString &filename, int flags, const QString & _path)
{
    QStringList mimetypes;
    if ( flags & SelectClipart )
        mimetypes += KoPictureFilePreview::clipartMimeTypes();
    if ( flags & SelectImage )
        mimetypes += KImageIO::mimeTypes( KImageIO::Reading );
    KFileDialog fd( _path, QString::null, 0, 0, TRUE );
    fd.setMimeFilter( mimetypes );
    fd.setCaption(i18n("Choose Image"));
    QString file = selectPicture( fd );
    if ( !file.isEmpty() )
    {
        filename = file;
        KMimeType::Ptr mime = KMimeType::findByPath( file );
        if ( flags & SelectClipart &&
             KoPictureFilePreview::clipartMimeTypes().contains( mime->name() ) )
            return SelectClipart;
        return SelectImage;
    }
    return 0;
}



#if 0
void KWInsertPicDia::slotChooseClipart()
{
    if ( KWInsertPicDia::selectClipartDia(m_filename ) )
    {
    }
}

bool KWInsertPicDia::selectClipartDia( QString &filename, const QString & _path)
{
    KFileDialog fd( _path, KoPictureFilePreview::clipartPattern(), 0, 0, true );
    fd.setCaption(i18n("Choose Clipart"));
    QString file = selectPicture( fd );
    if ( !file.isEmpty() )
    {
        filename = file;
        return true;
    }
    return false;
}
#endif

QString KWInsertPicDia::selectPicture( KFileDialog & fd )
{
    fd.setPreviewWidget( new KoPictureFilePreview( &fd ) );
    KURL url;
    if ( fd.exec() == QDialog::Accepted )
        url = fd.selectedURL();

    if( url.isEmpty() )
      return QString::null;

    QString chosen = QString::null;
    if (!KIO::NetAccess::download( url, chosen ))
        return QString::null;
    return chosen;
}

QSize KWInsertPicDia::pixmapSize() const
{
    return m_preview->pixmapSize();
}

#include "kwinsertpicdia.moc"
