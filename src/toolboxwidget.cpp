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

#include <cstdint>

#include <cairomm/context.h>
#include <giomm/resource.h>
#include <gdkmm/general.h> // set_source_pixbuf()
#include <glibmm/fileutils.h>
#include <iostream>

#include "toolboxwidget.h"

ToolboxWidget::ToolboxWidget() {
    // Set masks for mouse events
    add_events(Gdk::BUTTON_PRESS_MASK);

    for(auto i : {9, 17, 18, 33, 34, 36, 66, 68, 72, 85, 132, 136, 144, 170}) {
        loadSymbol(i);
    }

    for(auto i : {19, 25, 35, 38, 49, 50, 70, 76, 98, 100, 137, 140, 145, 152, 196, 200}) {
        loadSymbol(i);
    }

    for(auto i : {39, 51, 57, 78, 102, 114, 147, 153, 201, 204,228, 256}) {
        loadSymbol(i);
    }

    for(auto i : {1, 2, 4, 8, 16, 32, 64, 128}) {
        loadSymbol(i);
    }
}

void ToolboxWidget::loadSymbol(int symbol) {
    std::stringstream ss;
    ss << "./resources/" << symbol << ".bmp";
    Element element;

    element.image = Gdk::Pixbuf::create_from_file(ss.str());
    element.symbol = Symbol{static_cast<std::uint8_t>(symbol)};
    symbols.push_back(element);
}

bool ToolboxWidget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {

    int x_offset = 10;
    int y_offset = -10;

    int x = 0;
    int y = 0;
    symbolsPerRow = this->get_width() / SYMBOL_WIDTH;

    for(auto iter = symbols.begin(); iter != symbols.end(); ++iter) {
        if (!(x % symbolsPerRow)) {
           y++;
           x = 0;
        }

        Gdk::Cairo::set_source_pixbuf(cr, iter->image, x_offset + x * SYMBOL_WIDTH, y_offset + y * SYMBOL_WIDTH);
        cr->rectangle(x_offset + x * SYMBOL_WIDTH, y_offset + y * SYMBOL_WIDTH, iter->image->get_width(), iter->image->get_height());
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

bool ToolboxWidget::on_button_press_event(GdkEventButton *event) {
    // Check if the event is a left(1) button click.
    if((event->type != GDK_BUTTON_PRESS) || (event->button != 1)) {
        return true;
    }

    int x_offset = -10;
    int y_offset = 10;

    int x = static_cast<int>((x_offset + event->x) / SYMBOL_WIDTH);
    int y = static_cast<int>((y_offset + event->y) / SYMBOL_WIDTH);
    int s = (y - 1) * symbolsPerRow + x;


    if(symbols.size() <= static_cast<unsigned long>(s)) {
        return true;
    }

    if(callback) {
        callback(symbols[s].symbol);
    }

    return true;
}