/* This file is part of the KDE project
   Copyright (C) 2002 Laurent MONTEL <lmontel@mandrakesoft.com>

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

#include "KWordTextFrameSetIface.h"
#include "KWordViewIface.h"
#include "kwtextframeset.h"
#include <KoTextViewIface.h>
#include "kwview.h"
#include <kotextobject.h>
#include "kwcanvas.h"
#include "kwdoc.h"
#include <kapplication.h>
#include <dcopclient.h>
#include "KWordFrameSetIface.h"
#include <kdebug.h>
#include <kcommand.h>

KWordTextFrameSetIface::KWordTextFrameSetIface( KWTextFrameSet *_frame )
    : KWordFrameSetIface( _frame)
{
   m_frametext = _frame;
}

DCOPRef KWordTextFrameSetIface::startEditing()
{
    KWDocument *doc=m_frametext->kWordDocument();
    QPtrList <KWView> lst=doc->getAllViews();
    lst.at(0)->getGUI()->canvasWidget()->checkCurrentTextEdit(m_frametext);
    return DCOPRef( kapp->dcopClient()->appId(),
		    (static_cast<KWTextFrameSetEdit *>( lst.at(0)->getGUI()->canvasWidget()->currentFrameSetEdit()))->dcopObject()->objId() );
}

bool KWordTextFrameSetIface::hasSelection() const
{
    return m_frametext->hasSelection();
}

int KWordTextFrameSetIface::numberOfParagraphs()
{
    return m_frametext->paragraphs();
}

QString KWordTextFrameSetIface::name() const
{
    return m_frametext->getName();
}

QString KWordTextFrameSetIface::selectedText() const
{
    return m_frametext->selectedText();
}

void KWordTextFrameSetIface::selectAll( bool select )
{
    m_frametext->textObject()->selectAll(select);
}

int KWordTextFrameSetIface::paragraphsSelected()
{
    return m_frametext->paragraphsSelected();
}

void KWordTextFrameSetIface::setBoldText( bool b )
{
    KCommand *cmd=m_frametext->textObject()->setBoldCommand( b );
    delete cmd;
}

void KWordTextFrameSetIface::setItalicText( bool b )
{
    KCommand *cmd=m_frametext->textObject()->setItalicCommand(b);
    delete cmd;
}

void KWordTextFrameSetIface::setUnderlineText( bool b )
{
    KCommand *cmd=m_frametext->textObject()->setUnderlineCommand(b);
    delete cmd;
}

void KWordTextFrameSetIface::setDoubleUnderlineText(bool b)
{
    KCommand *cmd=m_frametext->textObject()->setDoubleUnderlineCommand(b);
    delete cmd;
}

void KWordTextFrameSetIface::setStrikeOutText( bool b )
{
    KCommand *cmd=m_frametext->textObject()->setStrikeOutCommand(b);
    delete cmd;
}

void KWordTextFrameSetIface::setTextColor( const QColor &col )
{
    KCommand *cmd=m_frametext->textObject()->setTextColorCommand(col);
    delete cmd;
}

void KWordTextFrameSetIface::setTextPointSize( int s )
{
    KCommand *cmd=m_frametext->textObject()->setPointSizeCommand( s );
    delete cmd;
}

void KWordTextFrameSetIface::setTextSubScript( bool b )
{
    KCommand *cmd=m_frametext->textObject()->setTextSubScriptCommand( b );
    delete cmd;
}

void KWordTextFrameSetIface::setTextSuperScript( bool b )
{
    KCommand *cmd=m_frametext->textObject()->setTextSuperScriptCommand( b );
    delete cmd;
}

void KWordTextFrameSetIface::setTextDefaultFormat()
{
    KCommand *cmd=m_frametext->textObject()->setDefaultFormatCommand();
    delete cmd;
}

void KWordTextFrameSetIface::setTextBackgroundColor(const QColor & col)
{
    KCommand *cmd=m_frametext->textObject()->setTextBackgroundColorCommand(col);
    delete cmd;
}

void KWordTextFrameSetIface::setUnderlineColor( const QColor & color )
{

    KCommand *cmd=m_frametext->textObject()->setUnderlineColorCommand( color );
    delete cmd;
}


QColor KWordTextFrameSetIface::textColor() const
{
    return m_frametext->textObject()->textColor();
}

QFont KWordTextFrameSetIface::textFont() const
{
    return m_frametext->textObject()->textFont();
}

QString KWordTextFrameSetIface::textFontFamily()const
{
    return m_frametext->textObject()->textFontFamily();
}

QColor KWordTextFrameSetIface::textBackgroundColor() const
{
    return m_frametext->textObject()->textBackgroundColor();
}

bool KWordTextFrameSetIface::textItalic() const
{
    return m_frametext->textObject()->textItalic();
}

bool KWordTextFrameSetIface::textBold() const
{
    return m_frametext->textObject()->textBold();
}

bool KWordTextFrameSetIface::textUnderline()const
{
    return m_frametext->textObject()->textUnderline();
}

bool KWordTextFrameSetIface::textDoubleUnderline()const
{
    return m_frametext->textObject()->textDoubleUnderline();
}

QColor KWordTextFrameSetIface::textUnderlineColor() const
{
    return m_frametext->textObject()->textUnderlineColor();
}

bool KWordTextFrameSetIface::textStrikeOut()const
{
    return m_frametext->textObject()->textStrikeOut();
}

bool KWordTextFrameSetIface::textSubScript() const
{
    return m_frametext->textObject()->textSubScript();
}

bool KWordTextFrameSetIface::textSuperScript() const
{
    return m_frametext->textObject()->textSuperScript();
}

void KWordTextFrameSetIface::setTextFamilyFont(const QString &font)
{
    KCommand *cmd=m_frametext->textObject()->setFamilyCommand(font);
    delete cmd;
}

void KWordTextFrameSetIface::changeCaseOfText( const QString & caseType)
{
    KCommand *cmd=0L;
    if( caseType.lower() == "uppercase" )
    {
        cmd = m_frametext->textObject()->setChangeCaseOfTextCommand( KoChangeCaseDia::UpperCase );
    }
    else if( caseType.lower() =="lowercase" )
    {
        cmd = m_frametext->textObject()->setChangeCaseOfTextCommand( KoChangeCaseDia::LowerCase );
    }
    else if( caseType.lower() =="titlecase" )
    {
        cmd = m_frametext->textObject()->setChangeCaseOfTextCommand( KoChangeCaseDia::TitleCase );
    }
    else if( caseType.lower() =="togglecase" )
    {
        cmd = m_frametext->textObject()->setChangeCaseOfTextCommand( KoChangeCaseDia::ToggleCase );
    }
    else
        kdDebug()<<"Error in void KWordTextFrameSetIface::changeCaseOfText( const QString & caseType) parameter\n";
    delete cmd;
}

void KWordTextFrameSetIface::setProtectContent ( bool _protect )
{
    m_frametext->textObject()->setProtectContent( _protect );
}

bool KWordTextFrameSetIface::isProtectContent() const
{
    return m_frametext->textObject()->protectContent();
}

