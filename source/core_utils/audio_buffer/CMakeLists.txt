target_include_directories(core_utils PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(core_utils PUBLIC "audio_buffer.hpp" "circular_buffer.hpp")