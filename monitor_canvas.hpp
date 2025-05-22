#ifndef MONITOR_CANVAS_HPP
#define MONITOR_CANVAS_HPP

#include <wx/wx.h>
#include "system_monitor.hpp"

namespace system_monitor {
    class MonitorCanvas : public wxFrame {
        public:
            MonitorCanvas(const wxString& title);

        private:
            void on_paint(wxPaintEvent& event);
            void render (wxDC& dc);

            Monitor monitor_;
            wxPanel* panel_;
    };
}

#endif
