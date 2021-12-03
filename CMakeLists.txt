cmake_minimum_required (VERSION 3.8)
project ("raptorrds")

find_package(volk CONFIG REQUIRED)

option(RAPTORRDS_DEBUG_OUTPUT "Debug setting to output intermediary files." OFF)

if (RAPTORRDS_DEBUG_OUTPUT)
	add_definitions(-DRAPTORRDS_DEBUG_OUTPUT)
endif()

add_library (raptorrds STATIC "src/sync/sync.cpp" "include/raptorrds/sync/sync.h" "include/raptorrds/sync/frame.h" "include/raptorrds/sync/handler.h" "include/raptorrds/demod/demod.h" "src/demod/demod.cpp"    "include/raptorrds/demod/handler.h" "include/raptorrds/decode/message_basic_tuning.h" "include/raptorrds/client/client.h" "src/client/api.cpp" "src/client/core.cpp" "src/decode/message.cpp" "src/client/settings.h" "include/raptorrds/decode/message_radiotext.h" "src/client/radiotext.cpp" "src/client/basic_tuning.cpp" "include/raptorrds/demod_bcast_fm_stereo_rds.h" "include/raptorrds/pipeline.h" "src/pipeline.cpp" "src/demod_bcast_fm_stereo_rds.cpp" "include/raptorrds/data/data.h" "src/data/us.cpp" "src/data/europe.cpp" "include/raptorrds/debug.h" "src/debug.cpp")
target_include_directories (raptorrds PUBLIC include)
target_link_libraries(raptorrds raptordsp Volk::volk)