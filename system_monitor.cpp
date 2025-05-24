#include "system_monitor.hpp"
#include <fstream>
#include <sstream>
// See: https://man7.org/linux/man-pages/man2/sysinfo.2.html
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <iostream>
#include <iomanip>


namespace system_monitor {

    // CPU
    double Monitor::Cpu::get_usage() {
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
                usage = 1.0 - (double(idle_diff) / double(total_diff));
                if(usage < 0.0) usage = 0.0;
                if(usage > 1.0) usage = 1.0;
            }

            last_total_ = total_time;
            last_idle_ = idle_time;
            return usage;
        }
    }


    // RAM
    double Monitor::Ram::get_usage() {
        struct sysinfo info;
        if(sysinfo(&info) != 0) return 0.0;
        if(info.totalram == 0) return 0.0;
        return static_cast<double>(info.totalram - info.freeram) / static_cast<double>(info.totalram);
    }

    unsigned long long Monitor::Ram::total() {
        struct sysinfo info;
        if(sysinfo(&info) != 0) return 0;
        return info.totalram;
    }

    unsigned long long Monitor::Ram::free() {
        struct sysinfo info;
        if(sysinfo(&info) != 0) return 0;
        return info.freeram;
    }

    unsigned long long Monitor::Ram::used() {
        struct sysinfo info;
        if(sysinfo(&info) != 0) return 0;
        return info.totalram - info.freeram;
    }


    // Drive
    double Monitor::Drive::get_usage(const std::string& path) {
        unsigned long long t = total(path);
        unsigned long long  u = used(path);
        if(t == 0) return 0.0;
        return static_cast<double>(u) / static_cast<double>(t);
    }

    unsigned long long Monitor::Drive::total(const std::string& path) {
        struct statvfs vfs;
        if(statvfs(path.c_str(), &vfs) != 0) return 0.0;
        return vfs.f_blocks * vfs.f_frsize;
    }

    unsigned long long Monitor::Drive::free(const std::string& path) {
        struct statvfs vfs;
        if(statvfs(path.c_str(), &vfs) != 0) return 0.0;
        return vfs.f_bfree * vfs.f_frsize;
    }

    unsigned long long Monitor::Drive::used(const std::string& path) {
        if(total(path) < free(path)) return 0;
        return total(path) - free(path);
    }
}
