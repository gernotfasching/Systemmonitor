#ifndef MONITOR_CANVAS_HPP
#define MONITOR_CANVAS_HPP

#include <wx/wx.h>
#include "system_monitor.hpp"

namespace system_monitor {
    class MonitorCanvas : public wxFrame {
        public:
            MonitorCanvas(const wxString& title);

        private:
            Monitor monitor_;
            wxPanel* panel_;

            void on_paint(wxPaintEvent& event);
            void render (wxDC& dc);

            void draw_card(wxDC& dc, const wxRect& rect, const wxString& label, double usage);
            void draw_usage_circle(wxDC& dc, int center_x, int center_y, int radius, double usage, const wxColour& color, const wxString& usage_text);
    };
}

#endif
