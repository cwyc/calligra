/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010-2011 C. Boemann <cbo@boemann.dk>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SIMPLEINSERTWIDGET_H
#define SIMPLEINSERTWIDGET_H

#include <ui_SimpleInsertWidget.h>
#include <KoListStyle.h>

#include <QWidget>
#include <QTextBlock>

class TextTool;
class KoStyleManager;

class SimpleInsertWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleInsertWidget(TextTool *tool, QWidget *parent = 0);

public Q_SLOTS:
    void setStyleManager(KoStyleManager *sm);

Q_SIGNALS:
    void doneWithFocus();
    void insertTableQuick(int, int);

private:
    Ui::SimpleInsertWidget widget;
    KoStyleManager *m_styleManager;
    bool m_blockSignals;
    TextTool *m_tool;
};

#endif
