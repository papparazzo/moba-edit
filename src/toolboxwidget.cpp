/*
 *  Project:    moba-edit
 *
 *  Copyright (C) 2018 Stefan Paproth <pappi-@gmx.de>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as
 *  published by the Free Software Foundation, either version 3 of the
 *  License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/agpl.txt>.
 *
 */

#include <cairomm/context.h>
#include <giomm/resource.h>
#include <gdkmm/general.h> // set_source_pixbuf()
#include <glibmm/fileutils.h>
#include <iostream>

#include "toolboxwidget.h"

ToolboxWidget::ToolboxWidget() {
    // Set masks for mouse events
    add_events(Gdk::BUTTON_PRESS_MASK);

    for(auto i : {9, 17, 18, 33, 34, 36, 66, 68, 72, 132, 136, 144}) {
        std::stringstream ss;
        ss << "./resources/" << i << ".bmp";
        auto image = Gdk::Pixbuf::create_from_file(ss.str());
        images[i] = image;
    }
}

bool ToolboxWidget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {

    int x = 1;

    for(auto iter = images.begin(); iter != images.end(); ++iter) {
        auto image = iter->second;
        Gdk::Cairo::set_source_pixbuf(cr, image, x * SYMBOL_WIDTH, 1 * SYMBOL_WIDTH);
        cr->rectangle(x * SYMBOL_WIDTH, 1 * SYMBOL_WIDTH, image->get_width(), image->get_height());
        cr->fill();
        x++;
    }

    return true;
}

void ToolboxWidget::refresh() {
    auto win = get_window();
    if(!win) {
        return;
    }
    Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
    win->invalidate_rect(r, false);
}

bool ToolboxWidget::on_button_press_event(GdkEventButton *event)
{
    // Check if the event is a left(1) button click.
    if((event->type == GDK_BUTTON_PRESS) && (event->button == 1)) {
        std::cout <<
            " x: " << event->x << " xr: " << event->x_root <<
            " y: " << event->y << " yr: " << event->y_root << std::endl;
    }
    // The event has been handled.
    return true;
}