#ifndef SYSTEM_APPLICATION_HPP
#define SYSTEM_APPLICATION_HPP

#include <wx/wx.h>

namespace system_monitor {
    class SystemApp : public wxApp {
        public:
            bool OnInit() override;
    };
}

#endif
