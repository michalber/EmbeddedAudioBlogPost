add_library(core_utils "")

target_compile_features(core_utils INTERFACE cxx_std_17)

target_include_directories(core_utils PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(core_utils PUBLIC singleton.hpp types.h time.hpp lifetime_obj.hpp helpers.hpp PRIVATE dummy.cpp)

add_subdirectory(audio_buffer)
