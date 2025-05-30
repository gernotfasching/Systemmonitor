#ifndef SYSTEM_MONITOR_HPP
#define SYSTEM_MONITOR_HPP
#include <string>
#include <chrono>

namespace system_monitor {

    class Monitor {
        public:
            class General {         // General informations about the system
                public:
                    unsigned long get_uptime();
                    unsigned long get_procs_num();

                    // Hardware
                    unsigned int get_cpu_cores();
                    std::string get_cpu_model();
                    std::string get_product_name();

                    // Software
                    std::string get_os_version();
                    std::string get_kernel_version();
            };

            class Network {         // Network informations
                public:
                    std::string get_wifi_ssid();
                    double get_download_rate();
                    double get_upload_rate();

                private:
                    unsigned long long last_rx_bytes_ = 0;
                    unsigned long long last_tx_bytes_ = 0;
                    std::chrono::steady_clock::time_point last_time_ = std::chrono::steady_clock::now();
                    std::string get_primary_interface();
                    void update_counter();
                    double last_download_rate_ = 0.0;
                    double last_upload_rate_ = 0.0;
            };

            class Cpu {         // CPU informations
                public:
                    double get_usage();

                private:
                    unsigned long long last_total_ = 0;
                    unsigned long long last_idle_ = 0;
                    bool first_call_ = true;
            };

            class Ram {         // RAM informations
                public:
                    double get_usage();
                    unsigned long long total();
                    unsigned long long free();
                    unsigned long long used();
            };

            class Drive {       // Drive informations
                public:
                    double get_usage(const std::string& path = "/");
                    unsigned long long total(const std::string& path = "/");
                    unsigned long long free(const std::string& path = "/");
                    unsigned long long used(const std::string& path = "/");
            };

            Cpu cpu;
            Ram ram;
            Drive drive;
            General general;
            Network network;
    };
}

#endif
