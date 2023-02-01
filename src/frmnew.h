/*
 *  Project:    moba-edit
 *
 *  Copyright (C) 2019 Stefan Paproth <pappi-@gmx.de>
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

#include "moba/endpoint.h"

#include <gtkmm/window.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/liststore.h>

#include <string>

class FrmNew: public Gtk::Dialog {
public:
    FrmNew(EndpointPtr mhp);
    virtual ~FrmNew();

    void reset();

protected:
    const static int BUTTON_ID_LOAD = 1;
    const static int BUTTON_ID_CANCEL = 2;

    Gtk::Box m_VBox{Gtk::ORIENTATION_VERTICAL, 6};
    Gtk::Box m_HBox_Name{Gtk::ORIENTATION_HORIZONTAL, 6};
    Gtk::Box m_HBox_Description{Gtk::ORIENTATION_HORIZONTAL, 6};
    Gtk::Box m_HBox_Active{Gtk::ORIENTATION_HORIZONTAL, 6};

    Gtk::Label m_Label_Name;
    Gtk::Entry m_Entry_Name;

    Gtk::Label m_Label_Description;
    Gtk::Entry m_Entry_Description;

    Gtk::Label       m_Label_Active;
    Gtk::CheckButton m_Check_Active;

    EndpointPtr msgEndpoint;

    void initForm();
    void clearAndHide();

    void on_button_createTracklayout();
    void on_button_cancel();
};
