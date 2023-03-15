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

#include <boost/algorithm/string.hpp>

#include <iostream>
#include <exception>
#include <algorithm>
#include <functional>

#include <ctime>
#include <sys/timeb.h>
#include <vector>

#include "frmmain.h"
#include "config.h"

#include "moba/direction.h"
#include "moba/position.h"


FrmMain::FrmMain(EndpointPtr mhp): FrmBase{mhp},
    m_VBox_Toolbox(Gtk::ORIENTATION_VERTICAL, 6), m_Button_New("Neu..."), m_Button_Load("Laden..."),
    m_Button_Delete("Löschen..."), m_Button_Save{"Speichern.."},
    frmSelect(mhp), frmNew(mhp), selectedTrackLayoutId(-1), hasChanged(false) {
    sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &FrmMain::on_timeout), 1);
    sigc::connection conn = Glib::signal_timeout().connect(my_slot, 25); // 25 ms

    set_border_width(10);
    set_size_request(600, 400);
    set_position(Gtk::WIN_POS_CENTER);

    add(m_VBox);

    m_VBox.pack_start(m_VPaned_Container);
    m_VPaned_Container.add1(layoutWidget);

    toolboxWidget.addCallbackHandler(std::bind(&FrmMain::addSymbol, this, std::placeholders::_1));
    toolboxWidget.set_sensitive(false);

    m_VBox_Toolbox.pack_start(toolboxWidget);
    m_VBox_Toolbox.pack_end(m_Button_New, Gtk::PACK_SHRINK);
    m_VBox_Toolbox.pack_end(m_Button_Load, Gtk::PACK_SHRINK);
    m_VBox_Toolbox.pack_end(m_Button_Delete, Gtk::PACK_SHRINK);
    m_VBox_Toolbox.pack_end(m_Button_Save, Gtk::PACK_SHRINK);
    m_VBox_Toolbox.pack_end(infoboxWidget, Gtk::PACK_SHRINK);

    m_VPaned_Container.add2(m_VBox_Toolbox);
    m_VPaned_Container.set_position(450);

    m_VBox.pack_start(m_HBox, Gtk::PACK_SHRINK);

    m_Button_Load.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_loadTracklayout));
    m_Button_Delete.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_deleteTracklayout));
    m_Button_Save.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_saveTracklayout));
    m_Button_New.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_newTracklayout));

    setSensitive(false);
    m_Button_Emergency.set_sensitive(false);
    initAboutDialog();

    registry.registerHandler<GuiSystemNotice>(std::bind(&FrmMain::setSystemNotice, this, std::placeholders::_1));
    registry.registerHandler<SystemHardwareStateChanged>(std::bind(&FrmMain::setHardwareState, this, std::placeholders::_1));
    registry.registerHandler<LayoutGetLayoutsRes>(std::bind(&FrmMain::setTrackLayouts, this, std::placeholders::_1));
    registry.registerHandler<LayoutDeleteLayout>([this](const LayoutDeleteLayout &d) {frmSelect.deleteTracklayout(d.layoutId);});
    registry.registerHandler<LayoutUnlockLayout>([this](const LayoutUnlockLayout &d) {frmSelect.setLockStatus(d.layoutId, false);});
    registry.registerHandler<LayoutLockLayout>([this](const LayoutLockLayout &d) {frmSelect.setLockStatus(d.layoutId, true);});
    registry.registerHandler<LayoutCreateLayout>(std::bind(&FrmMain::setTrackLayout, this, std::placeholders::_1));
    registry.registerHandler<LayoutUpdateLayout>(std::bind(&FrmMain::updateTrackLayout, this, std::placeholders::_1));
    registry.registerHandler<LayoutGetLayoutRes>(std::bind(&FrmMain::setCurrentLayout, this, std::placeholders::_1));
    registry.registerHandler<ClientError>(std::bind(&FrmMain::setErrorNotice, this, std::placeholders::_1));
    show_all_children();
    m_InfoBar.hide();
}

