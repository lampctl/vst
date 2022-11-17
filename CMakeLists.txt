cmake_minimum_required(VERSION 3.15.0)

set(SMTG_ADD_VSTGUI ON)

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
    resource/plug.uidesc
    source/lampctlcids.h
    source/lampctlcontroller.h
    source/lampctlcontroller.cpp
    source/lampctlentry.cpp
    source/lampctlprocessor.h
    source/lampctlprocessor.cpp
    source/session.h
    source/session.cpp
    source/socket.h
    source/socket.cpp
)

target_link_libraries(lampctl
    PRIVATE
        sdk
        vstgui_support
)

smtg_target_add_plugin_resources(lampctl
    RESOURCES
        resource/plug.uidesc
)

smtg_target_configure_version_file(lampctl)
