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

#include <gtkmm.h>

#include <moba/msgendpoint.h>
#include <moba/msgsystemhandler.h>

#include <gtkmm/window.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/liststore.h>

class FrmSelect : public Gtk::Dialog {
    public:
        FrmSelect();
        virtual ~FrmSelect() {

        }

    protected:
        const static int BUTTON_ID_LOAD = 1;
        const static int BUTTON_ID_CANCEL = 2;



/*

        Gtk::Paned                   m_VPaned_Incomming{Gtk::ORIENTATION_VERTICAL};
        Gtk::ScrolledWindow          m_ScrolledWindow_Incomming;
        Gtk::TreeView                m_TreeView_Incomming;
        Glib::RefPtr<Gtk::ListStore> m_refTreeModel_Incomming;

        void initLayoutSelect();

        class ModelColumnsIncomming : public Gtk::TreeModel::ColumnRecord {
            public:
                ModelColumnsIncomming() {
                    add(m_col_id);
                    add(m_col_timestamp);
                    add(m_col_name);
                    add(m_col_data);
                }

                Gtk::TreeModelColumn<int>           m_col_id;
                Gtk::TreeModelColumn<Glib::ustring> m_col_timestamp;
                Gtk::TreeModelColumn<Glib::ustring> m_col_name;
                Gtk::TreeModelColumn<std::string>   m_col_data;
        };
 */
};

