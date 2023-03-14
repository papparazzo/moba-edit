/*
 *  Project:    moba-edit
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

#include "moba/registry.h"
#include "moba/guimessages.h"
#include "moba/systemmessages.h"

#include <gtkmm/window.h>
#include <gtkmm/comboboxtext.h>
#include <gtkmm/liststore.h>

#include "layoutwidget.h"
#include "toolboxwidget.h"
#include "frmselect.h"
#include "frmnew.h"
#include "infoboxwidget.h"
#include "derivedmessages.h"

#include "moba/symbol.h"
#include "moba/endpoint.h"
#include "moba/layoutmessages.h"
#include "moba/clientmessages.h"

class FrmMain: public Gtk::Window {
public:
    FrmMain(EndpointPtr mhp);
    virtual ~FrmMain() {
    }

    void addSymbol(std::uint8_t symbol);

protected:
    EndpointPtr msgEndpoint;
    Registry registry;

    LayoutWidget  layoutWidget;
    ToolboxWidget toolboxWidget;
    InfoBoxWidget infoboxWidget;

    Gtk::ButtonBox m_ButtonBox;
    Gtk::Button    m_Button_Emergency;
    Gtk::Box       m_VBox;
    Gtk::Box       m_HBox;

    Gtk::Box    m_VBox_Toolbox;
    Gtk::Button m_Button_New;
    Gtk::Button m_Button_Load;
    Gtk::Button m_Button_Delete;
    Gtk::Button m_Button_Save;

    Gtk::Label m_Label_Connectivity_HW;
    Gtk::Label m_Label_Connectivity_SW;

    // about
    Gtk::Button m_Button_About;
    Gtk::AboutDialog m_Dialog;

    // info-bar
    Gtk::InfoBar m_InfoBar;
    Gtk::Label m_Label_InfoBarMessage;

    // load tracklayout
    FrmSelect frmSelect;

    // create new one
    FrmNew frmNew;

    int selectedTrackLayoutId;
    bool hasChanged;

    Gtk::Paned m_VPaned_Container{Gtk::ORIENTATION_HORIZONTAL};

    std::string getDisplayMessage(std::string caption, std::string text);

    void setNotice(Gtk::MessageType noticeType, std::string caption, std::string text);

    void initAboutDialog();

    // Signal handlers:
    bool on_timeout(int timer_number);
    void on_button_about_clicked();
    void on_button_emergency_clicked();
    void on_about_dialog_response(int response_id);
    void on_infobar_response(int response);

    void on_button_loadTracklayout();
    void on_button_deleteTracklayout();
    void on_button_saveTracklayout();
    void on_button_newTracklayout();

    bool on_key_press_event(GdkEventKey *key_event);

    // msg-response
    void setSystemNotice(const GuiSystemNotice &data);
    void setErrorNotice(const ClientError &data);
    void setHardwareState(const SystemHardwareStateChanged &data);
    void setTrackLayouts(const LayoutGetLayoutsRes &data);
    void setTrackLayout(const LayoutCreateLayout &data);
    void updateTrackLayout(const LayoutUpdateLayout &data);
    void setCurrentLayout(const LayoutGetLayoutRes &data);
    void setSensitive(bool sensitive);
    void setBlockList(const ControlGetBlockListRes &data);
    void setHasChanged();
    void setHasSaved();
};
