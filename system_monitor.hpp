#ifndef SYSTEM_MONITOR_HPP
#define SYSTEM_MONITOR_HPP
#include <string>

namespace system_monitor {

    class Monitor {
        public:
            class General {         // for General informations about the system
                public:
                    unsigned long get_uptime();
                    unsigned long get_procs_num();

                    // Hardware
                    unsigned int get_cpu_cores();
                    std::string get_cpu_model();
                    std::string get_product_name();

                    // Software
                    std::string get_distro_version();
                    std::string get_qt_version();
                    std::string get_kernel_version();
            };
            class Cpu {
                public:
                    double get_usage();

                private:
                    unsigned long long last_total_ = 0;
                    unsigned long long last_idle_ = 0;
                    bool first_call_ = true;
            };

            class Ram {
                public:
                    double get_usage();
                    unsigned long long total();
                    unsigned long long free();
                    unsigned long long used();
            };

            class Drive {
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
    };
}

#endif
