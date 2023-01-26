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

#include "infoboxwidget.h"
#include <string>

InfoBoxWidget::InfoBoxWidget() {
    set_border_width(2);

    add(m_VBox_ServerDataKey);
    add(m_VBox_ServerDataValue);

    for(int i = 0; i < 4; ++i) {
        m_VBox_ServerDataKey.pack_start(lblName[0][i]);
        m_VBox_ServerDataValue.pack_start(lblName[1][i]);
        lblName[0][i].set_justify(Gtk::JUSTIFY_RIGHT);
        lblName[0][i].set_justify(Gtk::JUSTIFY_LEFT);
    }

    lblName[0][0].set_markup("<b>Symbol:</b>");
    lblName[0][1].set_markup("<b>Id:</b>");
    lblName[0][2].set_markup("<b>X:</b>");
    lblName[0][3].set_markup("<b>Y:</b>");
}

InfoBoxWidget::~InfoBoxWidget() {
}

void InfoBoxWidget::clear() {
   for(auto i = 0; i < 4; ++i) {
       lblName[1][i].set_label("");
   }
}

void InfoBoxWidget::setInfobox(int x, int y, int id, int symbol) {
    lblName[1][0].set_label(std::to_string(symbol));
    lblName[1][1].set_label(std::to_string(id));
    lblName[1][2].set_label(std::to_string(x));
    lblName[1][3].set_label(std::to_string(y));
}
