#include "monitor_canvas.hpp"
#include "catch_amalgamated.hpp"
#include <unistd.h>
#include <wx/font.h>
#include <wx/gtk/bitmap.h>

namespace system_monitor {
    MonitorCanvas::MonitorCanvas(const wxString& title)
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1400, 800))
        {
            SetBackgroundStyle(wxBG_STYLE_PAINT);

            scroll_panel_ = new wxScrolledWindow(this);
            scroll_panel_->SetScrollRate(10, 10);
            scroll_panel_->SetBackgroundStyle(wxBG_STYLE_PAINT);
            scroll_panel_->SetScrollbars(0, 20, 0, 100, 0, 0, true);

            scroll_panel_->Bind(wxEVT_PAINT, &MonitorCanvas::on_paint, this);
            scroll_panel_->Bind(wxEVT_LEFT_DOWN, &MonitorCanvas::on_click, this);

            timer_ = new wxTimer(this);
            Bind(wxEVT_TIMER, &MonitorCanvas::on_timer, this);
            timer_->Start(500);

            cards_[0].label = "RAM";
            cards_[1].label = "Drive";
            cards_[2].label = "CPU";
            cards_[0].usage = monitor_.ram.get_usage();
            cards_[1].usage = monitor_.drive.get_usage();
            cards_[2].usage = monitor_.cpu.get_usage();
        }

    void MonitorCanvas::on_timer(wxTimerEvent&) {
        cards_[0].usage = monitor_.ram.get_usage();
        cards_[1].usage = monitor_.drive.get_usage();
        cards_[2].usage = monitor_.cpu.get_usage();
        scroll_panel_->Refresh();
    }

    void MonitorCanvas::on_paint(wxPaintEvent&) {
        wxPaintDC dc(scroll_panel_);
        scroll_panel_->DoPrepareDC(dc);
        render(dc);
    }

    void MonitorCanvas::on_click(wxMouseEvent& event) {
        int x, y;
        scroll_panel_->CalcUnscrolledPosition(event.GetX(), event.GetY(), &x, &y);

        wxClientDC dc(scroll_panel_);
        scroll_panel_->DoPrepareDC(dc);

        for(int i = 0; i < n_cards; ++i){
            if(get_show_more_rect(cards_[i], dc).Contains(x, y)){
                cards_[i].expanded = !cards_[i].expanded;
                scroll_panel_->Refresh();
                break;
            }
        }
    }

    wxRect MonitorCanvas::get_show_more_rect(const Cards& card, wxDC& dc) const {
        wxFont font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        dc.SetFont(font);
        dc.SetTextForeground(*wxBLUE);

        wxString text = card.expanded ? "show less" : "show more";

        int tw, th;
        dc.GetTextExtent(text, &tw, &th);

        int base_cardHeight = card.rect.height / (card.expanded ? 2 : 1);
        int center_x = card.rect.x + card.rect.width / 2;
        int center_y = card.rect.y + base_cardHeight / 2 - 10;
        int circle_size = std::min(card.rect.width, base_cardHeight) * 0.6;
        int circle_radius = circle_size / 2;
        int show_more_y = center_y + circle_radius + 18;

        return wxRect(center_x - tw / 2 - 4, show_more_y - 2, tw + 8, th + 4);
    }

    void MonitorCanvas::render(wxDC& dc) {
        int width, height;
        scroll_panel_->GetClientSize(&width, &height);

        int base_cardWidth = (width - (n_cards + 1) * spacing) / n_cards;
        int base_cardHeight = height / 2 - 2 * spacing;

        // Draw cards
        int x = spacing;
        int y = spacing;
        int cards_bottom = 0;

        for(int i = 0; i < n_cards; ++i){
            cards_[i].rect = wxRect(x, spacing, base_cardWidth, cards_[i].expanded ? 2 * base_cardHeight : base_cardHeight);
            draw_card(dc, cards_[i], base_cardHeight);
            x += base_cardWidth + spacing;
            if(cards_[i].rect.GetBottom() > cards_bottom)
                cards_bottom = cards_[i].rect.GetBottom();
        }

        int info_y = cards_bottom + spacing;

        int section_width = (width - 3 * spacing) / 2;
        int section_height = (height / 2) - 2 * spacing;

        // General info Section
        draw_info_section(dc, spacing, info_y, section_width, section_height, true);

        // Network Section
        draw_info_section(dc, 2 * spacing + section_width, info_y, section_width, section_height, false);

        int scroll_height = info_y + section_height + spacing;
        scroll_panel_->SetVirtualSize(wxSize(width, scroll_height));
    }

    void MonitorCanvas::draw_card(wxDC& dc, Cards& card, int base_cardHeight) {
        // Draw rounded rectangle (card background)
        wxColour card_bg(255, 255, 255);
        wxColour card_border(180, 180, 180);
        dc.SetBrush(wxBrush(card_bg));
        dc.SetPen(wxPen(card_border, 2));
        const int corner_radius = 20;
        dc.DrawRoundedRectangle(card.rect.x, card.rect.y, card.rect.width, card.rect.height, corner_radius);

        // Draw the usage circle
        int center_x = card.rect.x + card.rect.width / 2;
        int center_y = card.rect.y + base_cardHeight / 2 - 10;
        int circle_size = std::min(card.rect.width, base_cardHeight) * 0.6;
        int circle_radius = circle_size / 2;

        wxColour usage_col(76, 175, 80); // RAM
        if (card.label == "CPU") usage_col = wxColour(33, 150, 243); // CPU
        else if (card.label == "Drive") usage_col = wxColour(255, 152, 0); // Drive

        wxString usage_text = wxString::Format("%.1f%%", card.usage * 100.0);

        draw_usage_circle(dc, center_x, center_y, circle_radius, card.usage, usage_col, usage_text);

        draw_title(dc, card.rect.x, card.rect.y, card.label, card.rect.width);
        int show_more_y = center_y + circle_radius + 18;
        draw_show_more_text(dc, center_x, show_more_y, card.expanded);

        if(card.expanded) {
            int info_y = show_more_y + 40;
            int info_x = card.rect.x + 30;
            if(card.label == "RAM")
                draw_ram_info(dc, card, info_x, info_y);
            if(card.label == "Drive")
                draw_drive_info(dc, card, info_x, info_y);
            if(card.label == "CPU")
                draw_cpu_info(dc, card, info_x, info_y);
        }
    }

    void MonitorCanvas::draw_info_section(wxDC& dc, int x, int y, int w, int h, bool is_general) {
        wxColour card_bg(255, 255, 255);
        wxColour card_border(180, 180, 180);
        dc.SetBrush(wxBrush(card_bg));
        dc.SetPen(wxPen(card_border, 2));
        const int corner_radius = 20;
        dc.DrawRoundedRectangle(x, y, w, h, corner_radius);

        if(is_general) {
            draw_system_infos(dc, x + spacing, y + spacing);
        } else {
            draw_network_infos(dc, x + spacing, y + spacing);
        }
    }


    void MonitorCanvas::draw_usage_circle(wxDC& dc, int center_x, int center_y, int radius, double usage, const wxColour& color, const wxString& usage_text) {
        wxColour bg_circle(220, 220, 220);
        dc.SetPen(wxPen(bg_circle, 10));
        dc.SetBrush(*wxTRANSPARENT_BRUSH);
        dc.DrawEllipse(center_x - radius, center_y - radius, 2 * radius, 2 * radius);

        dc.SetPen(wxPen(color, 10));
        double start_angle = -90.0; // top
        double end_angle = start_angle + usage * 360.0;

        if (usage > 0.0) {
            if (usage >= 1.0) usage = 0.999; // DrawEllipticArc does not allow values over 0.999
            dc.DrawEllipticArc(center_x - radius, center_y - radius, 2 * radius, 2 * radius, start_angle, end_angle);
        }
        draw_percentage_text(dc, center_x, center_y, usage_text);
    }

    void MonitorCanvas::draw_title(wxDC& dc, int x, int y, const wxString& label, int box_width) {
        wxFont font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        dc.SetFont(font);
        dc.SetTextForeground(*wxBLACK);

        int tw, th;
        dc.GetTextExtent(label, &tw, &th);

        int label_x = x + (box_width - tw) / 2;
        int label_y = y +  18;

        dc.DrawText(label, label_x, label_y);
    }

    void MonitorCanvas::draw_percentage_text(wxDC& dc, int center_x, int center_y, const wxString& usage_text) {
        wxFont font(percent_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        dc.SetFont(font);
        dc.SetTextForeground(*wxBLACK);

        int tw, th;
        dc.GetTextExtent(usage_text, &tw, &th);
        dc.DrawText(usage_text, center_x - tw / 2, center_y - th / 2);
    }

    void MonitorCanvas::draw_show_more_text(wxDC& dc, int center_x, int y, bool expanded) {
        wxFont font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        dc.SetFont(font);
        dc.SetTextForeground(*wxBLUE);

        wxString text = expanded ? "show less" : "show more";

        int tw, th;
        dc.GetTextExtent(text, &tw, &th);

        dc.DrawText(text, center_x - tw / 2, y);
    }

    void MonitorCanvas::draw_ram_info(wxDC& dc, const Cards& card, int info_x, int info_y) {
        wxFont heading_font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont info_font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        dc.SetFont(heading_font);
        dc.SetTextForeground(*wxBLACK);

        dc.DrawText("RAM informations:", info_x, info_y);

        unsigned long long total = monitor_.ram.total();
        unsigned long long used = monitor_.ram.used();
        unsigned long long free = monitor_.ram.free();

        int line_y = info_y + 35;
        dc.SetFont(info_font);

        dc.DrawText(wxString::Format("Total memory: %.2f GiB", static_cast<double>(total) / (1024.0 * 1024 * 1024)), info_x, line_y);
        line_y += 25;
        dc.DrawText(wxString::Format("Total memory: %.2f GiB", static_cast<double>(used) / (1024.0 * 1024 * 1024)), info_x, line_y);
        line_y += 25;
        dc.DrawText(wxString::Format("Total memory: %.2f GiB", static_cast<double>(free) / (1024.0 * 1024 * 1024)), info_x, line_y);
    }

    void MonitorCanvas::draw_drive_info(wxDC& dc, const Cards& card, int info_x, int info_y) {
        wxFont heading_font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont info_font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        dc.SetFont(heading_font);
        dc.SetTextForeground(*wxBLACK);

        dc.DrawText("Drive informations:", info_x, info_y);

        unsigned long long total = monitor_.drive.total();
        unsigned long long used = monitor_.drive.used();
        unsigned long long free = monitor_.drive.free();

        wxCoord line_y = info_y + 35;
        dc.SetFont(info_font);

        dc.DrawText(wxString::Format("Total memory: %.2f GiB", static_cast<double>(total) / (1024.0 * 1024 * 1024)), info_x, line_y);
        line_y += 25;
        dc.DrawText(wxString::Format("Total memory: %.2f GiB", static_cast<double>(used) / (1024.0 * 1024 * 1024)), info_x, line_y);
        line_y += 25;
        dc.DrawText(wxString::Format("Total memory: %.2f GiB", static_cast<double>(free) / (1024.0 * 1024 * 1024)), info_x, line_y);
    }

    void MonitorCanvas::draw_cpu_info(wxDC& dc, const Cards& card, int info_x, int info_y) {
        wxFont heading_font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont info_font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        dc.SetFont(heading_font);
        dc.SetTextForeground(*wxBLACK);

        dc.DrawText("CPU informations:", info_x, info_y);
        int line_y = info_y + 28;

        dc.SetFont(info_font);
        wxString info_text = wxString::Format("Further Informations about CPU need to \nbe implemented.");
        dc.DrawText(info_text, info_x, line_y);
    }

    void MonitorCanvas::draw_system_infos(wxDC& dc, int info_x, int info_y) {
        wxFont heading_font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont info_font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        dc.SetFont(heading_font);
        dc.SetTextForeground(*wxBLACK);

        dc.DrawText("General informations:", info_x, info_y);

        unsigned int core_num = monitor_.general.get_cpu_cores();
        wxString model_name = monitor_.general.get_cpu_model();

        unsigned long uptime = monitor_.general.get_uptime();
        unsigned long procs_num = monitor_.general.get_procs_num();

        int line_y = info_y + 40;

        dc.SetFont(info_font);

        wxString cpus = wxString::Format("Processors: %ui x " + model_name, core_num);
        wxString uptime_text = wxString::Format("System uptime since boot (seconds): %llu", uptime);
        wxString procs_text = wxString::Format("Number of processes running: %llu", procs_num);

        dc.DrawText(cpus, info_x, line_y);
        line_y += spacing;
        dc.DrawText(uptime_text, info_x, line_y);
        line_y += spacing;
        dc.DrawText(procs_text, info_x, line_y);
    }

    void MonitorCanvas::draw_network_infos(wxDC& dc, int info_x, int info_y) {
        wxFont heading_font(title_font_size, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        wxFont info_font(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

        dc.SetFont(heading_font);
        dc.SetTextForeground(*wxBLACK);

        dc.DrawText("Network informations:", info_x, info_y);

        // unsigned long uptime = monitor_.general.get_uptime();
        // unsigned long procs_num = monitor_.general.get_procs_num();
        //
        // int line_y = info_y + 40;
        //
        // dc.SetFont(info_font);
        //
        // wxString uptime_text = wxString::Format("System uptime since boot (seconds): %llu", uptime);
        // wxString procs_text = wxString::Format("Number of processes running: %llu", procs_num);
        //
        // dc.DrawText(uptime_text, info_x, line_y);
        // line_y += spacing;
        // dc.DrawText(procs_text, info_x, line_y);
    }
}
