#include "catch_amalgamated.hpp"
#include "system_monitor.hpp"
#include "monitor_canvas.hpp"
#include <limits>
#include <string>

// General Tests
// uptime and number of processes
TEST_CASE("Monitor::General get_uptime/get_procs_num/", "[system_monitor][General]") {
    system_monitor::Monitor::General general;

    unsigned long uptime = general.get_uptime();
    unsigned long procs_num = general.get_procs_num();

    CHECK(uptime >= 0);       // Uptime is over 0
    CHECK(procs_num >= 0);        // Number of processes is over 0
}

// Number of Cores and cpu model name
TEST_CASE("Monitor::General get_cpu_cores/get_cpu_model/", "[system_monitor][General]") {
    system_monitor::Monitor::General general;

    unsigned int num_cores = general.get_cpu_cores();
    std::string model_name = general.get_cpu_model();

    CHECK(num_cores >= 0);       // num_cores is over 0
    CHECK(model_name != "");     // model_name should not equal empty string
}

TEST_CASE("Monitor::General get_distro_version/get_kernel_version/get_product_name/get_qt_version", "[system_monitor][General]") {
    system_monitor::Monitor::General general;

    std::string distro_version = general.get_distro_version();
    std::string kernel_version = general.get_kernel_version();
    std::string product_name = general.get_product_name();
    std::string qt_version = general.get_qt_version();

    CHECK(distro_version != "");     // distro_version should not equal empty string
    CHECK(kernel_version != "");     // kernel_version should not equal empty string
    CHECK(product_name != "");       // product_name should not equal empty string
    CHECK(qt_version != "");         // qt_version should not equal empty string
}

// CPU Tests
TEST_CASE("Monitor::CPU get_usage", "[system_monitor][Cpu]") {
    system_monitor::Monitor::Cpu cpu;

    double first_usage = cpu.get_usage();
    CHECK(first_usage == 0.0);      // First Call should always return 0.0

    double usage1 = cpu.get_usage();
    CHECK(usage1 >= 0.0);       // CPU usage1 should be >= 0
    CHECK(usage1<= 1.0);        // CPU usage1 should be <= 1

    // for redundance
    double usage2 = cpu.get_usage();
    CHECK(usage2 >= 0.0);
    CHECK(usage2 <= 1.0);
}

// RAM Tests
// usage
TEST_CASE("Monitor::RAM get_usage", "[system_monitor][Ram]") {
    system_monitor::Monitor::Ram ram;

    double usage1 = ram.get_usage();
    CHECK(usage1 >= 0.0);       // RAM usage1 should be >= 0
    CHECK(usage1<= 1.0);        // RAM usage1 should be <= 1

    // For redundance
    double usage2 = ram.get_usage();
    CHECK(usage2 >= 0.0);
    CHECK(usage2 <= 1.0);

    auto total = ram.total();
    auto used = ram.used();

    // Used should be approximately constistent with usage2
    if(total != 0) {
        CHECK(static_cast<double>(used) / static_cast<double>(total) == Catch::Approx(usage2).epsilon(0.05));
    }

    CHECK(ram.free() <= total);         // Free should not be greater than total
}

// total, free & used
TEST_CASE("Monitor::RAM total/free/used", "[system_monitor][Ram]") {
    system_monitor::Monitor::Ram ram;

    auto total = ram.total();
    auto free = ram.free();
    auto used = ram.used();

    CHECK(total >= free);       // Total RAM should be at least as much as free
    CHECK(total >= used);       // Total RAM should be at least as much as used
    CHECK(total == Catch::Approx(free + used).epsilon(0.5));        // total should be approximately constistent with free + used
}

// Drive Tests
// usage
TEST_CASE("Monitor::Drive get_usage", "[system_monitor][Drive]") {
    system_monitor::Monitor::Drive drive;

    double usage1 = drive.get_usage();
    CHECK(usage1 >= 0.0);       // Drive usage1 should be >= 0
    CHECK(usage1<= 1.0);        // Drive usage1 should be <= 1

    // For redundance
    double usage2 = drive.get_usage();
    CHECK(usage2 >= 0.0);
    CHECK(usage2 <= 1.0);

    auto total = drive.total();
    auto used = drive.used();

    // Used should be approximately constistent with usage2
    if(total != 0) {
        CHECK(static_cast<double>(used) / static_cast<double>(total) == Catch::Approx(usage2).epsilon(0.05));
    }

    CHECK(drive.free() <= total);       // Free should not be greater than total
}

// total, free & used
TEST_CASE("Monitor::Drive total/free/used", "[system_monitor][Drive]") {
    system_monitor::Monitor::Drive drive;

    auto total = drive.total();
    auto free = drive.free();
    auto used = drive.used();

    CHECK(total >= free);       // Total RAM should be at least as much as free
    CHECK(total >= used);       // Total RAM should be at least as much as used
    CHECK(total == Catch::Approx(free + used).epsilon(0.5));        // total should be approximately constistent with free + used
}

// path
TEST_CASE("Monitor::Drive path", "[system_monitor][Drive]") {
    system_monitor::Monitor::Drive drive;

    // Invalid Path should return 0
    CHECK(drive.total("/wrong/path/for/total/test") == 0);
    CHECK(drive.free("/wrong/path/for/total/test") == 0);
    CHECK(drive.used("/wrong/path/for/total/test") == 0);
}
