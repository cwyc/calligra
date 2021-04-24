/* This file is part of the KDE project
* SPDX-FileCopyrightText: 2011 Paul Mendez <paulestebanms@gmail.com>
*
* SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TestRenameCustomSlideShowCommand.h"

#include "KPrDocument.h"
#include "KoPAMasterPage.h"
#include "KoPAPage.h"
#include "PAMock.h"
#include "commands/KPrRenameCustomSlideShowCommand.h"
#include "KPrCustomSlideShows.h"
#include "KPrCustomSlideShowsModel.h"

#include <QTest>

void TestRenameCustomSlideShowCommand::renameCustomShow()
{
    MockDocument doc;

    KoPAMasterPage *master1 = new KoPAMasterPage();
    doc.insertPage(master1, 0);

    KoPAPage *page1 = new KoPAPage(master1);
    doc.insertPage(page1, 0);

    KoPAPage *p1 = dynamic_cast<KoPAPage *>(doc.pageByIndex(0, false));
    KoPAMasterPage * m1 = dynamic_cast<KoPAMasterPage *>(doc.pageByIndex(0, true));

    QVERIFY(p1 != 0);
    QVERIFY(m1 != 0);

    KoPAPage *page2 = new KoPAPage(master1);
    doc.insertPage(page2, 0);

    KoPAPage *page3 = new KoPAPage(master1);
    doc.insertPage(page3, 0);

    QList<KoPAPageBase*> slideList;

    slideList.append(page1);
    slideList.append(page2);
    slideList.append(page3);

    QString customShowName = "test 1";
    QString customShowNewName = "new test 1";

    KPrCustomSlideShowsModel model(&doc, 0);

    doc.customSlideShows()->insert(customShowName, slideList);

    QCOMPARE(doc.customSlideShows()->names().count(), 1);

    KPrRenameCustomSlideShowCommand cmd(&doc, &model, customShowName, customShowNewName);

    cmd.redo();
    QVERIFY(!doc.customSlideShows()->names().contains(customShowName));
    QVERIFY(doc.customSlideShows()->names().contains(customShowNewName));

    cmd.undo();
    QVERIFY(doc.customSlideShows()->names().contains(customShowName));
    QVERIFY(!doc.customSlideShows()->names().contains(customShowNewName));
}

QTEST_MAIN(TestRenameCustomSlideShowCommand)
