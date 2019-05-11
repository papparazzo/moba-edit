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

namespace {
    const char license[] =
        "Project:    moba-edit\n"
        "\n"
        "Copyright (C) 2018 Stefan Paproth <pappi-@gmx.de>\n"
        "\n"
        "This program is free software: you can redistribute it and/or modify\n"
        "it under the terms of the GNU Affero General Public License as\n"
        "published by the Free Software Foundation, either version 3 of the\n"
        "License, or (at your option) any later version.\n"
        "\n"
        "This program is distributed in the hope that it will be useful,\n"
        "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the\n"
        "GNU Affero General Public License for more details.\n"
        "\n"
        "You should have received a copy of the GNU Affero General Public License\n"
        "along with this program. If not, see <http://www.gnu.org/licenses/agpl.txt>.";
}

FrmMain::FrmMain(EndpointPtr mhp) :
    msgEndpoint(mhp), m_VBox(Gtk::ORIENTATION_VERTICAL, 6), m_HBox(Gtk::ORIENTATION_HORIZONTAL, 6),
    m_VBox_Toolbox(Gtk::ORIENTATION_VERTICAL, 6), m_Button_New("Neu..."), m_Button_Load("Laden..."),
    m_Button_Delete("Löschen..."), m_Button_Save{"Speichern.."},
    m_Label_Connectivity_HW(" \xe2\x96\x84"), m_Label_Connectivity_SW(" \xe2\x96\x84"),
    m_Button_About("About..."), frmSelect(mhp), frmNew(mhp){
    sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &FrmMain::on_timeout), 1);
    sigc::connection conn = Glib::signal_timeout().connect(my_slot, 25); // 25 ms

    set_border_width(10);
    set_size_request(600, 400);
    set_position(Gtk::WIN_POS_CENTER);

    add(m_VBox);

    // Add the message label to the InfoBar:
    auto infoBarContainer = dynamic_cast<Gtk::Container*>(m_InfoBar.get_content_area());
    if(infoBarContainer) {
        infoBarContainer->add(m_Label_InfoBarMessage);
    }

    m_InfoBar.signal_response().connect(sigc::mem_fun(*this, &FrmMain::on_infobar_response));
    m_InfoBar.add_button("_OK", 0);

    m_VBox.pack_start(m_InfoBar, Gtk::PACK_SHRINK);
    m_VBox.pack_start(m_VPaned_Container);
    m_VPaned_Container.add1(layoutWidget);

    toolboxWidget.addCallbackHandler(std::bind(&FrmMain::addSymbol, this, std::placeholders::_1));

    m_VBox_Toolbox.pack_start(toolboxWidget);
    m_VBox_Toolbox.pack_end(m_Button_New, Gtk::PACK_SHRINK);
    m_VBox_Toolbox.pack_end(m_Button_Load, Gtk::PACK_SHRINK);
    m_VBox_Toolbox.pack_end(m_Button_Delete, Gtk::PACK_SHRINK);
    m_VBox_Toolbox.pack_end(m_Button_Save, Gtk::PACK_SHRINK);

    m_VPaned_Container.add2(m_VBox_Toolbox);
    m_VPaned_Container.set_position(450);

    m_VBox.pack_start(m_HBox, Gtk::PACK_SHRINK);
    m_HBox.pack_end(m_ButtonBox, Gtk::PACK_SHRINK);

    m_HBox.pack_start(m_Label_Connectivity_HW, Gtk::PACK_SHRINK);
    m_Label_Connectivity_HW.set_justify(Gtk::JUSTIFY_LEFT);
    m_Label_Connectivity_HW.override_color(Gdk::RGBA("Gray"), Gtk::STATE_FLAG_NORMAL);
    m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> unbekannt");

    m_HBox.pack_start(m_Label_Connectivity_SW, Gtk::PACK_SHRINK);
    m_Label_Connectivity_SW.set_justify(Gtk::JUSTIFY_LEFT);
    m_Label_Connectivity_SW.override_color(Gdk::RGBA("Gray"), Gtk::STATE_FLAG_NORMAL);
    m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> unbekannt");

    // about-dialog
    m_ButtonBox.pack_start(m_Button_About, Gtk::PACK_EXPAND_WIDGET, 5);
    m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);
    m_Button_About.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_about_clicked));

    m_ButtonBox.pack_start(m_Button_Emegerency, Gtk::PACK_EXPAND_WIDGET, 5);
    m_Button_Emegerency.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_emegency_clicked));
    m_Button_Emegerency.set_label("Nothalt");

    m_Button_Load.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_loadTracklayout));
    m_Button_Delete.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_deleteTracklayout));
    m_Button_Save.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_saveTracklayout));
    m_Button_New.signal_clicked().connect(sigc::mem_fun(*this, &FrmMain::on_button_newTracklayout));

    initAboutDialog();

    registry.registerHandler<GuiSystemNotice>(std::bind(&FrmMain::setSystemNotice, this, std::placeholders::_1));
    registry.registerHandler<SystemHardwareStateChanged>(std::bind(&FrmMain::setHardwareState, this, std::placeholders::_1));
    registry.registerHandler<LayoutGetLayoutsRes>(std::bind(&FrmMain::setTrackLayouts, this, std::placeholders::_1));
    registry.registerHandler<LayoutLayoutDeleted>(std::bind(&FrmMain::deleteTrackLayout, this, std::placeholders::_1));
    registry.registerHandler<LayoutLayoutUnlocked>(std::bind(&FrmMain::setLockStateUnlocked, this, std::placeholders::_1));
    registry.registerHandler<LayoutLayoutCreated>(std::bind(&FrmMain::setTrackLayout, this, std::placeholders::_1));
    registry.registerHandler<LayoutGetLayoutRes>(std::bind(&FrmMain::setCurrentLayout, this, std::placeholders::_1));
    registry.registerHandler<ClientError>(std::bind(&FrmMain::displayError, this, std::placeholders::_1));
    show_all_children();
    m_InfoBar.hide();
}

