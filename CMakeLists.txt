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

add_executable(${PROJECT_NAME} ${CPP_SRCS})
target_link_libraries(${PROJECT_NAME} ${wxWidgets_LIBRARIES})
