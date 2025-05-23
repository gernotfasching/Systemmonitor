#include "monitor_canvas.hpp"

namespace system_monitor {
    MonitorCanvas::MonitorCanvas(const wxString& title)
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(700, 400))
        {
            SetBackgroundStyle(wxBG_STYLE_PAINT);
            panel_ = new wxPanel(this);
            panel_->SetBackgroundStyle(wxBG_STYLE_PAINT);

            panel_->Bind(wxEVT_PAINT, &MonitorCanvas::on_paint, this);
            timer_ = new wxTimer(this);
            Bind(wxEVT_TIMER, &MonitorCanvas::on_timer, this);
            timer_->Start(1000);

            ram_usage_ = monitor_.get_ram_usage();
            drive_usage_ = monitor_.get_drive_usage("/");
        }

    void MonitorCanvas::on_timer(wxTimerEvent&) {
        ram_usage_ = monitor_.get_ram_usage();
        drive_usage_ = monitor_.get_drive_usage("/");
        panel_->Refresh();
    }

    void MonitorCanvas::on_paint(wxPaintEvent&) {
        wxPaintDC dc(panel_);
        render(dc);
    }

    void MonitorCanvas::render(wxDC& dc) {
        int width, height;
        GetClientSize(&width, &height);

        const int n_cards = 3;
        const int spacing = 30;

        int cardWidth = (width - (n_cards + 1) * spacing) / n_cards;
        int cardHeight = height / 2 - 2 * spacing;

        wxRect ramRect(spacing, spacing, cardWidth, cardHeight);
        wxRect driveRect(2 * spacing + cardWidth, spacing, cardWidth, cardHeight);
        wxRect cpuRect(3 * spacing + 2 *cardWidth, spacing, cardWidth, cardHeight);

        draw_card(dc, ramRect, "RAM", ram_usage_);
        draw_card(dc, driveRect, "Drive", drive_usage_);
        draw_card(dc, cpuRect, "CPU", 0.12);
    }

    void MonitorCanvas::draw_card(wxDC& dc, const wxRect& rect, const wxString& label, double usage) {
        // Draw rounded rectangle (card background)
        wxColour card_bg(255, 255, 255);
        wxColour card_border(180, 180, 180);
        dc.SetBrush(wxBrush(card_bg));
        dc.SetPen(wxPen(card_border, 2));
        const int corner_radius = 20;
        dc.DrawRoundedRectangle(rect.x, rect.y, rect.width, rect.height, corner_radius);

        // Draw the usage circle
        int circle_size = std::min(rect.width, rect.height) * 0.6;
        int center_x = rect.x + rect.width / 2;
        int center_y = rect.y + rect.height / 2 - 10;
        int circle_radius = circle_size / 2;

        wxColour usage_col(76, 175, 80); // RAM
        if (label == "CPU") usage_col = wxColour(33, 150, 243); // CPU
        else if (label == "Drive") usage_col = wxColour(255, 152, 0); // Drive

        wxString usage_text = wxString::Format("%.1f%%", usage * 100.0);

        draw_usage_circle(dc, center_x, center_y, circle_radius, usage, usage_col, usage_text);

        // Draw the label below
        wxFont label_font(14, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
        dc.SetFont(label_font);
        int tw, th;
        dc.GetTextExtent(label, &tw, &th);
        dc.DrawText(label, center_x - tw/2, rect.y + rect.height - th);
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

        // Draw usage percent centered
        wxFont usage_font(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
        dc.SetFont(usage_font);
        dc.SetTextForeground(*wxBLACK);
        int tw, th;
        dc.GetTextExtent(usage_text, &tw, &th);
        dc.DrawText(usage_text, center_x - tw/2, center_y - th/2);
    }

}
