cmake_minimum_required(VERSION 3.15.0)

set(SMTG_ADD_VSTGUI OFF)

if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "You must specify the path to the VST3 SDK!")
endif()

project(lampctl
    VERSION 1.0.0.0
    DESCRIPTION "lampctl VST3 plugin"
)

find_package(Boost 1.75.0 REQUIRED)

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

smtg_add_vst3plugin(lampctl
    src/lampctlcids.h
    src/lampctlcontroller.h
    src/lampctlcontroller.cpp
    src/lampctlentry.cpp
    src/lampctlprocessor.h
    src/lampctlprocessor.cpp
)

target_link_libraries(lampctl
    PRIVATE
        sdk
)

smtg_target_configure_version_file(lampctl)
