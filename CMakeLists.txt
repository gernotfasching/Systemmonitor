cmake_minimum_required(VERSION 3.18)
project(system_monitor VERSION 1.0)

include (CMakeLists.config)

find_package(wxWidgets REQUIRED COMPONENTS net core base)
if(wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif()

set(CPP_SRCS
    system_application.cpp
    monitor_canvas.cpp
    system_monitor.cpp
)

# Main Executable
add_executable(${PROJECT_NAME} ${CPP_SRCS})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})

# TESTS

# Sources
set(TEST_SRCS
    system_monitor_tests.cpp
    system_monitor.cpp
)

add_executable(system_monitor_tests ${TEST_SRCS})
target_link_libraries(system_monitor_tests catch2 ${wxWidgets_LIBRARIES})

# Register tests with CTest
add_test(NAME system_monitor_tests COMMAND system_monitor_tests)

enable_testing()
