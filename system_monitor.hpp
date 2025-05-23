#ifndef SYSTEM_MONITOR_HPP
#define SYSTEM_MONITOR_HPP
#include <string>

namespace system_monitor {

    class Monitor {
        public:
            Monitor() = default;

            double get_ram_usage();
            double get_drive_usage(const std::string& path = "/");
            double get_cpu_usage();



        private:
            unsigned long long last_total_ = 0;
            unsigned long long last_idle_ = 0;
            bool first_call_ = true;

            double calc_used_ram(unsigned long long total, unsigned long long free);
            double calc_used_drive(unsigned long long total, unsigned long long used);
    };
}

#endif