void FrmMain::on_button_loadTracklayout() {
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

}

void FrmMain::on_button_newTracklayout() {
    frmNew.set_transient_for(*this);
    frmNew.show();
    frmNew.present();
}

std::string FrmMain::getDisplayMessage(std::string caption, std::string text) {
    std::replace(caption.begin(), caption.end(), '<', '"');
    std::replace(caption.begin(), caption.end(), '>', '"');
    std::replace(text.begin(), text.end(), '<', '"');
    std::replace(text.begin(), text.end(), '>', '"');

    std::stringstream ss;
    ss << "<b>" << caption << "!</b>\n" << text;
    return std::move(ss.str());
}

void FrmMain::initAboutDialog() {
    m_Dialog.set_transient_for(*this);

    m_Dialog.set_program_name(PACKAGE_NAME);
    m_Dialog.set_version(PACKAGE_VERSION);
    m_Dialog.set_copyright("Stefan Paproth");
    m_Dialog.set_comments("Application for controlling the moba-server.");
    m_Dialog.set_license(license);

    m_Dialog.set_website("<pappi-@gmx.de>");
    m_Dialog.set_website_label("pappi-@gmx.de");

    std::vector<Glib::ustring> list_authors;
    list_authors.push_back("Stefan Paproth");
    m_Dialog.set_authors(list_authors);

    m_Dialog.signal_response().connect(sigc::mem_fun(*this, &FrmMain::on_about_dialog_response));

    m_Button_About.grab_focus();
}

////////////////////////////////////////////////////////////////////////////////
// Call-back-methodes

void FrmMain::on_button_about_clicked() {
    m_Dialog.show();
    m_Dialog.present();
}

void FrmMain::on_button_emegency_clicked() {
    if(m_Button_Emegerency.get_label() == "Nothalt") {
        msgEndpoint->sendMsg(SystemSetEmergencyStop{true});
    } else {
        msgEndpoint->sendMsg(SystemSetEmergencyStop{false});
    }
}

