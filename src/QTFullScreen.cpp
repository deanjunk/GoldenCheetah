/*
* Copyright (c) 2011 Mark Liversedge (liversedge@gmail.com)
*
* This program is free software; you can redistribute it and/or modify it
* under the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
* more details.
*
* You should have received a copy of the GNU General Public License along
* with this program; if not, write to the Free Software Foundation, Inc., 51
* Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "QTFullScreen.h"

QTFullScreen::QTFullScreen(MainWindow *main) : QObject(main), main(main), isFull(false)
{
    // watch for ESC key being hit when in full screen
    main->installEventFilter(this);
}

bool
QTFullScreen::eventFilter(QObject *obj, QEvent *event)
{
    if (obj != main) return false;

    // F11 toggle full screen
    if (event->type() == QEvent::KeyPress && static_cast<QKeyEvent *>(event)->key() == Qt::Key_F11) {
        toggle();
    }

    // ESC cancels fullscreen
    if (event->type() == QEvent::KeyPress && static_cast<QKeyEvent *>(event)->key() == Qt::Key_Escape) {

        // if in full screen then toggle, otherwise do nothing
        if (isFull) {
            main->showNormal();
            isFull = false;
        }
        return false;
    }
    return false; // always pass thru, just in case
}


void
QTFullScreen::toggle()
{
    if (isFull) {
        main->showNormal();
    } else {
        main->showFullScreen();
    }
    isFull = !isFull;
}
