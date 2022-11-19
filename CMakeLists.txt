cmake_minimum_required(VERSION 3.15.0)
project(lampctl
    VERSION 1.0.0.0
    DESCRIPTION "lampctl VST3 plugin"
)

if(NOT vst3sdk_SOURCE_DIR)
    message(FATAL_ERROR "You must specify the path to the VST3 SDK!")
endif()

set(Boost_USE_STATIC_LIBS ON)
set(SMTG_ADD_VSTGUI ON)

find_package(Boost 1.75.0 REQUIRED COMPONENTS context coroutine json thread)
include_directories(${Boost_INCLUDE_DIRS})

add_subdirectory(${vst3sdk_SOURCE_DIR} ${PROJECT_BINARY_DIR}/vst3sdk)
smtg_enable_vst3_sdk()

if(MSVC)
  add_compile_options(/bigobj)
endif()

smtg_add_vst3plugin(lampctl
    resource/plug.uidesc
    source/lampctlconnectioncontroller.h
    source/lampctlconnectioncontroller.cpp
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
        ${Boost_LIBRARIES}
)

smtg_target_add_plugin_resources(lampctl
    RESOURCES
        resource/plug.uidesc
)

smtg_target_configure_version_file(lampctl)
