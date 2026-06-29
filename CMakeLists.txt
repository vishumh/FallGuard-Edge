cmake_minimum_required(VERSION 3.16)

project(FallGuardEdge
    VERSION 0.1.0
    DESCRIPTION "Privacy-preserving TinyML fall detection for ESP32-CAM"
    LANGUAGES CXX
)

option(FALLGUARD_BUILD_TESTS "Build FallGuard Edge tests" ON)

add_subdirectory(shared)
add_subdirectory(simulator)

if(FALLGUARD_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
