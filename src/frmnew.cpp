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

#include "frmnew.h"

#include <string>
#include "moba/layoutmessages.h"

FrmNew::FrmNew(EndpointPtr mhp) : msgEndpoint(mhp) {

    set_title("Gleisplan erstellen");
    set_border_width(10);
    set_size_request(600, 400);
    set_position(Gtk::WIN_POS_CENTER);

    //get_content_area();
    auto m_Button_Action = add_button("Gleisplan erstellen", BUTTON_ID_LOAD);
    auto m_Button_Cancel = add_button("Abbrechen", BUTTON_ID_CANCEL);

    m_Button_Action->signal_clicked().connect(sigc::mem_fun(*this, &FrmNew::on_button_createTracklayout));
    m_Button_Cancel->signal_clicked().connect(sigc::mem_fun(*this, &FrmNew::on_button_cancel));

    initForm();
    show_all_children();
}

FrmNew::~FrmNew() {

}

void FrmNew::initForm() {
    get_content_area()->pack_start(m_VBox);
    m_VBox.pack_start(m_HBox_Name, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_HBox_Description, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_HBox_Active, Gtk::PACK_SHRINK);

    m_HBox_Name.pack_start(m_Label_Name, Gtk::PACK_SHRINK);
    m_HBox_Name.pack_end(m_Entry_Name, Gtk::PACK_SHRINK);

    m_HBox_Description.pack_start(m_Label_Description, Gtk::PACK_SHRINK);
    m_HBox_Description.pack_end(m_Entry_Description, Gtk::PACK_SHRINK);

    m_HBox_Active.pack_start(m_Label_Active, Gtk::PACK_SHRINK);
    m_HBox_Active.pack_end(m_Check_Active, Gtk::PACK_SHRINK);

    m_Label_Name.set_label("Name");
    m_Label_Description.set_label("Beschreibung");
    m_Label_Active.set_label("Aktiver Gleisplan");
}

void FrmNew::on_button_createTracklayout() {
    auto name = m_Entry_Name.get_text();
    auto description = m_Entry_Description.get_text();
    auto active = m_Check_Active.get_active();

    if(name == "") {
        Gtk::MessageDialog dialog(*this, "Gleisplan erstellen", false, Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK);
        dialog.set_secondary_text("Es wurde kein Name angegeben");
        return;
    }
    msgEndpoint->sendMsg(LayoutCreateLayout{name, description, active});
    clearAndHide();
}

void FrmNew::on_button_cancel() {
    clearAndHide();
}

void FrmNew::clearAndHide() {
    m_Entry_Name.set_text("");
    m_Entry_Description.set_text("");
    m_Check_Active.set_active(false);
    hide();
}