void FrmMain::on_button_loadTracklayout() {
    if(hasChanged) {
        Gtk::MessageDialog dialog(*this, "noch nicht gespeicherte Änderungen", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
        dialog.set_secondary_text("Soll ein anderer Gleisplan ausgewählt werden? Nicht gespeicherte Änderungen gehen dabei verloren!");
        if(dialog.run() == Gtk::RESPONSE_NO) {
            return;
        }
    }

    frmSelect.set_transient_for(*this);
    frmSelect.show(FrmSelect::LOAD);
    frmSelect.present();
}

void FrmMain::on_button_deleteTracklayout() {
    frmSelect.set_transient_for(*this);
    frmSelect.show(FrmSelect::DELETE);
    frmSelect.present();
}

void FrmMain::on_button_saveTracklayout() {
    setHasSaved();
    LayoutSaveLayout l;
    l.specificLayoutData.symbols = layoutWidget.getSymbols();
    l.specificLayoutData.id = selectedTrackLayoutId;
    msgEndpoint->sendMsg(l);
}

void FrmMain::on_button_newTracklayout() {
    if(hasChanged) {
        Gtk::MessageDialog dialog(*this, "noch nicht gespeicherte Änderungen", false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO);
        dialog.set_secondary_text("Soll ein neuer Gleisplan angelegt werden? Nicht gespeicherte Änderungen gehen dabei verloren!");
        if(dialog.run() == Gtk::RESPONSE_NO) {
            return;
        }
    }
    frmNew.set_transient_for(*this);
    frmNew.show();
    frmNew.present();
}

////////////////////////////////////////////////////////////////////////////////
// <editor-fold defaultstate="collapsed" desc="call-back-methodes">
bool FrmMain::on_timeout(int) {
    static bool connected = false;

    try {
        if(!connected) {
            msgEndpoint->connect();
            m_Label_Connectivity_HW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
            m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
            m_Label_Connectivity_SW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
            m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
            msgEndpoint->sendMsg(SystemGetHardwareState{});
            msgEndpoint->sendMsg(LayoutGetLayoutsReq{});
            msgEndpoint->sendMsg(SystemGetHardwareState{});
            setSensitive(true);

            connected = true;
            return true;
        }
        registry.handleMsg(msgEndpoint->recieveMsg());

    } catch(std::exception &e) {
        if(connected) {
            frmSelect.reset();
            m_Button_Emergency.set_sensitive(false);
            m_Label_Connectivity_HW.override_color(Gdk::RGBA("gray"), Gtk::STATE_FLAG_NORMAL);
            m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zum Server");
            m_Label_Connectivity_SW.override_color(Gdk::RGBA("gray"), Gtk::STATE_FLAG_NORMAL);
            m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zum Server");
            m_InfoBar.set_message_type(Gtk::MESSAGE_ERROR);
            std::stringstream ss;
            ss << "<b>msg-handler exception:</b>\n" << e.what();
            m_Label_InfoBarMessage.set_markup(ss.str());
            m_InfoBar.show();
            setSensitive(false);
            connected = false;
        }
    }
    return true;
}

bool FrmMain::on_key_press_event(GdkEventKey* key_event) {
    switch(key_event->keyval) {
        case GDK_KEY_Delete:
            setHasChanged();
            layoutWidget.removeSymbol();
            layoutWidget.setCursorRel(1, 0);
            break;

        case GDK_KEY_BackSpace:
            setHasChanged();
            layoutWidget.removeSymbol();
            layoutWidget.setCursorRel(-1, 0);
            break;

        case GDK_KEY_KP_1:
            layoutWidget.setCursorRel(-1, +1);
            break;

        case GDK_KEY_KP_2:
        case GDK_KEY_Down:
            layoutWidget.setCursorRel(0, +1);
            break;

        case GDK_KEY_KP_3:
            layoutWidget.setCursorRel(+1, +1);
            break;

        case GDK_KEY_KP_4:
        case GDK_KEY_Left:
            layoutWidget.setCursorRel(-1, 0);
            break;

        case GDK_KEY_KP_7:
            layoutWidget.setCursorRel(-1, -1);
            break;

        case GDK_KEY_KP_8:
        case GDK_KEY_Up:
            layoutWidget.setCursorRel(0, -1);
            break;

        case GDK_KEY_KP_9:
            layoutWidget.setCursorRel(+1, -1);
            break;

        case GDK_KEY_KP_6:
        case GDK_KEY_Right:
            layoutWidget.setCursorRel(+1, 0);
            break;

        default:
            return Gtk::Window::on_key_press_event(key_event);

    }
    layoutWidget.refresh();
    return true;
}

void FrmMain::setHardwareState(const SystemHardwareStateChanged &data) {
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::ERROR) {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
        m_Button_Emergency.set_sensitive(false);
        return;
    }
    m_Button_Emergency.set_sensitive(true);
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::EMERGENCY_STOP) {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Nohalt ausgelöst");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Nohalt ausgelöst");
        m_Button_Emergency.set_label("Freigabe");
        return;
    }
    m_Button_Emergency.set_label("Nothalt");
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::STANDBY) {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Energiesparmodus");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Energiesparmodus");
        m_Button_Emergency.set_sensitive(false);
        return;
    }
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::MANUEL) {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("green"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> manuell");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> manuell");
    } else if(data.hardwareState == SystemHardwareStateChanged::HardwareState::AUTOMATIC) {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("green"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("green"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> automatisch");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> automatisch");
    }
}

