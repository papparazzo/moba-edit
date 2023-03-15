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

#include "moba/endpoint.h"

#include "moba/guimessages.h"
#include "moba/clientmessages.h"
#include "moba/systemmessages.h"

#include <gtkmm.h>

class FrmBase: public Gtk::Window {
public:
    FrmBase(EndpointPtr mhp);
    virtual ~FrmBase() {
    }

protected:
    EndpointPtr msgEndpoint;

    // about
    Gtk::Button m_Button_About;
    Gtk::AboutDialog m_Dialog;

    // info-bar
    Gtk::InfoBar m_InfoBar;
    Gtk::Label m_Label_InfoBarMessage;

    Gtk::Button    m_Button_Emergency;

    // status label
    Gtk::Label m_Label_Connectivity_HW;
    Gtk::Label m_Label_Connectivity_SW;

    Gtk::ButtonBox m_ButtonBox;

    Gtk::Box       m_VBox;
    Gtk::Box       m_HBox;

    std::string getDisplayMessage(std::string caption, std::string text);

    void setNotice(Gtk::MessageType noticeType, std::string caption, std::string text);

    void initAboutDialog();

    // msg-response
    void setSystemNotice(const GuiSystemNotice &data);
    void setErrorNotice(const ClientError &data);

    // Signal handlers:
    void on_about_dialog_response(int response_id);
    void on_button_about_clicked();
    void on_button_emergency_clicked();
    void on_infobar_response(int response);

};
