/*
 *  Project:    moba-systemmanager
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

#include <gtkmm.h>

#include <chrono>

#include <moba/msgendpoint.h>
#include <moba/msgsystemhandler.h>
#include <moba/msgclienthandler.h>
#include <moba/jsonabstractitem.h>

#include <gtkmm/window.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/liststore.h>

class FrmMain : public Gtk::Window {
    public:
        FrmMain(moba::MsgEndpointPtr mhp);
        virtual ~FrmMain() {
        }

    protected:
        Gtk::Notebook m_Notebook;
        Gtk::ButtonBox m_ButtonBox;
        Gtk::Button m_Button_Emegerency;
        Gtk::Box m_VBox;
        Gtk::Box m_HBox;
        Gtk::Label m_Label_Connectivity_HW;
        Gtk::Label m_Label_Connectivity_SW;

        // about
        Gtk::Button m_Button_About;
        Gtk::AboutDialog m_Dialog;

        // info-bar
        Gtk::InfoBar m_InfoBar;
        Gtk::Label m_Label_InfoBarMessage;

        void initAboutDialog();

        moba::MsgEndpointPtr   msgEndpoint;
        moba::MsgSystemHandler sysHandler;
        moba::MsgClientHandler cltHandler;

        std::chrono::time_point<std::chrono::system_clock> start;
        int pingctr;

        // Signal handlers:
        bool on_timeout(int timer_number);
        void on_button_about_clicked();
        void on_button_emegency_clicked();
        void on_button_system_shutdown_clicked();
        void on_button_system_reset_clicked();
        void on_button_system_automatic_clicked();
        void on_button_system_standby_clicked();
        void on_button_system_ping_clicked();
        void on_about_dialog_response(int response_id);
        void on_infobar_response(int response);

        // msg-response
        void setServerInfoRes(moba::JsonItemPtr data);
        void setSystemNotice(moba::JsonItemPtr data);
        void setHardwareState(moba::JsonItemPtr data);
};
