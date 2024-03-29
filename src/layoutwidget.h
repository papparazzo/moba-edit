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
#include "moba/shared.h"
#include "derivedmessages.h"

class LayoutWidget: public Gtk::DrawingArea {
public:
    LayoutWidget();
    virtual ~LayoutWidget() noexcept {

    }

    void setBlocks(BlockContactDataMapPtr blockContactMap);
    BlockContactDataMapPtr getBlocks();

    void setSymbols(SymbolsPtr symbols);
    SymbolsPtr getSymbols();

    void clear();

    void setCursorAbs(int x, int y);
    void setCursorRel(int x, int y);

    void setCursorAbs(Position pos) {
        setCursorAbs(pos.x, pos.y);
    }
    void setCursorRel(Position pos) {
        setCursorRel(pos.x, pos.y);
    }

    int getCursorX() const {
        return cursor_x;
    }

    int getCursorY() const {
        return cursor_y;
    }
    
    int getCurSymbolId() const {
        return cur_symbolId;
    }
    
    void setActive(bool active) {
        isActive = active;
        refresh();
    }

    void addSymbol(size_t x, size_t y, std::uint8_t s, bool suppressRefresh = false);
    void addSymbol(std::uint8_t s);

    void removeSymbol(size_t x, size_t y, bool suppressRefresh = false);
    void removeSymbol();

    void refresh();

protected:
    SymbolsPtr symbols;
    BlockContactDataMapPtr blockContacts;

    int cursor_x{0};
    int cursor_y{0};
    int cur_symbolId{0};

    //Override default signal handler:
    bool on_button_press_event(GdkEventButton * event);
    bool on_draw(const Cairo::RefPtr<Cairo::Context>& cr) override;

    const int SYMBOL_SIZE = 23;

    std::unordered_map<int, Glib::RefPtr<Gdk::Pixbuf>> images;
    Glib::RefPtr<Gdk::Pixbuf> getImage(size_t i);

    bool isActive;
};
