#ifndef SYSTEM_MONITOR_HPP
#define SYSTEM_MONITOR_HPP
#include <string>

namespace system_monitor {

    class Monitor {
        public:
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
    };
}

#endif
