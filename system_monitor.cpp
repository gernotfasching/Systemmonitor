#include "system_monitor.hpp"
#include <iostream>
#include <sys/sysinfo.h>


namespace system_monitor {

    double Monitor::get_ram_usage() {
        struct sysinfo info;
        if(sysinfo(&info) != 0) return 0.0;
        return calc_used_ram(info.totalram, info.freeram);
    }

    double Monitor::calc_used_ram(unsigned long long total, unsigned long long free) {
        double used_fraction = 0.0;

        if(total > 0) {
            used_fraction = static_cast<double>(total - free) / total;
            if (used_fraction < 0.0) used_fraction = 0.0;
            if (used_fraction > 1.0) used_fraction = 1.0;
        }

        return used_fraction;
    }
}
