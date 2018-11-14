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
                 // Prellböcke

    for(auto i : {9, 17, 18, 33, 34, 36, 66, 68, 72, 85, 132, 136, 144, 170}) {
        std::stringstream ss;
        ss << "./resources/" << i << ".bmp";
        auto image = Gdk::Pixbuf::create_from_file(ss.str());
        images[0][i] = image;
    }

    for(auto i : {19, 25, 35, 38, 49, 50, 70, 76, 98, 100, 137, 140, 145, 152, 196, 200}) {
        std::stringstream ss;
        ss << "./resources/" << i << ".bmp";
        auto image = Gdk::Pixbuf::create_from_file(ss.str());
        images[1][i] = image;
    }

    for(auto i : {39, 51, 57, 78, 102, 114, 147, 153, 201, 204,228, 256}) {
        std::stringstream ss;
        ss << "./resources/" << i << ".bmp";
        auto image = Gdk::Pixbuf::create_from_file(ss.str());
        images[2][i] = image;
    }

    for(auto i : {1, 2, 4, 8, 16, 32, 64, 128}) {
        std::stringstream ss;
        ss << "./resources/" << i << ".bmp";
        auto image = Gdk::Pixbuf::create_from_file(ss.str());
        images[3][i] = image;
    }
}

bool ToolboxWidget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {

    int x_offset = 10;
    int y_offset = -10;

    int x;
    int y = 0;

    for(auto iter = images.begin(); iter != images.end(); ++iter) {
        x = 0;
        for(auto iter2 = iter->second.begin(); iter2 != iter->second.end(); ++iter2) {
            if (!(x % 4)) {
               y++;
               x = 0;
            }

            auto image = iter2->second;

            Gdk::Cairo::set_source_pixbuf(cr, image, x_offset + x * SYMBOL_WIDTH, y_offset + y * SYMBOL_WIDTH);
            cr->rectangle(x_offset + x * SYMBOL_WIDTH, y_offset + y * SYMBOL_WIDTH, image->get_width(), image->get_height());
            cr->fill();
            x++;
        }
        y++;
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