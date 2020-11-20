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

#include <string>
#include "moba/layouthandler.h"

FrmSelect::FrmSelect(EndpointPtr mhp) : msgEndpoint(mhp) {

    set_title("Gleisplan laden");
    set_border_width(10);
    set_size_request(600, 400);
    set_position(Gtk::WIN_POS_CENTER);

    //get_content_area();
    m_Button_Action = add_button("Gleisplan laden", BUTTON_ID_LOAD);
    auto m_Button_Cancel = add_button("Abbrechen", BUTTON_ID_CANCEL);

    m_Button_Action->set_sensitive(false);

    m_Button_Action->signal_clicked().connect(sigc::mem_fun(*this, &FrmSelect::on_button_loadTracklayout));
    m_Button_Cancel->signal_clicked().connect(sigc::mem_fun(*this, &FrmSelect::on_button_cancel));

    initListbox();
    show_all_children();
}

FrmSelect::~FrmSelect() {

}

void FrmSelect::initListbox() {

    get_content_area()->pack_start(m_VPaned_Tracklayouts);
    m_VPaned_Tracklayouts.set_position(150);
    m_VPaned_Tracklayouts.add1(m_ScrolledWindow_Tracklayouts);

    m_ScrolledWindow_Tracklayouts.add(m_TreeView_Tracklayouts);
    m_ScrolledWindow_Tracklayouts.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    m_refTreeModel_Tracklayouts = Gtk::ListStore::create(m_Columns_Tracklayouts);
    m_TreeView_Tracklayouts.set_model(m_refTreeModel_Tracklayouts);

    m_TreeView_Tracklayouts.append_column("ID",          m_Columns_Tracklayouts.m_col_id);
    m_TreeView_Tracklayouts.append_column("Erstellt am", m_Columns_Tracklayouts.m_col_created);
    m_TreeView_Tracklayouts.append_column("Geändert am", m_Columns_Tracklayouts.m_col_modified);
    m_TreeView_Tracklayouts.append_column("Gesperrt",    m_Columns_Tracklayouts.m_col_locked);
    m_TreeView_Tracklayouts.append_column("Aktiv",       m_Columns_Tracklayouts.m_col_active);
    m_TreeView_Tracklayouts.append_column("Name",        m_Columns_Tracklayouts.m_col_name);

    m_VPaned_Tracklayouts.add2(m_ScrolledWindow_Descripton);

    m_ScrolledWindow_Descripton.add(m_Label_Description);
    m_ScrolledWindow_Descripton.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

    Glib::RefPtr<Gtk::TreeSelection> refTreeSelection = m_TreeView_Tracklayouts.get_selection();
    refTreeSelection->signal_changed().connect(sigc::mem_fun(*this, &FrmSelect::on_selection_changed));
}

void FrmSelect::updateTracklayout(int id, const std::string &created, const std::string &modified, const std::string &name, bool locked, bool active, const std::string &description) {
    if(active) {
        deactivateLayout();
    }

    auto children = m_refTreeModel_Tracklayouts->children();
    for(auto iter = children.begin(); iter != children.end(); ++iter) {
        auto row = *iter;
        if(row[m_Columns_Tracklayouts.m_col_id] != id) {
            continue;
        }
        row[m_Columns_Tracklayouts.m_col_id      ] = id;
        row[m_Columns_Tracklayouts.m_col_created ] = created;
        row[m_Columns_Tracklayouts.m_col_modified] = modified;
        row[m_Columns_Tracklayouts.m_col_name    ] = name;
        row[m_Columns_Tracklayouts.m_col_locked  ] = locked;
        row[m_Columns_Tracklayouts.m_col_active  ] = active;
        row[m_Columns_Tracklayouts.m_col_data    ] = description;
        return;
    }
}

void FrmSelect::deleteTracklayout(int id) {
    auto children = m_refTreeModel_Tracklayouts->children();
    for(auto iter = children.begin(); iter != children.end(); ++iter) {
        if((*iter)[m_Columns_Tracklayouts.m_col_id] != id) {
            continue;
        }
        m_refTreeModel_Tracklayouts->erase(iter);
        return;
    }
}

