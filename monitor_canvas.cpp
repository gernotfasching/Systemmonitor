#include "monitor_canvas.hpp"
#include <wx/font.h>

namespace system_monitor {
    MonitorCanvas::MonitorCanvas(const wxString& title)
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(1400, 800))
        {
            SetBackgroundStyle(wxBG_STYLE_PAINT);
            panel_ = new wxPanel(this);
            panel_->SetBackgroundStyle(wxBG_STYLE_PAINT);

            panel_->Bind(wxEVT_PAINT, &MonitorCanvas::on_paint, this);
            panel_->Bind(wxEVT_LEFT_DOWN, &MonitorCanvas::on_click, this);
            timer_ = new wxTimer(this);
            Bind(wxEVT_TIMER, &MonitorCanvas::on_timer, this);
            timer_->Start(500);

            ram_usage_ = monitor_.get_ram_usage();
            drive_usage_ = monitor_.get_drive_usage("/");
            cpu_usage_ = monitor_.get_cpu_usage();
        }

    void MonitorCanvas::on_timer(wxTimerEvent&) {
        ram_usage_ = monitor_.get_ram_usage();
        drive_usage_ = monitor_.get_drive_usage("/");
        cpu_usage_ = monitor_.get_cpu_usage();
        panel_->Refresh();
    }

    void MonitorCanvas::on_paint(wxPaintEvent&) {
        wxPaintDC dc(panel_);
        render(dc);
    }

    void MonitorCanvas::on_click(wxMouseEvent& event) {
        int x = event.GetX();
        int y = event.GetY();

        int width, height;
        GetClientSize(&width, &height);

        int cardWidth = (width - (n_cards + 1) * spacing) / n_cards;
        int cardHeight = height / 2 - 2 * spacing;

        wxRect ramRect(spacing, spacing, cardWidth, cardHeight);
        wxRect driveRect(2 * spacing + cardWidth, spacing, cardWidth, cardHeight);
        wxRect cpuRect(3 * spacing + 2 * cardWidth, spacing, cardWidth, cardHeight);

        int circle_size = std::min(cardWidth, cardHeight) * 0.6;
        int center_x, center_y, show_more_y;
        wxClientDC dc(panel_);

        const int circle_offset_y = 10;
        const int show_more_offset_y = 18;

        // RAM
        center_x = ramRect.x + ramRect.width / 2;
        center_y = ramRect.y + ramRect.height / 2 - circle_offset_y;
        show_more_y = center_y + (circle_size / 2) + show_more_offset_y;
        if(get_show_more_rect(center_x, show_more_y, dc, ram_expanded_).Contains(x, y)) {
            ram_expanded_ = !ram_expanded_;
            panel_->Refresh();
            return;
        }
        // Drive
        center_x = driveRect.x + driveRect.width / 2;
        center_y = driveRect.y + driveRect.height / 2 - circle_offset_y;
        show_more_y = center_y + (circle_size / 2) + show_more_offset_y;
        if(get_show_more_rect(center_x, show_more_y, dc, ram_expanded_).Contains(x, y)) {
            drive_expanded_ = !drive_expanded_;
            panel_->Refresh();
            return;
        }
        // CPU
        center_x = cpuRect.x + cpuRect.width / 2;
        center_y = cpuRect.y + cpuRect.height / 2 - circle_offset_y;
        show_more_y = center_y + (circle_size / 2) + show_more_offset_y;
        if(get_show_more_rect(center_x, show_more_y, dc, ram_expanded_).Contains(x, y)) {
            cpu_expanded_ = !cpu_expanded_;
            panel_->Refresh();
            return;
        }
    }

    wxRect MonitorCanvas::get_show_more_rect(int center_x, int y, wxDC& dc, bool expanded) {
        wxFont font(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        dc.SetFont(font);
        dc.SetTextForeground(*wxBLUE);

        wxString text = expanded ? "show less" : "show more";

        int tw, th;
        dc.GetTextExtent(text, &tw, &th);

        return wxRect(center_x - tw / 2 - 4, y - 2, tw + 8, th + 4);
    }

    void MonitorCanvas::render(wxDC& dc) {
        int width, height;
        GetClientSize(&width, &height);

        int base_cardWidth = (width - (n_cards + 1) * spacing) / n_cards;
        int base_cardHeight = height / 2 - 2 * spacing;

        bool expanded_arr[n_cards] = {ram_expanded_, drive_expanded_, cpu_expanded_};

        // Set height of cards
        int card_heights[n_cards];
        for(int i = 0; i < n_cards; ++i){
            card_heights[i] = expanded_arr[i] ? 2 *base_cardHeight : base_cardHeight;
        }

        // Draw cards
        int x = spacing;
        double usages[n_cards] = {ram_usage_, drive_usage_, cpu_usage_};
        wxString labels[n_cards] = {"RAM", "Drive", "CPU"};

        for(int i = 0; i < n_cards; ++i){
            wxRect cardRect(x, spacing, base_cardWidth, card_heights[i]);
            draw_card(dc, cardRect, labels[i], usages[i], expanded_arr[i], base_cardHeight);
            x += base_cardWidth + spacing;
        }

        // wxRect ramRect(spacing, spacing, cardWidth, cardHeight);
        // wxRect driveRect(2 * spacing + cardWidth, spacing, cardWidth, cardHeight);
        // wxRect cpuRect(3 * spacing + 2 *cardWidth, spacing, cardWidth, cardHeight);
        //
        // draw_card(dc, ramRect, "RAM", ram_usage_);
        // draw_card(dc, driveRect, "Drive", drive_usage_);
        // draw_card(dc, cpuRect, "CPU", cpu_usage_);
    }

    void MonitorCanvas::draw_card(wxDC& dc, const wxRect& rect, const wxString& label, double usage, bool expanded, int base_cardHeight) {
        // Draw rounded rectangle (card background)
        wxColour card_bg(255, 255, 255);
        wxColour card_border(180, 180, 180);
        dc.SetBrush(wxBrush(card_bg));
        dc.SetPen(wxPen(card_border, 2));
        const int corner_radius = 20;
        dc.DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, corner_radius);

        // Draw the usage circle
        int center_x = rect.x + rect.width / 2;
        int center_y = rect.y + base_cardHeight / 2 - 10;
        int circle_size = std::min(rect.width, base_cardHeight) * 0.6;
        int circle_radius = circle_size / 2;

        wxColour usage_col(76, 175, 80); // RAM
        if (label == "CPU") usage_col = wxColour(33, 150, 243); // CPU
        else if (label == "Drive") usage_col = wxColour(255, 152, 0); // Drive

        wxString usage_text = wxString::Format("%.1f%%", usage * 100.0);

        draw_usage_circle(dc, center_x, center_y, circle_radius, usage, usage_col, usage_text);

        draw_title(dc, rect.x, rect.y, label, rect.width);
        int show_more_y = center_y + circle_radius + 18;
        draw_show_more_text(dc, center_x, show_more_y, expanded);
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
            dc.DrawEllipticArc(center_x - radius, center_y - radius, 2 * radius, 2 * radius,
                               start_angle, end_angle);
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
}
