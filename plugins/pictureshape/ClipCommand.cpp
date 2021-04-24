/* This file is part of the KDE project
   SPDX-FileCopyrightText: 2009 Thorsten Zachmann <zachmann@kde.org>

   SPDX-License-Identifier: LGPL-2.0-or-later
*/
#include "ClipCommand.h"

#include "PictureDebug.h"

#include <klocalizedstring.h>

ClipCommand::ClipCommand(PictureShape *shape, bool clip)
    : KUndo2Command(0)
    , m_pictureShape(shape)
    , m_clip(clip)
{
    if (clip) {
        setText(kundo2_i18n("Contour image (by image analysis)"));
    } else {
        setText(kundo2_i18n("Remove image contour"));
    }
}

ClipCommand::~ClipCommand()
{
}

void ClipCommand::redo()
{
    if (m_clip) {
        m_pictureShape->setClipPath(m_pictureShape->generateClipPath());
    } else {
        m_pictureShape->setClipPath(0);
    }
    m_pictureShape->update();
}

void ClipCommand::undo()
{
    if (m_clip) {
        m_pictureShape->setClipPath(0);
    } else {
        m_pictureShape->setClipPath(m_pictureShape->generateClipPath());
    }
    m_pictureShape->update();
}
