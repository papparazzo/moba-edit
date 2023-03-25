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

#include "moba/layoutmessages.h"


#include "frmbase.h"

class FrmMain: public FrmBase {
public:
    FrmMain(EndpointPtr mhp);
    virtual ~FrmMain() {
    }

    void addSymbol(std::uint8_t symbol);

protected:

    LayoutWidget  layoutWidget;
    ToolboxWidget toolboxWidget;
    InfoBoxWidget infoboxWidget;

    Gtk::Box    m_VBox_Toolbox;
    Gtk::Button m_Button_New{"Neu..."};
    Gtk::Button m_Button_Load{"Laden..."};
    Gtk::Button m_Button_Delete{"Löschen..."};
    Gtk::Button m_Button_Save{"Speichern.."};

    // load tracklayout
    FrmSelect frmSelect;

    // create new one
    FrmNew frmNew;

    int selectedTrackLayoutId;
    bool hasChanged;

    Gtk::Paned m_VPaned_Container{Gtk::ORIENTATION_HORIZONTAL};

    // Signal handlers:
    void on_button_loadTracklayout();
    void on_button_deleteTracklayout();
    void on_button_saveTracklayout();
    void on_button_newTracklayout();

    bool on_key_press_event(GdkEventKey *key_event);

    // msg-response
    void setTrackLayouts(const LayoutGetLayoutsRes &data);
    void setTrackLayout(const LayoutCreateLayout &data);
    void updateTrackLayout(const LayoutUpdateLayout &data);
    void setCurrentLayout(const LayoutGetLayoutRes &data);
    void setSensitive(bool sensitive);
    void setBlockList(const ControlGetBlockListRes &data);
    void setHasChanged();
    void setHasSaved();
    void initialSend();
};
