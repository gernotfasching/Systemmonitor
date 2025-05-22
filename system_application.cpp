#include "system_application.hpp"
#include "monitor_canvas.hpp"

namespace system_monitor {
    bool SystemApp::OnInit() {
        MonitorCanvas* mainframe = new MonitorCanvas("System Monitor");
        mainframe->Center();
        mainframe->Show();
        return true;
    }
}

wxIMPLEMENT_APP(system_monitor::SystemApp);
