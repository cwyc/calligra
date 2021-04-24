/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2010 C. Boemann <cbo@boemann.dk>
 * SPDX-FileCopyrightText: 2011 Gopalakrishna Bhat A <gopalakbhat@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */
#ifndef SIMPLETABLEOFCONTENTSWIDGET_H
#define SIMPLETABLEOFCONTENTSWIDGET_H

#include <ui_SimpleTableOfContentsWidget.h>

#include <QWidget>
#include <QTextBlock>
#include <QList>

class ReferencesTool;
class KoStyleManager;
class KoTableOfContentsGeneratorInfo;
class TableOfContentsPreview;
class TableOfContentsTemplate;

class SimpleTableOfContentsWidget : public QWidget
{
    Q_OBJECT
public:
    explicit SimpleTableOfContentsWidget(ReferencesTool *tool, QWidget *parent = 0);
    ~SimpleTableOfContentsWidget() override;

public Q_SLOTS:
    void setStyleManager(KoStyleManager *sm);
    void prepareTemplateMenu();
    void pixmapReady(int templateId);

Q_SIGNALS:
    void doneWithFocus();

private Q_SLOTS:
    void applyTemplate(int templateId);
    void insertCustomToC();
    
private:
    Ui::SimpleTableOfContentsWidget widget;
    KoStyleManager *m_styleManager;
    bool m_blockSignals;
    QTextBlock m_currentBlock;
    QList<KoTableOfContentsGeneratorInfo *> m_templateList;
    //each template in the template list will have have a previewGenerator that will be deleted after preview is generated
    QList<TableOfContentsPreview *> m_previewGenerator;
    ItemChooserAction *m_chooser;
    ReferencesTool *m_referenceTool;
    TableOfContentsTemplate *m_templateGenerator;
};

#endif
