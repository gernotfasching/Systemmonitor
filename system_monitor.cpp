#include "system_monitor.hpp"
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <cstdio>
#include <array>
#include <chrono>
#include <regex>

// See: https://man7.org/linux/man-pages/man2/sysinfo.2.html
#include <sys/sysinfo.h>
#include <sys/statvfs.h>
#include <sys/utsname.h>

#include <unistd.h>
#include <iostream>

using std::string;


namespace system_monitor {

    // General informations
    // uptime
    unsigned long Monitor::General::get_uptime() {
        struct sysinfo info;
        if(sysinfo(&info) != 0) return 0.0;
        return static_cast<unsigned long>(info.uptime);
    }
    // number of processes
    unsigned long Monitor::General::get_procs_num() {
        struct sysinfo info;
        if(sysinfo(&info) != 0) return 0.0;
        return static_cast<unsigned long>(info.procs);
    }

    // number of cpu cores
    unsigned int Monitor::General::get_cpu_cores() {
        return std::thread::hardware_concurrency();
    }

    // cpu model name
    string Monitor::General::get_cpu_model() {
        std::ifstream cpuinfo("/proc/cpuinfo");
        string line, model;
        if(!cpuinfo.is_open()) return "";

        while(std::getline(cpuinfo, line)) {
            if(line.find("model name") !=string::npos) {
                size_t pos = line.find(":");
                if(pos != string::npos) {
                    model = line.substr(pos + 2);
                    break;
                }
            }
        }
        return model;
    }

    // name of product
    string Monitor::General::get_product_name() {
        std::ifstream file("/sys/devices/virtual/dmi/id/product_name");
        string name;
        if(!file.is_open()) return "Name of product is unknown.";
        std::getline(file, name);
        return name;
    }

    // version of os
    string Monitor::General::get_os_version() {
        FILE* pipe = popen("plasmashell --version 2>/dev/null", "r");
        if(!pipe)
            return "Version of OS is unknown, probably not on kde plasma.";

        std::array<char, 120> buffer;
        string result;

        while(fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
            result += buffer.data();
        }
        pclose(pipe);

        if(result.empty())
            return "Version is undetectable.";

        size_t pos = result.find(" ");
        if(pos != string::npos)
            return result.substr(pos + 1);
        return result;
    }

    // version of kernel
    string Monitor::General::get_kernel_version() {
        struct utsname buffer;
        if(uname(&buffer) != 0)
            return "Kernel version unknown";
        return string(buffer.release);
    }


    // Network
    // Helper funtion which returns the primary wirles interface e.g. wlan...
    string Monitor::Network::get_primary_interface() {
        std::ifstream wireless("/proc/net/wireless");
        string line;
        while(std::getline(wireless, line)) {
            std::smatch m;
            if(std::regex_search(line, m, std::regex(R"((\w+):)"))) {
                return m[1];
            }
        }
        return "";
    }

    // Function to update download and upload
    void Monitor::Network::update_counter() {
        string intf = get_primary_interface();
        if(intf.empty()) return;

        std::ifstream netdev("/proc/net/dev");
        std::string line;
        unsigned long long rx_bytes = 0, tx_bytes = 0;
        while(std::getline(netdev, line)) {
            if(line.find(intf) != std::string::npos) {
                std::istringstream iss(line.substr(line.find(":") + 1));
                iss >> rx_bytes;
                for(int i = 0; i < 8; ++i){
                    iss >> tx_bytes;
                    break;
                }
            }
        }

        auto now = std::chrono::steady_clock::now();
        double seconds = std::chrono::duration_cast<std::chrono::duration<double>>(now - last_time_).count();

        if(last_rx_bytes_ != 0 && last_tx_bytes_ != 0 && seconds > 0) {
            last_download_rate_ = (rx_bytes - last_rx_bytes_) / seconds;
            last_upload_rate_ = (tx_bytes - last_tx_bytes_) / seconds;
        }
        last_rx_bytes_ = rx_bytes;
        last_tx_bytes_ = tx_bytes;
        last_time_ = now;
    }

    // get + update last_download_rate_
    double Monitor::Network::get_download_rate() {
        update_counter();
        return last_download_rate_;
    }

    // get + update last_upload_rate_
    double Monitor::Network::get_upload_rate() {
        update_counter();
        return last_upload_rate_;
    }

    // CPU
    double Monitor::Cpu::get_usage() {
        std::ifstream stat_file("/proc/stat");
        string line;
        if(!stat_file.is_open()) return 0.0;

        std::getline(stat_file, line);
        std::istringstream ss(line);

        string cpu_label;
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


    // Used drive space (e.g.: 0.0 to 1.0)
    double Monitor::Drive::get_usage(const std::string& path) {
        unsigned long long t = total(path);
        unsigned long long  u = used(path);
        if(t == 0) return 0.0;
        return static_cast<double>(u) / static_cast<double>(t);
    }
    // Total drive space
    unsigned long long Monitor::Drive::total(const std::string& path) {
        struct statvfs vfs;
        if(statvfs(path.c_str(), &vfs) != 0) return 0.0;
        return vfs.f_blocks * vfs.f_frsize;
    }

    // Free drive space
    unsigned long long Monitor::Drive::free(const std::string& path) {
        struct statvfs vfs;
        if(statvfs(path.c_str(), &vfs) != 0) return 0.0;
        return vfs.f_bfree * vfs.f_frsize;
    }

    // Used Drive space
    unsigned long long Monitor::Drive::used(const std::string& path) {
        if(total(path) < free(path)) return 0;
        return total(path) - free(path);
    }
}
