/* This file is part of the KDE project
   Copyright (C) 2002 Ariya Hidayat <ariya@kde.org>

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

#include <hancomwordfilter.h>

#include <qstring.h>
#include <qtextstream.h>

#include <filterbase.h>

// helper function to escape string for XML-ness
static QString XMLEscape( const QString& str )
{
  QString result; 

  for( unsigned i=0; i<str.length(); i++ )
    if( str[i] == '&' ) result += "&amp;";
    else if( str[i] == '<' ) result += "&lt;"; 
    else if( str[i] == '>' ) result += "&gt;"; 
    else if( str[i] == '"' ) result += "&quot;";
    else if( str[i] == QChar(39) ) result += "&apos;";
    else result += str[i]; 

  return result; 
}

HancomWordFilter::HancomWordFilter( const QByteArray &prvText ):
        FilterBase()
{
  // still a nasty hack, HancomWord uses UCS4 (UTF32) but we pretend it's UCS2 (utf16)
  // well, not much char > 65536 anyway at the moment :-)
  QTextStream stream( prvText, IO_ReadOnly );
  stream.setEncoding( QTextStream::Unicode );
  m_text = stream.read();
}

HancomWordFilter::~HancomWordFilter()
{
}

bool HancomWordFilter::filter()
{
    return !m_text.isNull();
}

// still not well-formatted
QCString HancomWordFilter::CString() const
{
    QString newstr;

    // Page sizes, margins etc. all in points.

    const unsigned height = 841; // Height.
    const unsigned width = 595;  // Width.
    const unsigned hMargin = 28; // Horizontal margin.
    const unsigned vMargin = 42; // Vertical margin.

    newstr = QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<!DOCTYPE DOC >\n"
        "<DOC editor=\"KWord\" mime=\"application/x-kword\">\n"
        " <PAPER format=\"1\" ptWidth=\"595\" ptHeight=\"841\" mmWidth =\"210\" mmHeight=\"297\" inchWidth =\"8.26772\" inchHeight=\"11.6929\" orientation=\"0\" columns=\"1\" ptColumnspc=\"2\" mmColumnspc=\"1\" inchColumnspc=\"0.0393701\" hType=\"0\" fType=\"0\" ptHeadBody=\"9\" ptFootBody=\"9\" mmHeadBody=\"3.5\" mmFootBody=\"3.5\" inchHeadBody=\"0.137795\" inchFootBody=\"0.137795\">\n"
        "  <PAPERBORDERS mmLeft=\"10\" mmTop=\"15\" mmRight=\"10\" mmBottom=\"15\" ptLeft=\"");
    newstr.append(QString::number(hMargin));
    newstr.append("\" ptTop=\"");
    newstr.append(QString::number(vMargin));
    newstr.append("\" ptRight=\"");
    newstr.append(QString::number(hMargin));
    newstr.append("\" ptBottom=\"");
    newstr.append(QString::number(vMargin));
    newstr.append("\" inchLeft=\"0.393701\" inchTop=\"0.590551\" inchRight=\"0.393701\" inchBottom=\"0.590551\"/>\n"
        " </PAPER>\n"
        " <ATTRIBUTES processing=\"0\" standardpage=\"1\" hasHeader=\"0\" hasFooter=\"0\" unit=\"mm\"/>\n"
        " <FOOTNOTEMGR>\n"
        "  <START value=\"1\"/>\n"
        "  <FORMAT superscript=\"1\" type=\"1\"/>\n"
        "  <FIRSTPARAG ref=\"(null)\"/>\n"
        " </FOOTNOTEMGR>\n"
        " <FRAMESETS>\n");
    newstr.append(
        "  <FRAMESET frameType=\"1\" frameInfo=\"0\" removeable=\"0\" visible=\"1\">\n"
        "   <FRAME left=\"");
    newstr.append(QString::number(hMargin));
    newstr.append("\" top=\"");
    newstr.append(QString::number(vMargin));
    newstr.append("\" right=\"");
    newstr.append(QString::number(width - hMargin));
    newstr.append("\" bottom=\"");
    newstr.append(QString::number(height - vMargin));
    newstr.append("\" runaround=\"1\" runaGapPT=\"2\" runaGapMM=\"1\" runaGapINCH=\"0.0393701\"  lWidth=\"1\" lRed=\"255\" lGreen=\"255\" lBlue=\"255\" lStyle=\"0\"  rWidth=\"1\" rRed=\"255\" rGreen=\"255\" rBlue=\"255\" rStyle=\"0\"  tWidth=\"1\" tRed=\"255\" tGreen=\"255\" tBlue=\"255\" tStyle=\"0\"  bWidth=\"1\" bRed=\"255\" bGreen=\"255\" bBlue=\"255\" bStyle=\"0\" bkRed=\"255\" bkGreen=\"255\" bkBlue=\"255\" bleftpt=\"0\" bleftmm=\"0\" bleftinch=\"0\" brightpt=\"0\" brightmm=\"0\" brightinch=\"0\" btoppt=\"0\" btopmm=\"0\" btopinch=\"0\" bbottompt=\"0\" bbottommm=\"0\" bbottominch=\"0");
    newstr.append("\" autoCreateNewFrame=\"1\" newFrameBehaviour=\"0\"/>\n"
        "   <PARAGRAPH>\n"
        "    <TEXT>");

    newstr.append( XMLEscape( m_text ) );

    newstr.append("</TEXT>\n"
        "   </PARAGRAPH>\n"
        "  </FRAMESET>\n");
    newstr.append(
        " </FRAMESETS>\n"
        "</DOC>\n");

   return newstr.utf8();
}

#include <hancomwordfilter.moc>
