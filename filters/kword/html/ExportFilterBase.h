// $Header$

/*
   This file is part of the KDE project
   Copyright (C) 2001 Nicolas GOUTTE <nicog@snafu.de>

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

#ifndef EXPORTFILTERBASE_H
#define EXPORTFILTERBASE_H

#include <qvaluelist.h>
#include <qstring.h>
#include <qdom.h>

#include <KWEFStructures.h>
#include <KWEFBaseClass.h>

// ClassExportFilterBase is the most important class for the HTML export filter

class ClassExportFilterBase : public KWEFBaseClass
{
    public:
        ClassExportFilterBase(void) {}
        virtual ~ClassExportFilterBase(void) {}
    public: //Non-virtual
    public: //virtual
        virtual QString escapeText(const QString& str) const;
        virtual QString getHtmlOpeningTagExtraAttributes(void) const;
        virtual bool filter(const QString  &filenameIn, const QString  &filenameOut);
        virtual bool isXML(void) const;
        virtual QString getDocType(void) const;
        virtual QString getBodyOpeningTagExtraAttributes(void) const = 0;
        virtual void ProcessParagraphData ( QString &paraText, ValueListFormatData &paraFormatDataList, QString &outputText) = 0;
        virtual QString processDocTagStylesOnly(QDomElement myNode);
        virtual QString getStartOfListOpeningTag(const CounterData::Style typeList, bool& ordered)=0;
        virtual QString getParagraphElement(const QString& strTag, const QString& strParagraphText, LayoutData& layout)=0;
        virtual QString getAfterBodyOpeningTag(void) const;
        virtual QString getBeforeBodyClosingTag(void) const;
        virtual void processStyleTag (QDomNode myNode, void * tagData, QString &strStyles);
    protected:
        virtual void helpStyleProcessing(QDomNode myNode,LayoutData* layout);
};

#endif
