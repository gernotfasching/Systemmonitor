#include "system_monitor.hpp"
#include <fstream>
#include <sstream>
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <unistd.h>


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

    double Monitor::get_cpu_usage() {
        std::ifstream stat_file("/proc/stat");
        std::string line;
        if(!stat_file.is_open()) return 0.0;

        std::getline(stat_file, line);
        std::istringstream ss(line);

        std::string cpu_label;

        unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
        ss >> cpu_label >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
        unsigned long long idle_time = idle + iowait;
        unsigned long long total_time = user + nice + system + idle + iowait + irq + softirq + steal;

        double usage = 0.0;

        if(first_call_){
            last_total_ = total_time;
            last_idle_ = idle_time;
            first_call_ = false;
            return 0.0;
        } else {
            unsigned long long total_diff = total_time - last_total_;
            unsigned long long idle_diff = idle_time - last_idle_;

            if(total_diff > 0){
                usage = 1.0 - (double(idle_diff) / total_diff);
                if(usage < 0.0) usage = 0.0;
                if(usage > 1.0) usage = 1.0;
            }

            last_total_ = total_time;
            last_idle_ = idle_time;
            return usage;
        }
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
