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

#pragma once

#include <gtkmm/drawingarea.h>
#include <gdkmm/pixbuf.h>

#include <vector>

class LayoutWidget : public Gtk::DrawingArea {
public:
    LayoutWidget();
    virtual ~LayoutWidget() {

    }
    int cursor_x;
    int cursor_y;


    struct Item {
        size_t x;
        size_t y;
        size_t s;
    };

    void addSymbol(size_t x, size_t y, size_t s);

    void maleneu();

protected:
    //Override default signal handler:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;
    std::vector<Item> items;

};