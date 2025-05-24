#ifndef MONITOR_CANVAS_HPP
#define MONITOR_CANVAS_HPP

#include <wx/wx.h>
#include "system_monitor.hpp"

namespace system_monitor {
    class MonitorCanvas : public wxFrame {
        public:
            MonitorCanvas(const wxString& title);

        private:
            struct Cards {
                wxString label;
                wxRect rect;
                bool expanded = false;
                double usage = 0.0;
            };

            static constexpr int n_cards = 3;               // number of n_cards
            static constexpr int spacing = 30;              // spacing between n_cards
            static constexpr int title_font_size = 14;
            static constexpr int percent_font_size = 18;

            Monitor monitor_;
            wxScrolledWindow* scroll_panel_;
            wxTimer* timer_;
            Cards cards_[n_cards];

            bool is_expanded_ = true;

            wxRect get_show_more_rect(const Cards& card, wxDC& dc) const;

            void on_paint(wxPaintEvent& event);
            void on_timer(wxTimerEvent& event);
            void on_click(wxMouseEvent& event);

            void render (wxDC& dc);

            void draw_card(wxDC& dc, Cards& card, int base_cardHeight);
            void draw_info_section(wxDC& dc, int x, int y, int w, int h, bool is_general);
            void draw_usage_circle(wxDC& dc, int center_x, int center_y, int radius, double usage, const wxColour& color, const wxString& usage_text);
            void draw_title(wxDC&, int x, int y, const wxString& label, int box_width);
            void draw_percentage_text(wxDC& dc, int center_x, int center_y, const wxString& usage_text);
            void draw_show_more_text(wxDC& dc, int center_x, int y, bool expanded);

            void draw_ram_info(wxDC& dc, const Cards& card, int info_x, int info_y);
            void draw_drive_info(wxDC& dc, const Cards& card, int info_x, int info_y);
            void draw_cpu_info(wxDC& dc, const Cards& card, int info_x, int info_y);
            void draw_system_infos(wxDC& dc, int info_x, int info_y);
            void draw_network_infos(wxDC& dc, int info_x, int info_y);
    };
}

#endif
