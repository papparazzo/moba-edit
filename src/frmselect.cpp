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

#include "frmselect.h"

FrmSelect::FrmSelect() {

    set_title("Gleisplan laden");
    set_border_width(10);
    set_size_request(600, 400);
    set_position(Gtk::WIN_POS_CENTER);


    //get_content_area();
    add_button("Gleisplan laden", BUTTON_ID_LOAD)->set_sensitive(false);
    add_button("Abbrechen", BUTTON_ID_CANCEL);




    /*
    add(m_VBox);

    m_VBox.pack_end(m_ButtonBox, Gtk::PACK_SHRINK);

    // Gleisplan laden
    m_ButtonBox.pack_start(m_Button_Load, Gtk::PACK_EXPAND_WIDGET, 5);
    //m_Button_Load.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_emegency_clicked));
    m_Button_Load.set_label();
    m_Button_Load.set_sensitive(false);

    // Abbrechen
    m_ButtonBox.pack_start(m_Button_Cancel, Gtk::PACK_EXPAND_WIDGET, 5);
    m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);

    m_Button_Cancel.set_label("Abbrechen");
    //m_Button_Cancel.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_about_clicked));
*/
}



    /*
void FrmSelect::initLayoutSelect() {

    m_HPaned.add2(m_VPaned_Incomming);
    m_VPaned_Incomming.set_position(150);
    m_VPaned_Incomming.add1(m_VBox_Incomming);
    m_VBox_Incomming.pack_start(m_ScrolledWindow_Incomming);
    m_ScrolledWindow_Incomming.add(m_TreeView_Incomming);
    m_ScrolledWindow_Incomming.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    m_refTreeModel_Incomming = Gtk::ListStore::create(m_Columns_Incomming);
    m_TreeView_Incomming.set_model(m_refTreeModel_Incomming);

    m_TreeView_Incomming.append_column("Timestamp", m_Columns_Incomming.m_col_timestamp);
    m_TreeView_Incomming.append_column("ID",        m_Columns_Incomming.m_col_id);
    m_TreeView_Incomming.append_column("Name",      m_Columns_Incomming.m_col_name);

    m_VPaned_Incomming.add2(m_ScrolledWindow_Data);

    m_ScrolledWindow_Data.add(m_Label_Data);
    m_ScrolledWindow_Data.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    m_VBox_Incomming.pack_end(m_HBox_CheckRow, Gtk::PACK_SHRINK);
    m_HBox_CheckRow.pack_start(m_Button_AutoCheckLast, Gtk::PACK_SHRINK);
    m_HBox_CheckRow.pack_end(m_ButtonBox_Incomming, Gtk::PACK_SHRINK);
    m_Button_AutoCheckLast.set_label("letzten Eintrag markieren");

    m_ButtonBox_Incomming.pack_start(m_Button_ClearIncomming, Gtk::PACK_EXPAND_WIDGET, 5);
    m_ButtonBox_Incomming.set_layout(Gtk::BUTTONBOX_END);

    Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_TreeView_Incomming.get_selection();
    refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &FrmMain::on_selection_changed_incomming));
    m_Button_ClearIncomming.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_clear_incomming_clicked));

}
     *      */