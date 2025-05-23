#include "system_monitor.hpp"
#include <iostream>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>


namespace system_monitor {

    double Monitor::get_ram_usage() {
        struct sysinfo info;
        if(sysinfo(&info) != 0) return 0.0;
        return calc_used_ram(info.totalram, info.freeram);
    }

    double Monitor::get_drive_usage(const std::string& path) {
        struct statvfs vfs;
        if(statvfs(path.c_str(), &vfs) != 0) return 0.0;
        unsigned long long total =  vfs.f_blocks * vfs.f_frsize;
        unsigned long long free = vfs.f_bfree * vfs.f_frsize;
        unsigned long long used = total - free;

        return calc_used_drive(total, used);
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

    double Monitor::calc_used_drive(unsigned long long total, unsigned long long used) {
        if(total == 0) return 0.0;
        double used_fraction = static_cast<double>(used) / static_cast<double>(total);
        if (used_fraction < 0.0) used_fraction = 0.0;
        if (used_fraction > 1.0) used_fraction = 1.0;

        return used_fraction;
    }
}