void FrmSelect::setLockStatus(int id, bool locked) {
    auto children = m_refTreeModel_Tracklayouts->children();
    for(auto iter = children.begin(); iter != children.end(); ++iter) {
        auto row = *iter;
        if(row[m_Columns_Tracklayouts.m_col_id] != id) {
            continue;
        }
        row[m_Columns_Tracklayouts.m_col_locked] = locked;
        return;
    }
}

void FrmSelect::deactivateLayout() {
    auto children = m_refTreeModel_Tracklayouts->children();
    for(auto iter = children.begin(); iter != children.end(); ++iter) {
        auto row = *iter;
        if(!row[m_Columns_Tracklayouts.m_col_active]) {
            continue;
        }
        row[m_Columns_Tracklayouts.m_col_active] = false;
        return;
    }
}

void FrmSelect::reset() {
    m_refTreeModel_Tracklayouts->clear();
}

Glib::ustring FrmSelect::getNameById(int id) {
    auto children = m_refTreeModel_Tracklayouts->children();
    for(auto iter = children.begin(); iter != children.end(); ++iter) {
        auto row = *iter;
        if(row[m_Columns_Tracklayouts.m_col_id] == id) {
            return row[m_Columns_Tracklayouts.m_col_name];
        }
    }
    return Glib::ustring("");
}

void FrmSelect::show(FrmSelect::Mode mode) {
    this->mode = mode;
    if(mode == FrmSelect::DELETE) {
        set_title("Gleisplan löschen");
        m_Button_Action->set_label("Gleisplan löschen");
    } else {
        set_title("Gleisplan laden");
        m_Button_Action->set_label("Gleisplan laden");
    }
    Gtk::Dialog::show();
}

void FrmSelect::addTracklayout(
    int id, const std::string &created, const std::string &modified, const std::string &name, bool locked, bool active, const std::string &description
) {
    if(active) {
        deactivateLayout();
    }

    auto iter = m_refTreeModel_Tracklayouts->append();
    auto row = *iter;
    row[m_Columns_Tracklayouts.m_col_id      ] = id;
    row[m_Columns_Tracklayouts.m_col_created ] = created;
    row[m_Columns_Tracklayouts.m_col_modified] = modified;
    row[m_Columns_Tracklayouts.m_col_name    ] = name;
    row[m_Columns_Tracklayouts.m_col_locked  ] = locked;
    row[m_Columns_Tracklayouts.m_col_active  ] = active;
    row[m_Columns_Tracklayouts.m_col_data    ] = description;
}

void FrmSelect::on_selection_changed() {
    Glib::RefPtr<Gtk::TreeSelection> selection = m_TreeView_Tracklayouts.get_selection();
    Gtk::TreeModel::iterator iter = selection->get_selected();
    if(!iter) {
        m_Button_Action->set_sensitive(false);
        return;
    }
    Gtk::TreeModel::Row row = *iter;
    currentLayout = (int)row[m_Columns_Tracklayouts.m_col_id];
    m_Label_Description.set_text((std::string)row[m_Columns_Tracklayouts.m_col_data]);
    get_widget_for_response(BUTTON_ID_LOAD)->set_sensitive(true);
    if((bool)row[m_Columns_Tracklayouts.m_col_locked]) {
        m_Button_Action->set_sensitive(false);
    } else {
        m_Button_Action->set_sensitive(true);
    }
}

void FrmSelect::on_button_loadTracklayout() {
    if(mode == FrmSelect::LOAD) {
        msgEndpoint->sendMsg(LayoutGetLayoutReq{currentLayout});
        hide();
        return;
    }

    Gtk::MessageDialog dialog(*this, "Gleisplan löschen", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
    dialog.set_secondary_text("Soll der ausgewählte Gleisplan gelöscht werden?");

    if(dialog.run() == Gtk::RESPONSE_YES) {
        msgEndpoint->sendMsg(LayoutDeleteLayout{currentLayout});
        return;
    }
}

void FrmSelect::on_button_cancel() {
    hide();
}
