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

#include "frmbase.h"

#include "config.h"

namespace {
    const char license[] =
        "Project:    " PACKAGE_NAME "\n"
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

FrmBase::FrmBase(EndpointPtr mhp):
    msgEndpoint(mhp), m_Button_About("About..."),
    m_Label_Connectivity_HW(" \xe2\x96\x84"), m_Label_Connectivity_SW(" \xe2\x96\x84"),
    m_VBox(Gtk::ORIENTATION_VERTICAL, 6), m_HBox(Gtk::ORIENTATION_HORIZONTAL, 6)
{

    // Add the message label to the InfoBar:
    auto infoBarContainer = dynamic_cast<Gtk::Container*>(m_InfoBar.get_content_area());
    if(infoBarContainer) {
        infoBarContainer->add(m_Label_InfoBarMessage);
    }

    m_VBox.pack_start(m_InfoBar, Gtk::PACK_SHRINK);

    m_HBox.pack_end(m_ButtonBox, Gtk::PACK_SHRINK);

    m_HBox.pack_start(m_Label_Connectivity_HW, Gtk::PACK_SHRINK);
    m_Label_Connectivity_HW.set_justify(Gtk::JUSTIFY_LEFT);
    m_Label_Connectivity_HW.override_color(Gdk::RGBA("Gray"), Gtk::STATE_FLAG_NORMAL);
    m_Label_Connectivity_HW.set_tooltip_markup("<b>Status:</b> unbekannt");

    m_HBox.pack_start(m_Label_Connectivity_SW, Gtk::PACK_SHRINK);
    m_Label_Connectivity_SW.set_justify(Gtk::JUSTIFY_LEFT);
    m_Label_Connectivity_SW.override_color(Gdk::RGBA("Gray"), Gtk::STATE_FLAG_NORMAL);
    m_Label_Connectivity_SW.set_tooltip_markup("<b>Status:</b> unbekannt");

    m_InfoBar.signal_response().connect(sigc::mem_fun(*this, &FrmBase::on_infobar_response));
    m_InfoBar.add_button("_OK", 0);

    // about-dialog
    m_ButtonBox.pack_start(m_Button_About, Gtk::PACK_EXPAND_WIDGET, 5);
    m_ButtonBox.set_layout(Gtk::BUTTONBOX_END);
    m_Button_About.signal_clicked().connect(sigc::mem_fun(*this, &FrmBase::on_button_about_clicked));

    m_ButtonBox.pack_start(m_Button_Emergency, Gtk::PACK_EXPAND_WIDGET, 5);
    m_Button_Emergency.signal_clicked().connect(sigc::mem_fun(*this, &FrmBase::on_button_emergency_clicked));
    m_Button_Emergency.set_label("Nothalt");

}

std::string FrmBase::getDisplayMessage(std::string caption, std::string text) {
    std::replace(caption.begin(), caption.end(), '<', '"');
    std::replace(caption.begin(), caption.end(), '>', '"');
    std::replace(text.begin(), text.end(), '<', '"');
    std::replace(text.begin(), text.end(), '>', '"');

    std::stringstream ss;
    ss << "<b>" << caption << "!</b>\n" << text;
    return std::move(ss.str());
}

void FrmBase::initAboutDialog() {
    m_Dialog.set_transient_for(*this);

    m_Dialog.set_program_name(PACKAGE_NAME);
    m_Dialog.set_version(PACKAGE_VERSION);
    m_Dialog.set_copyright("Stefan Paproth");
    m_Dialog.set_comments("Application for controlling the moba-server.");
    m_Dialog.set_license(license);

    m_Dialog.set_website("<pappi-@gmx.de>");
    m_Dialog.set_website_label("pappi-@gmx.de");

    m_Dialog.set_logo(Gdk::Pixbuf::create_from_file("/usr/local/share/icons/hicolor/scalable/apps/moba-edit.svg"));

    std::vector<Glib::ustring> list_authors;
    list_authors.push_back("Stefan Paproth");
    m_Dialog.set_authors(list_authors);

    m_Dialog.signal_response().connect(sigc::mem_fun(*this, &FrmBase::on_about_dialog_response));

    m_Button_About.grab_focus();
}

void FrmBase::on_about_dialog_response(int) {
    m_Dialog.hide();
}

void FrmBase::setNotice(Gtk::MessageType noticeType, std::string caption, std::string text) {
   // m_Notice_Logger.setNotice(noticeType, caption, text);

    m_Label_InfoBarMessage.set_markup(getDisplayMessage(caption, text));
    m_InfoBar.set_message_type(noticeType);
    m_InfoBar.show();
}

void FrmBase::setErrorNotice(const ClientError &data) {
    setNotice(Gtk::MESSAGE_ERROR, data.errorId, data.additionalMsg);
}

void FrmBase::setSystemNotice(const GuiSystemNotice &data) {
    Gtk::MessageType mt;
    switch(data.noticeType) {
        case GuiSystemNotice::NoticeType::ERROR:
            mt = Gtk::MESSAGE_ERROR;
            break;

        case GuiSystemNotice::NoticeType::WARNING:
            mt = Gtk::MESSAGE_WARNING;
            break;

        case GuiSystemNotice::NoticeType::INFO:
        default:
            mt = Gtk::MESSAGE_INFO;
            break;
    }
    setNotice(mt, data.caption, data.text);
}


void FrmBase::on_button_about_clicked() {
    m_Dialog.show();
    m_Dialog.present();
}

void FrmBase::on_button_emergency_clicked() {
    if(m_Button_Emergency.get_label() == "Nothalt") {
        msgEndpoint->sendMsg(SystemTriggerEmergencyStop{});
    } else {
        msgEndpoint->sendMsg(SystemReleaseEmergencyStop{});
    }
}

void FrmBase::on_infobar_response(int) {
    m_Label_InfoBarMessage.set_text("");
    m_InfoBar.hide();
}

