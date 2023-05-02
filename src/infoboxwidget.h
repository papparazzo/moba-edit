/*
 *  Project:    moba-edit
 *
 *  Copyright (C) 2023 Stefan Paproth <pappi-@gmx.de>
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

#include <gtkmm.h>

class InfoBoxWidget: public Gtk::Box {
public:
    InfoBoxWidget();
    InfoBoxWidget(const InfoBoxWidget&) = delete;
    InfoBoxWidget& operator=(const InfoBoxWidget&) = delete;

    virtual ~InfoBoxWidget();

    void setInfobox(int x, int y, int id, int symbol);
    void clear();

private:
    Gtk::Label lblName[2][4];
    Gtk::Box  m_VBox_ServerDataKey{Gtk::ORIENTATION_VERTICAL, 6};
    Gtk::Box  m_VBox_ServerDataValue{Gtk::ORIENTATION_VERTICAL, 6};

    void setLabel(int pos, int value);
};






