add_library(utils_audio_processor)

target_compile_features(utils_audio_processor INTERFACE cxx_std_17)

target_include_directories(utils_audio_processor PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_sources(utils_audio_processor
    PUBLIC audio_processor.hpp filter_processor.hpp i2s_processor.hpp
)

target_link_libraries(utils_audio_processor PUBLIC core_utils)
