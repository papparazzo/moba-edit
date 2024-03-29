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

#pragma once

#include <vector>

#include <gtkmm/drawingarea.h>
#include <gdkmm/pixbuf.h>

#include "moba/symbol.h"

class ToolboxWidget: public Gtk::DrawingArea {
public:
    ToolboxWidget();

    virtual ~ToolboxWidget() {

    }

    void addCallbackHandler(std::function<void(std::uint8_t)> callback) {
        this->callback = callback;
    }

private:

    void refresh();
    //Override default signal handler:
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

    const int SYMBOL_WIDTH = 30;

    struct Element {
        Glib::RefPtr<Gdk::Pixbuf> image;
        Symbol symbol;
    };

    std::vector<Element> symbols;

    int symbolsPerRow;

    // Override mouse events
    bool on_button_press_event(GdkEventButton * event);

    void loadSymbol(int symbol);

    std::function<void(std::uint8_t)> callback;
};

