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
    add_events(Gdk::BUTTON_PRESS_MASK);
}

void LayoutWidget::setSymbols(SymbolsPtr symbols) {
    this->symbols = symbols;
    refresh();
}

void LayoutWidget::clear() {
    symbols->clear();
    refresh();
}

void LayoutWidget::setCursorAbs(int x, int y) {
    if(x < 0) {
        x = 0;
    }
    if(y < 0) {
        y = 0;
    }
    cursor_x = x;
    cursor_y = y;
}

void LayoutWidget::setCursorRel(int x, int y) {
     setCursorAbs(cursor_x + x, cursor_y + y);
}

bool LayoutWidget::on_draw(const Cairo::RefPtr<Cairo::Context>& cr) {
    cr->set_source_rgb(0.0, 0.0, 0.0);
    cr->paint();

    for(const auto& symbol : *symbols) {
        auto image = getImage(static_cast<int>(symbol.second.getType()));

        Gdk::Cairo::set_source_pixbuf(cr, image, symbol.first.first * SYMBOL_SIZE, symbol.first.second * SYMBOL_SIZE);
        cr->rectangle(symbol.first.first * SYMBOL_SIZE, symbol.first.second * SYMBOL_SIZE, image->get_width(), image->get_height());
        cr->fill();
    }

    cr->set_line_width(1.0);
    cr->set_source_rgb(0.8, 0.0, 0.0);

    cr->move_to(cursor_x * SYMBOL_SIZE, cursor_y * SYMBOL_SIZE);
    cr->line_to(cursor_x * SYMBOL_SIZE, (cursor_y +1) * SYMBOL_SIZE);
    cr->line_to((cursor_x +1) * SYMBOL_SIZE, (cursor_y +1) * SYMBOL_SIZE);
    cr->line_to((cursor_x +1) * SYMBOL_SIZE, cursor_y * SYMBOL_SIZE);
    cr->line_to(cursor_x * SYMBOL_SIZE, cursor_y * SYMBOL_SIZE);
    cr->stroke();

    return true;
}

void LayoutWidget::addSymbol(size_t x, size_t y, Symbol s, bool suppressRefresh) {
    (*symbols)[{x, y}] = std::move(s);
    if(!suppressRefresh) {
        refresh();
    }
}

void LayoutWidget::addSymbol(Symbol s) {
    addSymbol(cursor_x, cursor_y, s);
}

void LayoutWidget::removeSymbol(size_t x, size_t y, bool suppressRefresh) {
    symbols->erase({x, y});
    if(!suppressRefresh) {
        refresh();
    }
}

void LayoutWidget::removeSymbol() {
    removeSymbol(cursor_x, cursor_y);
}

void LayoutWidget::refresh() {
    auto win = get_window();
    if(!win) {
        return;
    }
    Gdk::Rectangle r(0, 0, get_allocation().get_width(), get_allocation().get_height());
    win->invalidate_rect(r, false);
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

bool LayoutWidget::on_button_press_event(GdkEventButton *event)
{
    // Check if the event is a left(1) button click.
    if((event->type == GDK_BUTTON_PRESS) && (event->button == 1)) {
        setCursorAbs(
            static_cast<int>(event->x / SYMBOL_SIZE),
            static_cast<int>(event->y / SYMBOL_SIZE)
        );
        refresh();
    }
    // The event has been handled.
    return true;
}