void FrmMain::setTrackLayouts(const LayoutGetLayoutsRes &data) {
    for(auto iter : data.layouts) {
        frmSelect.addTracklayout(
            iter.id, iter.created, iter.modified, iter.name, iter.locked, iter.active, iter.description
        );
    }
}

void FrmMain::setTrackLayout(const LayoutCreateLayout &data) {
    frmSelect.addTracklayout(
        data.tracklayout.id,
        data.tracklayout.created,
        data.tracklayout.modified,
        data.tracklayout.name,
        data.tracklayout.locked,
        data.tracklayout.active,
        data.tracklayout.description
    );

    if(data.tracklayout.locked != msgEndpoint->getAppId()) {
        return;
    }

    if(selectedTrackLayoutId != -1) {
        msgEndpoint->sendMsg(LayoutUnlockLayout{selectedTrackLayoutId});
    }

    selectedTrackLayoutId = data.tracklayout.id;
    layoutWidget.clear();
    layoutWidget.setActive(true);

    toolboxWidget.set_sensitive(true);
    setHasSaved();
}

void FrmMain::updateTrackLayout(const LayoutUpdateLayout &data) {
    frmSelect.updateTracklayout(
        data.tracklayout.id,
        data.tracklayout.created,
        data.tracklayout.modified,
        data.tracklayout.name,
        data.tracklayout.locked,
        data.tracklayout.active,
        data.tracklayout.description
    );
}

void FrmMain::setCurrentLayout(const LayoutGetLayoutRes &data) {
    frmSelect.setLockStatus(data.specificLayoutData.id, true);
    if(selectedTrackLayoutId != -1) {
        msgEndpoint->sendMsg(LayoutUnlockLayout{selectedTrackLayoutId});
    }

    selectedTrackLayoutId = data.specificLayoutData.id;
    toolboxWidget.set_sensitive(true);
    setHasSaved();
    layoutWidget.setSymbols(data.specificLayoutData.symbols);
}

void FrmMain::setSensitive(bool sensitive) {
    toolboxWidget.set_sensitive(sensitive);
    layoutWidget.set_sensitive(sensitive);
    m_Button_Load.set_sensitive(sensitive);
    m_Button_Delete.set_sensitive(sensitive);
    m_Button_New.set_sensitive(sensitive);
    if(sensitive) {
        m_Button_Save.set_sensitive(selectedTrackLayoutId != -1 && hasChanged);
    } else {
        m_Button_Save.set_sensitive(false);
    }
}

void FrmMain::addSymbol(std::uint8_t symbol) {
    setHasChanged();
    layoutWidget.addSymbol(symbol);

    Symbol s = Symbol{symbol};

    Position pos{s.getNextJunction()};
    layoutWidget.setCursorRel(pos);
}

void FrmMain::setHasChanged() {
    hasChanged = true;
    m_Button_Save.set_sensitive(true);
    set_title(Glib::ustring(PACKAGE_NAME) + " [" + frmSelect.getNameById(selectedTrackLayoutId) + "]*");
}

void FrmMain::setHasSaved() {
    hasChanged = false;
    m_Button_Save.set_sensitive(false);
    set_title(Glib::ustring(PACKAGE_NAME) + " [" + frmSelect.getNameById(selectedTrackLayoutId) + "]");
}
// </editor-fold>
