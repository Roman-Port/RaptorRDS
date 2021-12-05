cmake_minimum_required (VERSION 3.8)
project ("raptorrds")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

find_package(volk CONFIG REQUIRED)
find_package(raptordsp CONFIG REQUIRED)

option(RAPTORRDS_DEBUG_OUTPUT "Debug setting to output intermediary files." OFF)

if (RAPTORRDS_DEBUG_OUTPUT)
	add_definitions(-DRAPTORRDS_DEBUG_OUTPUT)
endif()

add_library (raptorrds SHARED "src/sync/sync.cpp" "include/raptorrds/sync/sync.h" "include/raptorrds/sync/frame.h" "include/raptorrds/sync/handler.h" "include/raptorrds/demod/demod.h" "src/demod/demod.cpp"    "include/raptorrds/demod/handler.h" "include/raptorrds/decode/message_basic_tuning.h" "include/raptorrds/client/client.h" "src/client/api.cpp" "src/client/core.cpp" "src/decode/message.cpp" "src/client/settings.h" "include/raptorrds/decode/message_radiotext.h" "src/client/radiotext.cpp" "src/client/basic_tuning.cpp" "include/raptorrds/demod_bcast_fm_stereo_rds.h" "include/raptorrds/pipeline.h" "src/pipeline.cpp" "src/demod_bcast_fm_stereo_rds.cpp" "include/raptorrds/data/data.h" "src/data/us.cpp" "src/data/europe.cpp" "include/raptorrds/debug.h" "src/debug.cpp")

target_include_directories(raptorrds PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>
)

target_link_libraries(raptorrds raptordsp Volk::volk)

# Set up install
set(installhelper_name raptorrds)
set(installhelper_version 0.1.0)
include(install_helper.cmake)
