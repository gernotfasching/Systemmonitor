#ifndef SYSTEM_MONITOR_HPP
#define SYSTEM_MONITOR_HPP
#include <string>

namespace system_monitor {

    class Monitor {
        public:
            Monitor() = default;

            double get_ram_usage();
            double get_drive_usage(const std::string& path = "/");
            const char* get_cpu_usage() const {return "CPU: ";}

            double calc_used_ram(unsigned long long total, unsigned long long free);
            double calc_used_drive(unsigned long long total, unsigned long long used);
    };
}

#endif
