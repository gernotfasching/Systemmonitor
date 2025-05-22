#include "monitor_canvas.hpp"

namespace system_monitor {
    MonitorCanvas::MonitorCanvas(const wxString& title)
        : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(300, 200)) {
            SetBackgroundStyle(wxBG_STYLE_PAINT);
            panel_ = new wxPanel(this);
            panel_->SetBackgroundStyle(wxBG_STYLE_PAINT);

            panel_->Bind(wxEVT_PAINT, &MonitorCanvas::on_paint, this);
        }

    void MonitorCanvas::on_paint(wxPaintEvent&) {
        wxPaintDC dc(panel_);
        render(dc);
    }

    void MonitorCanvas::render(wxDC& dc) {
        dc.DrawText(monitor_.get_cpu_usage(), wxPoint(20, 20));
        dc.DrawText(monitor_.get_ram_usage(), wxPoint(20, 50));
        dc.DrawText(monitor_.get_drive_usage(), wxPoint(20, 80));
    }
}
