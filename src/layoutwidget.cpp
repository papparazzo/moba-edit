/*
 *  Project:    moba-edit
 *
 *  Copyright (C) 2016 Stefan Paproth <pappi-@gmx.de>
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

#include "layoutwidget.h"

LayoutWidget::LayoutWidget() : cursor_x{0}, cursor_y{0} {


}

bool LayoutWidget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {

    Glib::RefPtr<Gdk::Pixbuf> image = Gdk::Pixbuf::create_from_file("./resources/68.bmp");

    // Draw the image at 110, 90, except for the outermost 10 pixels.

    for (int i = 0; i < 10; ++i) {
        Gdk::Cairo::set_source_pixbuf(cr, image, (cursor_x + i) * 23, cursor_y * 23);
        cr->rectangle((cursor_x + i) * 23, cursor_y * 23, image->get_width(), image->get_height());
        cr->fill();

    }

    cr->set_line_width(1.0);
    cr->set_source_rgb(0.8, 0.0, 0.0);

    cr->move_to(cursor_x * 23, cursor_y * 23);
    cr->line_to(cursor_x * 23, (cursor_y +1) * 23);
    cr->line_to((cursor_x +1) * 23, (cursor_y +1) * 23);
    cr->line_to((cursor_x +1) * 23, cursor_y * 23);
    cr->line_to(cursor_x * 23, cursor_y * 23);
    cr->stroke();


    return true;
}

void LayoutWidget::addSymbol(size_t x, size_t y, size_t s) {
    Item item;
    item.x = x;
    item.y = y;
    item.s = s;
    items.push_back(item);
}


void LayoutWidget::maleneu() {
    auto win = get_window();
    if(win) {
        Gdk::Rectangle r(
            0,
            0,
            get_allocation().get_width(),
            get_allocation().get_height()
        );
        win->invalidate_rect(r, false);
    }
}