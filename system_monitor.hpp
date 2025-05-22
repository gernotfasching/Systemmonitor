#ifndef SYSTEM_MONITOR_HPP
#define SYSTEM_MONITOR_HPP

namespace system_monitor {

    class Monitor {
        public:
            Monitor() = default;

            const char* get_cpu_usage() const {return "CPU: ";}
            const char* get_ram_usage() const {return "RAM: ";}
            const char* get_drive_usage() const {return "Drive: ";}
    };
}

#endif
