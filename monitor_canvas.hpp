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
            wxTimer* timer_;

            double ram_usage_; // last read RAM usage
            double drive_usage_; // last read Drive usage
            double cpu_usage_;; // last read CPU usage

            bool ram_expanded_ = false;
            bool drive_expanded_ = false;
            bool cpu_expanded_ = true;

            static constexpr int n_cards = 3;               // number of n_cards
            static constexpr int spacing = 30;              // spacing between n_cards
            static constexpr int card_corner_radius = 20;
            static constexpr int title_font_size = 14;
            static constexpr int percent_font_size = 18;

            wxRect get_show_more_rect(int center_x, int y, wxDC& dc, bool expanded);

            void on_paint(wxPaintEvent& event);
            void on_timer(wxTimerEvent& event);
            void on_click(wxMouseEvent& event);

            void render (wxDC& dc);

            void draw_card(wxDC& dc, const wxRect& rect, const wxString& label, double usage);
            void draw_usage_circle(wxDC& dc, int center_x, int center_y, int radius, double usage, const wxColour& color, const wxString& usage_text);
            void draw_title(wxDC&, int x, int y, const wxString& label, int box_width);
            void draw_percentage_text(wxDC& dc, int center_x, int center_y, const wxString& usage_text);
            void draw_show_more_text(wxDC& dc, int center_x, int y, bool expanded);
    };
}

#endif
