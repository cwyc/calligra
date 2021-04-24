/* This file is part of the KDE project
 * SPDX-FileCopyrightText: 2014 Dan Leinir Turthra Jensen <admin@leinir.dk>
 *
 *  SPDX-License-Identifier: GPL-2.0-or-later
 */

import QtQuick 2.11
import QtQuick.Controls 2.2 as QtControls
import org.kde.kirigami 2.7 as Kirigami
import org.calligra 1.0
import "dropbox" as Dropbox

Kirigami.OverlaySheet {
    id: component
    property string serviceName: "";
    header: Kirigami.Heading {
        text: "Add DropBox Account"
    }
    Dropbox.SetupPage {
    }
} 
