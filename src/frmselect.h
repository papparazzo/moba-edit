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

#include "moba/endpoint.h"

#include <gtkmm/window.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/liststore.h>

#include <string>

class FrmSelect : public Gtk::Dialog {
    public:
        enum Mode {
            LOAD,
            DELETE
        };

        FrmSelect(EndpointPtr mhp);
        virtual ~FrmSelect();

        void addTracklayout(
            int id, const std::string &created, const std::string &modified, const std::string &name, bool locked, bool active, const std::string &description
        );

        void updateTracklayout(
            int id, const std::string &created, const std::string &modified, const std::string &name, bool locked, bool active, const std::string &description
        );
        void deleteTracklayout(int id);
        void setLockStatus(int id, bool locked);
        void deactivateLayout();

        void reset();

        Glib::ustring getNameById(int id);
        void show(Mode mode);

    protected:
        const static int BUTTON_ID_LOAD = 1;
        const static int BUTTON_ID_CANCEL = 2;

        Gtk::Paned                   m_VPaned_Tracklayouts{Gtk::ORIENTATION_VERTICAL};
        Gtk::ScrolledWindow          m_ScrolledWindow_Tracklayouts;

        Gtk::TreeView                m_TreeView_Tracklayouts;
        Glib::RefPtr<Gtk::ListStore> m_refTreeModel_Tracklayouts;

        Gtk::Label                   m_Label_Description;
        Gtk::ScrolledWindow          m_ScrolledWindow_Descripton;

        int                          currentLayout;
        Mode                         mode;

        Gtk::Button*                 m_Button_Action;

        class ModelColumnsTracklayouts : public Gtk::TreeModel::ColumnRecord {
            public:
                ModelColumnsTracklayouts() {
                    add(m_col_id);
                    add(m_col_created);
                    add(m_col_modified);
                    add(m_col_name);
                    add(m_col_locked);
                    add(m_col_active);
                    add(m_col_data);
                }

                Gtk::TreeModelColumn<int>           m_col_id;
                Gtk::TreeModelColumn<Glib::ustring> m_col_created;
                Gtk::TreeModelColumn<Glib::ustring> m_col_modified;
                Gtk::TreeModelColumn<Glib::ustring> m_col_name;
                Gtk::TreeModelColumn<bool>          m_col_locked;
                Gtk::TreeModelColumn<bool>          m_col_active;
                Gtk::TreeModelColumn<std::string>   m_col_data;
        };

        ModelColumnsTracklayouts m_Columns_Tracklayouts;

        EndpointPtr msgEndpoint;

        void initListbox();

        void on_selection_changed();
        void on_button_loadTracklayout();
        void on_button_cancel();
};
