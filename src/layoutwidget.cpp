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

    for(auto item : items) {
        auto image = getImage(item.s);

        Gdk::Cairo::set_source_pixbuf(cr, image, item.x * SYMBOL_WIDTH, item.y * SYMBOL_WIDTH);
        cr->rectangle(item.x * SYMBOL_WIDTH, item.y * SYMBOL_WIDTH, image->get_width(), image->get_height());
        cr->fill();
    }

    cr->set_line_width(1.0);
    cr->set_source_rgb(0.8, 0.0, 0.0);

    cr->move_to(cursor_x * SYMBOL_WIDTH, cursor_y * SYMBOL_WIDTH);
    cr->line_to(cursor_x * SYMBOL_WIDTH, (cursor_y +1) * SYMBOL_WIDTH);
    cr->line_to((cursor_x +1) * SYMBOL_WIDTH, (cursor_y +1) * SYMBOL_WIDTH);
    cr->line_to((cursor_x +1) * SYMBOL_WIDTH, cursor_y * SYMBOL_WIDTH);
    cr->line_to(cursor_x * SYMBOL_WIDTH, cursor_y * SYMBOL_WIDTH);
    cr->stroke();


    return true;
}

void LayoutWidget::addSymbol(size_t s) {
    addSymbol(cursor_x, cursor_y, s);
}

void LayoutWidget::addSymbol(size_t x, size_t y, size_t s) {
    Item item;
    item.x = x;
    item.y = y;
    item.s = s;
    items.push_back(item);
    maleneu();
}


void LayoutWidget::maleneu() {
    // TODO: Invalidate smaller area
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

Glib::RefPtr<Gdk::Pixbuf> LayoutWidget::getImage(size_t i) {
    auto iter = images.find(i);
    if(iter != images.end()) {
        return iter->second;
    }

    std::stringstream ss;
    ss << "./resources/" << i << ".bmp";
    auto image = Gdk::Pixbuf::create_from_file(ss.str());
    images[i] = image;
    return image;
}

bool LayoutWidget::on_key_press_event(GdkEventKey* key_event) {
    switch(key_event->keyval) {
        case GDK_KEY_KP_1:
        case GDK_KEY_1:
            cursor_x--;

        case GDK_KEY_KP_2:
        case GDK_KEY_2:
            cursor_y++;
            break;

        case GDK_KEY_KP_3:
        case GDK_KEY_3:
            cursor_x++;
            cursor_y++;
            break;

        case GDK_KEY_KP_4:
        case GDK_KEY_4:
            cursor_x--;
            break;

        //case GDK_KEY_KP_5:
        //case GDK_KEY_5:

        case GDK_KEY_KP_7:
        case GDK_KEY_7:
            cursor_x--;

        case GDK_KEY_KP_8:
        case GDK_KEY_8:
            cursor_y--;
            break;

        case GDK_KEY_KP_9:
        case GDK_KEY_9:
            cursor_y--;

        case GDK_KEY_KP_6:
        case GDK_KEY_6:
            cursor_x++;
            break;

        default:
            return false;

    }

    maleneu();
    return true;
    //return Gtk::Window::on_key_press_event(key_event);

}