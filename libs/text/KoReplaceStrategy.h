/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2008 Thorsten Zachmann <zachmann@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KOREPLACESTRATEGY_H
#define KOREPLACESTRATEGY_H

#include "KoFindStrategyBase.h"

class QWidget;
class KReplaceDialog;

/**
 * Strategy used for implementing replace
 */
class KoReplaceStrategy : public KoFindStrategyBase
{
public:
    explicit KoReplaceStrategy(QWidget *parent);
    ~KoReplaceStrategy() override;

    /// reimplmented
    KFindDialog *dialog() const override;

    /// reimplmented
    void reset() override;

    /// reimplmented
    void displayFinalDialog() override;

    /// reimplmented
    bool foundMatch(QTextCursor &cursor, FindDirection *findDirection) override;

private:
    KReplaceDialog *m_dialog;
    int m_replaced;
};

#endif /* KOREPLACESTRATEGY_H */