void FrmMain::on_about_dialog_response(int) {
    m_Dialog.hide();
}

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
            connected = true;
            return true;
        }
        registry.handleMsg(msgEndpoint->recieveMsg());

    } catch(std::exception &e) {
        if(connected) {
            frmSelect.reset();
            m_Button_Emegerency.set_sensitive(false);
            m_Label_Connectivity_HW.override_color(Gdk::RGBA("gray"), Gtk::STATE_FLAG_NORMAL);
            m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zum Server");
            m_Label_Connectivity_SW.override_color(Gdk::RGBA("gray"), Gtk::STATE_FLAG_NORMAL);
            m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zum Server");
            m_InfoBar.set_message_type(Gtk::MESSAGE_ERROR);
            std::stringstream ss;
            ss << "<b>msg-handler exception:</b>\n" << e.what();
            m_Label_InfoBarMessage.set_markup(ss.str());
            m_InfoBar.show();
            connected = false;
        }
    }
    return true;
}

void FrmMain::on_infobar_response(int) {
    m_Label_InfoBarMessage.set_text("");
    m_InfoBar.hide();
}

bool FrmMain::on_key_press_event(GdkEventKey* key_event) {
    switch(key_event->keyval) {
        case GDK_KEY_Delete:
            layoutWidget.removeSymbol();
            layoutWidget.setCursorRel(1, 0);
            break;

        case GDK_KEY_BackSpace:
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

void FrmMain::setSystemNotice(const GuiSystemNotice &data) {
    switch(data.noticeType) {
        case GuiSystemNotice::NoticeType::INFO:
            m_InfoBar.set_message_type(Gtk::MESSAGE_INFO);
            break;

        case GuiSystemNotice::NoticeType::WARNING:
            m_InfoBar.set_message_type(Gtk::MESSAGE_WARNING);
            break;

        case GuiSystemNotice::NoticeType::ERROR:
            m_InfoBar.set_message_type(Gtk::MESSAGE_ERROR);
            break;
    }
    m_Label_InfoBarMessage.set_markup(getDisplayMessage(data.caption, data.text));
    m_InfoBar.show();
}

void FrmMain::setHardwareState(const SystemHardwareStateChanged &data) {
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::ERROR) {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
        m_Button_Emegerency.set_sensitive(false);
        return;
    }
    m_Button_Emegerency.set_sensitive(true);
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::EMERGENCY_STOP) {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Nohalt ausgelöst");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Nohalt ausgelöst");
        m_Button_Emegerency.set_label("Freigabe");
        return;
    }
    m_Button_Emegerency.set_label("Nothalt");
    if(data.hardwareState == SystemHardwareStateChanged::HardwareState::STANDBY) {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Energiesparmodus");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Energiesparmodus");
        m_Button_Emegerency.set_sensitive(false);
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

void FrmMain::deleteTrackLayout(const LayoutLayoutDeleted &data) {
    frmSelect.deleteTracklayout(data.layoutId);
}

void FrmMain::setTrackLayout(const LayoutLayoutCreated &data) {
    frmSelect.addTracklayout(
        data.tracklayout.id,
        data.tracklayout.created,
        data.tracklayout.modified,
        data.tracklayout.name,
        data.tracklayout.locked,
        data.tracklayout.active,
        data.tracklayout.description
    );
}

void FrmMain::setLockStateUnlocked(const LayoutLayoutUnlocked &data) {
    frmSelect.setLockStatus(data.layoutId, false);
}

void FrmMain::setCurrentLayout(const LayoutGetLayoutRes &data) {
    for(auto iter : data.layoutData.symbols) {
        layoutWidget.addSymbol(iter.xPos, iter.yPos, iter.symbol);
    }
}

void FrmMain::displayError(const ClientError &data) {
    m_InfoBar.set_message_type(Gtk::MESSAGE_WARNING);
    m_Label_InfoBarMessage.set_markup(getDisplayMessage(data.errorId, data.additionalMsg));
    m_InfoBar.show();
}

void FrmMain::addSymbol(Symbol symbol) {
    layoutWidget.addSymbol(symbol);

    Position pos{symbol.getNextJunktion()};
    layoutWidget.setCursorRel(pos);
}