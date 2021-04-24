/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "FindDirection_p.h"

#include <QTextCursor>
#include <KoCanvasResourceManager.h>

#include "KoText.h"
#include "KoFind_p.h"

FindDirection::FindDirection(KoCanvasResourceManager *provider)
        : m_provider(provider)
{
}

FindDirection::~FindDirection()
{
}

FindForward::FindForward(KoCanvasResourceManager *provider)
        : FindDirection(provider)
{
}

FindForward::~FindForward()
{
}

bool FindForward::positionReached(const QTextCursor &currentPos, const QTextCursor &endPos)
{
    return currentPos > endPos;
}

void FindForward::positionCursor(QTextCursor &currentPos)
{
    currentPos.movePosition(QTextCursor::Start);
}

void FindForward::select(const QTextCursor &cursor)
{
    m_provider->setResource(KoText::CurrentTextPosition, cursor.position());
    m_provider->setResource(KoText::CurrentTextAnchor, cursor.anchor());
}

void FindForward::nextDocument(QTextDocument *document, KoFindPrivate *findPrivate)
{
    findPrivate->findDocumentSetNext(document);
}

FindBackward::FindBackward(KoCanvasResourceManager *provider)
        : FindDirection(provider)
{
}

FindBackward::~FindBackward()
{
}

bool FindBackward::positionReached(const QTextCursor &currentPos, const QTextCursor &endPos)
{
    return currentPos < endPos;
}

void FindBackward::positionCursor(QTextCursor &currentPos)
{
    currentPos.movePosition(QTextCursor::End);
}

void FindBackward::select(const QTextCursor &cursor)
{
    m_provider->setResource(KoText::CurrentTextPosition, cursor.anchor());
    m_provider->setResource(KoText::CurrentTextAnchor, cursor.position());
}

void FindBackward::nextDocument(QTextDocument *document, KoFindPrivate *findPrivate)
{
    findPrivate->findDocumentSetPrevious(document);
}

