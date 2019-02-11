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

#include <unordered_map>
#include <utility>

#include "moba/position.h"
#include "moba/symbol.h"

class LayoutWidget : public Gtk::DrawingArea {
    public:
    LayoutWidget();
    virtual ~LayoutWidget() noexcept {

    }

    void clear();

    void setCursorAbs(int x, int y);
    void setCursorRel(int x, int y);

    void setCursorAbs(Position pos) {
        setCursorAbs(pos.x, pos.y);
    }
    void setCursorRel(Position pos) {
        setCursorRel(pos.x, pos.y);
    }

    void addSymbol(size_t x, size_t y, Symbol s, bool suppressRefresh = false);
    void addSymbol(Symbol s);

    void removeSymbol(size_t x, size_t y, bool suppressRefresh = false);
    void removeSymbol();

    void refresh();

protected:
    int cursor_x;
    int cursor_y;

    //Override default signal handler:
    bool on_button_press_event(GdkEventButton * event);
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

    // Thanks to https://stackoverflow.com/a/45395204
    using IntPair = std::pair<int, int>;

    struct IntPairHash {
        static_assert(sizeof(int) * 2 == sizeof(size_t));
        std::size_t operator()(const IntPair &p) const noexcept {
            return size_t(p.first) << 32 | p.second;
        }
    };

    std::unordered_map<IntPair, Symbol, IntPairHash> symbols;

    const int SYMBOL_SIZE = 23;

    std::unordered_map<int, Glib::RefPtr<Gdk::Pixbuf> > images;
    Glib::RefPtr<Gdk::Pixbuf> getImage(size_t i);

};
