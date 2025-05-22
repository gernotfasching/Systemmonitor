#ifndef SYSTEM_MONITOR_HPP
#define SYSTEM_MONITOR_HPP

namespace system_monitor {

    class Monitor {
        public:
            Monitor() = default;

            double get_ram_usage();
            const char* get_drive_usage() const {return "Drive: ";}
            const char* get_cpu_usage() const {return "CPU: ";}

            double calc_used_ram(unsigned long long total, unsigned long long free);

    };
}

#endif
