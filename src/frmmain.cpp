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

#include <iostream>
#include <exception>
#include <algorithm>

#include <ctime>
#include <sys/timeb.h>

#include "frmmain.h"
#include "config.h"

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

FrmMain::FrmMain(moba::MsgEndpointPtr mhp) :
    msgEndpoint(mhp), sysHandler(mhp), cltHandler(mhp), m_VBox(Gtk::ORIENTATION_VERTICAL, 6),
    m_Button_About("About..."), m_HBox(Gtk::ORIENTATION_HORIZONTAL, 6),
    m_Label_Connectivity_HW(" \xe2\x96\x84"), m_Label_Connectivity_SW(" \xe2\x96\x84")
{
    sigc::slot<bool> my_slot = sigc::bind(sigc::mem_fun(*this, &FrmMain::on_timeout), 1);
    sigc::connection conn = Glib::signal_timeout().connect(my_slot, 25); // 25 ms

    set_title(PACKAGE_NAME);

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



    m_VBox.pack_start(widget);
    widget.show();
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

    initAboutDialog();

    // in the class constructor
    signal_key_press_event().connect(sigc::mem_fun(*this, &FrmMain::on_key_press_event));
    // We override the default event signal handler.
    add_events(Gdk::KEY_PRESS_MASK);

    sysHandler.sendGetHardwareState();
    show_all_children();
    m_InfoBar.hide();
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
        sysHandler.sendSetEmergencyStop(true);
    } else {
        sysHandler.sendSetEmergencyStop(false);
    }
}

void FrmMain::on_about_dialog_response(int) {
    m_Dialog.hide();
}

bool FrmMain::on_timeout(int) {
    static bool connected = true;
    moba::MessagePtr msg;

    try {
        if(!connected) {
            msgEndpoint->connect();
            m_Label_Connectivity_HW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
            m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
            m_Label_Connectivity_SW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
            m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
            sysHandler.sendGetHardwareState();
            connected = true;
            return true;
        }
        msg = msgEndpoint->recieveMsg();
    } catch(std::exception &e) {
        if(connected) {
            m_Label_Connectivity_HW.override_color(Gdk::RGBA("gray"), Gtk::STATE_FLAG_NORMAL);
            m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zum Server");
            m_Label_Connectivity_SW.override_color(Gdk::RGBA("gray"), Gtk::STATE_FLAG_NORMAL);
            m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zum Server");
            Gtk::MessageDialog dialog(
                *this,
                "msg-handler exception:",
                false,
                Gtk::MESSAGE_ERROR,
                Gtk::BUTTONS_OK
            );
            dialog.set_secondary_text(e.what());
            dialog.run();
            connected = false;
        }
        return true;
    }

    if(!msg) {
        return true;
    }

    switch(msg->getMsgType()) {
        case moba::Message::MT_SYSTEM_NOTICE:
            setSystemNotice(msg->getData());
            break;

        case moba::Message::MT_HARDWARE_STATE_CHANGED:
            setHardwareState(msg->getData());
            break;

    }
    return true;
}

void FrmMain::on_infobar_response(int) {
    m_Label_InfoBarMessage.set_text("");
    m_InfoBar.hide();
}

bool FrmMain::on_key_press_event(GdkEventKey* key_event) {
    switch(key_event->keyval) {
        case GDK_KEY_KP_1:
        case GDK_KEY_1:
            widget.cursor_x--;

        case GDK_KEY_KP_2:
        case GDK_KEY_2:
            widget.cursor_y++;
            widget.maleneu();
            return true;

        case GDK_KEY_KP_3:
        case GDK_KEY_3:
            widget.cursor_x++;
            widget.cursor_y++;
            widget.maleneu();
            return true;

        case GDK_KEY_KP_4:
        case GDK_KEY_4:
            widget.cursor_x--;
            widget.maleneu();
            return true;

        //case GDK_KEY_KP_5:
        //case GDK_KEY_5:

        case GDK_KEY_KP_7:
        case GDK_KEY_7:
            widget.cursor_x--;

        case GDK_KEY_KP_8:
        case GDK_KEY_8:
            widget.cursor_y--;
            widget.maleneu();
            return true;

        case GDK_KEY_KP_9:
        case GDK_KEY_9:
            widget.cursor_y--;

        case GDK_KEY_KP_6:
        case GDK_KEY_6:
            widget.cursor_x++;
            widget.maleneu();
            return true;

    }

    return Gtk::Window::on_key_press_event(key_event);
}

void FrmMain::setSystemNotice(moba::JsonItemPtr data) {
    moba::JsonObjectPtr o = boost::dynamic_pointer_cast<moba::JsonObject>(data);
    std::string type = moba::castToString(o->at("type"));
    std::string caption = moba::castToString(o->at("caption"));
    std::string text = moba::castToString(o->at("text"));

    if(type == "INFO") {
        m_InfoBar.set_message_type(Gtk::MESSAGE_INFO);
    }
    if(type == "WARNING") {
        m_InfoBar.set_message_type(Gtk::MESSAGE_WARNING);
    }
    if(type == "ERROR") {
        m_InfoBar.set_message_type(Gtk::MESSAGE_ERROR);
    }

    std::replace(caption.begin(), caption.end(), '<', '"');
    std::replace(caption.begin(), caption.end(), '>', '"');
    std::replace(text.begin(), text.end(), '<', '"');
    std::replace(text.begin(), text.end(), '>', '"');

    std::stringstream ss;
    ss << "<b>" << caption << "!</b>\n" << text;

    m_Label_InfoBarMessage.set_markup(ss.str());
    m_InfoBar.show();
}

void FrmMain::setHardwareState(moba::JsonItemPtr data) {
    std::string status = moba::castToString(data);
    if(status == "ERROR") {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Keine Verbindung zur Hardware");
        m_Button_Emegerency.set_sensitive(false);
        return;
    }
    m_Button_Emegerency.set_sensitive(true);
    if(status == "EMERGENCY_STOP") {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("red"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Nohalt ausgelöst");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Nohalt ausgelöst");
        m_Button_Emegerency.set_label("Freigabe");
        return;
    }
    m_Button_Emegerency.set_label("Nothalt");
    if(status == "STANDBY") {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> Energiesparmodus");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> Energiesparmodus");
        m_Button_Emegerency.set_sensitive(false);
        return;
    }
    if(status == "MANUEL") {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("green"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("gold"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> manuell");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> manuell");
    } else if(status == "AUTOMATIC") {
        m_Label_Connectivity_HW.override_color(Gdk::RGBA("green"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_SW.override_color(Gdk::RGBA("green"), Gtk::STATE_FLAG_NORMAL);
        m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> automatisch");
        m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> automatisch");
    }

    std::stringstream ss;
    ss << "<b>Hardwarestatus:</b> " << moba::castToString(data);
}
